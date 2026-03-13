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

static void sbutton_button_press_cb(GtkWidget * widget, GdkEventButton * event, SButton * b)
{
	if (inside_widget(event->x, event->y, b) && event->button == 1)
	{
		b->pressed = TRUE;
		draw_widget(b);
	}
}

static void sbutton_button_release_cb(GtkWidget * widget, GdkEventButton * event, SButton * b)
{
	if (b->pressed)
	{
		b->pressed = FALSE;
		if (inside_widget(event->x, event->y, b))
		{
			if (b->callback)
				b->callback();
		}
		draw_widget(b);
	}
}

static void sbutton_motion_cb(GtkWidget * widget, GdkEventMotion * event, SButton * b)
{
	gboolean p;

	if (b->pressed)
	{
		p = inside_widget(event->x, event->y, b);
		if (p != b->inside)
		{
			b->inside = p;
			draw_widget(b);
		}
	}
}

static void sbutton_draw(SButton * b, cairo_t *cr)
{
	gint xsrc;

	xsrc = b->pressed && b->inside ? b->width : 0;
	skin_draw_pixmap(cr, b->si, xsrc + b->xsrc, b->ysrc, b->sb_widget.x, b->sb_widget.y, b->width, b->height);
}

SButton *create_sbutton(GList ** list, cairo_surface_t * parent, gint x, gint y, gint width, gint height, gint xsrc, gint ysrc, SkinIndex si, void (*callback) (void))
{
	SButton *b;

	b = g_malloc0(sizeof (SButton));
	b->sb_widget.parent = parent;
	b->sb_widget.x = x;
	b->sb_widget.y = y;
	b->sb_widget.width = width;
	b->sb_widget.height = height;
	b->sb_widget.visible = TRUE;
	b->sb_widget.button_press_cb = (void (*) (GtkWidget *, GdkEventButton *, gpointer)) sbutton_button_press_cb;
	b->sb_widget.button_release_cb = (void (*) (GtkWidget *, GdkEventButton *, gpointer)) sbutton_button_release_cb;
	b->sb_widget.motion_cb = (void (*) (GtkWidget *, GdkEventMotion *, gpointer)) sbutton_motion_cb;
	b->sb_widget.draw = (void (*) (void *, cairo_t *)) sbutton_draw;
	b->xsrc = xsrc;
	b->ysrc = ysrc;
	b->si = si;
	b->callback = callback;
	b->inside = TRUE;

	add_widget(list, b);

	return b;
}
