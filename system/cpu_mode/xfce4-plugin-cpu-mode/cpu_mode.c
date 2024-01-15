// https://developer.xfce.org/xfce4-panel/
#include "cpu_mode.h"
#include "cpu_mode_config_dialog.h"

#include <gtk/gtk.h>
#include <libxfce4util/libxfce4util.h>

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>

const char * const PACKAGE_VERSION = "0.1b";
const char * const PACKAGE_NAME = "cpu_mode";


XFCE_PANEL_PLUGIN_REGISTER(cpu_mode_plugin_create);

////// Shared Variables

CpuModePlugin *cpu_mode;
gboolean update_active = TRUE;
pthread_t update_thread_id;

////// Plugin Lifecycle

void cpu_mode_plugin_create(XfcePanelPlugin *plugin) {
	cpu_mode = g_slice_new0(CpuModePlugin);
	cpu_mode->plugin = plugin;

	cpu_mode_config_read(cpu_mode);
	cpu_mode->hvbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);

	create_governor_buttons(cpu_mode);
	create_governor_menu(cpu_mode);

	gtk_widget_show(cpu_mode->hvbox);

	gtk_widget_set_has_tooltip(cpu_mode->hvbox, TRUE);
	cpu_mode->tooltip = gtk_label_new(NULL);
	g_object_ref(cpu_mode->tooltip);
	g_signal_connect(cpu_mode->hvbox, "query-tooltip", G_CALLBACK(cpu_mode_tooltip), cpu_mode);

	gtk_container_add(GTK_CONTAINER(plugin), cpu_mode->hvbox);
	xfce_panel_plugin_add_action_widget(plugin, cpu_mode->hvbox);

	g_signal_connect(G_OBJECT(plugin), "free-data", G_CALLBACK(cpu_mode_free), cpu_mode);
	xfce_panel_plugin_set_expand(XFCE_PANEL_PLUGIN(plugin), FALSE);

	xfce_panel_plugin_menu_show_configure(plugin);
	g_signal_connect(plugin, "configure-plugin", G_CALLBACK(show_config), cpu_mode);

	xfce_panel_plugin_menu_show_about(plugin);
	g_signal_connect(plugin, "about", G_CALLBACK(show_about), cpu_mode);


	g_timeout_add(5000, (GSourceFunc)update_label, (gpointer)NULL);
	update_label();
}

void cpu_mode_free(XfcePanelPlugin *plugin, CpuModePlugin *cpu_mode) {
	update_active = FALSE;
	pthread_join(update_thread_id, NULL);

	gtk_widget_destroy(cpu_mode->hvbox);
	gtk_widget_destroy(cpu_mode->button_schedutil_ondemand);
	gtk_widget_destroy(cpu_mode->button_powersave);
	gtk_widget_destroy(cpu_mode->button_conservative);
	gtk_widget_destroy(cpu_mode->button_performance);
	gtk_widget_destroy(cpu_mode->button_unknown);

	g_slice_free(CpuModePlugin, cpu_mode);
}

////// Panel Events

void change_governor(const char *restrict governor) {
	char *cmd = malloc(sizeof(char) * 256);
	snprintf(cmd, 256, "cpu_mode %s", governor);
	system(cmd);
	update_label();
}

gboolean item_selected(GtkMenuItem *widget, void *event) {
	change_governor(gtk_menu_item_get_label(widget));
	return TRUE;
}

gboolean on_click(GtkWidget *widget, GdkEventButton *event) {
	if (event->button != 1) {
		return FALSE;
	}

	char *gov = read_cpu_governor();

	// TODO: Only switches between powersave and schedutil
	if (strncmp(gov, "powersave", 9) == 0) {
		change_governor("schedutil");
	} else {
		change_governor("powersave");
	}
	
	update_label();
	free(gov);
	return TRUE;
}

gboolean cpu_mode_tooltip(
	GtkWidget *widget, gint x, gint y, gboolean keyboard, GtkTooltip *tooltip, CpuModePlugin *cpu_mode
) {
	gtk_tooltip_set_custom(tooltip, cpu_mode->tooltip);
	return TRUE;
}

