/*  XMMS - Cross-platform multimedia player
 *  Copyright (C) 1998-2001  Peter Alm, Mikael Alm, Olle Hallnas, Thomas Nilsson and 4Front Technologies
 *  Copyright (C) 1999-2001  Haavard Kvaalen
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "xmms.h"
#include "libxmms/configfile.h"

GtkWidget *equalizerwin;

static GtkWidget *equalizerwin_load_window = NULL;
static GtkWidget *equalizerwin_load_auto_window = NULL;
static GtkWidget *equalizerwin_save_window = NULL, *equalizerwin_save_entry;
static GtkWidget *equalizerwin_save_auto_window = NULL, *equalizerwin_save_auto_entry;
static GtkWidget *equalizerwin_delete_window = NULL;
static GtkWidget *equalizerwin_delete_auto_window = NULL;
static GtkWidget *equalizerwin_configure_window = NULL;

static GtkWidget *eqconfwin_options_eqdf_entry, *eqconfwin_options_eqef_entry;

cairo_surface_t *equalizerwin_bg, *equalizerwin_bg_dblsize;
cairo_t *equalizerwin_gc;

GList *equalizerwin_wlist = NULL;

GtkAccelGroup *equalizerwin_accel;

static TButton *equalizerwin_on, *equalizerwin_auto;
extern TButton *mainwin_eq;
static PButton *equalizerwin_presets, *equalizerwin_shade;
PButton *equalizerwin_close;
static EqGraph *equalizerwin_graph;
static EqSlider *equalizerwin_preamp, *equalizerwin_bands[10];
static HSlider *equalizerwin_volume, *equalizerwin_balance;

static GtkActionEntry *equalizerwin_presets_menu;

gboolean equalizerwin_focus = FALSE;

typedef struct
{
	gchar *name;
	gfloat preamp, bands[10];
}
EqualizerPreset;

static GList *equalizer_presets = NULL, *equalizer_auto_presets = NULL;

void equalizerwin_presets_menu_cb(gpointer cb_data, guint action, GtkWidget * w);
GtkWidget * equalizerwin_create_conf_window(void);

enum
{
	EQUALIZER_PRESETS_LOAD_PRESET, EQUALIZER_PRESETS_LOAD_AUTOPRESET,
	EQUALIZER_PRESETS_LOAD_DEFAULT,	EQUALIZER_PRESETS_LOAD_ZERO,
	EQUALIZER_PRESETS_LOAD_FROM_FILE, EQUALIZER_PRESETS_LOAD_FROM_WINAMPFILE,
	EQUALIZER_PRESETS_IMPORT_WINAMPFILE,
	EQUALIZER_PRESETS_SAVE_PRESET, EQUALIZER_PRESETS_SAVE_AUTOPRESET,
	EQUALIZER_PRESETS_SAVE_DEFAULT,	EQUALIZER_PRESETS_SAVE_TO_FILE,
	EQUALIZER_PRESETS_SAVE_TO_WINAMPFILE, EQUALIZER_PRESETS_DELETE_PRESET,
	EQUALIZER_PRESETS_DELETE_AUTOPRESET, EQUALIZER_PRESETS_CONFIGURE
};

GtkItemFactoryEntry equalizerwin_presets_menu_entries[] =
{
	{N_("/Load"), NULL, NULL, 0, "<Branch>"},
	{N_("/Load/Preset"), NULL, equalizerwin_presets_menu_cb, EQUALIZER_PRESETS_LOAD_PRESET, "<Item>"},
	{N_("/Load/Auto-load preset"), NULL, equalizerwin_presets_menu_cb, EQUALIZER_PRESETS_LOAD_AUTOPRESET, "<Item>"},
	{N_("/Load/Default"), NULL, equalizerwin_presets_menu_cb, EQUALIZER_PRESETS_LOAD_DEFAULT, "<Item>"},
	{N_("/Load/-"), NULL, NULL, 0, "<Separator>"},
	{N_("/Load/Zero"), NULL, equalizerwin_presets_menu_cb, EQUALIZER_PRESETS_LOAD_ZERO, "<Item>"},
	{N_("/Load/-"), NULL, NULL, 0, "<Separator>"},
	{N_("/Load/From file"), NULL, equalizerwin_presets_menu_cb, EQUALIZER_PRESETS_LOAD_FROM_FILE, "<Item>"},
	{N_("/Load/From WinAMP EQF file"), NULL, equalizerwin_presets_menu_cb, EQUALIZER_PRESETS_LOAD_FROM_WINAMPFILE, "<Item>"},
	{N_("/Import"), NULL, NULL, 0, "<Branch>"},
	{N_("/Import/WinAMP Presets"), NULL, equalizerwin_presets_menu_cb, EQUALIZER_PRESETS_IMPORT_WINAMPFILE, "<Item>"},
	{N_("/Save"), NULL, NULL, 0, "<Branch>"},
	{N_("/Save/Preset"), NULL, equalizerwin_presets_menu_cb, EQUALIZER_PRESETS_SAVE_PRESET, "<Item>"},
	{N_("/Save/Auto-load preset"), NULL, equalizerwin_presets_menu_cb, EQUALIZER_PRESETS_SAVE_AUTOPRESET, "<Item>"},
	{N_("/Save/Default"), NULL, equalizerwin_presets_menu_cb, EQUALIZER_PRESETS_SAVE_DEFAULT, "<Item>"},
	{N_("/Save/-"), NULL, NULL, 0, "<Separator>"},
	{N_("/Save/To file"), NULL, equalizerwin_presets_menu_cb, EQUALIZER_PRESETS_SAVE_TO_FILE, "<Item>"},
	{N_("/Save/To WinAMP EQF file"), NULL, equalizerwin_presets_menu_cb, EQUALIZER_PRESETS_SAVE_TO_WINAMPFILE, "<Item>"},
	{N_("/Delete"), NULL, NULL, 0, "<Branch>"},
	{N_("/Delete/Preset"), NULL, equalizerwin_presets_menu_cb, EQUALIZER_PRESETS_DELETE_PRESET, "<Item>"},
	{N_("/Delete/Auto-load preset"), NULL, equalizerwin_presets_menu_cb, EQUALIZER_PRESETS_DELETE_AUTOPRESET, "<Item>"},
	{N_("/Configure Equalizer"), NULL, equalizerwin_presets_menu_cb, EQUALIZER_PRESETS_CONFIGURE, "<Item>"},
};

static gint equalizerwin_presets_menu_entries_num = 
	sizeof(equalizerwin_presets_menu_entries) / 
	sizeof(equalizerwin_presets_menu_entries[0]);

void equalizerwin_set_shape_mask(void)
{
	if (cfg.show_wm_decorations)
		return;

	gtk_widget_shape_combine_region(equalizerwin, skin_get_mask(SKIN_MASK_EQ, EQUALIZER_DOUBLESIZE, cfg.equalizer_shaded));
}

void equalizerwin_set_doublesize(gboolean ds)
{
	gint height;
	
	if(cfg.equalizer_shaded)
		height = 14;
	else
		height = 116;

	equalizerwin_set_shape_mask();

	if (ds)
	{
		dock_resize(dock_window_list, equalizerwin, 550, height * 2);
		
	}
	else
	{
		dock_resize(dock_window_list, equalizerwin, 275, height);
		
	}
	draw_equalizer_window(TRUE);
}

void equalizerwin_set_shade(gboolean shaded)
{
	cfg.equalizer_shaded = shaded;

	equalizerwin_set_shape_mask();
	
	if (shaded)
	{
		dock_shade(dock_window_list, equalizerwin, 14 * (EQUALIZER_DOUBLESIZE + 1));
		pbutton_set_button_data(equalizerwin_shade, -1, 3, -1, 47);
		pbutton_set_skin_index1(equalizerwin_shade, SKIN_EQ_EX);
		pbutton_set_button_data(equalizerwin_close, 11, 38, 11, 47);
		pbutton_set_skin_index(equalizerwin_close, SKIN_EQ_EX);
		show_widget(equalizerwin_volume);
		show_widget(equalizerwin_balance);
	}
	else
	{
		dock_shade(dock_window_list, equalizerwin, 116 * (EQUALIZER_DOUBLESIZE + 1));
		pbutton_set_button_data(equalizerwin_shade, -1, 137, -1, 38);
		pbutton_set_skin_index1(equalizerwin_shade, SKIN_EQMAIN);
		pbutton_set_button_data(equalizerwin_close, 0, 116, 0, 125);
		pbutton_set_skin_index(equalizerwin_close, SKIN_EQMAIN);
		hide_widget(equalizerwin_volume);
		hide_widget(equalizerwin_balance);
	}
	
	draw_equalizer_window(TRUE);
}

void equalizerwin_shade_cb(void)
{
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_item_factory_get_widget(mainwin_options_menu, "/Equalizer WindowShade Mode")), !cfg.equalizer_shaded);
}

void equalizerwin_raise(void)
{
	if (cfg.equalizer_visible)
		gdk_window_raise(gtk_widget_get_window(equalizerwin));
}

void equalizerwin_eq_changed(void)
{
	int i;

	cfg.equalizer_preamp = eqslider_get_position(equalizerwin_preamp);
	for (i = 0; i < 10; i++)
		cfg.equalizer_bands[i] = eqslider_get_position(equalizerwin_bands[i]);
	input_set_eq(cfg.equalizer_active, cfg.equalizer_preamp, cfg.equalizer_bands);
	draw_widget(equalizerwin_graph);
}

void equalizerwin_on_pushed(gboolean toggled)
{
	cfg.equalizer_active = toggled;
	equalizerwin_eq_changed();
}

void equalizerwin_presets_pushed(void)
{
	GdkModifierType modmask;
	gint x, y;

	/* pointer handled by GTK3 */ x=0; y=0;
	util_item_factory_popup(equalizerwin_presets_menu, x, y, 1, GDK_CURRENT_TIME);
}

