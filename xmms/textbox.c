/*  XMMS - Cross-platform multimedia player
 *  Copyright (C) 1998-2001  Peter Alm, Mikael Alm, Olle Hallnas,
 *                           Thomas Nilsson and 4Front Technologies
 *  Copyright (C) 1999-2001  Haavard Kvaalen
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
#include <ctype.h>

static void textbox_generate_pixmap(TextBox * tb);

static void textbox_draw(TextBox * tb, cairo_t *cr)
{
	gint cw;
	cairo_surface_t *src;

	if (tb->tb_text &&
	    (!tb->tb_pixmap_text ||
	     strcmp(tb->tb_text, tb->tb_pixmap_text)))
		textbox_generate_pixmap(tb);

	if (tb->tb_pixmap)
	{
		if (skin_get_id() != tb->tb_skin_id)
		{
			tb->tb_skin_id = skin_get_id();
			textbox_generate_pixmap(tb);
		}
		src = tb->tb_pixmap;

		cw = tb->tb_pixmap_width - tb->tb_offset;
		if (cw > tb->tb_widget.width)
			cw = tb->tb_widget.width;

        /* Draw first part */
        cairo_save(cr);
        cairo_rectangle(cr, tb->tb_widget.x, tb->tb_widget.y, cw, tb->tb_widget.height);
        cairo_clip(cr);
        cairo_set_source_surface(cr, src, tb->tb_widget.x - tb->tb_offset, tb->tb_widget.y);
        cairo_paint(cr);
        cairo_restore(cr);

		if (cw < tb->tb_widget.width)
		{
            /* Draw wrapped part */
            cairo_save(cr);
            cairo_rectangle(cr, tb->tb_widget.x + cw, tb->tb_widget.y, tb->tb_widget.width - cw, tb->tb_widget.height);
            cairo_clip(cr);
            cairo_set_source_surface(cr, src, tb->tb_widget.x + cw, tb->tb_widget.y);
            cairo_paint(cr);
            cairo_restore(cr);
		}
	}
}

static gint textbox_scroll(gpointer data)
{
	TextBox *tb = (TextBox *) data;

	if (!tb->tb_is_dragging)
	{
		if (cfg.smooth_title_scroll)
			tb->tb_offset++;
		else
			tb->tb_offset += 5;
		if (tb->tb_offset >= tb->tb_pixmap_width)
			tb->tb_offset -= tb->tb_pixmap_width;
		draw_widget(tb);
	}
	return TRUE;
}

static void textbox_button_press(GtkWidget * w, GdkEventButton * event, TextBox * tb)
{
	if (event->button != 1)
		return;
	if (inside_widget(event->x, event->y, &tb->tb_widget) &&
	    tb->tb_scroll_allowed &&
	    tb->tb_pixmap_width > tb->tb_widget.width && tb->tb_is_scrollable)
	{
		tb->tb_is_dragging = TRUE;
		tb->tb_drag_off = tb->tb_offset;
		tb->tb_drag_x = event->x;
	}
}

static void textbox_motion(GtkWidget * w, GdkEventMotion * event, TextBox * tb)
{
	if (tb->tb_is_dragging)
	{
		if (tb->tb_scroll_allowed &&
		    tb->tb_pixmap_width > tb->tb_widget.width)
		{
			tb->tb_offset = tb->tb_drag_off - (event->x - tb->tb_drag_x);
			while (tb->tb_offset < 0)
				tb->tb_offset += tb->tb_pixmap_width;
			while (tb->tb_offset > tb->tb_pixmap_width)
				tb->tb_offset -= tb->tb_pixmap_width;
			draw_widget(tb);
		}
	}
}

static void textbox_button_release(GtkWidget * w, GdkEventButton * event, TextBox * tb)
{
	if (event->button == 1)
		tb->tb_is_dragging = FALSE;
}

static gboolean textbox_should_scroll(TextBox *tb)
{
	if (!tb->tb_scroll_allowed)
		return FALSE;

	if (tb->tb_font)
	{
        /* TODO: Use Pango layout to get width if font is active */
        return TRUE; 
	}

	if (strlen(tb->tb_text) * 5 > tb->tb_widget.width)
		return TRUE;

	return FALSE;
}