void update_label() {
	char *gov = read_cpu_governor();

	gtk_label_set_text(GTK_LABEL(cpu_mode->tooltip), gov);

	gtk_widget_hide(cpu_mode->button_powersave);
	gtk_widget_hide(cpu_mode->button_schedutil_ondemand);
	gtk_widget_hide(cpu_mode->button_conservative);
	gtk_widget_hide(cpu_mode->button_performance);
	gtk_widget_hide(cpu_mode->button_unknown);

	if (strcmp(gov, "powersave") == 0) {
		gtk_widget_show(cpu_mode->button_powersave);
	} else if (strcmp(gov, "schedutil") == 0) {
		gtk_widget_show(cpu_mode->button_schedutil_ondemand);
	} else if (strcmp(gov, "ondemand") == 0) {
		gtk_widget_show(cpu_mode->button_schedutil_ondemand);
	} else if (strcmp(gov, "conservative") == 0) {
		gtk_widget_show(cpu_mode->button_conservative);
	} else if (strcmp(gov, "performance") == 0) {
		gtk_widget_show(cpu_mode->button_performance);
	} else {
		gtk_widget_show(cpu_mode->button_unknown);
	}

	free(gov);
}

////// Dialogs

void show_about(XfcePanelPlugin *plugin, CpuModePlugin *cpu_mode) {
	gtk_show_about_dialog(NULL,
		"logo-icon-name", "cpu_mode_50",
		"version", PACKAGE_VERSION,
		"program-name", PACKAGE_NAME,
		"comments", "Change CPU governor",
		"website", "https://dev.himmelrath.net/projects/cpu_mdoe",
		NULL
	);

}

void show_config(XfcePanelPlugin *plugin, CpuModePlugin *cpu_mode) {
	cpu_mode_config_dialog_show(cpu_mode);
}


////// Configuration

void cpu_mode_config_read(CpuModePlugin *cpu_mode) {
	cpu_mode_config_free(cpu_mode);
	cpu_mode->config = g_slice_new0(CpuModeConfig);

	cpu_mode->config->start_set_governor = FALSE;
	cpu_mode->config->start_governor = "";
	cpu_mode->config->active_governors_menu = NULL;
	cpu_mode->config->active_governors_switch = NULL;

	gchar *file;
	if (!(file = xfce_panel_plugin_lookup_rc_file(cpu_mode->plugin))) {
		return;
	}

	XfceRc *rc = xfce_rc_simple_open(file, TRUE);
	g_free (file);

	if (!rc) {
		return;
	}

	cpu_mode->config->start_set_governor = xfce_rc_read_bool_entry(rc, "start_set_governor", cpu_mode->config->start_set_governor);
	
	const char *gov = xfce_rc_read_entry(rc, "start_governor", "");

	int len = 0;
	if (gov != NULL) {
		len = strlen(gov);
	}
	cpu_mode->config->start_governor = malloc(len + 1);
	strcpy(cpu_mode->config->start_governor, gov);

	cpu_mode->config->active_governors_menu = xfce_rc_read_list_entry(rc, "active_governors_menu", NULL);
	
	cpu_mode->config->active_governors_switch = xfce_rc_read_list_entry(rc, "active_governors_switch", NULL);

	xfce_rc_close (rc);
}

void cpu_mode_config_write(CpuModeConfig* config) {
	gchar *file;

	if (!(file = xfce_panel_plugin_save_location(cpu_mode->plugin, TRUE))) {
		return;
	}

	XfceRc *rc = xfce_rc_simple_open(file, FALSE);
	g_free (file);
	if (!rc) {
		fprintf(stderr, "NO RC FILE!!!!\n");
		return;
	}

	xfce_rc_write_bool_entry(rc, "start_set_governor", cpu_mode->config->start_set_governor);

	xfce_rc_write_entry(rc, "start_governor", cpu_mode->config->start_governor);

	if (cpu_mode->config->active_governors_menu != NULL) {
		xfce_rc_write_list_entry(rc, "active_governors_menu", cpu_mode->config->active_governors_menu, NULL);
	}

	if (cpu_mode->config->active_governors_switch != NULL) {
		xfce_rc_write_list_entry(rc, "active_governors_switch", cpu_mode->config->active_governors_switch, NULL);
	}

	xfce_rc_close (rc);
}

void cpu_mode_config_free(CpuModePlugin* cpu_mode) {
	if (cpu_mode == NULL || cpu_mode->config == NULL) {
		return;
	}

	if (cpu_mode->config->start_governor != NULL && strlen(cpu_mode->config->start_governor) != 0) {
		free(cpu_mode->config->start_governor);
	}

	if (cpu_mode->config->active_governors_menu != NULL) {
		g_strfreev(cpu_mode->config->active_governors_menu);
	}

	if (cpu_mode->config->active_governors_switch != NULL) {
		g_strfreev(cpu_mode->config->active_governors_switch);
	}

	g_free(cpu_mode->config);
	cpu_mode->config = NULL;
}


////// Utility Functions