void equalizerwin_auto_pushed(gboolean toggled)
{
	cfg.equalizer_autoload = toggled;
}

void draw_equalizer_window(gboolean force)
{
	cairo_t *cr = cairo_create(EQUALIZER_DOUBLESIZE ? equalizerwin_bg_dblsize : equalizerwin_bg);
    
    /* Ensure background is opaque black */
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

	GtkImage *img, *img2;
	GList *wl;
	Widget *w;
	gboolean redraw;

	lock_widget_list(equalizerwin_wlist);
	if (force)
	{
		skin_draw_pixmap(cr, SKIN_EQMAIN,
				 0, 0, 0, 0, 275, 116);
		if (equalizerwin_focus || !cfg.dim_titlebar)
		{
			if (!cfg.equalizer_shaded)
				skin_draw_pixmap(cr,
						 SKIN_EQMAIN, 0, 134, 0, 0, 275, 14);
			else
				skin_draw_pixmap(cr,
						 SKIN_EQ_EX, 0, 0, 0, 0, 275, 14);
		}
		else
		{
			if(!cfg.equalizer_shaded)
				skin_draw_pixmap(cr,
						 SKIN_EQMAIN, 0, 149, 0, 0, 275, 14);
			else
				skin_draw_pixmap(cr,
						 SKIN_EQ_EX, 0, 15, 0, 0, 275, 14);

		}
		draw_widget_list(equalizerwin_wlist, cr, &redraw, TRUE);
	}
	else
		draw_widget_list(equalizerwin_wlist, cr, &redraw, FALSE);

	if (force || redraw)
	{
		if (cfg.doublesize && cfg.eq_doublesize_linked)
		{
			if (force)
			{
				/* img stub */
				/* img2 stub */
				/* draw_image stub */
				/* image_destroy stub */
				/* image_destroy stub */
			}
			else
			{
				wl = equalizerwin_wlist;
				while (wl)
				{
					w = (Widget *) wl->data;
					if (w->redraw && w->visible)
					{
						/* img stub */
						/* img2 stub */
						/* draw_image stub */
						/* image_destroy stub */
						/* image_destroy stub */
						w->redraw = FALSE;
					}
					wl = wl->next;
				}
			}
		}
		else
			clear_widget_list_redraw(equalizerwin_wlist);
		
		gdk_display_flush(gdk_display_get_default());
	}
	unlock_widget_list(equalizerwin_wlist);
	cairo_destroy(cr);
}

