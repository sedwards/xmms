#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <glib.h>
#include <math.h>

#include <CoreAudio/CoreAudio.h>
#include <AudioUnit/AudioUnit.h>
#include <CoreServices/CoreServices.h>

#include "xmms/plugin.h"
#include "xmms/i18n.h"
#include "libxmms/util.h"

/* Ring buffer settings */
#define BUFFER_SIZE (128 * 1024)

typedef struct {
    guchar *data;
    gint read_pos;
    gint write_pos;
    gint filled;
    pthread_mutex_t mutex;
    pthread_cond_t cond_free;
} RingBuffer;

static struct {
    AudioUnit au;
    RingBuffer ring;
    gint rate;
    gint nch;
    AFormat fmt;
    gboolean paused;
    gboolean active;
    gint written_bytes;
    int vol_l, vol_r;
} ca_data;

static void ring_buffer_init(RingBuffer *rb) {
    rb->data = g_malloc(BUFFER_SIZE);
    rb->read_pos = 0;
    rb->write_pos = 0;
    rb->filled = 0;
    pthread_mutex_init(&rb->mutex, NULL);
    pthread_cond_init(&rb->cond_free, NULL);
}

static void ring_buffer_cleanup(RingBuffer *rb) {
    if (rb->data) {
        g_free(rb->data);
        rb->data = NULL;
    }
    pthread_mutex_destroy(&rb->mutex);
    pthread_cond_destroy(&rb->cond_free);
}

static void ring_buffer_flush(RingBuffer *rb) {
    pthread_mutex_lock(&rb->mutex);
    rb->read_pos = 0;
    rb->write_pos = 0;
    rb->filled = 0;
    pthread_cond_broadcast(&rb->cond_free);
    pthread_mutex_unlock(&rb->mutex);
}

static OSStatus ca_render_cb(void *inRefCon,
                            AudioUnitRenderActionFlags *ioActionFlags,
                            const AudioTimeStamp *inTimeStamp,
                            UInt32 inBusNumber,
                            UInt32 inNumberFrames,
                            AudioBufferList *ioData) {
    RingBuffer *rb = &ca_data.ring;
    gint bytes_needed = ioData->mBuffers[0].mDataByteSize;
    guchar *out = (guchar *)ioData->mBuffers[0].mData;

    pthread_mutex_lock(&rb->mutex);

    if (ca_data.paused || rb->filled == 0 || !ca_data.active) {
        memset(out, 0, bytes_needed);
        pthread_mutex_unlock(&rb->mutex);
        return noErr;
    }

    gint to_copy = MIN(bytes_needed, rb->filled);
    gint first_part = MIN(to_copy, BUFFER_SIZE - rb->read_pos);

    memcpy(out, rb->data + rb->read_pos, first_part);
    if (to_copy > first_part) {
        memcpy(out + first_part, rb->data, to_copy - first_part);
    }

    rb->read_pos = (rb->read_pos + to_copy) % BUFFER_SIZE;
    rb->filled -= to_copy;

    if (to_copy < bytes_needed) {
        memset(out + to_copy, 0, bytes_needed - to_copy);
    }

    pthread_cond_broadcast(&rb->cond_free);
    pthread_mutex_unlock(&rb->mutex);

    return noErr;
}

static void ca_init(void) {
    memset(&ca_data, 0, sizeof(ca_data));
    ca_data.vol_l = 100;
    ca_data.vol_r = 100;
    ring_buffer_init(&ca_data.ring);
}

static void ca_cleanup(void) {
    ring_buffer_cleanup(&ca_data.ring);
}

static void ca_about(void) {
    xmms_show_message(_("About CoreAudio Plugin"),
                      _("XMMS CoreAudio Output Plugin\n\n"
                        "Ported for macOS GTK3 conversion."),
                      _("Ok"), FALSE, NULL, NULL);
}

static void ca_configure(void) {
}

static void ca_get_volume(int *l, int *r) {
    *l = ca_data.vol_l;
    *r = ca_data.vol_r;
}

static void ca_set_volume(int l, int r) {
    ca_data.vol_l = l;
    ca_data.vol_r = r;
}

static int ca_open_audio(AFormat fmt, int rate, int nch) {
    ca_data.fmt = fmt;
    ca_data.rate = rate;
    ca_data.nch = nch;
    ca_data.written_bytes = 0;

    AudioStreamBasicDescription asbd;
    memset(&asbd, 0, sizeof(asbd));
    asbd.mSampleRate = rate;
    asbd.mFormatID = kAudioFormatLinearPCM;
    asbd.mFormatFlags = kAudioFormatFlagIsPacked | kAudioFormatFlagsNativeEndian;
    
    if (fmt != FMT_U8)
        asbd.mFormatFlags |= kAudioFormatFlagIsSignedInteger;
        
    asbd.mChannelsPerFrame = nch;
    asbd.mFramesPerPacket = 1;
    asbd.mBitsPerChannel = (fmt == FMT_U8 || fmt == FMT_S8) ? 8 : 16;
    asbd.mBytesPerFrame = (asbd.mBitsPerChannel / 8) * nch;
    asbd.mBytesPerPacket = asbd.mBytesPerFrame;

    AudioComponentDescription desc;
    memset(&desc, 0, sizeof(desc));
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_DefaultOutput;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;

    AudioComponent comp = AudioComponentFindNext(NULL, &desc);
    if (!comp) return 0;

    if (AudioComponentInstanceNew(comp, &ca_data.au) != noErr) return 0;

    if (AudioUnitInitialize(ca_data.au) != noErr) return 0;

    if (AudioUnitSetProperty(ca_data.au, kAudioUnitProperty_StreamFormat,
                             kAudioUnitScope_Input, 0, &asbd, sizeof(asbd)) != noErr) return 0;

    AURenderCallbackStruct input;
    input.inputProc = ca_render_cb;
    input.inputProcRefCon = NULL;

    if (AudioUnitSetProperty(ca_data.au, kAudioUnitProperty_SetRenderCallback,
                             kAudioUnitScope_Input, 0, &input, sizeof(input)) != noErr) return 0;

    if (AudioOutputUnitStart(ca_data.au) != noErr) return 0;

    ca_data.active = TRUE;
    ca_data.paused = FALSE;
    
    return 1;
}

