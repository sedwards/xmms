#include "xmms.h"
#include <gtk/gtk.h>
#include <cairo.h>
#include "gtk3_compat.h"
#include "libxmms/configfile.h"
#include "libxmms/util.h"
#include "xmms/plugin.h"
#include "xmms/skin.h"
#include "xmms/widget.h"
#include "xmms/textbox.h"
#include "xmms/pbutton.h"
#include "xmms/sbutton.h"
#include "xmms/tbutton.h"
#include "xmms/hslider.h"
#include "xmms/monostereo.h"
#include "xmms/playstatus.h"
#include "xmms/vis.h"
#include "xmms/svis.h"
#include "xmms/number.h"
#include "xmms/menurow.h"
#include "main.h"
#include "about.h"
#include "equalizer.h"
#include "playlist.h"
#include "playlistwin.h"
#include "equalizer.h"
#include "prefswin.h"
#include "libxmms/dirbrowser.h"
#include "log.h"

#ifdef HAVE_GTK_MAC
#include <gtkosxapplication.h>
#endif

/* Global Window and Background */
GtkWidget *mainwin;
cairo_surface_t *mainwin_bg = NULL;
gint scaling_factor = 1;

/* Required Global State Symbols */
GtkItemFactory *mainwin_options_menu = NULL;
GtkItemFactory *mainwin_vis_menu = NULL;
GtkItemFactory *mainwin_general_menu = NULL;
GtkItemFactory *mainwin_songname_menu = NULL;
GList *disabled_iplugins = NULL;
GList *dock_window_list = NULL;
gint bitrate = 0, frequency = 0, numchannels = 0;
gboolean pposition_broken = FALSE;
cairo_t *mainwin_gc = NULL;
gboolean mainwin_moving = FALSE;

const GtkTargetEntry _xmms_drop_types[] = {
	{"text/plain", 0, 0},
	{"text/uri-list", 0, 1},
	{"STRING", 0, 2}
};

/* Global Config and State */
Config cfg;
gboolean mainwin_focus = TRUE;

void load_config(void)
{
    ConfigFile *cfgfile = xmms_cfg_open_default_file();
    
    /* Set defaults */
    cfg.player_x = -1;
    cfg.player_y = -1;
    cfg.playlist_x = -1;
    cfg.playlist_y = -1;
    cfg.equalizer_x = -1;
    cfg.equalizer_y = -1;
    cfg.playlist_width = 275;
    cfg.playlist_height = 116;
    cfg.doublesize = FALSE;
    cfg.player_shaded = FALSE;
    cfg.playlist_shaded = FALSE;
    cfg.equalizer_shaded = FALSE;
    cfg.playlist_visible = TRUE;
    cfg.equalizer_visible = TRUE;
    cfg.shuffle = FALSE;
    cfg.repeat = FALSE;
    cfg.timer_mode = 0;
    cfg.vis_type = 0;
    cfg.filesel_path = g_strdup(g_get_home_dir());
    cfg.save_window_position = TRUE;
    cfg.save_config_on_quit = TRUE;

    if (cfgfile) {
        xmms_cfg_read_int(cfgfile, "xmms", "player_x", &cfg.player_x);
        xmms_cfg_read_int(cfgfile, "xmms", "player_y", &cfg.player_y);
        xmms_cfg_read_int(cfgfile, "xmms", "playlist_x", &cfg.playlist_x);
        xmms_cfg_read_int(cfgfile, "xmms", "playlist_y", &cfg.playlist_y);
        xmms_cfg_read_int(cfgfile, "xmms", "equalizer_x", &cfg.equalizer_x);
        xmms_cfg_read_int(cfgfile, "xmms", "equalizer_y", &cfg.equalizer_y);
        xmms_cfg_read_int(cfgfile, "xmms", "playlist_width", &cfg.playlist_width);
        xmms_cfg_read_int(cfgfile, "xmms", "playlist_height", &cfg.playlist_height);
        xmms_cfg_read_boolean(cfgfile, "xmms", "doublesize", &cfg.doublesize);
        xmms_cfg_read_boolean(cfgfile, "xmms", "player_shaded", &cfg.player_shaded);
        xmms_cfg_read_boolean(cfgfile, "xmms", "playlist_visible", &cfg.playlist_visible);
        xmms_cfg_read_boolean(cfgfile, "xmms", "equalizer_visible", &cfg.equalizer_visible);
        xmms_cfg_read_boolean(cfgfile, "xmms", "shuffle", &cfg.shuffle);
        xmms_cfg_read_boolean(cfgfile, "xmms", "repeat", &cfg.repeat);
        xmms_cfg_read_string(cfgfile, "xmms", "filesel_path", &cfg.filesel_path);
        xmms_cfg_read_boolean(cfgfile, "xmms", "save_config_on_quit", &cfg.save_config_on_quit);
        xmms_cfg_free(cfgfile);
    }
}

