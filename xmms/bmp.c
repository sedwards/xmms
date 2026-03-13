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

cairo_surface_t *read_bmp(gchar * filename)
{
    GdkPixbuf *pb;
    cairo_surface_t *surface;
    cairo_t *cr;
    GError *error = NULL;

    pb = gdk_pixbuf_new_from_file(filename, &error);
    if (!pb) {
        g_warning("read_bmp(): Error loading %s: %s", filename, error->message);
        g_error_free(error);
        return NULL;
    }

    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, gdk_pixbuf_get_width(pb), gdk_pixbuf_get_height(pb));
    cr = cairo_create(surface);
    gdk_cairo_set_source_pixbuf(cr, pb, 0, 0);
    cairo_paint(cr);
    cairo_destroy(cr);
    
    g_object_unref(pb);

	return surface;
}
