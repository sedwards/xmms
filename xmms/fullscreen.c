/*  XMMS - Cross-platform multimedia player
 *  Copyright (C) 1998-2000  Zinx Verituse
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
#include <config.h>
#include "xmms.h"

gboolean xmms_fullscreen_available(void * dpy)
{
	return FALSE;
}
gboolean xmms_fullscreen_init(GtkWidget * win)
{
	return FALSE;
}

gboolean xmms_fullscreen_enter(GtkWidget * win, gint * w, gint * h)
{
	return FALSE;
}
void xmms_fullscreen_leave(GtkWidget * win)
{
	return;
}

gboolean xmms_fullscreen_in(GtkWidget * win)
{
	return FALSE;
}
gboolean xmms_fullscreen_mark(GtkWidget * win)
{
	return FALSE;
}
void xmms_fullscreen_unmark(GtkWidget * win)
{
	return;
}

void xmms_fullscreen_cleanup(GtkWidget * win)
{
	return;
}

GSList *xmms_fullscreen_modelist(GtkWidget * win)
{
	return NULL;
}
void xmms_fullscreen_modelist_free(GSList * modes)
{
	return;
}