void save_config(void)
{
    ConfigFile *cfgfile = xmms_cfg_new();
    
    /* Update positions from current window state */
    if (mainwin) gtk_window_get_position(GTK_WINDOW(mainwin), &cfg.player_x, &cfg.player_y);
    if (playlistwin) gtk_window_get_position(GTK_WINDOW(playlistwin), &cfg.playlist_x, &cfg.playlist_y);
    if (equalizerwin) gtk_window_get_position(GTK_WINDOW(equalizerwin), &cfg.equalizer_x, &cfg.equalizer_y);

    xmms_cfg_write_int(cfgfile, "xmms", "player_x", cfg.player_x);
    xmms_cfg_write_int(cfgfile, "xmms", "player_y", cfg.player_y);
    xmms_cfg_write_int(cfgfile, "xmms", "playlist_x", cfg.playlist_x);
    xmms_cfg_write_int(cfgfile, "xmms", "playlist_y", cfg.playlist_y);
    xmms_cfg_write_int(cfgfile, "xmms", "equalizer_x", cfg.equalizer_x);
    xmms_cfg_write_int(cfgfile, "xmms", "equalizer_y", cfg.equalizer_y);
    xmms_cfg_write_int(cfgfile, "xmms", "playlist_width", cfg.playlist_width);
    xmms_cfg_write_int(cfgfile, "xmms", "playlist_height", cfg.playlist_height);
    xmms_cfg_write_boolean(cfgfile, "xmms", "doublesize", cfg.doublesize);
    xmms_cfg_write_boolean(cfgfile, "xmms", "player_shaded", cfg.player_shaded);
    xmms_cfg_write_boolean(cfgfile, "xmms", "playlist_visible", cfg.playlist_visible);
    xmms_cfg_write_boolean(cfgfile, "xmms", "equalizer_visible", cfg.equalizer_visible);
    xmms_cfg_write_boolean(cfgfile, "xmms", "shuffle", cfg.shuffle);
    xmms_cfg_write_boolean(cfgfile, "xmms", "repeat", cfg.repeat);
    xmms_cfg_write_string(cfgfile, "xmms", "filesel_path", cfg.filesel_path);
    xmms_cfg_write_boolean(cfgfile, "xmms", "save_config_on_quit", cfg.save_config_on_quit);
    
    xmms_cfg_write_default_file(cfgfile);
    xmms_cfg_free(cfgfile);
}

static gboolean periodic_save_timer(gpointer data)
{
    if (cfg.save_window_position) {
        /* Only save window positions periodically */
        if (mainwin) gtk_window_get_position(GTK_WINDOW(mainwin), &cfg.player_x, &cfg.player_y);
        if (playlistwin) gtk_window_get_position(GTK_WINDOW(playlistwin), &cfg.playlist_x, &cfg.playlist_y);
        if (equalizerwin) gtk_window_get_position(GTK_WINDOW(equalizerwin), &cfg.equalizer_x, &cfg.equalizer_y);
        
        save_config();
    }
    return TRUE;
}

void mainwin_quit_cb(void) 
{ 
    xmms_log("Quitting XMMS..."); 
    if (cfg.save_config_on_quit) {
        save_config();
    }
    gtk_main_quit(); 
}

GList *mainwin_wlist = NULL;
Vis *active_vis;

/* Widgets */
PButton *mainwin_menubtn, *mainwin_minimize, *mainwin_shade, *mainwin_close;
PButton *mainwin_rew, *mainwin_play, *mainwin_pause, *mainwin_stop, *mainwin_fwd, *mainwin_eject;
SButton *mainwin_srew, *mainwin_splay, *mainwin_spause, *mainwin_sstop, *mainwin_sfwd, *mainwin_seject, *mainwin_about;
TButton *mainwin_shuffle, *mainwin_repeat, *mainwin_eq, *mainwin_pl;
TextBox *mainwin_info, *mainwin_rate_text, *mainwin_freq_text, *mainwin_stime_min, *mainwin_stime_sec;
MenuRow *mainwin_menurow;
HSlider *mainwin_volume, *mainwin_balance, *mainwin_position, *mainwin_sposition = NULL;
MonoStereo *mainwin_monostereo;
PlayStatus *mainwin_playstatus;
Number *mainwin_minus_num, *mainwin_10min_num, *mainwin_min_num, *mainwin_10sec_num, *mainwin_sec_num;
Vis *mainwin_vis;
SVis *mainwin_svis;

