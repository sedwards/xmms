/*  XMMS - Cross-platform multimedia player
 *  Copyright (C) 1998-2001  Peter Alm, Mikael Alm, Olle Hallnas,
 *                           Thomas Nilsson and 4Front Technologies
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

static PangoFontDescription *playlist_list_font = NULL;

gint playlist_list_auto_drag_down_func(gpointer data)
{
	PlayList_List *pl = data;

	if (pl->pl_auto_drag_down)
	{
		playlist_list_move_down(pl);
		pl->pl_first++;
		playlistwin_update_list();
		return TRUE;

	}
	return FALSE;
}

gint playlist_list_auto_drag_up_func(gpointer data)
{
	PlayList_List *pl = data;

	if (pl->pl_auto_drag_up)
	{
		playlist_list_move_up(pl);
		pl->pl_first--;
		playlistwin_update_list();
		return TRUE;

	}
	return FALSE;
}

void playlist_list_move_up(PlayList_List *pl)
{
	GList *list;

	PL_LOCK();
	list = get_playlist();
    if (!list) { PL_UNLOCK(); return; }
	if (((PlaylistEntry *) list->data)->selected)
	{
		/* We are at the top */
		PL_UNLOCK();
		return;
	}
	while (list)
	{
		if (((PlaylistEntry *) list->data)->selected)
			glist_moveup(list);
		list = g_list_next(list);
	}
	PL_UNLOCK();
	if (pl->pl_prev_selected != -1)
		pl->pl_prev_selected--;
	if (pl->pl_prev_min != -1)
		pl->pl_prev_min--;
	if (pl->pl_prev_max != -1)
		pl->pl_prev_max--;
}

void playlist_list_move_down(PlayList_List *pl)
{
	GList *list;

	PL_LOCK();
	list = g_list_last(get_playlist());
    if (!list) { PL_UNLOCK(); return; }
	if (((PlaylistEntry *) list->data)->selected)
	{
		/* We are at the bottom */
		PL_UNLOCK();
		return;
	}
	while (list)
	{
		if (((PlaylistEntry *) list->data)->selected)
			glist_movedown(list);
		list = g_list_previous(list);
	}
	PL_UNLOCK();
	if (pl->pl_prev_selected != -1)
		pl->pl_prev_selected++;
	if (pl->pl_prev_min != -1)
		pl->pl_prev_min++;
	if (pl->pl_prev_max != -1)
		pl->pl_prev_max++;
}

static void playlist_list_button_press_cb(GtkWidget * widget, GdkEventButton * event, PlayList_List * pl)
{
	if (event->button == 1 && pl->pl_fheight &&
	    inside_widget(event->x, event->y, &pl->pl_widget))
	{
		int nr, y;

		y = event->y - pl->pl_widget.y;
		nr = (y / pl->pl_fheight) + pl->pl_first;
		if (nr >= get_playlist_length())
			nr = get_playlist_length() - 1;
		if (!(event->state & GDK_CONTROL_MASK))
			playlist_select_all(FALSE);
		
		if (event->state & GDK_SHIFT_MASK && pl->pl_prev_selected != -1)
		{
			playlist_select_range(pl->pl_prev_selected, nr, TRUE);
			pl->pl_prev_min = pl->pl_prev_selected;
			pl->pl_prev_max = nr;
			pl->pl_drag_pos = nr - pl->pl_first;
		}
		else
		{
			if (playlist_select_invert(nr))
			{
				pl->pl_prev_selected = nr;
				pl->pl_drag_pos = nr - pl->pl_first;
				pl->pl_prev_min = -1;
			}
		}
		if (event->type == GDK_2BUTTON_PRESS)
		{
			gdk_device_ungrab(gdk_event_get_device((GdkEvent*)event), GDK_CURRENT_TIME);
			playlist_set_position(nr);
			if (!get_input_playing())
				playlist_play();
		}
		pl->pl_dragging = TRUE;
		playlistwin_update_list();
	}
}

int playlist_list_get_playlist_position(PlayList_List *pl, int x, int y)
{
	int iy, length;
	
	if (!inside_widget(x, y, pl) || !pl->pl_fheight)
		return -1;

	if ((length = get_playlist_length()) == 0)
		return -1;
	iy = y - pl->pl_widget.y;

	return(MIN((iy / pl->pl_fheight) + pl->pl_first, length - 1));
}

static void playlist_list_motion_cb(GtkWidget * widget, GdkEventMotion * event, PlayList_List * pl)
{
	gint nr, y, off, i;

	if (pl->pl_dragging)
	{
		y = event->y - pl->pl_widget.y;
		nr = (y / pl->pl_fheight);
		if (nr < 0)
		{
			nr = 0;
			if (!pl->pl_auto_drag_up)
			{
				pl->pl_auto_drag_up = TRUE;
				pl->pl_auto_drag_up_tag = g_timeout_add(100, playlist_list_auto_drag_up_func, pl);
			}
		}
		else if (pl->pl_auto_drag_up)
			pl->pl_auto_drag_up = FALSE;

		if (nr >= pl->pl_num_visible)
		{
			nr = pl->pl_num_visible - 1;
			if (!pl->pl_auto_drag_down)
			{
				pl->pl_auto_drag_down = TRUE;
				pl->pl_auto_drag_down_tag = g_timeout_add(100, playlist_list_auto_drag_down_func, pl);
			}
		}
		else if (pl->pl_auto_drag_down)
			pl->pl_auto_drag_down = FALSE;

		off = nr - pl->pl_drag_pos;
		if (off)
		{
			for (i = 0; i < abs(off); i++)
			{
				if (off < 0)
					playlist_list_move_up(pl);
				else
					playlist_list_move_down(pl);

			}
			playlistwin_update_list();
		}
		pl->pl_drag_pos = nr;
	}
}

