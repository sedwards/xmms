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
#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "configfile.h"

static ConfigSection *xmms_cfg_create_section(ConfigFile * cfg, gchar * name);
static ConfigLine *xmms_cfg_create_string(ConfigSection * section, gchar * key, gchar * value);
static ConfigSection *xmms_cfg_find_section(ConfigFile * cfg, gchar * name);
static ConfigLine *xmms_cfg_find_string(ConfigSection * section, gchar * key);

ConfigFile *xmms_cfg_new(void)
{
	ConfigFile *cfg;

	cfg = g_malloc0(sizeof (ConfigFile));

	return cfg;
}

ConfigFile *xmms_cfg_open_file(gchar * filename)
{
	ConfigFile *cfg;

	FILE *file;
	gchar *buffer, **lines, *tmp;
	gint i;
	struct stat stats;
	ConfigSection *section = NULL;

	if (lstat(filename, &stats) == -1)
		return NULL;
	if (!(file = fopen(filename, "r")))
		return NULL;

	buffer = g_malloc(stats.st_size + 1);
	if (fread(buffer, 1, stats.st_size, file) != stats.st_size)
	{
		g_free(buffer);
		fclose(file);
		return NULL;
	}
	fclose(file);
	buffer[stats.st_size] = '\0';

	cfg = g_malloc0(sizeof (ConfigFile));
	lines = g_strsplit(buffer, "\n", 0);
	g_free(buffer);
	i = 0;
	while (lines[i])
	{
		if (lines[i][0] == '[')
		{
			if ((tmp = strchr(lines[i], ']')))
			{
				*tmp = '\0';
				section = xmms_cfg_create_section(cfg, &lines[i][1]);
			}
		}
		else if (lines[i][0] != '#' && section)
		{
			if ((tmp = strchr(lines[i], '=')))
			{
				*tmp = '\0';
				tmp++;
				xmms_cfg_create_string(section, lines[i], tmp);
			}
		}
		i++;
	}
	g_strfreev(lines);
	return cfg;
}

gchar * xmms_cfg_get_default_filename(void)
{
	static gchar *filename = NULL;
	if (!filename)
		filename = g_strconcat(g_get_home_dir(), "/.xmms/config", NULL);
	return filename;
}

ConfigFile * xmms_cfg_open_default_file(void)
{
	ConfigFile *ret;

	ret = xmms_cfg_open_file(xmms_cfg_get_default_filename());
	if (!ret)
		ret = xmms_cfg_new();
	return ret;
}

gboolean xmms_cfg_write_file(ConfigFile * cfg, gchar * filename)
{
	FILE *file;
	GList *section_list, *line_list;
	ConfigSection *section;
	ConfigLine *line;

	if (!(file = fopen(filename, "w")))
		return FALSE;

	section_list = cfg->sections;
	while (section_list)
	{
		section = (ConfigSection *) section_list->data;
		if (section->lines)
		{
			fprintf(file, "[%s]\n", section->name);
			line_list = section->lines;
			while (line_list)
			{
				line = (ConfigLine *) line_list->data;
				fprintf(file, "%s=%s\n", line->key, line->value);
				line_list = g_list_next(line_list);
			}
			fprintf(file, "\n");
		}
		section_list = g_list_next(section_list);
	}
	fclose(file);
	return TRUE;
}

gboolean xmms_cfg_write_default_file(ConfigFile * cfg)
{
	return xmms_cfg_write_file(cfg, xmms_cfg_get_default_filename());
}

gboolean xmms_cfg_read_string(ConfigFile * cfg, gchar * section, gchar * key, gchar ** value)
{
	ConfigSection *sect;
	ConfigLine *line;

	if (!(sect = xmms_cfg_find_section(cfg, section)))
		return FALSE;
	if (!(line = xmms_cfg_find_string(sect, key)))
		return FALSE;
	*value = g_strdup(line->value);
	return TRUE;
}

gboolean xmms_cfg_read_int(ConfigFile * cfg, gchar * section, gchar * key, gint * value)
{
	gchar *str;

	if (!xmms_cfg_read_string(cfg, section, key, &str))
		return FALSE;
	*value = atoi(str);
	g_free(str);

	return TRUE;
}

gboolean xmms_cfg_read_boolean(ConfigFile * cfg, gchar * section, gchar * key, gboolean * value)
{
	gchar *str;

	if (!xmms_cfg_read_string(cfg, section, key, &str))
		return FALSE;
	if (!strcasecmp(str, "TRUE"))
		*value = TRUE;
	else
		*value = FALSE;
	g_free(str);
	return TRUE;
}