void textbox_set_text(TextBox * tb, gchar * text)
{
    lock_widget(tb);
	
	if (tb->tb_text)
	{
		if (!strcmp(text, tb->tb_text))
		{
		    unlock_widget(tb);
			return;
		}
		g_free(tb->tb_text);
	}

	tb->tb_text = g_strdup(text);

	unlock_widget(tb);
	draw_widget(tb);
}

static void textbox_generate_xfont_pixmap(TextBox * tb, gchar *pixmaptext)
{
    /* STUB: Implement Pango rendering here */
    tb->tb_pixmap_width = tb->tb_widget.width;
    tb->tb_pixmap = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, tb->tb_pixmap_width, tb->tb_widget.height);
}

static void textbox_handle_special_char(char c, int *x, int *y)
{
	switch (c)
	{
		case '"': *x = 130; *y = 0; break;
		case ':': *x = 60; *y = 6; break;
		case '(': *x = 65; *y = 6; break;
		case ')': *x = 70; *y = 6; break;
		case '-': *x = 75; *y = 6; break;
		case '`':
		case '\'': *x = 80; *y = 6; break;
		case '!': *x = 85; *y = 6; break;
		case '_': *x = 90; *y = 6; break;
		case '+': *x = 95; *y = 6; break;
		case '\\': *x = 100; *y = 6; break;
		case '/': *x = 105; *y = 6; break;
		case '[': *x = 110; *y = 6; break;
		case ']': *x = 115; *y = 6; break;
		case '^': *x = 120; *y = 6; break;
		case '&': *x = 125; *y = 6; break;
		case '%': *x = 130; *y = 6; break;
		case '.':
		case ',': *x = 135; *y = 6; break;
		case '=': *x = 140; *y = 6; break;
		case '$': *x = 145; *y = 6; break;
		case '#': *x = 150; *y = 6; break;
		case '?': *x = 15; *y = 12; break;
		case '*': *x = 20; *y = 12; break;
		default: *x = 145; *y = 0; break;
	}
}

static void textbox_generate_pixmap(TextBox * tb)
{
	gint length, i, x, y, wl;
	gchar *pixmaptext;
	cairo_t *cr;

	if (tb->tb_pixmap)
		cairo_surface_destroy(tb->tb_pixmap);
	tb->tb_pixmap = NULL;

	if (!(tb->tb_pixmap_text && strrchr(tb->tb_text, '(') &&
	      !strncmp(tb->tb_pixmap_text, tb->tb_text,
		       strrchr(tb->tb_text, '(') - tb->tb_text)))
		tb->tb_offset = 0;

	g_free(tb->tb_pixmap_text);
	tb->tb_pixmap_text = g_strdup(tb->tb_text);

	wl = tb->tb_widget.width / 5;
	if (wl * 5 != tb->tb_widget.width)
		wl++;

	length = strlen(tb->tb_text);
	tb->tb_is_scrollable = FALSE;

	if (textbox_should_scroll(tb))
	{
		tb->tb_is_scrollable = TRUE;
		pixmaptext = g_strconcat(tb->tb_pixmap_text, "  ***  ", NULL);
		length += 7;
	}
	else if (!tb->tb_font && length <= wl)
	{
		gint pad = wl - length;
		char *padchars = g_strnfill(pad, ' ');
		pixmaptext = g_strconcat(tb->tb_pixmap_text, padchars , NULL);
		g_free(padchars);
		length += pad;		
	}
	else
		pixmaptext = g_strdup(tb->tb_pixmap_text);

	if (tb->tb_is_scrollable)
	{
		if (tb->tb_scroll_enabled && !tb->tb_timeout_tag)
		{
			int tag = cfg.smooth_title_scroll ? TEXTBOX_SCROLL_SMOOTH_TIMEOUT : TEXTBOX_SCROLL_TIMEOUT;
			tb->tb_timeout_tag = g_timeout_add(tag, textbox_scroll, tb);
		}
	}
	else
	{
		if (tb->tb_timeout_tag)
		{
			g_source_remove(tb->tb_timeout_tag);
			tb->tb_timeout_tag = 0;
		}
		tb->tb_offset = 0;
	}

	if (tb->tb_font)
	{
		textbox_generate_xfont_pixmap(tb, pixmaptext);
		g_free(pixmaptext);
		return;
	}

	tb->tb_pixmap_width = length * 5;
	tb->tb_pixmap = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, tb->tb_pixmap_width, 6);
	cr = cairo_create(tb->tb_pixmap);

	for (i = 0; i < length; i++)
	{
		char c;
		x = y = -1;
		c = toupper(pixmaptext[i]);
		if (c >= 'A' && c <= 'Z')
		{
			x = 5 * (c - 'A');
			y = 0;
		}
		else if (c >= '0' && c <= '9')
		{
			x = 5 * (c - '0');
			y = 6;
		}
		else
			textbox_handle_special_char(c, &x, &y);

		skin_draw_pixmap(cr, tb->tb_skin_index, x, y, i * 5, 0, 5, 6);
	}
    cairo_destroy(cr);
	g_free(pixmaptext);
}