char** list_available_governors() {
	FILE *fp = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors", "r");
	long int file_size = 1024;

	char *buf = malloc(file_size);
	fgets(buf, 1024, fp);

	size_t length = strlen(buf);
	if (length == 0) {
		return NULL;
	}

	// Find spaces to count governors
	int len = 0;
	for (size_t i = 0; i < length; i++) {
		if (buf[i] == ' ') {
			len++;
		}
	}
	
	if (len == 0) {
		return NULL;
	}

	char** governors = malloc(sizeof(char*) * (len + 1));
	
	int iG = 0;
	int gov_length = 0;
	size_t pos = 0;
	for (size_t i = 0; i < length; i++) {
		if (buf[i] == ' ' || buf[i] == '\n') {
			gov_length = i - pos;
			governors[iG] = malloc(gov_length + 1);
			strncpy(governors[iG], buf+pos, gov_length);
			governors[iG][gov_length] = '\0';
			iG++;
			pos = i + 1;
		}
	}

	governors[len] = NULL;

	free(buf);
	fclose(fp);

	return governors;
}

void create_governor_menu(CpuModePlugin *cpu_mode) {
	char **governors = list_available_governors();

	char **govs = governors;
	while (*govs) {
		create_item(cpu_mode, *govs);
		govs++;
	}

	free_array(governors);
}

void free_array(char **array) {
	char** a = array;
	while (*a) {
		free(*a++);
	}
	free(array);
}

gboolean array_contains(gchar** array, char* str) {
	while (array != NULL && *array) {
		if (str != NULL && strcmp(*array++, str) == 0) {
			return TRUE;
		}
	}
	return FALSE;
}

void create_item(CpuModePlugin *cpu_mode, char *name) {
	GtkWidget *item = gtk_menu_item_new_with_label(name);
	g_signal_connect(item, "activate", G_CALLBACK(item_selected), item);

	gtk_widget_show(item);
	xfce_panel_plugin_menu_insert_item(cpu_mode->plugin, (GtkMenuItem *)item);
}

void create_governor_buttons(CpuModePlugin *cpu_mode) {
	cpu_mode->button_powersave =
			gtk_button_new_from_icon_name("cpu_mode_25", GTK_ICON_SIZE_LARGE_TOOLBAR);
	cpu_mode->button_schedutil_ondemand =
			gtk_button_new_from_icon_name("cpu_mode_50", GTK_ICON_SIZE_LARGE_TOOLBAR);
	cpu_mode->button_conservative =
			gtk_button_new_from_icon_name("cpu_mode_75", GTK_ICON_SIZE_LARGE_TOOLBAR);
	cpu_mode->button_performance = gtk_button_new_from_icon_name(
			"cpu_mode_100", GTK_ICON_SIZE_LARGE_TOOLBAR);
	cpu_mode->button_unknown =
			gtk_button_new_from_icon_name("cpu_mode_90", GTK_ICON_SIZE_LARGE_TOOLBAR);

	g_signal_connect(cpu_mode->button_powersave, "button-press-event",
									 G_CALLBACK(on_click), NULL);
	g_signal_connect(cpu_mode->button_schedutil_ondemand,
									 "button-press-event", G_CALLBACK(on_click), NULL);
	g_signal_connect(cpu_mode->button_conservative, "button-press-event",
									 G_CALLBACK(on_click), NULL);
	g_signal_connect(cpu_mode->button_performance, "button-press-event",
									 G_CALLBACK(on_click), NULL);
	g_signal_connect(cpu_mode->button_unknown, "button-press-event",
									 G_CALLBACK(on_click), NULL);

	gtk_container_add(GTK_CONTAINER(cpu_mode->hvbox),
										cpu_mode->button_powersave);
	gtk_container_add(GTK_CONTAINER(cpu_mode->hvbox),
										cpu_mode->button_schedutil_ondemand);
	gtk_container_add(GTK_CONTAINER(cpu_mode->hvbox),
										cpu_mode->button_conservative);
	gtk_container_add(GTK_CONTAINER(cpu_mode->hvbox),
										cpu_mode->button_performance);
	gtk_container_add(GTK_CONTAINER(cpu_mode->hvbox),
										cpu_mode->button_unknown);
}

char *read_cpu_governor() {
	long lSize = 256;
	char *buffer;
	size_t result;
	FILE *fp =
			fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", "r");

	if (fp == NULL) {
		fputs("Error reading governor file "
					"/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor\n",
					stderr);
		return "";
	}

	buffer = (char *)malloc(sizeof(char) * lSize);
	if (buffer == NULL) {
		fputs("Could not allocate memoryr\n", stderr);
		fclose(fp);
		return "";
	}

	result = fread(buffer, sizeof(char), lSize, fp);
	if (result < 1) {
		fputs("Could not read from governor file "
					"/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor\n",
					stderr);
		fclose(fp);
		free(buffer);
		return "";
	}

	fclose(fp);

	// remove \n at the end
	buffer[result - 1] = 0;

	return buffer;
}
