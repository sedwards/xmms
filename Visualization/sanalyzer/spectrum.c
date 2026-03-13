#include <config.h>
#include <gtk/gtk.h>
#include "gtk3_compat.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
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

VisPlugin sanalyzer_vp =
{
	NULL,
	NULL,
	0,
	NULL,
	0,
	2,
	spectrum_init,
	spectrum_cleanup,
	spectrum_about,
	spectrum_configure,
	NULL,
	spectrum_playback_start,
	spectrum_playback_stop,
	NULL,
	spectrum_render_freq
};

VisPlugin *get_vplugin_info(void)
{
	sanalyzer_vp.description = g_strdup(_("Spectrum Analyzer"));
	return &sanalyzer_vp;
}

static gboolean spectrum_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    if (draw_pixmap) {
        cairo_set_source_surface(cr, draw_pixmap, 0, 0);
        cairo_paint(cr);
    }
    return TRUE;
}

static void spectrum_init(void)
{
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), _("Spectrum Analyzer"));
    gtk_widget_set_size_request(window, WIDTH, HEIGHT);
    
    g_signal_connect(window, "draw", G_CALLBACK(spectrum_draw_cb), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroyed), &window);

    GdkPixbuf *pb = gdk_pixbuf_new_from_xpm_data((const char **)sanalyzer_xmms_logo_xpm);
    bg_pixmap = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH, HEIGHT);
    cairo_t *cr = cairo_create(bg_pixmap);
    gdk_cairo_set_source_pixbuf(cr, pb, 0, 0);
    cairo_paint(cr);
    cairo_destroy(cr);
    g_object_unref(pb);

    draw_pixmap = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH, HEIGHT);
    bar = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 25, HEIGHT);
    
    cr = cairo_create(bar);
    for (int i = 0; i < (HEIGHT / 2); i++)
    {
        double r = (double)i / (HEIGHT / 2.0);
        cairo_set_source_rgb(cr, r, 1.0 - r, 0.0);
        cairo_move_to(cr, 0, (HEIGHT/2) - i);
        cairo_line_to(cr, 24, (HEIGHT/2) - i);
        cairo_stroke(cr);
        
        cairo_set_source_rgb(cr, r, 0.0, 1.0 - r);
        cairo_move_to(cr, 0, (HEIGHT/2) + i);
        cairo_line_to(cr, 24, (HEIGHT/2) + i);
        cairo_stroke(cr);
    }
    cairo_destroy(cr);

	gtk_widget_show_all(window);
}

static void spectrum_cleanup(void)
{
	if (window)
		gtk_widget_destroy(window);
    if (bg_pixmap) cairo_surface_destroy(bg_pixmap);
    if (draw_pixmap) cairo_surface_destroy(draw_pixmap);
    if (bar) cairo_surface_destroy(bar);
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

static void spectrum_render_freq(gint16 freq_data[2][256])
{
    if (!window || !draw_pixmap) return;

    cairo_t *cr = cairo_create(draw_pixmap);
    
    /* Clear with background */
    cairo_set_source_surface(cr, bg_pixmap, 0, 0);
    cairo_paint(cr);

    for (int i = 0; i < 256; i++)
    {
        int h = (freq_data[0][i] + freq_data[1][i]) / 2;
        if (h > HEIGHT) h = HEIGHT;
        
        cairo_set_source_surface(cr, bar, i, 0);
        cairo_rectangle(cr, i, HEIGHT - h, 1, h);
        cairo_fill(cr);
    }
    cairo_destroy(cr);
    gtk_widget_queue_draw(window);
}