void textbox_set_scroll(TextBox * tb, gboolean s)
{
	tb->tb_scroll_enabled = s;
	if (tb->tb_scroll_enabled && tb->tb_is_scrollable && tb->tb_scroll_allowed)
	{
		int tag = cfg.smooth_title_scroll ? TEXTBOX_SCROLL_SMOOTH_TIMEOUT : TEXTBOX_SCROLL_TIMEOUT;
		tb->tb_timeout_tag = g_timeout_add(tag, textbox_scroll, tb);
	}
	else
	{
		if (tb->tb_timeout_tag)
		{
			g_source_remove(tb->tb_timeout_tag);
			tb->tb_timeout_tag = 0;
		}
		tb->tb_offset = 0;
		draw_widget(tb);
	}
}

void textbox_set_xfont(TextBox *tb, gboolean use_xfont, gchar *fontname)
{
	if (tb->tb_font)
		pango_font_description_free(tb->tb_font);
	tb->tb_font = NULL;
	tb->tb_widget.y = tb->tb_nominal_y;
	tb->tb_widget.height = tb->tb_nominal_height;
	
	g_free(tb->tb_pixmap_text);
	tb->tb_pixmap_text = NULL;
	
	if (!use_xfont || strlen(fontname) == 0)
		return;
    
    tb->tb_font = pango_font_description_from_string(fontname);
    /* TODO: calculate height using Pango */
}

TextBox *create_textbox(GList ** wlist, cairo_surface_t * parent, gint x, gint y, gint w, gboolean allow_scroll, SkinIndex si)
{
	TextBox *tb;

	tb = g_malloc0(sizeof (TextBox));
	tb->tb_widget.parent = parent;
	tb->tb_widget.x = x;
	tb->tb_widget.y = y;
	tb->tb_widget.width = w;
	tb->tb_widget.height = 6;
	tb->tb_widget.visible = 1;
	tb->tb_widget.button_press_cb = (void (*) (GtkWidget *, GdkEventButton *, gpointer)) textbox_button_press;
	tb->tb_widget.button_release_cb = (void (*) (GtkWidget *, GdkEventButton *, gpointer)) textbox_button_release;
	tb->tb_widget.motion_cb = (void (*) (GtkWidget *, GdkEventMotion *, gpointer)) textbox_motion;
	tb->tb_widget.draw = (void (*) (void *, cairo_t *)) textbox_draw;
	tb->tb_scroll_allowed = allow_scroll;
	tb->tb_scroll_enabled = TRUE;
	tb->tb_skin_index = si;
	tb->tb_nominal_y = y;
	tb->tb_nominal_height = tb->tb_widget.height;
	add_widget(wlist, tb);
	return tb;
}

void free_textbox(TextBox * tb)
{
	if (tb->tb_pixmap)
		cairo_surface_destroy(tb->tb_pixmap);
	if (tb->tb_font)
		pango_font_description_free(tb->tb_font);	
	g_free(tb->tb_text);
	g_free(tb);
}
