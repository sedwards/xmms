#include <config.h>
#include <gtk/gtk.h>
#include "gtk3_compat.h"
#include "xmms/i18n.h"
#include "xmms/plugin.h"
#include "libxmms/util.h"
#include "libxmms/configfile.h"

static void stereo_init(void);
static void stereo_about(void);
static void stereo_configure(void);
static int stereo_mod_samples(gpointer *data, gint length, AFormat fmt, gint srate, gint nch);

EffectPlugin stereo_ep =
{
	NULL,
	NULL,
	NULL,
	stereo_init,
	NULL,
	stereo_about,
	stereo_configure,
	stereo_mod_samples,
    NULL
};

static gfloat value = 1.0;
static GtkWidget *conf_dialog;

static const char *about_text =
N_("Extra Stereo Plugin\n"
   "By Johan Levin 1999");

EffectPlugin *get_eplugin_info(void)
{
	stereo_ep.description = g_strdup(_("Extra Stereo Plugin"));
	return &stereo_ep;
}

static void stereo_init(void)
{
	ConfigFile *cfg;

	if ((cfg = xmms_cfg_open_default_file()) != NULL)
	{
		xmms_cfg_read_float(cfg, "stereo_plugin", "value", &value);
		xmms_cfg_free(cfg);
	}
}

static void stereo_about(void)
{
	static GtkWidget *about_dialog = NULL;

	if (about_dialog != NULL)
		return;

	about_dialog = xmms_show_message(_("About Extra Stereo Plugin"),
					  _(about_text), _("Ok"), FALSE,
					  NULL, NULL);
	g_signal_connect(about_dialog, "destroy",
			   G_CALLBACK(gtk_widget_destroyed), &about_dialog);
}

static void apply_changes(GtkAdjustment *adj)
{
	ConfigFile *cfg;

	value = gtk_adjustment_get_value(adj);
	cfg = xmms_cfg_open_default_file();
	xmms_cfg_write_float(cfg, "stereo_plugin", "value", value);
	xmms_cfg_write_default_file(cfg);
	xmms_cfg_free(cfg);
}

void stereo_configure(void)
{
	GtkAdjustment *adjustment;

	if (conf_dialog)
	{
		gdk_window_raise(gtk_widget_get_window(conf_dialog));
		return;
	}

	conf_dialog = gtk_dialog_new_with_buttons(_("Extra Stereo Configuration"),
                                             NULL, GTK_DIALOG_MODAL,
                                             _("Ok"), GTK_RESPONSE_OK,
                                             _("Cancel"), GTK_RESPONSE_CANCEL,
                                             NULL);

	g_signal_connect(conf_dialog, "destroy",
			   G_CALLBACK(gtk_widget_destroyed), &conf_dialog);

    GtkWidget *table = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(table), 5);
    gtk_grid_set_column_spacing(GTK_GRID(table), 5);
	gtk_container_set_border_width(GTK_CONTAINER(table), 5);
	gtk_box_pack_start(GTK_BOX(gtk_dialog_vbox(conf_dialog)), table,
			   TRUE, TRUE, 0);

	GtkWidget *label = gtk_label_new(_("Effect intensity:"));
	gtk_grid_attach(GTK_GRID(table), label, 0, 0, 1, 1);

	adjustment = gtk_adjustment_new(value, 0.0, 16.0, 0.1, 1.0, 1.0);
	GtkWidget *scale = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, adjustment);
	gtk_scale_set_digits(GTK_SCALE(scale), 1);
    gtk_widget_set_hexpand(scale, TRUE);
	gtk_grid_attach(GTK_GRID(table), scale, 1, 0, 1, 1);

	gtk_widget_show_all(conf_dialog);
    
    if (gtk_dialog_run(GTK_DIALOG(conf_dialog)) == GTK_RESPONSE_OK) {
        apply_changes(adjustment);
    }
    gtk_widget_destroy(conf_dialog);
    conf_dialog = NULL;
}

static int stereo_mod_samples(gpointer *data, gint length, AFormat fmt, gint srate, gint nch)
{
	gint i, num_samples;
	gint16 *ptr = (gint16 *) *data;
	gfloat l, r, avg;

	if (value == 1.0 || nch != 2 || (fmt != FMT_S16_LE && fmt != FMT_S16_BE && fmt != FMT_S16_NE))
		return length;

    num_samples = length / 4; /* 2 channels * 2 bytes */

	for (i = 0; i < num_samples; i++)
	{
		l = ptr[i*2];
		r = ptr[i*2+1];
		avg = (l + r) / 2;
		l -= avg;
		r -= avg;
		l *= value;
		r *= value;
		ptr[i*2] = (gint16)(l + avg);
		ptr[i*2+1] = (gint16)(r + avg);
	}
    return length;
}
