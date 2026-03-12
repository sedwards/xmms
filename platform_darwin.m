#import <Cocoa/Cocoa.h>
#include "../platform.h"

void platform_open_directory_browser(const char *title,
                                     const char *path,
                                     xmms_dir_handler handler)
{
    NSOpenPanel *panel = [NSOpenPanel openPanel];

    panel.canChooseFiles = NO;
    panel.canChooseDirectories = YES;
    panel.allowsMultipleSelection = NO;

    if([panel runModal] == NSModalResponseOK)
    {
        NSURL *url = panel.URL;

        if(handler)
            handler((char*)url.path.UTF8String);
    }
}

int platform_audio_init(void)
{
    return 0; /* CoreAudio initialization later */
}

void platform_audio_shutdown(void)
{
}

int platform_visualizer_init(void)
{
    return 0; /* Metal setup later */
}

