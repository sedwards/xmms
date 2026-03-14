/*  XMMS - Cross-platform multimedia player
 *  Copyright (C) 1998-2001  Peter Alm, Mikael Alm, Olle Hallnas,
 *                           Thomas Nilsson and 4Front Technologies
 *  Copyright (C) 1999-2991  Haavard Kvaalen
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
#ifndef UTIL_H
#define UTIL_H

#include <gtk/gtk.h>
#include <cairo.h>

typedef struct {
    const gchar *path;
    const gchar *accelerator;
    void (*callback)(gpointer, guint, GtkWidget *);
    guint callback_action;
    const gchar *item_type;
    gconstpointer extra_data;
} GtkItemFactoryEntry;

#define GTK_ITEM_FACTORY(x) ((GtkItemFactory*)(x))

//typedef void (*GtkTranslateFunc) (const gchar *path, gpointer func_data);

GtkItemFactory* gtk_item_factory_new(GType container_type, const gchar *path, GtkAccelGroup *accel_group);
GtkWidget* gtk_item_factory_get_widget(GtkItemFactory *ifactory, const gchar *path);
void gtk_item_factory_create_items(GtkItemFactory *ifactory, guint n_entries, gpointer entries, gpointer callback_data);
void gtk_item_factory_set_translate_func(GtkItemFactory *ifactory, GtkTranslateFunc func, gpointer data, GDestroyNotify notify);
GtkItemFactory* gtk_item_factory_from_widget(GtkWidget *widget);
void gtk_item_factory_dump_rc(const gchar *file_name, const gchar *resource_name, gboolean pass_id);
void gtk_item_factory_parse_rc(const gchar *file_name);

#include "i18n.h"

gchar *find_file_recursively(const gchar *dirname, const gchar *file);
void del_directory(const gchar *dirname);
cairo_surface_t *create_dblsize_image(cairo_surface_t * img);
gchar *read_ini_string(const gchar *filename, const gchar *section, const gchar *key);
GArray *read_ini_array(const gchar * filename, const gchar * section, const gchar * key);
GArray *string_to_garray(const gchar * str);
void glist_movedown(GList * list);
void glist_moveup(GList * list);

const char *util_basename(const char *name);

void util_item_factory_popup(GtkItemFactory * ifactory, guint x, guint y, guint mouse_button, guint32 time);
void util_item_factory_popup_with_data(GtkItemFactory * ifactory, gpointer data, GDestroyNotify destroy, guint x, guint y, guint mouse_button, guint32 time);

GtkWidget *util_create_add_url_window(gchar *caption, GCallback ok_func, GCallback enqueue_func);

GtkWidget *util_create_filebrowser(gboolean clear_pl_on_ok);
gboolean util_filebrowser_is_dir(void * filesel);

void *util_font_load(gchar *name);
void util_set_cursor(GtkWidget *window);
void util_dump_menu_rc(void);
void util_read_menu_rc(void);

#ifdef ENABLE_NLS
    gchar* util_menu_translate(const gchar *path, gpointer func_data);
#else
#   define util_menu_translate NULL
#endif

#if defined(USE_DMALLOC)
void g_free_func(gpointer mem);
#else
#define g_free_func g_free
#endif

#endif