gboolean xmms_cfg_read_float(ConfigFile * cfg, gchar * section, gchar * key, gfloat * value)
{
	gchar *str;

	if (!xmms_cfg_read_string(cfg, section, key, &str))
		return FALSE;

	*value = (gfloat) g_strtod(str, NULL);
	g_free(str);

	return TRUE;
}

gboolean xmms_cfg_read_double(ConfigFile * cfg, gchar * section, gchar * key, gdouble * value)
{
	gchar *str;

	if (!xmms_cfg_read_string(cfg, section, key, &str))
		return FALSE;

	*value = g_strtod(str, NULL);
	g_free(str);

	return TRUE;
}

void xmms_cfg_write_string(ConfigFile * cfg, gchar * section, gchar * key, gchar * value)
{
	ConfigSection *sect;
	ConfigLine *line;

	sect = xmms_cfg_find_section(cfg, section);
	if (!sect)
		sect = xmms_cfg_create_section(cfg, section);
	if ((line = xmms_cfg_find_string(sect, key)))
	{
		g_free(line->value);
		line->value = g_strchug(g_strchomp(g_strdup(value)));
	}
	else
		xmms_cfg_create_string(sect, key, value);
}

void xmms_cfg_write_int(ConfigFile * cfg, gchar * section, gchar * key, gint value)
{
	gchar *strvalue;

	strvalue = g_strdup_printf("%d", value);
	xmms_cfg_write_string(cfg, section, key, strvalue);
	g_free(strvalue);
}

void xmms_cfg_write_boolean(ConfigFile * cfg, gchar * section, gchar * key, gboolean value)
{
	if (value)
		xmms_cfg_write_string(cfg, section, key, "TRUE");
	else
		xmms_cfg_write_string(cfg, section, key, "FALSE");
}

void xmms_cfg_write_float(ConfigFile * cfg, gchar * section, gchar * key, gfloat value)
{
	gchar *strvalue;

	strvalue = g_strdup_printf("%g", value);
	xmms_cfg_write_string(cfg, section, key, strvalue);
	g_free(strvalue);
}

void xmms_cfg_write_double(ConfigFile * cfg, gchar * section, gchar * key, gdouble value)
{
	gchar *strvalue;

	strvalue = g_strdup_printf("%g", value);
	xmms_cfg_write_string(cfg, section, key, strvalue);
	g_free(strvalue);
}

void xmms_cfg_remove_key(ConfigFile * cfg, gchar * section, gchar * key)
{
	ConfigSection *sect;
	ConfigLine *line;

	if ((sect = xmms_cfg_find_section(cfg, section)) != NULL)
	{
		if ((line = xmms_cfg_find_string(sect, key)) != NULL)
		{
			g_free(line->key);
			g_free(line->value);
			g_free(line);
			sect->lines = g_list_remove(sect->lines, line);
		}
	}
}

void xmms_cfg_free(ConfigFile * cfg)
{
	ConfigSection *section;
	ConfigLine *line;
	GList *section_list, *line_list;

	section_list = cfg->sections;
	while (section_list)
	{
		section = (ConfigSection *) section_list->data;
		g_free(section->name);

		line_list = section->lines;
		while (line_list)
		{
			line = (ConfigLine *) line_list->data;
			g_free(line->key);
			g_free(line->value);
			g_free(line);
			line_list = g_list_next(line_list);
		}
		g_list_free(section->lines);
		g_free(section);

		section_list = g_list_next(section_list);
	}
	g_list_free(cfg->sections);
}

static ConfigSection *xmms_cfg_create_section(ConfigFile * cfg, gchar * name)
{
	ConfigSection *section;

	section = g_malloc0(sizeof (ConfigSection));
	section->name = g_strdup(name);
	cfg->sections = g_list_append(cfg->sections, section);

	return section;
}

static ConfigLine *xmms_cfg_create_string(ConfigSection * section, gchar * key, gchar * value)
{
	ConfigLine *line;

	line = g_malloc0(sizeof (ConfigLine));
	line->key = g_strchug(g_strchomp(g_strdup(key)));
	line->value = g_strchug(g_strchomp(g_strdup(value)));
	section->lines = g_list_append(section->lines, line);

	return line;
}

static ConfigSection *xmms_cfg_find_section(ConfigFile * cfg, gchar * name)
{
	ConfigSection *section;
	GList *list;

	list = cfg->sections;
	while (list)
	{
		section = (ConfigSection *) list->data;
		if (!strcasecmp(section->name, name))
			return section;
		list = g_list_next(list);
	}
	return NULL;
}

static ConfigLine *xmms_cfg_find_string(ConfigSection * section, gchar * key)
{
	ConfigLine *line;
	GList *list;

	list = section->lines;
	while (list)
	{
		line = (ConfigLine *) list->data;
		if (!strcasecmp(line->key, key))
			return line;
		list = g_list_next(list);
	}
	return NULL;
}
