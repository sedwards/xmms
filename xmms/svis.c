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

static gint svis_redraw_delays[] = {1, 2, 4, 8};
static guint8 svis_scope_colors[] = {20, 19, 18, 19, 20};
static guint8 svis_vu_normal_colors[] = {17, 17, 17, 12, 12, 12, 2, 2};

#define SVIS_HEIGHT 5
#define SVIS_WIDTH 38

void svis_timeout_func(SVis * svis, guchar * data)
{
	static GTimer *timer = NULL;
	gulong micros = 9999999;
	gboolean falloff = FALSE;
	gint i;

	if (!timer)
	{
		timer = g_timer_new();
		g_timer_start(timer);
	}
	else
	{
		g_timer_elapsed(timer, &micros);
		if (micros > 14000)
			g_timer_reset(timer);
	}

	if (cfg.vis_type == INPUT_VIS_ANALYZER)
	{
		if (micros > 14000)
			falloff = TRUE;

		for (i = 0; i < 2; i++)
		{
			if (falloff || data)
			{
				if (data && data[i] > svis->vs_data[i])
					svis->vs_data[i] = data[i];
				else if (falloff)
				{
					if (svis->vs_data[i] >= 2)
						svis->vs_data[i] -= 2;
					else
						svis->vs_data[i] = 0;
				}
			}
		}
	}
	else if (data)
	{
		for (i = 0; i < 75; i++)
			svis->vs_data[i] = data[i];
	}

	if (micros > 14000)
	{
		if (!svis->vs_refresh_delay)
		{
			draw_widget(svis);
			svis->vs_refresh_delay = svis_redraw_delays[cfg.vis_refresh];
		}
		svis->vs_refresh_delay--;
	}
}

static void svis_draw(SVis * svis, cairo_t *cr)
{
	gint x, y, h, scale;
	guchar svis_color[24][3];

	get_skin_viscolor(svis_color);
    scale = cfg.doublesize ? 2 : 1;

    cairo_save(cr);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle(cr, svis->vs_widget.x, svis->vs_widget.y, svis->vs_widget.width * scale, svis->vs_widget.height * scale);
    cairo_fill(cr);

	if (cfg.vis_type == VIS_ANALYZER)
	{
		switch (cfg.vu_mode)
		{
			case VU_NORMAL:
				for (y = 0; y < 2; y++)
				{
					h = (svis->vs_data[y] * 7) / 37;
					for (x = 0; x < h; x++)
					{
						int color_idx = svis_vu_normal_colors[x];
                        cairo_set_source_rgb(cr, svis_color[color_idx][0]/255.0, svis_color[color_idx][1]/255.0, svis_color[color_idx][2]/255.0);
                        cairo_rectangle(cr, svis->vs_widget.x + (x * 5) * scale, svis->vs_widget.y + (y * 3) * scale, 3 * scale, 2 * scale);
                        cairo_fill(cr);
					}
				}
				break;
			case VU_SMOOTH:
				for (y = 0; y < 2; y++)
				{
					for (x = 0; x < svis->vs_data[y]; x++)
					{
						int color_idx = 17 - ((x * 15) / 37);
                        cairo_set_source_rgb(cr, svis_color[color_idx][0]/255.0, svis_color[color_idx][1]/255.0, svis_color[color_idx][2]/255.0);
                        cairo_rectangle(cr, svis->vs_widget.x + x * scale, svis->vs_widget.y + (y * 3) * scale, scale, 2 * scale);
                        cairo_fill(cr);
					}
				}
				break;
		}
	}
	else if (cfg.vis_type == VIS_SCOPE)
	{
		for (x = 0; x < 38; x++)
		{
			h = svis->vs_data[x << 1] / 3;
            int color_idx = svis_scope_colors[h];
            cairo_set_source_rgb(cr, svis_color[color_idx][0]/255.0, svis_color[color_idx][1]/255.0, svis_color[color_idx][2]/255.0);
            cairo_rectangle(cr, svis->vs_widget.x + x * scale, svis->vs_widget.y + (4 - h) * scale, scale, scale);
            cairo_fill(cr);
		}
	}
    cairo_restore(cr);
}

void svis_clear_data(SVis * svis)
{
	gint i;
	for (i = 0; i < 75; i++)
	{
		svis->vs_data[i] = (cfg.vis_type == VIS_SCOPE) ? 6 : 0;
	}
}

void svis_clear(SVis * svis)
{
    draw_widget(svis);
}

SVis *create_svis(GList ** wlist, cairo_surface_t * parent, gint x, gint y)
{
	SVis *svis;

	svis = (SVis *) g_malloc0(sizeof (SVis));
	svis->vs_widget.parent = parent;
	svis->vs_widget.x = x;
	svis->vs_widget.y = y;
	svis->vs_widget.width = SVIS_WIDTH;
	svis->vs_widget.height = SVIS_HEIGHT;
	svis->vs_widget.visible = 1;
    svis->vs_widget.draw = (void (*) (void *, cairo_t *)) svis_draw;

	add_widget(wlist, svis);
	return svis;
}
