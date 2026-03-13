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

void check_wm_hints(void)
{
    /* GTK 3 / macOS abstraction handles this internally */
}

gboolean hint_always_on_top_available(void)
{
	/* GTK 3 supports keep_above natively across platforms */
	return TRUE;
}

void hint_set_always(gboolean always)
{
	GList *node;

	node = dock_window_list;
	while (node)
	{
		if (GTK_IS_WINDOW(node->data))
		{
			gtk_window_set_keep_above(GTK_WINDOW(node->data), always);
		}
		node = node->next;
	}
}

void hint_set_sticky(gboolean sticky)
{
	GList *node;

	node = dock_window_list;
	while (node)
	{
		if (GTK_IS_WINDOW(node->data))
		{
			if (sticky)
				gtk_window_stick(GTK_WINDOW(node->data));
			else
				gtk_window_unstick(GTK_WINDOW(node->data));
		}
		node = node->next;
	}
}

void hint_set_skip_winlist(GtkWidget * window)
{
	if (GTK_IS_WINDOW(window))
	{
		gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window), TRUE);
		gtk_window_set_skip_pager_hint(GTK_WINDOW(window), TRUE);
	}
}