static gboolean inside_sensitive_widgets(gint x, gint y)
{
	return (inside_widget(x, y, equalizerwin_on) ||
		inside_widget(x, y, equalizerwin_auto) ||
		inside_widget(x, y, equalizerwin_presets) ||
		inside_widget(x, y, equalizerwin_close) ||
		inside_widget(x, y, equalizerwin_shade) ||
		inside_widget(x, y, equalizerwin_preamp) ||
		inside_widget(x, y, equalizerwin_bands[0]) ||
		inside_widget(x, y, equalizerwin_bands[1]) ||
		inside_widget(x, y, equalizerwin_bands[2]) ||
		inside_widget(x, y, equalizerwin_bands[3]) ||
		inside_widget(x, y, equalizerwin_bands[4]) ||
		inside_widget(x, y, equalizerwin_bands[5]) ||
		inside_widget(x, y, equalizerwin_bands[6]) ||
		inside_widget(x, y, equalizerwin_bands[7]) ||
		inside_widget(x, y, equalizerwin_bands[8]) ||
		inside_widget(x, y, equalizerwin_bands[9]) ||
		inside_widget(x, y, equalizerwin_volume) ||
		inside_widget(x, y, equalizerwin_balance));
}

void equalizerwin_press(GtkWidget * widget, GdkEventButton * event, gpointer callback_data)
{
	gint mx, my;
	gboolean grab = TRUE;

	mx = event->x;
	my = event->y;
	if (cfg.doublesize && cfg.eq_doublesize_linked)
	{
		event->x /= 2;
		event->y /= 2;
	}

	if (event->button == 1 && event->type == GDK_BUTTON_PRESS &&
	    ((cfg.easy_move || cfg.equalizer_shaded || event->y < 14) &&
	     !inside_sensitive_widgets(event->x, event->y)))
	{

		equalizerwin_raise();
		dock_move_press(dock_window_list, equalizerwin, event, FALSE);
	}
	else if (event->button == 1 && event->type == GDK_2BUTTON_PRESS && event->y < 14)
	{
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_item_factory_get_widget(mainwin_options_menu, "/Equalizer WindowShade Mode")), !cfg.equalizer_shaded);
		if(dock_is_moving(equalizerwin))
			dock_move_release(equalizerwin);
	}
	else if (event->button == 3 &&
		 !(inside_widget(event->x, event->y, equalizerwin_on) ||
		   inside_widget(event->x, event->y, equalizerwin_auto)))
	{
		/*
		 * Pop up the main menu a few pixels down to avoid
		 * anything to be selected initially.
		 */
		util_item_factory_popup(mainwin_general_menu, event->x_root, event->y_root + 2, 3, event->time);
		grab = FALSE;
	}
	else
	{
		handle_press_cb(equalizerwin_wlist, widget, event);
		draw_equalizer_window(FALSE);
	}
	}

