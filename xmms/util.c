#include "xmms.h"
#include <sys/stat.h>

gchar *find_file_recursively(const gchar *dirname, const gchar *file)
{
	DIR *dir;
	struct dirent *dirent;
	gchar *path = NULL;

	if ((dir = opendir(dirname)) != NULL)
	{
		while ((dirent = readdir(dir)) != NULL && path == NULL)
		{
			if (strcmp(dirent->d_name, ".") && strcmp(dirent->d_name, ".."))
			{
				gchar *newdir = g_strconcat(dirname, "/", dirent->d_name, NULL);
				struct stat statbuf;

				if (stat(newdir, &statbuf) != -1)
				{
					if (S_ISDIR(statbuf.st_mode))
						path = find_file_recursively(newdir, file);
					else if (!strcasecmp(dirent->d_name, file))
						path = g_strdup(newdir);
				}
				g_free(newdir);
			}
		}
		closedir(dir);
	}
	return path;
}

void del_directory(const gchar *dirname)
{
	DIR *dir;
	struct dirent *dirent;
	gchar *file;

	if ((dir = opendir(dirname)) != NULL)
	{
		while ((dirent = readdir(dir)) != NULL)
		{
			if (strcmp(dirent->d_name, ".") && strcmp(dirent->d_name, ".."))
			{
				file = g_strdup_printf("%s/%s", dirname, dirent->d_name);
				if (unlink(file) == -1)
					if (errno == EISDIR)
						del_directory(file);
				g_free(file);
			}
		}
		closedir(dir);
	}
	rmdir(dirname);
}

cairo_surface_t *create_dblsize_image(cairo_surface_t * img)
{
	cairo_surface_t *dblimg;
    cairo_t *cr;
    int w, h;

    if (!img) return NULL;

    w = cairo_image_surface_get_width(img);
    h = cairo_image_surface_get_height(img);

    dblimg = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w << 1, h << 1);
    cr = cairo_create(dblimg);
    
    cairo_scale(cr, 2.0, 2.0);
    cairo_set_source_surface(cr, img, 0, 0);
    cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_NEAREST);
    cairo_paint(cr);
    
    cairo_destroy(cr);

	return dblimg;
}

gchar *read_ini_string(const gchar *filename, const gchar *section, const gchar *key)
{
	FILE *file;
	char *buffer, *ret_buffer = NULL;
	int found_section = 0, found_key = 0, off = 0;
	struct stat statbuf;

	if (!filename)
		return NULL;

	if ((file = fopen(filename, "r")) == NULL)
		return NULL;

	stat(filename, &statbuf);
	buffer = g_malloc(statbuf.st_size);
	fread(buffer, 1, statbuf.st_size, file);
	while (!found_key && off < statbuf.st_size)
	{
		char *line = buffer + off;
		char *eol = strchr(line, '\n');
		int len;
		if (eol)
			len = eol - line;
		else
			len = strlen(line);

		if (line[0] == '[' && !found_key)
		{
			if (!strncasecmp(line + 1, section, strlen(section)) && line[strlen(section) + 1] == ']')
				found_section = 1;
			else
				found_section = 0;
		}
		if (found_section && !found_key)
		{
			if (!strncasecmp(line, key, strlen(key)) && line[strlen(key)] == '=')
			{
				ret_buffer = g_strndup(line + strlen(key) + 1, len - strlen(key) - 1);
				g_strchug(g_strchomp(ret_buffer));
				found_key = 1;
			}
		}
		off += len + 1;
	}
	g_free(buffer);
	fclose(file);
	return ret_buffer;
}

GArray *read_ini_array(const gchar * filename, const gchar * section, const gchar * key)
{
	gchar *str;
	GArray *a = NULL;

	if ((str = read_ini_string(filename, section, key)) != NULL)
	{
		a = string_to_garray(str);
		g_free(str);
	}
	return a;
}