/* Non-static function implementations */
void mainwin_menubtn_cb(void) { xmms_log("Menu Button Clicked"); show_prefs_window(); }
void mainwin_minimize_cb(void) { xmms_log("Minimize Clicked"); gtk_window_iconify(GTK_WINDOW(mainwin)); }
void mainwin_shade_cb(void) { xmms_log("Shade Clicked"); }
void mainwin_play_pushed(void) { xmms_log("Play Button Clicked"); }
void mainwin_stop_pushed(void) { xmms_log("Stop Button Clicked"); }
static void mainwin_dir_browser_callback(char *path)
{
    xmms_log("Native browser selected: %s", path);
    playlist_add(path);
    playlist_play();
}

void mainwin_eject_pushed(void) 
{ 
    xmms_log("Eject pushed - opening native browser");
    xmms_create_dir_browser(_("Open Files"), cfg.filesel_path, GTK_SELECTION_MULTIPLE, mainwin_dir_browser_callback);
}

void mainwin_pl_pushed(gboolean toggled) 
{ 
    xmms_log("Playlist button toggled: %d", toggled);
    if (toggled) {
        xmms_create_dir_browser(_("Add to Playlist"), cfg.filesel_path, GTK_SELECTION_MULTIPLE, mainwin_dir_browser_callback);
    }
}
void mainwin_shuffle_pushed(gboolean toggled) { xmms_log("Shuffle toggled: %d", toggled); cfg.shuffle = toggled; }
void mainwin_repeat_pushed(gboolean toggled) { xmms_log("Repeat toggled: %d", toggled); cfg.repeat = toggled; }

void mainwin_vis_set_type(InputVisType mode) { cfg.vis_type = mode; }

void draw_main_window(gboolean force)
{
    if (!mainwin_bg) return;

    cairo_t *cr = cairo_create(mainwin_bg);
    
    /* Fill with solid black background first to kill transparency */
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    /* Draw the main skin background */
    skin_draw_pixmap(cr, SKIN_MAIN, 0, 0, 0, 0, 275, cfg.player_shaded ? 14 : 116);
    
    lock_widget_list(mainwin_wlist);
    GList *wl = mainwin_wlist;
    while (wl)
    {
        Widget *w = (Widget *)wl->data;
        /* For the background surface, we only draw if forced or marked for redraw */
        if (w && w->visible && (w->redraw || force) && w->draw) {
            w->draw(w, cr);
            w->redraw = FALSE;
        }
        wl = wl->next;
    }
    unlock_widget_list(mainwin_wlist);

    cairo_destroy(cr);
    
    if (mainwin) gtk_widget_queue_draw(mainwin);
}

char *xmms_get_gentitle_format(void) { return "%p - %t"; }
void xmms_usleep(gint usec) { g_usleep(usec); }

void mainwin_adjust_volume_motion(gint pos) { xmms_log("Volume adjust motion: %d", pos); }
void mainwin_adjust_volume_release(void) { xmms_log("Volume adjust release"); }
void mainwin_adjust_balance_motion(gint pos) { xmms_log("Balance adjust motion: %d", pos); }
void mainwin_adjust_balance_release(void) { xmms_log("Balance adjust release"); }
void mainwin_set_volume_slider(gint volume) {}
void mainwin_set_balance_slider(gint balance) {}
void mainwin_set_balance_diff(gint diff) {}

void equalizerwin_load_auto_preset(gchar *filename) {}
void equalizerwin_presets_menu_cb(gpointer cb_data, guint action, GtkWidget *w) {}

/* Callbacks */
static void mainwin_mr_change(MenuRowItem item) { xmms_log("MenuRow item change: %d", item); }
static void mainwin_mr_release(MenuRowItem item) { xmms_log("MenuRow item release: %d", item); }