void equalizerwin_motion(GtkWidget * widget, GdkEventMotion * event, gpointer callback_data)
{
	

	if (cfg.doublesize && cfg.eq_doublesize_linked)
	{
		event->x /= 2;
		event->y /= 2;
	}
	if (dock_is_moving(equalizerwin))
	{
		dock_move_motion(equalizerwin, event);
	}
	else
	{
		handle_motion_cb(equalizerwin_wlist, widget, event);
		draw_main_window(FALSE);
	}
	gdk_display_flush(gdk_display_get_default());
	
}

void equalizerwin_release(GtkWidget * widget, GdkEventButton * event, gpointer callback_data)
{
	gdk_seat_ungrab(gdk_display_get_default_seat(gdk_display_get_default()));
	gdk_display_flush(gdk_display_get_default());
	if (dock_is_moving(equalizerwin))
	{
		dock_move_release(equalizerwin);
	}
	else
	{
		handle_release_cb(equalizerwin_wlist, widget, event);
		draw_equalizer_window(FALSE);
	}
}

void equalizerwin_focus_in(GtkWidget * widget, GdkEvent * event, gpointer callback_data)
{
	
	
	equalizerwin_focus = TRUE;
	draw_equalizer_window(TRUE);
}

void equalizerwin_focus_out(GtkWidget * widget, GdkEventButton * event, gpointer callback_data)
{
	
	
	equalizerwin_focus = FALSE;
	draw_equalizer_window(TRUE);
}

gboolean equalizerwin_keypress(GtkWidget * w, GdkEventKey * event, gpointer data)
{
	if(!cfg.equalizer_shaded)
	{
		gtk_widget_event(mainwin, (GdkEvent *) event);
		return TRUE;
	}
	
	switch(event->keyval)
	{
		case GDK_KEY_Left:
		case GDK_KEY_KP_Left:
			mainwin_set_balance_diff(-4);
			break;
		case GDK_KEY_Right:
		case GDK_KEY_KP_Right:
			mainwin_set_balance_diff(4);
			break;
		default:
			gtk_widget_event(mainwin, (GdkEvent *) event);
			break;
	}

	return TRUE;
}

static gboolean equalizerwin_configure(GtkWidget * window, GdkEventConfigure *event, gpointer data)
{
	if (!GTK_WIDGET_VISIBLE(window))
		return FALSE;

	if (cfg.show_wm_decorations)
		gdk_window_get_root_origin(gtk_widget_get_window(window),
					   &cfg.equalizer_x, &cfg.equalizer_y);
	else
		gdk_window_get_deskrelative_origin(gtk_widget_get_window(window),
						   &cfg.equalizer_x,
						   &cfg.equalizer_y);
	return FALSE;
}

void equalizerwin_set_back_pixmap(void) {}

gint equalizerwin_client_event(GtkWidget *w, GdkEvent *event, gpointer data)
{
	static GdkAtom atom_rcfiles = GDK_NONE;

	if (!atom_rcfiles)
		atom_rcfiles = gdk_atom_intern("_GTK_READ_RCFILES", FALSE);

	if(FALSE /* message_type stub */)
	{
		mainwin_set_back_pixmap();
		equalizerwin_set_back_pixmap();
		playlistwin_set_back_pixmap();
		return TRUE;
	}
	return FALSE;
}

void equalizerwin_close_cb(void)
{
	equalizerwin_show(FALSE);
}

int equalizerwin_delete(GtkWidget * w, gpointer data)
{
	equalizerwin_show(FALSE);
	return TRUE;
}

