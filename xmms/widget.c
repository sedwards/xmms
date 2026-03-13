/*  XMMS - Cross-platform multimedia player
 *  Copyright (C) 1998-2000  Peter Alm, Mikael Alm, Olle Hallnas, Thomas Nilsson and 4Front Technologies
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
#include "log.h"

int inside_widget(gint x, gint y, void *p)
{
	Widget *w = (Widget *) p;

    if (!w) return 0;
	if (x >= w->x && x < w->x + w->width && y >= w->y && y < w->y + w->height && w->visible)
		return 1;
	return 0;
}

void show_widget(void *w)
{
    if (!w) return;
	((Widget *) w)->visible = 1;
	draw_widget(w);
}

void hide_widget(void *w)
{
    if (!w) return;
	((Widget *) w)->visible = 0;
}

void resize_widget(void *w, gint width, gint height)
{
    if (!w) return;
	((Widget *) w)->width = width;
	((Widget *) w)->height = height;
	draw_widget(w);
}

void move_widget(void *w, gint x, gint y)
{
    if (!w) return;
	((Widget *) w)->x = x;
	((Widget *) w)->y = y;
	draw_widget(w);
}

void draw_widget(void *p)
{
	Widget *w = (Widget *) p;
    if (!w) return;
	lock_widget(w);
	w->redraw = TRUE;
	unlock_widget(w);
}

void add_widget(GList ** list, void *w)
{
    if (!list || !w) return;
	(*list) = g_list_append(*list, w);
	pthread_mutex_init(&((Widget *)w)->mutex,NULL);
}

gboolean handle_press_cb(GtkWidget * widget, GdkEventButton * event, gpointer data)
{
    GList **wlist_ptr = (GList **)data;
    if (!wlist_ptr || !*wlist_ptr) return FALSE;

    xmms_log("Mouse Press at %.1f, %.1f", event->x, event->y);

	GList *wl = *wlist_ptr;
    gboolean hit = FALSE;
	while (wl)
	{
		Widget *w = (Widget *) wl->data;
		if (w && w->button_press_cb) {
			w->button_press_cb(widget, event, w);
            if (inside_widget(event->x, event->y, w)) {
                xmms_log("Widget hit at %.1f, %.1f", event->x, event->y);
                hit = TRUE;
            }
        }
		wl = wl->next;
	}
    return hit;
}

void handle_release_cb(GtkWidget * widget, GdkEventButton * event, gpointer data)
{
    GList **wlist_ptr = (GList **)data;
    if (!wlist_ptr || !*wlist_ptr) return;

    xmms_log("Mouse Release at %.1f, %.1f", event->x, event->y);

	GList *wl = *wlist_ptr;
	while (wl)
	{
		Widget *w = (Widget *) wl->data;
		if (w && w->button_release_cb)
			w->button_release_cb(widget, event, w);
		wl = wl->next;
	}
}

void handle_motion_cb(GtkWidget * widget, GdkEventMotion * event, gpointer data)
{
    GList **wlist_ptr = (GList **)data;
    if (!wlist_ptr || !*wlist_ptr) return;

	GList *wl = *wlist_ptr;
	while (wl)
	{
		Widget *w = (Widget *) wl->data;
		if (w && w->motion_cb)
			w->motion_cb(widget, event, w);
		wl = wl->next;
	}
}

void draw_widget_list(GList *wlist, cairo_t *cr, gboolean *redraw, gboolean force)
{
    GList *wl = wlist;
    if (redraw) *redraw = FALSE;
    while (wl)
    {
        Widget *w = (Widget *) wl->data;
        if (w && w->visible && (w->redraw || force) && w->draw)
        {
            w->draw(w, cr);
            w->redraw = FALSE;
            if (redraw) *redraw = TRUE;
        }
        wl = wl->next;
    }
}

void widget_list_change_pixmap(GList * wlist, cairo_surface_t * surface)
{
	GList *wl = wlist;
	while (wl)
	{
		if (wl->data) ((Widget *) wl->data)->parent = surface;
		wl = wl->next;
	}
}

void clear_widget_list_redraw(GList * wlist)
{
	GList *wl = wlist;
	while (wl)
	{
		if (wl->data) ((Widget *) wl->data)->redraw = FALSE;
		wl = wl->next;
	}
}

void lock_widget(void *w)
{
    if (w) pthread_mutex_lock(&((Widget *) w)->mutex);
}

void unlock_widget(void *w)
{
    if (w) pthread_mutex_unlock(&((Widget *) w)->mutex);
}

void lock_widget_list(GList * wlist)
{
	GList *wl = wlist;
	while (wl)
	{
		if (wl->data) lock_widget(wl->data);
		wl = wl->next;
	}
}

void unlock_widget_list(GList * wlist)
{
	GList *wl = wlist;
	while (wl)
	{
		if (wl->data) unlock_widget(wl->data);
		wl = wl->next;
	}
}