static void playlist_list_button_release_cb(GtkWidget * widget, GdkEventButton * event, PlayList_List * pl)
{
	pl->pl_dragging = FALSE;
	pl->pl_auto_drag_down = FALSE;
	pl->pl_auto_drag_up = FALSE;
}

static void playlist_list_draw(PlayList_List * pl, cairo_t *cr)
{
	gint width, height;
	gchar *text, *title;
	gint i, tw, max_first;
    GdkColor *c;
    PangoLayout *layout;

	width = pl->pl_widget.width;
	height = pl->pl_widget.height;

    c = get_skin_color(SKIN_PLEDIT_NORMALBG);
    cairo_set_source_rgb(cr, c->red/65535.0, c->green/65535.0, c->blue/65535.0);
	cairo_rectangle(cr, pl->pl_widget.x, pl->pl_widget.y, width, height);
    cairo_fill(cr);

	if (playlist_list_font != NULL)
	{
		GList *list;
		PL_LOCK();
		list = get_playlist();
        
        layout = pango_cairo_create_layout(cr);
        pango_layout_set_font_description(layout, playlist_list_font);
        
        PangoRectangle rect;
        pango_layout_set_text(layout, "Ay", -1);
        pango_layout_get_pixel_extents(layout, NULL, &rect);
		pl->pl_fheight = rect.height + 1;
		pl->pl_num_visible = height / pl->pl_fheight;

		max_first = (g_list_length(list) - pl->pl_num_visible);
		if (max_first < 0)
			max_first = 0;
		if (pl->pl_first >= max_first)
			pl->pl_first = max_first;
		if (pl->pl_first < 0)
			pl->pl_first = 0;
		for (i = 0; i < pl->pl_first; i++)
			list = g_list_next(list);
		i = pl->pl_first;

		while (list && i < pl->pl_first + pl->pl_num_visible)
		{
			PlaylistEntry *entry = (PlaylistEntry *) list->data;
			if (entry->selected)
			{
                c = get_skin_color(SKIN_PLEDIT_SELECTEDBG);
                cairo_set_source_rgb(cr, c->red/65535.0, c->green/65535.0, c->blue/65535.0);
				cairo_rectangle(cr, pl->pl_widget.x, pl->pl_widget.y + ((i - pl->pl_first) * pl->pl_fheight), width, pl->pl_fheight);
                cairo_fill(cr);
			}
			if (i == __get_playlist_position())
                c = get_skin_color(SKIN_PLEDIT_CURRENT);
			else
                c = get_skin_color(SKIN_PLEDIT_NORMAL);
            
            cairo_set_source_rgb(cr, c->red/65535.0, c->green/65535.0, c->blue/65535.0);

			if (entry->title)
				title = entry->title;
			else
				title = (gchar*)g_path_get_basename(entry->filename);

			if (entry->length != -1)
			{
				gchar length_str[20];
				sprintf(length_str, "%d:%-2.2d", entry->length / 60000, (entry->length / 1000) % 60);
                pango_layout_set_text(layout, length_str, -1);
                pango_layout_get_pixel_extents(layout, NULL, &rect);
                
                cairo_move_to(cr, pl->pl_widget.x + width - rect.width - 2, pl->pl_widget.y + (i - pl->pl_first) * pl->pl_fheight);
                pango_cairo_show_layout(cr, layout);
				tw = width - rect.width - 5;
			}
			else
				tw = width;
                
			if (cfg.show_numbers_in_pl)
				text = g_strdup_printf("%d. %s", i + 1, title);
			else
				text = g_strdup_printf("%s", title);

            pango_layout_set_text(layout, text, -1);
            pango_layout_set_width(layout, tw * PANGO_SCALE);
            pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_END);
            
            cairo_move_to(cr, pl->pl_widget.x, pl->pl_widget.y + (i - pl->pl_first) * pl->pl_fheight);
            pango_cairo_show_layout(cr, layout);
            
			g_free(text);
			list = list->next;
			i++;
		}
        g_object_unref(layout);
		PL_UNLOCK();
	}
	else
	{
		g_log(NULL, G_LOG_LEVEL_CRITICAL, "Couldn't open playlist font");
	}
}

PlayList_List *create_playlist_list(GList ** wlist, cairo_surface_t * parent, gint x, gint y, gint w, gint h)
{
	PlayList_List *pl;

	pl = (PlayList_List *) g_malloc0(sizeof (PlayList_List));
	pl->pl_widget.parent = parent;
	pl->pl_widget.x = x;
	pl->pl_widget.y = y;
	pl->pl_widget.width = w;
	pl->pl_widget.height = h;
	pl->pl_widget.visible = TRUE;
	pl->pl_widget.button_press_cb = (void (*) (GtkWidget *, GdkEventButton *, gpointer)) playlist_list_button_press_cb;
	pl->pl_widget.button_release_cb = (void (*) (GtkWidget *, GdkEventButton *, gpointer)) playlist_list_button_release_cb;
	pl->pl_widget.motion_cb = (void (*) (GtkWidget *, GdkEventMotion *, gpointer)) playlist_list_motion_cb;
	pl->pl_widget.draw = (void (*) (void *, cairo_t *)) playlist_list_draw;
	pl->pl_prev_selected = -1;
	pl->pl_prev_min = -1;
	pl->pl_prev_max = -1;
	add_widget(wlist, pl);
	return pl;
}

void playlist_list_set_font(char *font)
{
	if (playlist_list_font)
		pango_font_description_free(playlist_list_font);

	playlist_list_font = pango_font_description_from_string(font);
}
