/*  XMMS - Cross-platform multimedia player
 *  Copyright (C) 1998-2001  Peter Alm, Mikael Alm, Olle Hallnas,
 *                           Thomas Nilsson and 4Front Technologies
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

void dock_set_uposition(GtkWidget *widget, gint x, gint y)
{
    gtk_widget_set_uposition(widget, x, y);
}

GList *dock_add_window(GList *window_list, GtkWidget *window)
{
    return g_list_append(window_list, window);
}

static void dock_snap(GtkWidget * w, GList *window_list, gint * off_x, gint * off_y)
{
	gint nx, ny, nw, nh;
	GList *node;

	gdk_window_get_root_origin(gtk_widget_get_window(w), &nx, &ny);
	gdk_window_get_size(gtk_widget_get_window(w), &nw, &nh);
	nx += *off_x;
	ny += *off_y;

	if (cfg.snap_windows)
	{
		if (abs(nx) < cfg.snap_distance)
			*off_x -= nx;
		if (abs(ny) < cfg.snap_distance)
			*off_y -= ny;
		if (abs(nx + nw - gdk_screen_width()) < cfg.snap_distance)
			*off_x -= nx + nw - gdk_screen_width();
		if (abs(ny + nh - gdk_screen_height()) < cfg.snap_distance)
			*off_y -= ny + nh - gdk_screen_height();

		node = window_list;
		while (node)
		{
			if (GTK_WIDGET(node->data) != w && GTK_WIDGET_VISIBLE(node->data))
			{
				gint tx, ty, tw, th;

				gdk_window_get_root_origin(gtk_widget_get_window(GTK_WIDGET(node->data)), &tx, &ty);
				gdk_window_get_size(gtk_widget_get_window(GTK_WIDGET(node->data)), &tw, &th);

				if (ny + nh > ty && ny < ty + th)
				{
					if (abs(nx + nw - tx) < cfg.snap_distance)
						*off_x -= nx + nw - tx;
					if (abs(nx - (tx + tw)) < cfg.snap_distance)
						*off_x -= nx - (tx + tw);
				}
				if (nx + nw > tx && nx < tx + tw)
				{
					if (abs(ny + nh - ty) < cfg.snap_distance)
						*off_y -= ny + nh - ty;
					if (abs(ny - (ty + th)) < cfg.snap_distance)
						*off_y -= ny - (ty + th);
				}
			}
			node = node->next;
		}
	}
}

void dock_move_press(GList *window_list, GtkWidget * w, GdkEventButton * event, gboolean move_list)
{
	gint mx, my;

	if (event->button != 1)
		return;

	gdk_window_get_device_position(gtk_widget_get_window(w), gdk_event_get_device((GdkEvent*)event), &mx, &my, NULL);
	g_object_set_data(G_OBJECT(w), "is_moving", GINT_TO_POINTER(TRUE));
    g_object_set_data(G_OBJECT(w), "move_list", GINT_TO_POINTER(move_list));
    g_object_set_data(G_OBJECT(w), "window_list", window_list);
	g_object_set_data(G_OBJECT(w), "move_off_x", GINT_TO_POINTER(mx));
	g_object_set_data(G_OBJECT(w), "move_off_y", GINT_TO_POINTER(my));
}

void dock_move_motion(GtkWidget * w, GdkEventMotion * event)
{
	gint mx, my, ox, oy, dx, dy, dw, dh, off_x, off_y;
	GList *node, *window_list;
    gboolean move_list;

	if (!GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "is_moving")))
		return;

	ox = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "move_off_x"));
	oy = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "move_off_y"));
    move_list = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "move_list"));
    window_list = g_object_get_data(G_OBJECT(w), "window_list");

	gdk_window_get_device_position(NULL, gdk_event_get_device((GdkEvent*)event), &mx, &my, NULL);
	gdk_window_get_root_origin(gtk_widget_get_window(w), &dx, &dy);
	gdk_window_get_size(gtk_widget_get_window(w), &dw, &dh);

	off_x = mx - ox - dx;
	off_y = my - oy - dy;

	dock_snap(w, window_list, &off_x, &off_y);

	if (off_x || off_y)
	{
        if (move_list)
        {
		    node = window_list;
		    while (node)
		    {
			    if (GTK_WIDGET_VISIBLE(node->data))
			    {
				    gint tx, ty;
				    gdk_window_get_root_origin(gtk_widget_get_window(GTK_WIDGET(node->data)), &tx, &ty);
				    gtk_widget_set_uposition(GTK_WIDGET(node->data), tx + off_x, ty + off_y);
			    }
			    node = node->next;
		    }
        }
        else
        {
            gtk_widget_set_uposition(w, dx + off_x, dy + off_y);
        }
	}
}

void dock_move_release(GtkWidget * w)
{
	g_object_set_data(G_OBJECT(w), "is_moving", GINT_TO_POINTER(FALSE));
}

void dock_get_widget_pos(GtkWidget *w, gint *x, gint *y)
{
    gdk_window_get_root_origin(gtk_widget_get_window(w), x, y);
}

gboolean dock_is_moving(GtkWidget *w)
{
    return GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "is_moving"));
}

void dock_shade(GList *window_list, GtkWidget *widget, gint new_h)
{
    /* STUB: Implement shading logic if needed for window groups */
}

void dock_resize(GList *window_list, GtkWidget *w, gint new_w, gint new_h)
{
    gtk_window_resize(GTK_WINDOW(w), new_w, new_h);
}