static GList *equalizerwin_read_presets(gchar * fname)
{
	gchar *filename, *name;
	ConfigFile *cfgfile;
	GList *list = NULL;
	gint i, p = 0;
	EqualizerPreset *preset;

	filename = g_strdup_printf("%s/.xmms/%s", g_get_home_dir(), fname);
	if ((cfgfile = xmms_cfg_open_file(filename)) == NULL)
	{
		g_free(filename);
		return NULL;
	}
	g_free(filename);

	for (;;)
	{
		gchar section[21];
		
		sprintf(section, "Preset%d", p++);
		if (xmms_cfg_read_string(cfgfile, "Presets", section, &name))
		{
			preset = g_malloc(sizeof (EqualizerPreset));
			preset->name = name;
			xmms_cfg_read_float(cfgfile, name, "Preamp",
					    &preset->preamp);
			for (i = 0; i < 10; i++)
			{
				gchar band[7];
				sprintf(band, "Band%d", i);
				xmms_cfg_read_float(cfgfile, name, band,
						    &preset->bands[i]);
			}
			list = g_list_prepend(list, preset);
		}
		else
			break;
	}
	list = g_list_reverse(list);
	xmms_cfg_free(cfgfile);
	return list;
}

gint equalizerwin_volume_frame_cb(gint pos)
{
	if(equalizerwin_volume)
	{
		if (pos < 32)
			equalizerwin_volume->hs_knob_nx = equalizerwin_volume->hs_knob_px = 1;
		else if (pos < 63)
			equalizerwin_volume->hs_knob_nx = equalizerwin_volume->hs_knob_px = 4;
		else
			equalizerwin_volume->hs_knob_nx = equalizerwin_volume->hs_knob_px = 7;
	}
	return 1;
}

void equalizerwin_volume_motion_cb(gint pos)
{
	gint v = (gint) rint(pos * 100 / 94.0);
	mainwin_adjust_volume_motion(v);
	mainwin_set_volume_slider(v);
}

void equalizerwin_volume_release_cb(gint pos)
{
	mainwin_adjust_volume_release();
}

gint equalizerwin_balance_frame_cb(gint pos)
{
	if(equalizerwin_balance)
	{
		if(pos < 13)
			equalizerwin_balance->hs_knob_nx = equalizerwin_balance->hs_knob_px = 11;
		else if (pos < 26)
			equalizerwin_balance->hs_knob_nx = equalizerwin_balance->hs_knob_px = 14;
		else
			equalizerwin_balance->hs_knob_nx = equalizerwin_balance->hs_knob_px = 17;
	}
			
	return 1;
}

void equalizerwin_balance_motion_cb(gint pos)
{
	gint b;
	pos = MIN(pos,38); /* The skin uses a even number of pixels
			      for the balance-slider *sigh* */
	b = (gint) rint((pos - 19) * 100 / 19.0);
	mainwin_adjust_balance_motion(b);
	mainwin_set_balance_slider(b);
}

void equalizerwin_balance_release_cb(gint pos)
{
	mainwin_adjust_balance_release();
}

void equalizerwin_set_balance_slider(gint percent)
{
	hslider_set_position(equalizerwin_balance, (gint) rint((percent*19/100.0)+19));
}

void equalizerwin_set_volume_slider(gint percent)
{
	hslider_set_position(equalizerwin_volume, (gint) rint(percent*94/100.0));
}

static void equalizerwin_create_widgets(void)
{
	int i;

	equalizerwin_on = create_tbutton(&equalizerwin_wlist, equalizerwin_bg, 14, 18, 25, 12, 10, 119, 128, 119, 69, 119, 187, 119, equalizerwin_on_pushed, SKIN_EQMAIN);
	tbutton_set_toggled(equalizerwin_on, cfg.equalizer_active);
	equalizerwin_auto = create_tbutton(&equalizerwin_wlist, equalizerwin_bg, 39, 18, 33, 12, 35, 119, 153, 119, 94, 119, 212, 119, equalizerwin_auto_pushed, SKIN_EQMAIN);
	tbutton_set_toggled(equalizerwin_auto, cfg.equalizer_autoload);
	equalizerwin_presets = create_pbutton(&equalizerwin_wlist, equalizerwin_bg, 217, 18, 44, 12, 224, 164, 224, 176, equalizerwin_presets_pushed, SKIN_EQMAIN);
	equalizerwin_close = create_pbutton(&equalizerwin_wlist, equalizerwin_bg, 264, 3, 9, 9, 0, 116, 0, 125, equalizerwin_close_cb, SKIN_EQMAIN);
	

	equalizerwin_shade = create_pbutton(&equalizerwin_wlist, equalizerwin_bg, 254, 3, 9, 9, 254, 137, 1, 38, equalizerwin_shade_cb, SKIN_EQMAIN);
	

	equalizerwin_graph = create_eqgraph(&equalizerwin_wlist, equalizerwin_bg, 86, 17);
	equalizerwin_preamp = create_eqslider(&equalizerwin_wlist, equalizerwin_bg, 21, 38);
	eqslider_set_position(equalizerwin_preamp, cfg.equalizer_preamp);
	for (i = 0; i < 10; i++)
	{
		equalizerwin_bands[i] = create_eqslider(&equalizerwin_wlist, equalizerwin_bg, 78 + (i * 18), 38);
		eqslider_set_position(equalizerwin_bands[i], cfg.equalizer_bands[i]);
	}

	equalizerwin_volume = create_hslider(&equalizerwin_wlist, equalizerwin_bg, 61, 4, 97, 8, 1, 30, 1, 30, 3, 7, 4, 61, 0, 94, equalizerwin_volume_frame_cb, equalizerwin_volume_motion_cb, equalizerwin_volume_release_cb, SKIN_EQ_EX);
	equalizerwin_balance = create_hslider(&equalizerwin_wlist, equalizerwin_bg, 164, 4, 42, 8, 11, 30, 11, 30, 3, 7, 4, 164, 0, 39, equalizerwin_balance_frame_cb, equalizerwin_balance_motion_cb, equalizerwin_balance_release_cb, SKIN_EQ_EX);

	if (!cfg.equalizer_shaded)
	{
		hide_widget(equalizerwin_volume);
		hide_widget(equalizerwin_balance);
	}
	else
	{
		pbutton_set_button_data(equalizerwin_shade, -1, 3, -1, 47);
		pbutton_set_skin_index1(equalizerwin_shade, SKIN_EQ_EX);
		pbutton_set_button_data(equalizerwin_close, 11, 38, 11, 47);
		pbutton_set_skin_index(equalizerwin_close, SKIN_EQ_EX);
	}


}


