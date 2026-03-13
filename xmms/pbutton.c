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

static void pbutton_draw(PButton * button, cairo_t *cr)
{
	if (button->pb_widget.visible)
	{
		if (button->pressed && button->inside)
		{
			skin_draw_pixmap(cr, button->si,
					 button->pb_px, button->pb_py,
					 button->pb_widget.x, button->pb_widget.y,
					 button->pb_widget.width, button->pb_widget.height);
		}
		else
		{
			skin_draw_pixmap(cr, button->si,
					 button->pb_nx, button->pb_ny,
					 button->pb_widget.x, button->pb_widget.y,
					 button->pb_widget.width, button->pb_widget.height);
		}
	}
}

static void pbutton_button_press_cb(GtkWidget * widget, GdkEventButton * event, PButton * b)
{
	if (inside_widget(event->x, event->y, b) && event->button == 1)
	{
		b->pressed = TRUE;
		b->inside = TRUE;
		draw_widget(b);
	}
}

static void pbutton_button_release_cb(GtkWidget * widget, GdkEventButton * event, PButton * b)
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

static void pbutton_motion_cb(GtkWidget * widget, GdkEventMotion * event, PButton * b)
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

PButton *create_pbutton(GList ** list, cairo_surface_t * parent, gint x, gint y, gint width, gint height, gint nx, gint ny, gint px, gint py, void (*callback) (void), SkinIndex si)
{
	PButton *b;

	b = g_malloc0(sizeof (PButton));
	b->pb_widget.parent = parent;
	b->pb_widget.x = x;
	b->pb_widget.y = y;
	b->pb_widget.width = width;
	b->pb_widget.height = height;
	b->pb_widget.visible = TRUE;
	b->pb_widget.button_press_cb = (void (*) (GtkWidget *, GdkEventButton *, gpointer)) pbutton_button_press_cb;
	b->pb_widget.button_release_cb = (void (*) (GtkWidget *, GdkEventButton *, gpointer)) pbutton_button_release_cb;
	b->pb_widget.motion_cb = (void (*) (GtkWidget *, GdkEventMotion *, gpointer)) pbutton_motion_cb;
	b->pb_widget.draw = (void (*) (void *, cairo_t *)) pbutton_draw;
	b->pb_nx = nx;
	b->pb_ny = ny;
	b->pb_px = px;
	b->pb_py = py;
	b->si = si;
	b->callback = callback;

	add_widget(list, b);

	return b;
}

void pbutton_set_button_data(PButton *b, gint nx, gint ny, gint px, gint py)
{
    if (nx != -1) b->pb_nx = nx;
    if (ny != -1) b->pb_ny = ny;
    if (px != -1) b->pb_px = px;
    if (py != -1) b->pb_py = py;
    draw_widget(b);
}

void pbutton_set_skin_index(PButton *b, SkinIndex si)
{
    b->si = si;
    draw_widget(b);
}

void pbutton_set_skin_index1(PButton *b, SkinIndex si)
{
    pbutton_set_skin_index(b, si);
}
