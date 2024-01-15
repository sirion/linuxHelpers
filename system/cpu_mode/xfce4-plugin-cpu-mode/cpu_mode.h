#ifndef __CPU_MODE_H__
#define __CPU_MODE_H__

#include <libxfce4panel/libxfce4panel.h>
#include <gtk/gtk.h>

typedef struct
{
	gboolean start_set_governor;
	char* start_governor;
	gchar** active_governors_menu;
	gchar** active_governors_switch;
} CpuModeConfig;


typedef struct
{
	XfcePanelPlugin *plugin;
	CpuModeConfig *config;
	GtkWidget *hvbox;
	GtkWidget *button_schedutil_ondemand;
	GtkWidget *button_powersave;
	GtkWidget *button_conservative;
	GtkWidget *button_performance;
	GtkWidget *button_unknown;
	GtkWidget *tooltip;
} CpuModePlugin;


////// Plugin Lifecycle

void cpu_mode_plugin_create(XfcePanelPlugin *plugin);

void cpu_mode_free(XfcePanelPlugin *plugin, CpuModePlugin *cpu_mode);


////// Panel Events

gboolean on_click(GtkWidget *widget, GdkEventButton *event);

gboolean cpu_mode_tooltip( GtkWidget *widget, gint x, gint y, gboolean keyboard, GtkTooltip * tooltip, CpuModePlugin *cpu_mode_plugin);

void update_label();


////// Dialogs

void show_about(XfcePanelPlugin *plugin, CpuModePlugin *cpu_mode);

void show_config(XfcePanelPlugin *plugin, CpuModePlugin *cpu_mode);


////// Configuration

void cpu_mode_config_read(CpuModePlugin *cpu_mode);

void cpu_mode_config_write(CpuModeConfig* config);

void cpu_mode_config_free(CpuModePlugin* config);


////// Utility Functions

char** list_available_governors();

char* read_cpu_governor();

void free_array(char **array);

gboolean array_contains(gchar** array, char* str);

void create_governor_buttons(CpuModePlugin *cpu_mode);

void create_governor_menu(CpuModePlugin *cpu_mode);

void create_item(CpuModePlugin *cpu_mode, char* name);


#endif /* !__CPU_MODE_H__ */