static gboolean equalizerwin_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	cairo_surface_t *bg = EQUALIZER_DOUBLESIZE ? equalizerwin_bg_dblsize : equalizerwin_bg;
    if (bg)
	{
		cairo_set_source_surface(cr, bg, 0, 0);
		cairo_paint(cr);
	}

    lock_widget_list(equalizerwin_wlist);
    GList *wl = equalizerwin_wlist;
    while (wl)
    {
        Widget *w = (Widget *)wl->data;
        if (w && w->visible && w->draw)
            w->draw(w, cr);
        wl = wl->next;
    }
    unlock_widget_list(equalizerwin_wlist);

	return TRUE;
}

static gboolean equalizerwin_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gboolean hit = handle_press_cb(widget, event, &equalizerwin_wlist);
    if (!hit && event->button == 1) {
        gtk_window_begin_move_drag(GTK_WINDOW(equalizerwin), event->button, event->x_root, event->y_root, event->time);
    }
    return TRUE;
}

static void equalizerwin_create_gtk(void)
{
	equalizerwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_app_paintable(equalizerwin, TRUE);
	gtk_window_set_title(GTK_WINDOW(equalizerwin), _("XMMS Equalizer"));
    gtk_window_set_decorated(GTK_WINDOW(equalizerwin), FALSE);
	gtk_window_set_transient_for(GTK_WINDOW(equalizerwin), GTK_WINDOW(mainwin));

	if (cfg.doublesize && cfg.eq_doublesize_linked)
		gtk_widget_set_size_request(equalizerwin, 550, (cfg.equalizer_shaded ? 28 : 232));
	else
		gtk_widget_set_size_request(equalizerwin, 275, (cfg.equalizer_shaded ? 14 : 116));

    GtkWidget *da = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(equalizerwin), da);

	g_signal_connect(da, "draw", G_CALLBACK(equalizerwin_draw_cb), NULL);
    g_signal_connect(da, "button-press-event", G_CALLBACK(equalizerwin_button_press), NULL);
    g_signal_connect(da, "button-release-event", G_CALLBACK(handle_release_cb), &equalizerwin_wlist);
    g_signal_connect(da, "motion-notify-event", G_CALLBACK(handle_motion_cb), &equalizerwin_wlist);

    gtk_widget_add_events(da, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);

	gtk_widget_realize(equalizerwin);
}

void equalizerwin_create(void)
{
	equalizerwin_presets_menu = gtk_item_factory_new(GTK_TYPE_MENU, "<Main>", equalizerwin_accel);
	gtk_item_factory_set_translate_func(equalizerwin_presets_menu,
					    util_menu_translate, NULL, NULL);
	gtk_item_factory_create_items(equalizerwin_presets_menu, equalizerwin_presets_menu_entries_num, equalizerwin_presets_menu_entries, NULL);
	equalizer_presets = equalizerwin_read_presets("eq.preset");
	equalizer_auto_presets = equalizerwin_read_presets("eq.auto_preset");

	equalizerwin_bg = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 275, 116);
	equalizerwin_bg_dblsize = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 550, 232);
	equalizerwin_create_gtk();
	equalizerwin_gc = NULL;
	equalizerwin_create_widgets();
}

void equalizerwin_recreate(void)
{
	dock_window_list = g_list_remove(dock_window_list, equalizerwin);
	gtk_widget_destroy(equalizerwin);
	equalizerwin_create_gtk();

	equalizerwin_set_shape_mask();
}

void equalizerwin_show(gboolean show)
{
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_item_factory_get_widget(mainwin_general_menu, "/Graphical EQ")), show);
}

