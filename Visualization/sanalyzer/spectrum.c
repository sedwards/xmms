#include <gtk/gtk.h>
#include "gtk3_compat.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "xmms/plugin.h"
#include "xmms/i18n.h"
#include "libxmms/util.h"

#include "xmms_logo.xpm"

#define WIDTH 256
#define HEIGHT 64

static GtkWidget *window = NULL;
static cairo_surface_t *bg_pixmap = NULL, *draw_pixmap = NULL, *bar = NULL;
static gboolean playback_started = FALSE;
static gint16 freq_data[2][256];

static void spectrum_init(void);
static void spectrum_cleanup(void);
static void spectrum_about(void);
static void spectrum_configure(void);
static void spectrum_playback_start(void);
static void spectrum_playback_stop(void);
static void spectrum_render_freq(gint16 freq_data[2][256]);

VisPlugin spectrum_vp =
{
	NULL,   /* handle */
	NULL,   /* filename */
	0,      /* xmms_session */
	NULL,   /* description */
	0,      /* num_pcm_chs_wanted */
	2,      /* num_freq_chs_wanted */
	spectrum_init,
	spectrum_cleanup,
	spectrum_about,
	spectrum_configure,
    NULL,   /* disable_plugin */
	spectrum_playback_start,
	spectrum_playback_stop,
    NULL,   /* render_pcm */
	spectrum_render_freq
};

VisPlugin *get_vplugin_info(void)
{
    spectrum_vp.description = g_strdup(_("Spectrum Analyzer"));
	return &spectrum_vp;
}

static void spectrum_init(void)
{
    (void)freq_data;
    (void)playback_started;
    (void)bg_pixmap; (void)draw_pixmap; (void)bar;

	if (window)
		return;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), _("Spectrum Analyzer"));
	gtk_container_set_border_width(GTK_CONTAINER(window), 0);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroyed), &window);

	GtkWidget *da = gtk_drawing_area_new();
	gtk_widget_set_size_request(da, WIDTH, HEIGHT);
	gtk_container_add(GTK_CONTAINER(window), da);

	gtk_widget_show_all(window);
}

static void spectrum_cleanup(void)
{
	if (window)
		gtk_widget_destroy(window);
}

static void spectrum_about(void)
{
	xmms_show_message(_("About Spectrum Analyzer"),
					  _("Spectrum Analyzer plugin\n\n"
					    "By Peter Alm 1998"), _("Ok"), FALSE, NULL, NULL);
}

static void spectrum_configure(void)
{
}

static void spectrum_playback_start(void)
{
	playback_started = TRUE;
}

static void spectrum_playback_stop(void)
{
	playback_started = FALSE;
}

static void spectrum_render_freq(gint16 freq_data_in[2][256])
{
	if (window)
	{
		memcpy(freq_data, freq_data_in, sizeof (freq_data));
	}
}