static gint mainwin_volume_framecb(gint pos) { return (pos * 27) / 100; }
static void mainwin_volume_motioncb(gint pos) { mainwin_adjust_volume_motion(pos); }
static void mainwin_volume_releasecb(gint pos) { mainwin_adjust_volume_release(); }
static gint mainwin_balance_framecb(gint pos) { return (pos * 27) / 100; }
static void mainwin_balance_motioncb(gint pos) { mainwin_adjust_balance_motion(pos); }
static void mainwin_balance_releasecb(gint pos) { mainwin_adjust_balance_release(); }
static void mainwin_position_motioncb(gint pos) { xmms_log("Position motion: %d", pos); }
static void mainwin_position_releasecb(gint pos) { xmms_log("Position release: %d", pos); }
static gint mainwin_spos_frame_cb(gint pos) { return pos; }
static void mainwin_spos_motion_cb(gint pos) { xmms_log("Shaded position motion: %d", pos); }
static void mainwin_spos_release_cb(gint pos) { xmms_log("Shaded position release: %d", pos); }

/* Drawing Callback for the Window */
static gboolean mainwin_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    cairo_save(cr);
    
    /* Scaling factor handled by GTK for sharpness, 
       but if window is 275 and we draw 275, it's correct. */

	if (mainwin_bg)
	{
		cairo_set_source_surface(cr, mainwin_bg, 0, 0);
		cairo_paint(cr);
	}

    /* We also draw widgets directly here to ensure they are updated 
       if they weren't rendered into the background surface yet */
    lock_widget_list(mainwin_wlist);
    GList *wl = mainwin_wlist;
    while (wl)
    {
        Widget *w = (Widget *)wl->data;
        if (w && w->visible && w->draw)
            w->draw(w, cr);
        wl = wl->next;
    }
    unlock_widget_list(mainwin_wlist);

    cairo_restore(cr);
	return TRUE;
}

static void mainwin_pl_toggle_cb(GtkCheckMenuItem *item, gpointer data)
{
    gboolean active = gtk_check_menu_item_get_active(item);
    if (active) playlistwin_real_show();
    else playlistwin_real_hide();
}

static void mainwin_eq_toggle_cb(GtkCheckMenuItem *item, gpointer data)
{
    gboolean active = gtk_check_menu_item_get_active(item);
    if (active) equalizerwin_real_show();
    else equalizerwin_real_hide();
}

static void mainwin_show_context_menu(GdkEventButton *event)
{
    GtkWidget *menu = gtk_menu_new();
    GtkWidget *item;

    item = gtk_menu_item_new_with_label("About XMMS...");
    g_signal_connect(item, "activate", G_CALLBACK(show_about_window), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());

    item = gtk_check_menu_item_new_with_label("Playlist Editor");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), cfg.playlist_visible);
    g_signal_connect(item, "toggled", G_CALLBACK(mainwin_pl_toggle_cb), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

    item = gtk_check_menu_item_new_with_label("Graphical EQ");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), cfg.equalizer_visible);
    g_signal_connect(item, "toggled", G_CALLBACK(mainwin_eq_toggle_cb), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());

    item = gtk_menu_item_new_with_label("Preferences...");
    g_signal_connect(item, "activate", G_CALLBACK(show_prefs_window), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

    item = gtk_menu_item_new_with_label("Exit");
    g_signal_connect(item, "activate", G_CALLBACK(mainwin_quit_cb), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

    gtk_widget_show_all(menu);
    gtk_menu_popup_at_pointer(GTK_MENU(menu), (GdkEvent *)event);
}

static gboolean mainwin_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->button == 3) {
        mainwin_show_context_menu(event);
        return TRUE;
    }

    /* First try custom widgets */
    gboolean hit = handle_press_cb(widget, event, &mainwin_wlist);

    /* If no widget was hit, allow window dragging if it's a left click on the background */
    if (!hit && event->button == 1) {
        xmms_log("Starting window drag...");
        gtk_window_begin_move_drag(GTK_WINDOW(mainwin), event->button, event->x_root, event->y_root, event->time);
    } else if (hit) {
        xmms_log("Widget hit - bypassing window drag");
    }
    
    return TRUE;
}