void equalizerwin_real_show(void)
{
	/*
	 * This function should only be called from the
	 * main menu signal handler
	 */
	if (!pposition_broken && cfg.equalizer_x != -1 &&
	    cfg.save_window_position && cfg.show_wm_decorations)
		dock_set_uposition(equalizerwin, cfg.equalizer_x, cfg.equalizer_y);
	gtk_widget_show_all(equalizerwin);
    gtk_window_present(GTK_WINDOW(equalizerwin));
	if (pposition_broken && cfg.equalizer_x != -1 && cfg.save_window_position)
		dock_set_uposition(equalizerwin, cfg.equalizer_x, cfg.equalizer_y);
	if (cfg.doublesize && cfg.eq_doublesize_linked)
		gtk_widget_set_usize(equalizerwin, 550, (cfg.equalizer_shaded ? 28 : 232));
	else
		gtk_widget_set_usize(equalizerwin, 275, (cfg.equalizer_shaded ? 14 : 116));
	gdk_display_flush(gdk_display_get_default());
	draw_equalizer_window(TRUE);
	cfg.equalizer_visible = TRUE;
	tbutton_set_toggled(mainwin_eq, TRUE);
	hint_set_always(cfg.always_on_top);
	hint_set_sticky(cfg.sticky);
	hint_set_skip_winlist(equalizerwin);
}

void equalizerwin_real_hide(void)
{
	/*
	 * This function should only be called from the
	 * main menu signal handler
	 */
	gtk_widget_hide(equalizerwin);
	cfg.equalizer_visible = FALSE;
	tbutton_set_toggled(mainwin_eq, FALSE);
}

static EqualizerPreset *equalizerwin_find_preset(GList * list, gchar * name)
{
	GList *node = list;
	EqualizerPreset *preset;

	while (node)
	{
		preset = node->data;
		if (!strcasecmp(preset->name, name))
			return preset;
		node = g_list_next(node);
	}
	return NULL;
}

static void equalizerwin_write_preset_file(GList * list, gchar * fname)
{
	gchar *filename, *tmp;
	gint i, p;
	EqualizerPreset *preset;
	ConfigFile *cfgfile;
	GList *node;

	cfgfile = xmms_cfg_new();
	p = 0;
	node = list;
	while (node)
	{
		preset = node->data;
		tmp = g_strdup_printf("Preset%d", p++);
		xmms_cfg_write_string(cfgfile, "Presets", tmp, preset->name);
		g_free(tmp);
		xmms_cfg_write_float(cfgfile, preset->name, "Preamp", preset->preamp);
		for (i = 0; i < 10; i++)
		{
			tmp = g_strdup_printf("Band%d\n", i);
			xmms_cfg_write_float(cfgfile, preset->name, tmp, preset->bands[i]);
			g_free(tmp);
		}
		node = g_list_next(node);
	}
	filename = g_strdup_printf("%s/.xmms/%s", g_get_home_dir(), fname);
	xmms_cfg_write_file(cfgfile, filename);
	xmms_cfg_free(cfgfile);
	g_free(filename);
}

static gboolean equalizerwin_load_preset(GList * list, gchar * name)
{
	EqualizerPreset *preset;
	gint i;

	if ((preset = equalizerwin_find_preset(list, name)) != NULL)
	{
		eqslider_set_position(equalizerwin_preamp, preset->preamp);
		for (i = 0; i < 10; i++)
			eqslider_set_position(equalizerwin_bands[i], preset->bands[i]);
		equalizerwin_eq_changed();
		return TRUE;
	}
	return FALSE;
}

static GList *equalizerwin_save_preset(GList * list, gchar * name, gchar * fname)
{
	gint i;
	EqualizerPreset *preset;

	if (!(preset = equalizerwin_find_preset(list, name)))
	{
		preset = g_malloc(sizeof (EqualizerPreset));
		preset->name = g_strdup(name);
		list = g_list_append(list, preset);
	}

	preset->preamp = eqslider_get_position(equalizerwin_preamp);
	for (i = 0; i < 10; i++)
		preset->bands[i] = eqslider_get_position(equalizerwin_bands[i]);

	equalizerwin_write_preset_file(list, fname);

	return list;
}

static GList *equalizerwin_delete_preset(GList * list, gchar * name, gchar * fname)
{
	EqualizerPreset *preset;
	GList *node;

	if ((preset = equalizerwin_find_preset(list, name)) && (node = g_list_find(list, preset)))
	{
		list = g_list_remove_link(list, node);
		g_free(preset->name);
		g_free(preset);
		g_list_free_1(node);

		equalizerwin_write_preset_file(list, fname);
	}

	return list;
}


