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
#ifndef XMMS_H
#define XMMS_H

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

/* GTK 3 Compatibility Macros */
#ifndef GtkObject
#define GtkObject GObject
#endif
#ifndef GTK_OBJECT
#define GTK_OBJECT(x) G_OBJECT(x)
#endif
#ifndef GTK_SIGNAL_FUNC
#define GTK_SIGNAL_FUNC(x) G_CALLBACK(x)
#endif
#define gtk_signal_connect(obj, sig, func, data) g_signal_connect(obj, sig, func, data)
#define gtk_signal_disconnect_by_func(obj, func, data) g_signal_handlers_disconnect_by_func(obj, func, data)
#define gtk_object_set_data g_object_set_data
#define gtk_object_get_data g_object_get_data
#define gtk_object_set_data_full g_object_set_data_full
#define gtk_widget_set_usize(w, width, height) gtk_widget_set_size_request(w, width, height)
#define gdk_window_get_size(win, w, h) do { *(w) = gdk_window_get_width(win); *(h) = gdk_window_get_height(win); } while(0)
#define gdk_screen_width() gdk_screen_get_width(gdk_screen_get_default())
#define gdk_screen_height() gdk_screen_get_height(gdk_screen_get_default())
#define GDK_THREADS_ENTER() gdk_threads_enter()
#define GDK_THREADS_LEAVE() gdk_threads_leave()

#define gtk_object_remove_data(obj, key) g_object_set_data(G_OBJECT(obj), key, NULL)
#define GTK_WIDGET_VISIBLE(w) gtk_widget_get_visible(w)
#define gtk_widget_set_uposition(w, x, y) gtk_window_move(GTK_WINDOW(w), x, y)
#define gdk_window_get_deskrelative_origin(win, x, y) gdk_window_get_root_origin(win, x, y)

typedef void GtkItemFactory;

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <math.h>

//#include "bmp.h"
#include "util.h"
#include "skin.h"
#include "plugin.h"
#include "output.h"
#include "input.h"
#include "effect.h"
#include "general.h"
#include "visualization.h"
#include "fullscreen.h"
#include "pluginenum.h"
#include "playlist.h"
#include "controlsocket.h"
#include "dock.h"
#include "widget.h"
#include "sbutton.h"
#include "pbutton.h"
#include "tbutton.h"
#include "textbox.h"
#include "menurow.h"
#include "hslider.h"
#include "monostereo.h"
#include "vis.h"
#include "svis.h"
#include "number.h"
#include "playstatus.h"
#include "playlist_list.h"
#include "playlist_slider.h"
#include "playlist_popup.h"
#include "eq_graph.h"
#include "eq_slider.h"
#include "main.h"
#include "skinwin.h"
#include "prefswin.h"
#include "playlistwin.h"
#include "equalizer.h"
#include "about.h"
#include "hints.h"
#include "i18n.h"
#include "sm.h"
#include "dnd.h"
#include "urldecode.h"

#include "config.h"

#endif
