/*  XMMS - Cross-platform multimedia player
 *  Copyright (C) 1998-2000  Peter Alm, Mikael Alm, Olle Hallnas, Thomas Nilsson and 4Front Technologies
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public Licensse as published by
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

/* Designate dropped data types that we know and care about */
enum {
	XMMS_DROP_STRING,
	XMMS_DROP_PLAINTEXT,
	XMMS_DROP_URLENCODED
};

/* Drag data format listing for gtk_drag_dest_set() */
extern const GtkTargetEntry _xmms_drop_types[];

#define xmms_drag_dest_set(widget) gtk_drag_dest_set(widget, \
		GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_DROP, \
		_xmms_drop_types, 3, GDK_ACTION_COPY | GDK_ACTION_MOVE)