static GList *equalizerwin_import_winamp_eqf(FILE *file)
{
	gchar header[31];
	gchar tmp[257];
	gchar bands[11];
	gint i=0;
	GList *list = NULL;
	EqualizerPreset *preset;

	fread(header, 1, 31, file);
	if (!strncmp(header, "Winamp EQ library file v1.1", 27))
	{
		while (fread(tmp, 1, 257, file)) {
			preset = g_malloc(sizeof (EqualizerPreset));

			fread(bands, 1, 11, file);

			preset->name = g_strdup(tmp);
			preset->preamp =  20.0 - ((bands[10] * 40.0) / 64);

			for (i = 0; i < 10; i++)
				preset->bands[i] = 20.0 - ((bands[i] * 40.0) / 64);

			list = g_list_prepend(list, preset);
		}
        }

	list = g_list_reverse(list);
	return list;

}

static void equalizerwin_read_winamp_eqf(FILE *file)
{
	gchar header[31];
	guchar bands[11];
	gint i;
	
	fread(header, 1, 31, file);
	if (!strncmp(header, "Winamp EQ library file v1.1", 27))
	{
		if (fseek(file, 257, SEEK_CUR) == -1)	/* Skip name */
			return;
		if (fread(bands, 1, 11, file) != 11)
			return;
		eqslider_set_position(equalizerwin_preamp, 20.0 - ((bands[10] * 40.0) / 63.0));
		for (i = 0; i < 10; i++)
			eqslider_set_position(equalizerwin_bands[i], 20.0 - ((bands[i] * 40.0) / 64.0));
	}
	equalizerwin_eq_changed();
}

static void equalizerwin_read_xmms_preset(ConfigFile *cfgfile)
{
	gfloat val;
	gint i;
	
	if (xmms_cfg_read_float(cfgfile, "Equalizer preset", "Preamp", &val))
		eqslider_set_position(equalizerwin_preamp, val);
	for (i = 0; i < 10; i++)
	{
		gchar tmp[7];
		sprintf(tmp, "Band%d", i);
		if (xmms_cfg_read_float(cfgfile, "Equalizer preset", tmp, &val))
			eqslider_set_position(equalizerwin_bands[i], val);
	}
	equalizerwin_eq_changed();
}


static void equalizerwin_save_ok(GtkWidget * widget, gpointer data) {}
static void equalizerwin_save_select(void * clist, gint row, gint column, GdkEventButton * event, gpointer data) {}
static void equalizerwin_load_ok(GtkWidget * widget, gpointer data) {}
static void equalizerwin_load_select(void * widget, gint row, gint column, GdkEventButton * event, gpointer data) {}
static void equalizerwin_delete_delete(GtkWidget * widget, gpointer data) {}
static void equalizerwin_save_auto_ok(GtkWidget * widget, gpointer data) {}
static void equalizerwin_save_auto_select(void * clist, gint row, gint column, GdkEventButton * event, gpointer data) {}
static void equalizerwin_load_auto_ok(GtkWidget * widget, gpointer data) {}
static void equalizerwin_load_auto_select(GtkWidget * widget, gint row, gint column, GdkEventButton * event, gpointer data) {}
static void equalizerwin_delete_auto_delete(GtkWidget * widget, gpointer data) {}
static void equalizerwin_load_filesel_ok(GtkWidget * w, void * filesel) {}
static void equalizerwin_import_winamp_filesel_ok(GtkWidget * w, void * filesel) {}
static void equalizerwin_load_winamp_filesel_ok(GtkWidget * w, void * filesel) {}
static void equalizerwin_save_filesel_ok(GtkWidget * w, void * filesel) {}
static void equalizerwin_save_winamp_filesel_ok(GtkWidget * w, void * filesel) {}
static gint equalizerwin_list_sort_func(void * clist, gconstpointer ptr1, gconstpointer ptr2) { return 0; }
static GtkWidget *equalizerwin_create_list_window(GList * preset_list, gchar * title, GtkWidget ** window, GtkSelectionMode sel_mode, GtkWidget ** entry, gchar * btn1_caption, gchar * btn2_caption, GCallback btn1_func, GCallback select_row_func) { return NULL; }
void equalizerwin_presets_load_preset(void) {}
void equalizerwin_presets_save_preset(void) {}
void equalizerwin_presets_delete_preset(void) {}
void equalizerwin_set_preamp(gfloat preamp) { eqslider_set_position(equalizerwin_preamp, preamp); equalizerwin_eq_changed(); }
void equalizerwin_set_band(gint band, gfloat value) { eqslider_set_position(equalizerwin_bands[band], value); }
gfloat equalizerwin_get_preamp(void) { return eqslider_get_position(equalizerwin_preamp); }
gfloat equalizerwin_get_band(gint band) { return eqslider_get_position(equalizerwin_bands[band]); }
static void equalizerwin_conf_apply_changes(void) {}
static void equalizerwin_conf_ok_cb(GtkWidget * w, gpointer data) {}
static void equalizerwin_conf_apply_cb(GtkWidget * w, gpointer data) {}
GtkWidget * equalizerwin_create_conf_window(void) { return NULL; }