static void ca_write_audio(void *ptr, int length) {
    RingBuffer *rb = &ca_data.ring;
    gint remaining = length;
    
    /* Software volume control */
    guchar *src;
    gboolean free_src = FALSE;
    
    if (ca_data.vol_l != 100 || ca_data.vol_r != 100) {
        src = g_malloc(length);
        memcpy(src, ptr, length);
        free_src = TRUE;
        
        float vl = (float)ca_data.vol_l / 100.0f;
        float vr = (float)ca_data.vol_r / 100.0f;
        
        if (ca_data.fmt == FMT_S16_LE || ca_data.fmt == FMT_S16_BE || ca_data.fmt == FMT_S16_NE) {
            gint16 *s = (gint16 *)src;
            int samples = length / 2;
            for (int i = 0; i < samples; i += ca_data.nch) {
                s[i] = (gint16)((float)s[i] * vl);
                if (ca_data.nch > 1) s[i+1] = (gint16)((float)s[i+1] * vr);
            }
        }
    } else {
        src = (guchar *)ptr;
    }

    while (remaining > 0 && ca_data.active) {
        pthread_mutex_lock(&rb->mutex);
        
        while (rb->filled == BUFFER_SIZE && ca_data.active) {
            pthread_cond_wait(&rb->cond_free, &rb->mutex);
        }

        if (!ca_data.active) {
            pthread_mutex_unlock(&rb->mutex);
            break;
        }

        gint can_write = MIN(remaining, BUFFER_SIZE - rb->filled);
        gint first_part = MIN(can_write, BUFFER_SIZE - rb->write_pos);

        memcpy(rb->data + rb->write_pos, src + (length - remaining), first_part);
        if (can_write > first_part) {
            memcpy(rb->data, src + (length - remaining) + first_part, can_write - first_part);
        }

        rb->write_pos = (rb->write_pos + can_write) % BUFFER_SIZE;
        rb->filled += can_write;
        remaining -= can_write;
        ca_data.written_bytes += can_write;

        pthread_mutex_unlock(&rb->mutex);
    }
    
    if (free_src) g_free(src);
}

static void ca_close_audio(void) {
    if (!ca_data.active) return;
    ca_data.active = FALSE;
    
    pthread_mutex_lock(&ca_data.ring.mutex);
    pthread_cond_broadcast(&ca_data.ring.cond_free);
    pthread_mutex_unlock(&ca_data.ring.mutex);

    if (ca_data.au) {
        AudioOutputUnitStop(ca_data.au);
        AudioUnitUninitialize(ca_data.au);
        AudioComponentInstanceDispose(ca_data.au);
        ca_data.au = NULL;
    }
}

static void ca_flush(int time) {
    ring_buffer_flush(&ca_data.ring);
}

static void ca_pause(short paused) {
    ca_data.paused = paused;
}

static int ca_buffer_free(void) {
    gint free_space;
    pthread_mutex_lock(&ca_data.ring.mutex);
    free_space = BUFFER_SIZE - ca_data.ring.filled;
    pthread_mutex_unlock(&ca_data.ring.mutex);
    return free_space;
}

static int ca_buffer_playing(void) {
    return ca_data.ring.filled > 0;
}

static int ca_output_time(void) {
    if (ca_data.rate == 0 || ca_data.nch == 0) return 0;
    int bytes_per_sec = ca_data.rate * ca_data.nch * ((ca_data.fmt == FMT_U8 || ca_data.fmt == FMT_S8) ? 1 : 2);
    if (bytes_per_sec == 0) return 0;
    return (ca_data.written_bytes - ca_data.ring.filled) * 1000 / bytes_per_sec;
}

static int ca_written_time(void) {
    if (ca_data.rate == 0 || ca_data.nch == 0) return 0;
    int bytes_per_sec = ca_data.rate * ca_data.nch * ((ca_data.fmt == FMT_U8 || ca_data.fmt == FMT_S8) ? 1 : 2);
    if (bytes_per_sec == 0) return 0;
    return ca_data.written_bytes * 1000 / bytes_per_sec;
}

OutputPlugin ca_op = {
    NULL,
    NULL,
    NULL,
    ca_init,
    ca_about,
    ca_configure,
    ca_get_volume,
    ca_set_volume,
    ca_open_audio,
    ca_write_audio,
    ca_close_audio,
    ca_flush,
    ca_pause,
    ca_buffer_free,
    ca_buffer_playing,
    ca_output_time,
    ca_written_time,
    NULL, /* add_vis_func */
    NULL, /* del_vis_func */
    ca_cleanup
};

OutputPlugin *get_oplugin_info(void) {
    ca_op.description = g_strdup_printf(_("CoreAudio Output Plugin %s"), VERSION);
    return &ca_op;
}