static void mainwin_create_widgets(void)
{
    xmms_log("Creating main window widgets...");
	mainwin_menubtn = create_pbutton(&mainwin_wlist, mainwin_bg, 6, 3, 9, 9, 0, 0, 0, 9, mainwin_menubtn_cb, SKIN_TITLEBAR);
	mainwin_minimize = create_pbutton(&mainwin_wlist, mainwin_bg, 244, 3, 9, 9, 9, 0, 9, 9, mainwin_minimize_cb, SKIN_TITLEBAR);
	mainwin_shade = create_pbutton(&mainwin_wlist, mainwin_bg, 254, 3, 9, 9, 0, cfg.player_shaded ? 27 : 18, 9, cfg.player_shaded ? 27 : 18, mainwin_shade_cb, SKIN_TITLEBAR);
	mainwin_close = create_pbutton(&mainwin_wlist, mainwin_bg, 264, 3, 9, 9, 18, 0, 18, 9, mainwin_quit_cb, SKIN_TITLEBAR);

	mainwin_rew = create_pbutton(&mainwin_wlist, mainwin_bg, 16, 88, 23, 18, 0, 0, 0, 18, (void*)playlist_prev, SKIN_CBUTTONS);
	mainwin_play = create_pbutton(&mainwin_wlist, mainwin_bg, 39, 88, 23, 18, 23, 0, 23, 18, mainwin_play_pushed, SKIN_CBUTTONS);
	mainwin_pause = create_pbutton(&mainwin_wlist, mainwin_bg, 62, 88, 23, 18, 46, 0, 46, 18, (void*)input_pause, SKIN_CBUTTONS);
	mainwin_stop = create_pbutton(&mainwin_wlist, mainwin_bg, 85, 88, 23, 18, 69, 0, 69, 18, mainwin_stop_pushed, SKIN_CBUTTONS);
	mainwin_fwd = create_pbutton(&mainwin_wlist, mainwin_bg, 108, 88, 22, 18, 92, 0, 92, 18, (void*)playlist_next, SKIN_CBUTTONS);
	mainwin_eject = create_pbutton(&mainwin_wlist, mainwin_bg, 136, 89, 22, 16, 114, 0, 114, 16, mainwin_eject_pushed, SKIN_CBUTTONS);

	mainwin_srew = create_sbutton(&mainwin_wlist, mainwin_bg, 169, 4, 8, 7, 0, 0, SKIN_TITLEBAR, (void*)playlist_prev);
	mainwin_splay = create_sbutton(&mainwin_wlist, mainwin_bg, 177, 4, 10, 7, 0, 0, SKIN_TITLEBAR, mainwin_play_pushed);
	mainwin_spause = create_sbutton(&mainwin_wlist, mainwin_bg, 187, 4, 10, 7, 0, 0, SKIN_TITLEBAR, (void*)input_pause);
	mainwin_sstop = create_sbutton(&mainwin_wlist, mainwin_bg, 197, 4, 9, 7, 0, 0, SKIN_TITLEBAR, mainwin_stop_pushed);
	mainwin_sfwd = create_sbutton(&mainwin_wlist, mainwin_bg, 206, 4, 8, 7, 0, 0, SKIN_TITLEBAR, (void*)playlist_next);
	mainwin_seject = create_sbutton(&mainwin_wlist, mainwin_bg, 216, 4, 9, 7, 0, 0, SKIN_TITLEBAR, mainwin_eject_pushed);

	mainwin_shuffle = create_tbutton(&mainwin_wlist, mainwin_bg, 164, 89, 46, 15, 28, 0, 28, 15, 28, 30, 28, 45, mainwin_shuffle_pushed, SKIN_SHUFREP);
	mainwin_repeat = create_tbutton(&mainwin_wlist, mainwin_bg, 210, 89, 28, 15, 0, 0, 0, 15, 0, 30, 0, 45, mainwin_repeat_pushed, SKIN_SHUFREP);

	mainwin_eq = create_tbutton(&mainwin_wlist, mainwin_bg, 219, 58, 23, 12, 0, 61, 46, 61, 0, 73, 46, 73, (void*)equalizerwin_show, SKIN_SHUFREP);
	mainwin_pl = create_tbutton(&mainwin_wlist, mainwin_bg, 242, 58, 23, 12, 23, 61, 69, 61, 23, 73, 69, 73, mainwin_pl_pushed, SKIN_SHUFREP);

	mainwin_info = create_textbox(&mainwin_wlist, mainwin_bg, 112, 27, 153, 1, SKIN_TEXT);
	mainwin_rate_text = create_textbox(&mainwin_wlist, mainwin_bg, 111, 43, 15, 0, SKIN_TEXT);
	mainwin_freq_text = create_textbox(&mainwin_wlist, mainwin_bg, 156, 43, 10, 0, SKIN_TEXT);

	mainwin_menurow = create_menurow(&mainwin_wlist, mainwin_bg, 10, 22, 304, 0, 304, 44, mainwin_mr_change, mainwin_mr_release, SKIN_TITLEBAR);

	mainwin_volume = create_hslider(&mainwin_wlist, mainwin_bg, 107, 57, 68, 13, 15, 422, 0, 422, 14, 11, 15, 0, 0, 51, mainwin_volume_framecb, mainwin_volume_motioncb, mainwin_volume_releasecb, SKIN_VOLUME);
	mainwin_balance = create_hslider(&mainwin_wlist, mainwin_bg, 177, 57, 38, 13, 15, 422, 0, 422, 14, 11, 15, 9, 0, 24, mainwin_balance_framecb, mainwin_balance_motioncb, mainwin_balance_releasecb, SKIN_BALANCE);

	mainwin_monostereo = create_monostereo(&mainwin_wlist, mainwin_bg, 212, 41, SKIN_MONOSTEREO);
	mainwin_playstatus = create_playstatus(&mainwin_wlist, mainwin_bg, 24, 28);

	mainwin_minus_num = create_number(&mainwin_wlist, mainwin_bg, 36, 26, SKIN_NUMBERS);
	mainwin_10min_num = create_number(&mainwin_wlist, mainwin_bg, 48, 26, SKIN_NUMBERS);
	mainwin_min_num = create_number(&mainwin_wlist, mainwin_bg, 60, 26, SKIN_NUMBERS);
	mainwin_10sec_num = create_number(&mainwin_wlist, mainwin_bg, 78, 26, SKIN_NUMBERS);
	mainwin_sec_num = create_number(&mainwin_wlist, mainwin_bg, 90, 26, SKIN_NUMBERS);

	mainwin_about = create_sbutton(&mainwin_wlist, mainwin_bg, 247, 83, 20, 25, 0, 0, SKIN_TITLEBAR, (void*)show_about_window);

	mainwin_vis = create_vis(&mainwin_wlist, mainwin_bg, NULL, 24, 43, 76, cfg.doublesize);
	mainwin_svis = create_svis(&mainwin_wlist, mainwin_bg, 79, 5);
	active_vis = (Vis *)mainwin_vis;

	mainwin_position = create_hslider(&mainwin_wlist, mainwin_bg, 16, 72, 248, 10, 248, 0, 278, 0, 29, 10, 10, 0, 0, 219, NULL, mainwin_position_motioncb, mainwin_position_releasecb, SKIN_POSBAR);
	mainwin_sposition = create_hslider(&mainwin_wlist, mainwin_bg, 226, 4, 17, 7, 17, 36, 17, 36, 3, 7, 36, 0, 1, 13, mainwin_spos_frame_cb, mainwin_spos_motion_cb, mainwin_spos_release_cb, SKIN_TITLEBAR);

	mainwin_stime_min = create_textbox(&mainwin_wlist, mainwin_bg, 130, 4, 15, FALSE, SKIN_TEXT);
	mainwin_stime_sec = create_textbox(&mainwin_wlist, mainwin_bg, 147, 4, 10, FALSE, SKIN_TEXT);

	if (!cfg.player_shaded)
	{
		hide_widget(mainwin_svis);
		hide_widget(mainwin_srew);
		hide_widget(mainwin_splay);
		hide_widget(mainwin_spause);
		hide_widget(mainwin_sstop);
		hide_widget(mainwin_sfwd);
		hide_widget(mainwin_seject);
		hide_widget(mainwin_stime_min);
		hide_widget(mainwin_stime_sec);
        hide_widget(mainwin_sposition);
	}
    xmms_log("Widgets created.");
}

