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
    gtk_window_move(GTK_WINDOW(widget), x, y);
}

GList *dock_add_window(GList *window_list, GtkWidget *window)
{
    if (g_list_find(window_list, window)) return window_list;
    return g_list_append(window_list, window);
}

static void dock_snap(GtkWidget * w, GList *window_list, gint * nx, gint * ny)
{
	gint nw, nh;
	GList *node;

    if (!gtk_widget_get_window(w)) return;
	gdk_window_get_size(gtk_widget_get_window(w), &nw, &nh);

	if (cfg.snap_windows)
	{
		if (abs(*nx) < cfg.snap_distance)
			*nx = 0;
		if (abs(*ny) < cfg.snap_distance)
			*ny = 0;
		
		node = window_list;
		while (node)
		{
			if (GTK_WIDGET(node->data) != w && gtk_widget_get_visible(GTK_WIDGET(node->data)))
			{
				gint tx, ty, tw, th;

				gdk_window_get_root_origin(gtk_widget_get_window(GTK_WIDGET(node->data)), &tx, &ty);
				gdk_window_get_size(gtk_widget_get_window(GTK_WIDGET(node->data)), &tw, &th);

				if (*ny + nh > ty && *ny < ty + th)
				{
					if (abs(*nx + nw - tx) < cfg.snap_distance)
						*nx = tx - nw;
					if (abs(*nx - (tx + tw)) < cfg.snap_distance)
						*nx = tx + tw;
				}
				if (*nx + nw > tx && *nx < tx + tw)
				{
					if (abs(*ny + nh - ty) < cfg.snap_distance)
						*ny = ty - nh;
					if (abs(*ny - (ty + th)) < cfg.snap_distance)
						*ny = ty + th;
				}
			}
			node = node->next;
		}
	}
}

static GList *dock_find_connected(GList *window_list, GtkWidget *w)
{
    GList *connected = NULL, *queue = NULL, *node;
    
    connected = g_list_append(connected, w);
    queue = g_list_append(queue, w);
    
    while (queue)
    {
        GtkWidget *curr = queue->data;
        queue = g_list_remove(queue, curr);
        
        gint cx, cy, cw, ch;
        gdk_window_get_root_origin(gtk_widget_get_window(curr), &cx, &cy);
        gdk_window_get_size(gtk_widget_get_window(curr), &cw, &ch);
        
        for (node = window_list; node; node = node->next)
        {
            GtkWidget *other = node->data;
            if (other == curr || g_list_find(connected, other) || !gtk_widget_get_visible(other))
                continue;
                
            gint ox, oy, ow, oh;
            gdk_window_get_root_origin(gtk_widget_get_window(other), &ox, &oy);
            gdk_window_get_size(gtk_widget_get_window(other), &ow, &oh);
            
            /* Check if touching - use a 2px tolerance for docking */
            gboolean touching = FALSE;
            gint tolerance = 2;
            
            /* Horizontal touch */
            if (cy + ch + tolerance > oy && cy < oy + oh + tolerance)
            {
                if (abs(cx + cw - ox) <= tolerance || abs(cx - (ox + ow)) <= tolerance)
                    touching = TRUE;
            }
            /* Vertical touch */
            if (cx + cw + tolerance > ox && cx < ox + ow + tolerance)
            {
                if (abs(cy + ch - oy) <= tolerance || abs(cy - (oy + oh)) <= tolerance)
                    touching = TRUE;
            }
            
            if (touching)
            {
                connected = g_list_append(connected, other);
                queue = g_list_append(queue, other);
            }
        }
    }
    return connected;
}

void dock_move_press(GList *window_list, GtkWidget * w, GdkEventButton * event, gboolean move_list)
{
    GList *node;

	if (event->button != 1)
		return;

	g_object_set_data(G_OBJECT(w), "is_moving", GINT_TO_POINTER(TRUE));
	g_object_set_data(G_OBJECT(w), "root_press_x", GINT_TO_POINTER((gint)event->x_root));
	g_object_set_data(G_OBJECT(w), "root_press_y", GINT_TO_POINTER((gint)event->y_root));

    GList *connected = NULL;
    if (move_list)
        connected = dock_find_connected(window_list, w);
    else
        connected = g_list_append(NULL, w);

    g_object_set_data(G_OBJECT(w), "move_list_current", connected);

    /* Store initial positions of all windows in the moving group */
    for (node = connected; node; node = node->next)
    {
        gint wx, wy;
        gdk_window_get_root_origin(gtk_widget_get_window(GTK_WIDGET(node->data)), &wx, &wy);
        g_object_set_data(G_OBJECT(node->data), "initial_x", GINT_TO_POINTER(wx));
        g_object_set_data(G_OBJECT(node->data), "initial_y", GINT_TO_POINTER(wy));
    }
}

void dock_move_motion(GtkWidget * w, GdkEventMotion * event)
{
	gint mx, my, px, py, dx, dy, delta_x, delta_y;
	GList *node, *move_list;

	if (!GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "is_moving")))
		return;

	px = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "root_press_x"));
	py = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "root_press_y"));
    move_list = g_object_get_data(G_OBJECT(w), "move_list_current");

    mx = (gint)event->x_root;
    my = (gint)event->y_root;

    /* How far we've moved from the press point */
	delta_x = mx - px;
	delta_y = my - py;

    /* Target position for the primary window */
    dx = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "initial_x")) + delta_x;
    dy = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "initial_y")) + delta_y;

    /* Apply snap to the primary window */
    extern GList *dock_window_list;
	dock_snap(w, dock_window_list, &dx, &dy);

    /* Re-calculate final delta after snapping */
    delta_x = dx - GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "initial_x"));
    delta_y = dy - GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "initial_y"));

    if (delta_x || delta_y)
	{
		node = move_list;
		while (node)
		{
            GtkWidget *win = GTK_WIDGET(node->data);
            gint ix = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(win), "initial_x"));
            gint iy = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(win), "initial_y"));
            
            gtk_window_move(GTK_WINDOW(win), ix + delta_x, iy + delta_y);
            node = node->next;
		}
	}
}

void dock_move_release(GtkWidget * w)
{
    if (!GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "is_moving")))
        return;

    GList *move_list = g_object_get_data(G_OBJECT(w), "move_list_current");
    if (move_list) g_list_free(move_list);
    g_object_set_data(G_OBJECT(w), "move_list_current", NULL);
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
}

void dock_resize(GList *window_list, GtkWidget *w, gint new_w, gint new_h)
{
    gtk_window_resize(GTK_WINDOW(w), new_w, new_h);
}