GArray *string_to_garray(const gchar * str)
{
	GArray *a;
	gchar **sv, **ptr;
	gint val;

	a = g_array_new(FALSE, FALSE, sizeof (gint));
	sv = g_strsplit(str, ",", -1);
	ptr = sv;
	while (*ptr)
	{
		val = atoi(*ptr);
		g_array_append_val(a, val);
		ptr++;
	}
	g_strfreev(sv);
	return a;
}

void glist_movedown(GList * list)
{
	gpointer data;
	GList *next;

	if (list && (next = g_list_next(list)) != NULL)
	{
		data = list->data;
		list->data = next->data;
		next->data = data;
	}
}

void glist_moveup(GList * list)
{
	gpointer data;
	GList *prev;

	if (list && (prev = g_list_previous(list)) != NULL)
	{
		data = list->data;
		list->data = prev->data;
		prev->data = data;
	}
}

/* STUBS for GTK 3 / macOS port */

void util_item_factory_popup(GtkItemFactory * ifactory, guint x, guint y, guint mouse_button, guint32 time)
{
    if (!ifactory) return;
    gtk_menu_popup_at_pointer(GTK_MENU(ifactory), NULL);
}

void util_item_factory_popup_with_data(GtkItemFactory * ifactory, gpointer data, GDestroyNotify destroy, guint x, guint y, guint mouse_button, guint32 time)
{
    if (!ifactory) return;
    gtk_menu_popup_at_pointer(GTK_MENU(ifactory), NULL);
}

GtkWidget* util_create_add_url_window(gchar *caption, GCallback ok_func, GCallback enqueue_func)
{
    g_message("util_create_add_url_window: STUBBED");
    return NULL;
}

GtkWidget * util_create_filebrowser(gboolean clear_pl_on_ok)
{
    g_message("util_create_filebrowser: STUBBED (use macOS native dialog)");
    return NULL;
}

gboolean util_filebrowser_is_dir(void * filesel)
{
    return FALSE;
}

void *util_font_load(gchar *name)
{
    /* STUB: Use Pango in modern code */
    return NULL;
}

void util_set_cursor(GtkWidget *window)
{
    if (!window || !gtk_widget_get_window(window)) return;
    GdkCursor *cursor = gdk_cursor_new_for_display(gtk_widget_get_display(window), GDK_LEFT_PTR);
    gdk_window_set_cursor(gtk_widget_get_window(window), cursor);
    g_object_unref(cursor);
}

void util_dump_menu_rc(void) {}
void util_read_menu_rc(void) {}

#ifdef ENABLE_NLS
gchar* util_menu_translate(const gchar *path, gpointer func_data)
{
	char *translation = gettext(path);
	if (!translation || *translation != '/')
		translation = (char*) path;
	return translation;
}
#endif

#if defined(USE_DMALLOC)
void g_free_func (gpointer mem) { g_free (mem); }
#endif

/* GtkItemFactory Shims for GTK 3 */
GtkItemFactory* gtk_item_factory_new(GType container_type, const gchar *path, GtkAccelGroup *accel_group)
{
    return (GtkItemFactory*)gtk_menu_new();
}

GtkWidget* gtk_item_factory_get_widget(GtkItemFactory *ifactory, const gchar *path)
{
    /* STUB: Return the factory itself if it's a menu, or a dummy for paths */
    return GTK_WIDGET(ifactory);
}

void gtk_item_factory_create_items(GtkItemFactory *ifactory, guint n_entries, gpointer entries, gpointer callback_data) {}
void gtk_item_factory_set_translate_func(GtkItemFactory *ifactory, GtkTranslateFunc func, gpointer data, GDestroyNotify notify) {}
GtkItemFactory* gtk_item_factory_from_widget(GtkWidget *widget) { return NULL; }
void gtk_item_factory_dump_rc(const gchar *file_name, const gchar *resource_name, gboolean pass_id) {}
void gtk_item_factory_parse_rc(const gchar *file_name) {}
