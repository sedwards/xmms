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
#ifndef PBUTTON_H
#define	PBUTTON_H

typedef struct
{
	Widget pb_widget;
	gint pb_nx, pb_ny, pb_px, pb_py;
	SkinIndex si;
	void (*callback) (void);
	gboolean pressed, inside;
} PButton;

void pbutton_set_button_data(PButton *b, gint nx, gint ny, gint px, gint py);
void pbutton_set_skin_index(PButton *b, SkinIndex si);
void pbutton_set_skin_index1(PButton *b, SkinIndex si);
PButton *create_pbutton(GList ** list, cairo_surface_t * parent, gint x, gint y, gint width, gint height, gint nx, gint ny, gint px, gint py, void (*callback) (void), SkinIndex si);

#endif
