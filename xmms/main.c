#include "xmms.h"
#include <cairo.h>

GtkWidget *mainwin, *mainwin_url_window = NULL, *mainwin_dir_browser = NULL;
GtkWidget *mainwin_jtt = NULL, *mainwin_jtf = NULL;
GtkItemFactory *mainwin_options_menu, *mainwin_songname_menu, *mainwin_vis_menu, *mainwin_general_menu;
cairo_surface_t *mainwin_bg = NULL;
cairo_t *mainwin_gc = NULL;

GtkAccelGroup *mainwin_accel;
GList *mainwin_wlist = NULL;
PButton *mainwin_prev, *mainwin_play, *mainwin_pause, *mainwin_stop, *mainwin_next, *mainwin_eject;
TButton *mainwin_shuffle, *mainwin_repeat, *mainwin_eq, *mainwin_pl;
HSlider *mainwin_volume, *mainwin_balance, *mainwin_sposition;
TextBox *mainwin_info, *mainwin_stime_min, *mainwin_stime_sec;
Vis *mainwin_vis;
SVis *mainwin_svis;
Number *mainwin_10min_num, *mainwin_min_num, *mainwin_10sec_num, *mainwin_sec_num;
PlayStatus *mainwin_playstatus;
GtkWidget *mainwin_menu;
gboolean mainwin_focus = TRUE, mainwin_info_text_locked = FALSE;
gboolean mainwin_moving = FALSE;

/* Forward declarations */
static gboolean mainwin_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data);

void mainwin_set_shape_mask(void)
{
    if (cfg.show_wm_decorations) return;
    gtk_widget_shape_combine_region(mainwin, skin_get_mask(SKIN_MASK_MAIN, cfg.doublesize, cfg.player_shaded));
}

void draw_main_window(gboolean force)
{
	GList *wl;
	Widget *w;
	gboolean redraw;
    cairo_t *cr;

	if (!cfg.player_visible || !mainwin_bg)
		return;
        
	lock_widget_list(mainwin_wlist);
    
    cr = cairo_create(mainwin_bg);
	if (force)
	{
		skin_draw_pixmap(cr, SKIN_MAIN, 0, 0, 0, 0, 275, cfg.player_shaded ? 14 : 116);
		/* TODO: draw_mainwin_titlebar(mainwin_focus); */
		draw_widget_list(mainwin_wlist, cr, &redraw, TRUE);
	}
	else
		draw_widget_list(mainwin_wlist, cr, &redraw, FALSE);
    cairo_destroy(cr);

	if (redraw || force)
	{
        gtk_widget_queue_draw(mainwin);
	}
	unlock_widget_list(mainwin_wlist);
}

static gboolean mainwin_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    if (!mainwin_bg) return FALSE;
    
    cairo_save(cr);
    if (cfg.doublesize) {
        cairo_scale(cr, 2.0, 2.0);
    }
    cairo_set_source_surface(cr, mainwin_bg, 0, 0);
    cairo_paint(cr);
    cairo_restore(cr);
    
    return TRUE;
}

static void mainwin_create_gtk(void)
{
	mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	dock_window_list = dock_add_window(dock_window_list, mainwin);
	gtk_widget_set_app_paintable(mainwin, TRUE);
	gtk_window_set_title(GTK_WINDOW(mainwin), _("XMMS"));
	
	gtk_widget_set_events(mainwin, GDK_FOCUS_CHANGE_MASK | GDK_BUTTON_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_STRUCTURE_MASK);
	if (cfg.player_x != -1 && cfg.save_window_position)
		dock_set_uposition(mainwin, cfg.player_x, cfg.player_y);
	gtk_widget_realize(mainwin);

	util_set_cursor(mainwin);
	
	if (cfg.doublesize)
		gtk_widget_set_size_request(mainwin, 550, cfg.player_shaded ? 28 : 232);
	else
		gtk_widget_set_size_request(mainwin, 275, cfg.player_shaded ? 14 : 116);

	gtk_window_add_accel_group(GTK_WINDOW(mainwin), mainwin_accel);

	g_signal_connect(mainwin, "draw", G_CALLBACK(mainwin_draw_cb), NULL);
    /* TODO: Connect other signals using g_signal_connect */

	mainwin_set_shape_mask();
}

void mainwin_create(void)
{
	mainwin_bg = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 275, 116);
	mainwin_create_gtk();
	/* mainwin_create_widgets(); - STUB */
}

/* Rest of the file stubs or minimal implementation to allow building */
void mainwin_show(gboolean show) { if (show) gtk_widget_show(mainwin); else gtk_widget_hide(mainwin); }
void mainwin_set_info_text(void) {}
void mainwin_set_song_info(gint rate, gint freq, gint nch) {}
void mainwin_lock_info_text(gchar * text) {}
void mainwin_release_info_text(void) {}
void mainwin_set_back_pixmap(void) {}
void mainwin_shuffle_pushed(gboolean toggled) {}
void mainwin_repeat_pushed(gboolean toggled) {}
void mainwin_real_show(void) { gtk_widget_show(mainwin); }
void mainwin_real_hide(void) { gtk_widget_hide(mainwin); }