static void mainwin_create_menubar(GtkWidget *vbox)
{
    GtkWidget *menubar = gtk_menu_bar_new();
    
    /* File Menu */
    GtkWidget *file_menu = gtk_menu_new();
    GtkWidget *file_item = gtk_menu_item_new_with_mnemonic("_File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    
    GtkWidget *open_item = gtk_menu_item_new_with_label("Play File...");
    g_signal_connect(open_item, "activate", G_CALLBACK(mainwin_eject_pushed), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_item);
    
    GtkWidget *quit_item = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(quit_item, "activate", G_CALLBACK(mainwin_quit_cb), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);
    
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_item);
    
    /* View Menu */
    GtkWidget *view_menu = gtk_menu_new();
    GtkWidget *view_item = gtk_menu_item_new_with_mnemonic("_View");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_item), view_menu);
    
    GtkWidget *pref_item = gtk_menu_item_new_with_label("Preferences...");
    g_signal_connect(pref_item, "activate", G_CALLBACK(show_prefs_window), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), pref_item);
    
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), view_item);

#ifdef HAVE_GTK_MAC
    GtkosxApplication *theApp = g_object_new(GTKOSX_TYPE_APPLICATION, NULL);
    gtkosx_application_set_menu_bar(theApp, GTK_MENU_SHELL(menubar));
    gtk_widget_hide(menubar);
    gtkosx_application_ready(theApp);
