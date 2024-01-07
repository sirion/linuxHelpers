
// #include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4panel/libxfce4panel.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>
#include <string.h>

typedef struct
{
    XfcePanelPlugin *plugin;

    /* panel widgets */
    GtkWidget *hvbox;
    GtkWidget *button_powersave;
    GtkWidget *button_schedutil;

    GtkWidget *tooltip;

} CpuModePlugin;

static void cpu_mode_plugin_create(XfcePanelPlugin *plugin);
static void cpu_mode_free(XfcePanelPlugin *plugin, CpuModePlugin *sample);
static gboolean cpu_mode_tooltip( GtkWidget *widget, gint x, gint y, gboolean keyboard, GtkTooltip * tooltip, CpuModePlugin *cpu_mode_plugin);
void *updateThread(void *vArgP);
gboolean on_click(GtkWidget *widget, GdkEventButton *event);

XFCE_PANEL_PLUGIN_REGISTER(cpu_mode_plugin_create);

CpuModePlugin *cpu_mode_plugin;
gboolean update_active = TRUE;
pthread_t update_thread_id;

static void cpu_mode_plugin_create(XfcePanelPlugin *plugin)
{

    cpu_mode_plugin = g_slice_new0(CpuModePlugin);
    cpu_mode_plugin->plugin = plugin;

    cpu_mode_plugin->button_powersave = gtk_button_new_from_icon_name("indicator-cpufreq-25", GTK_ICON_SIZE_LARGE_TOOLBAR);
    cpu_mode_plugin->button_schedutil = gtk_button_new_from_icon_name("indicator-cpufreq-75", GTK_ICON_SIZE_LARGE_TOOLBAR);

    g_signal_connect(cpu_mode_plugin->button_powersave, "button-press-event", G_CALLBACK(on_click), NULL);
    g_signal_connect(cpu_mode_plugin->button_schedutil, "button-press-event", G_CALLBACK(on_click), NULL);



    cpu_mode_plugin->hvbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
    gtk_container_add(GTK_CONTAINER(cpu_mode_plugin->hvbox), cpu_mode_plugin->button_powersave);
    gtk_container_add(GTK_CONTAINER(cpu_mode_plugin->hvbox), cpu_mode_plugin->button_schedutil);
    gtk_widget_show(cpu_mode_plugin->hvbox);


    gtk_widget_set_has_tooltip( cpu_mode_plugin->hvbox, TRUE);
    cpu_mode_plugin->tooltip = gtk_label_new( NULL );
	g_object_ref( cpu_mode_plugin->tooltip );
	g_signal_connect( cpu_mode_plugin->hvbox, "query-tooltip", G_CALLBACK( cpu_mode_tooltip ), cpu_mode_plugin );

    gtk_container_add(GTK_CONTAINER(plugin), cpu_mode_plugin->hvbox);
    xfce_panel_plugin_add_action_widget(plugin, cpu_mode_plugin->hvbox);

    
    pthread_create(&update_thread_id, NULL, updateThread, NULL);
    

    g_signal_connect(G_OBJECT(plugin), "free-data", G_CALLBACK(cpu_mode_free), cpu_mode_plugin);
    xfce_panel_plugin_set_expand(XFCE_PANEL_PLUGIN(plugin), FALSE);
}

char *read_cpu_governor()
{
    long lSize = 256;
    char *buffer;
    size_t result;
    FILE *fp = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", "r");

    if (fp == NULL)
    {
        fputs("File error\n", stderr);
        return "";
    }

    buffer = (char *)malloc(sizeof(char) * lSize);
    if (buffer == NULL)
    {
        fputs("Memory error\n", stderr);
        fclose(fp);
        return "";
    }

    result = fread(buffer, sizeof(char), lSize, fp);
    if (result < 1)
    {
        fputs("Reading error\n", stderr);
        fclose(fp);
        free(buffer);
        return "";
    }

    fclose(fp);

    // remove \n at the end
    buffer[result - 1] = 0;

    return buffer;
}

void updateLabel()
{
    char *gov = read_cpu_governor();

    gtk_label_set_text(GTK_LABEL(cpu_mode_plugin->tooltip), gov);

    if (strcmp(gov, "powersave") == 0)
    {
        gtk_widget_show(cpu_mode_plugin->button_powersave);
        gtk_widget_hide(cpu_mode_plugin->button_schedutil);
    }
    else
    {
        gtk_widget_show(cpu_mode_plugin->button_schedutil);
        gtk_widget_hide(cpu_mode_plugin->button_powersave);
    }
}

void *updateThread(void *vArgP)
{
    while (update_active)
    {
        updateLabel();
        sleep(2);
    }

    return NULL;
}

gboolean on_click(GtkWidget *widget, GdkEventButton *event)
{

    if (event->button == 1)
    {
        system("cpu_mode powersave schedutil");
        updateLabel();
        return TRUE;
    }
    return FALSE;
}

static void cpu_mode_free(XfcePanelPlugin *plugin, CpuModePlugin *cpu_mode_plugin)
{
    update_active = FALSE;
    pthread_join(update_thread_id, NULL);

    gtk_widget_destroy(cpu_mode_plugin->hvbox);
    gtk_widget_destroy((GtkWidget *)cpu_mode_plugin->button_powersave);
    gtk_widget_destroy((GtkWidget *)cpu_mode_plugin->button_schedutil);

    g_slice_free(CpuModePlugin, cpu_mode_plugin);
}

static gboolean cpu_mode_tooltip( GtkWidget *widget, gint x, gint y, gboolean keyboard, GtkTooltip * tooltip, CpuModePlugin *cpu_mode_plugin)
{
	gtk_tooltip_set_custom( tooltip, cpu_mode_plugin->tooltip );
	return TRUE;
}
