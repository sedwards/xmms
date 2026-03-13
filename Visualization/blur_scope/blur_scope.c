#include <config.h>
#include <gtk/gtk.h>
#include "gtk3_compat.h"
#include <stdlib.h>
#include <string.h>
#include "xmms/plugin.h"
#include "xmms/i18n.h"
#include "libxmms/util.h"

#include "xmms_logo.xpm"

#define WIDTH 80
#define HEIGHT 40
#define BPL (WIDTH + 2)

static GtkWidget *window = NULL, *area = NULL;
static cairo_surface_t *bg_pixmap = NULL;
static guchar *rgb_buf = NULL;
static guint32 colors[256];

static struct
{
	guint32 color;
} bscope_cfg;

static void bscope_init(void);
static void bscope_cleanup(void);
static void bscope_about(void);
static void bscope_configure(void);
static void bscope_render_pcm(gint16 pcm_data[2][512]);

VisPlugin bscope_vp =
{
	NULL,
	NULL,
	0,
	NULL,
	512,
	0,
	bscope_init,
	bscope_cleanup,
	bscope_about,
	bscope_configure,
	NULL,
	NULL,
	NULL,
	bscope_render_pcm,
	NULL
};

VisPlugin *get_vplugin_info(void)
{
	bscope_vp.description = g_strdup(_("Blur Scope"));
	return &bscope_vp;
}

static gboolean bscope_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    if (!rgb_buf) return TRUE;

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            guchar idx = rgb_buf[(y + 1) * BPL + (x + 1)];
            if (idx == 0 && bg_pixmap) {
                /* Draw from background if available */
                cairo_set_source_surface(cr, bg_pixmap, 0, 0);
                cairo_rectangle(cr, x, y, 1, 1);
                cairo_fill(cr);
            } else {
                guint32 c = colors[idx];
                cairo_set_source_rgb(cr, ((c >> 16) & 0xff)/255.0, ((c >> 8) & 0xff)/255.0, (c & 0xff)/255.0);
                cairo_rectangle(cr, x, y, 1, 1);
                cairo_fill(cr);
            }
        }
    }
    return TRUE;
}

static void bscope_init(void)
{
	int i;

	rgb_buf = g_malloc0(BPL * (HEIGHT + 2));
	for (i = 0; i < 256; i++)
	{
		colors[i] = (((i * 200) / 255) << 16) | (((i * 200) / 255) << 8) | ((i * 255) / 255);
	}

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), _("Blur Scope"));
    gtk_widget_set_size_request(window, WIDTH, HEIGHT);

    area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), area);
    
    g_signal_connect(area, "draw", G_CALLBACK(bscope_draw_cb), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroyed), &window);

    GdkPixbuf *pb = gdk_pixbuf_new_from_xpm_data((const char **)bscope_xmms_logo_xpm);
    bg_pixmap = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH, HEIGHT);
    cairo_t *cr = cairo_create(bg_pixmap);
    gdk_cairo_set_source_pixbuf(cr, pb, 0, 0);
    cairo_paint(cr);
    cairo_destroy(cr);
    g_object_unref(pb);

	gtk_widget_show_all(window);
}

static void bscope_cleanup(void)
{
	if (window)
		gtk_widget_destroy(window);
	g_free(rgb_buf);
    if (bg_pixmap) cairo_surface_destroy(bg_pixmap);
}

static void bscope_about(void)
{
	xmms_show_message(_("About Blur Scope"),
					  _("Blur Scope plugin\n\n"
					    "By Peter Alm 1999"), _("Ok"), FALSE, NULL, NULL);
}

static void bscope_configure(void)
{
}

static void bscope_render_pcm(gint16 pcm_data[2][512])
{
	int i, y;
	guchar *ptr;

	if (!area || !gtk_widget_get_realized(area))
		return;

	for (y = 1; y < HEIGHT + 1; y++)
	{
		ptr = rgb_buf + (y * BPL) + 1;
		for (i = 0; i < WIDTH; i++)
		{
			ptr[i] = (ptr[i] + ptr[i - 1] + ptr[i + 1] + ptr[i - BPL] + ptr[i + BPL]) / 5;
			if (ptr[i])
				ptr[i]--;
		}
	}
	for (i = 0; i < WIDTH; i++)
	{
		y = (pcm_data[0][i * 512 / WIDTH] + pcm_data[1][i * 512 / WIDTH]) / 2;
		y = (y * HEIGHT) / 65536 + (HEIGHT / 2);
		if (y < 0)
			y = 0;
		if (y >= HEIGHT)
			y = HEIGHT - 1;
		rgb_buf[(y + 1) * BPL + (i + 1)] = 255;
	}
    gtk_widget_queue_draw(area);
}