#else
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
#endif
}

static void mainwin_create_gtk(void)
{
    xmms_log("Creating GTK window...");
	mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_app_paintable(mainwin, TRUE);
	gtk_window_set_title(GTK_WINDOW(mainwin), "XMMS");
    gtk_window_set_resizable(GTK_WINDOW(mainwin), FALSE);
    
    /* On macOS, if we want a global menu bar, the window shouldn't be undecorated 
       if we want it to feel like a normal app, but XMMS is skin-based. 
       Actually, GTK on macOS will use the menu bar regardless. */
    gtk_window_set_decorated(GTK_WINDOW(mainwin), FALSE);

    /* Get HiDPI scale factor */
    scaling_factor = gtk_widget_get_scale_factor(mainwin);
    xmms_log("System scaling factor detected: %d", scaling_factor);

    gint w = 275, h = 116;
    if (cfg.doublesize) { w *= 2; h *= 2; }
    
    /* We need a vbox to hold the menu bar (which will be hidden on macOS/Quartz 
       but used to populate the system menu bar) and the drawing area */
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(mainwin), vbox);
    
    mainwin_create_menubar(vbox);
    
    /* Create a drawing area for the skinned UI */
    GtkWidget *da = gtk_drawing_area_new();
    gtk_widget_set_size_request(da, w, h);
    gtk_box_pack_start(GTK_BOX(vbox), da, TRUE, TRUE, 0);
    
	g_signal_connect(da, "draw", G_CALLBACK(mainwin_draw_cb), NULL);
	g_signal_connect(mainwin, "destroy", G_CALLBACK(mainwin_quit_cb), NULL);

    /* Events for custom widgets - connect to the drawing area now */
    gtk_widget_add_events(da, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);
    
    g_signal_connect(da, "button-press-event", G_CALLBACK(mainwin_button_press), NULL);
    g_signal_connect(da, "button-release-event", G_CALLBACK(handle_release_cb), &mainwin_wlist);
    g_signal_connect(da, "motion-notify-event", G_CALLBACK(handle_motion_cb), &mainwin_wlist);

	gtk_widget_realize(mainwin);
    xmms_log("GTK window realized.");
}

void mainwin_set_shape_mask(void)
{
    cairo_region_t *mask = skin_get_mask(SKIN_MASK_MAIN, cfg.doublesize, cfg.player_shaded);
    if (mask) {
        gtk_widget_shape_combine_region(mainwin, mask);
    }
}

void mainwin_create(void)
{
	mainwin_bg = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 275, 116);
	mainwin_create_widgets();
	mainwin_create_gtk();
    draw_main_window(TRUE);
    mainwin_set_shape_mask();
}

/* Stubs */
void mainwin_show(gboolean show) 
{ 
    if (show) {
        gtk_widget_show_all(mainwin);
        gtk_window_present(GTK_WINDOW(mainwin));
    } else {
        gtk_widget_hide(mainwin); 
    }
}
void mainwin_set_info_text(void) { if (mainwin_info) textbox_set_text(mainwin_info, "XMMS Modernized"); }
void mainwin_set_song_info(int bitrate_val, int freq_val, int nch_val) 
{ 
    bitrate = bitrate_val;
    frequency = freq_val;
    numchannels = nch_val;
    
    if (mainwin_rate_text) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", bitrate);
        textbox_set_text(mainwin_rate_text, buf);
    }
    if (mainwin_freq_text) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", frequency / 1000);
        textbox_set_text(mainwin_freq_text, buf);
    }
    if (mainwin_monostereo) {
        monostereo_set_num_channels(mainwin_monostereo, numchannels);
    }
}

