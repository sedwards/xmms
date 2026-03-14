#include <gtk/gtk.h>
#include "gtk3_compat.h"
#include <stdlib.h>
#include <string.h>
#include "xmms/plugin.h"
#include "xmms/i18n.h"
#include "libxmms/util.h"

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "xmms_logo.xpm"

#define WIDTH 80
#define HEIGHT 40
#define BPL (WIDTH + 2)

static GtkWidget *window = NULL, *area = NULL;
static cairo_surface_t *bg_pixmap = NULL;
static guchar *rgb_buf = NULL;
static guint32 colors[256];

static void bscope_init(void);
static void bscope_cleanup(void);
static void bscope_about(void);
static void bscope_configure(void);
static void bscope_render_pcm(gint16 pcm_data[2][512]);

VisPlugin bscope_vp =
{
	NULL,   /* handle */
	NULL,   /* filename */
	0,      /* xmms_session */
	NULL,   /* description */
	512,    /* num_pcm_chs_wanted */
	0,      /* num_freq_chs_wanted */
	bscope_init,
	bscope_cleanup,
	bscope_about,
	bscope_configure,
    NULL,   /* disable_plugin */
    NULL,   /* playback_start */
    NULL,   /* playback_stop */
	bscope_render_pcm,
    NULL    /* render_freq */
};

VisPlugin *get_vplugin_info(void)
{
	bscope_vp.description = g_strdup(_("Blur Scope"));
	return &bscope_vp;
}

static gboolean bscope_expose(GtkWidget * widget, cairo_t *cr, gpointer data)
{
    if (bg_pixmap) {
        cairo_set_source_surface(cr, bg_pixmap, 0, 0);
        cairo_paint(cr);
    }
    return TRUE;
}

static struct
{
	guint32 color;
} bscope_cfg;

static void bscope_init(void)
{
	int i;
    (void)bscope_cfg;

	rgb_buf = g_malloc0(BPL * (HEIGHT + 2));
	for (i = 0; i < 256; i++)
	{
		colors[i] = (((i * 200) / 255) << 16) | (((i * 200) / 255) << 8) | ((i * 255) / 255);
	}

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), _("Blur Scope"));
    gtk_widget_set_size_request(window, WIDTH, HEIGHT);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroyed), &window);

	area = gtk_drawing_area_new();
	gtk_widget_set_size_request(area, WIDTH, HEIGHT);
	gtk_container_add(GTK_CONTAINER(window), area);
	g_signal_connect(area, "draw", G_CALLBACK(bscope_expose), NULL);

	gtk_widget_show_all(window);
}

static void bscope_cleanup(void)
{
	if (window)
		gtk_widget_destroy(window);
	g_free(rgb_buf);
}

static void bscope_about(void)
{
	xmms_show_message(_("About Blur Scope"),
					  _("Blur Scope\n\n"
					    "By Peter Alm 1998"), _("Ok"), FALSE, NULL, NULL);
}

static void bscope_configure(void)
{
}

static void bscope_render_pcm(gint16 pcm_data[2][512])
{
    (void)pcm_data;
    (void)colors;
	if (window)
	{
        /* Placeholder for rendering logic */
	}
}