static gboolean mainwin_update_timer(gpointer data)
{
    static gint last_time = -1;
    gint time = input_get_time();
    
    if (time != last_time) {
        if (time >= 0) {
            gint sec = (time / 1000) % 60;
            gint min = (time / 1000) / 60;
            
            number_set_number(mainwin_10min_num, min / 10);
            number_set_number(mainwin_min_num, min % 10);
            number_set_number(mainwin_10sec_num, sec / 10);
            number_set_number(mainwin_sec_num, sec % 10);
            number_set_number(mainwin_minus_num, 10); /* Blank */
        } else {
            number_set_number(mainwin_10min_num, 10);
            number_set_number(mainwin_min_num, 10);
            number_set_number(mainwin_10sec_num, 10);
            number_set_number(mainwin_sec_num, 10);
            number_set_number(mainwin_minus_num, 10);
        }
        last_time = time;
    }

    if (get_input_playing()) {
        if (get_input_paused())
            playstatus_set_status(mainwin_playstatus, STATUS_PAUSE);
        else
            playstatus_set_status(mainwin_playstatus, STATUS_PLAY);
    } else {
        playstatus_set_status(mainwin_playstatus, STATUS_STOP);
    }

    return TRUE;
}
void mainwin_set_volume_diff(int diff) {}
void mainwin_set_volume(int vol) {}
void mainwin_set_balance(int bal) {}
void mainwin_set_always_on_top(gboolean always) {}
void mainwin_set_shade(gboolean shaded) {}
void mainwin_set_back_pixmap(void) {}
void mainwin_disable_seekbar(void) {}
void mainwin_lock_info_text(char *text) {}
void mainwin_release_info_text(void) {}

/* Main Entry Point */
int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);
    xmms_log("XMMS Starting main...");

    /* Ensure config directory exists before we do anything else */
    g_mkdir_with_parents(xmms_get_config_dir(), 0755);

    load_config();
    xmms_log("Config loaded.");

	init_skins();
    xmms_log("Skins initialized.");
    
    init_plugins();
    xmms_log("Plugins initialized.");
    
	mainwin_create();
    xmms_log("Main window created.");

    playlistwin_create();
    xmms_log("Playlist window created.");

    equalizerwin_create();
    xmms_log("Equalizer window created.");
    
    mainwin_set_info_text();

    /* Show windows first, THEN move them. 
       This often works better with modern window managers/GTK on macOS. */
    mainwin_show(TRUE);
    if (cfg.playlist_visible) playlistwin_real_show();
    if (cfg.equalizer_visible) equalizerwin_real_show();

    /* Center windows on first run or if requested */
    if (cfg.player_x == -1 || cfg.player_y == -1) {
        GdkDisplay *display = gdk_display_get_default();
        GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
        if (monitor) {
            GdkRectangle geo;
            gdk_monitor_get_geometry(monitor, &geo);
            
            gint win_w = 275, win_h = 116;
            if (cfg.doublesize) { win_w *= 2; win_h *= 2; }
            
            cfg.player_x = geo.x + (geo.width - win_w) / 2;
            cfg.player_y = geo.y + (geo.height - (win_h + 116 + 116)) / 2;
            
            cfg.playlist_x = cfg.player_x;
            cfg.playlist_y = cfg.player_y + win_h;
            
            cfg.equalizer_x = cfg.player_x;
            cfg.equalizer_y = cfg.playlist_y + 116;
            
            xmms_log("First run: Centering windows at %d, %d (Monitor: %dx%d)", cfg.player_x, cfg.player_y, geo.width, geo.height);
        }
    }

    if (cfg.player_x != -1) {
        gtk_window_move(GTK_WINDOW(mainwin), cfg.player_x, cfg.player_y);
    }
    if (cfg.playlist_visible && cfg.playlist_x != -1) {
        gtk_window_move(GTK_WINDOW(playlistwin), cfg.playlist_x, cfg.playlist_y);
    }
    if (cfg.equalizer_visible && cfg.equalizer_x != -1) {
        gtk_window_move(GTK_WINDOW(equalizerwin), cfg.equalizer_x, cfg.equalizer_y);
    }

    /* 100ms UI update timer */
    g_timeout_add(100, mainwin_update_timer, NULL);
    
    /* 3-minute periodic save timer (180,000 ms) */
    g_timeout_add(180000, periodic_save_timer, NULL);

    xmms_log("Entering gtk_main()...");
	gtk_main();
	return 0;
}
