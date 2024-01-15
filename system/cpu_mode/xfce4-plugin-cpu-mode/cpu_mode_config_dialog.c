#include "cpu_mode_config_dialog.h"

#include <gtk/gtk.h>
#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4util/libxfce4util.h>



void cpu_mode_config_dialog_response (GtkWidget *dlg, int response, CpuModePlugin *cpu_mode) {
	switch (response) {
		case GTK_RESPONSE_OK:
			cpu_mode_config_write(cpu_mode->config);
			cpu_mode_config_free(cpu_mode);
			xfce_panel_plugin_unblock_menu(cpu_mode->plugin);
			gtk_widget_destroy(dlg);
			break;

		case GTK_RESPONSE_DELETE_EVENT: // Dialog was closed
		case GTK_RESPONSE_CANCEL:
			cpu_mode_config_free(cpu_mode);
			xfce_panel_plugin_unblock_menu(cpu_mode->plugin);
			gtk_widget_destroy(dlg);
			break;

		default:
			fprintf(stderr, "Unknown Dialog Response: %d\n", response);
			break;
	}
}

void cpu_mode_config_dialog_show(CpuModePlugin *cpu_mode) {
	cpu_mode_config_read(cpu_mode);

	GtkWidget *dialog;
	GtkWidget *vbox, *hbox_start_action;
	GtkGrid *governor_list;
	GtkWidget *lbl_start_governor;
	GtkWidget *lbl_governor_list;
	GtkWidget *inp_start_governor;

	xfce_panel_plugin_block_menu(cpu_mode->plugin);


	dialog = xfce_titled_dialog_new_with_mixed_buttons(
		"Configure CPU-Mode Plugin",
		GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(cpu_mode->plugin))),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		"window-close", "Save", GTK_RESPONSE_OK,
		"window-close", "Cancel", GTK_RESPONSE_CANCEL,
		NULL
	);
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	gtk_window_set_icon_name(GTK_WINDOW(dialog), "cpu_mode_50");

	// Main Layout
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), GTK_WIDGET(vbox), TRUE, TRUE, 0);

	// Starting action - whether to set governor on start
	hbox_start_action = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_box_pack_start(GTK_BOX(vbox), hbox_start_action, FALSE, FALSE, 0);

	lbl_start_governor = gtk_check_button_new_with_label("Set Governor on Start:");
	gtk_toggle_button_set_active((GtkToggleButton*) lbl_start_governor, cpu_mode->config->start_set_governor);
	gtk_box_pack_start(GTK_BOX(hbox_start_action), lbl_start_governor, FALSE, FALSE, 0);

	inp_start_governor = gtk_entry_new();
	gtk_entry_set_text((GtkEntry*) inp_start_governor, cpu_mode->config->start_governor);
	gtk_box_pack_start(GTK_BOX(hbox_start_action), inp_start_governor, FALSE, FALSE, 0);


	// List of governors
	char** governors = list_available_governors();

	lbl_governor_list = gtk_label_new("Active Governors:");
	gtk_box_pack_start(GTK_BOX(vbox), lbl_governor_list, FALSE, FALSE, 0);

	// governor_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
	governor_list = (GtkGrid*) gtk_grid_new();
	gtk_grid_set_column_spacing(governor_list, 16);
	gtk_box_pack_start(GTK_BOX(vbox), (GtkWidget*) governor_list, FALSE, FALSE, 0);

	gtk_grid_insert_column(governor_list, 0);
	gtk_grid_insert_column(governor_list, 0);
	gtk_grid_insert_column(governor_list, 0);


	// Grid Headers
	gtk_grid_insert_row(governor_list, 0);
	gtk_grid_attach(governor_list, gtk_label_new("Governor"), 0, 0, 1, 1);
	gtk_grid_attach(governor_list, gtk_label_new("Switch"), 1, 0, 1, 1);
	gtk_grid_attach(governor_list, gtk_label_new("Menu"), 2, 0, 1, 1);

	char **govs = governors;
	int row = 1;
	while (*govs) {
		gtk_grid_insert_row(governor_list, row);

		GtkToggleButton* chk_switch = (GtkToggleButton*) gtk_check_button_new();
		// gtk_toggle_button_set_mode(chk_switch, FALSE);
		gtk_toggle_button_set_active(chk_switch, cpu_mode->config->active_governors_switch == NULL || array_contains(cpu_mode->config->active_governors_switch, *govs));

		GtkToggleButton* chk_menu = (GtkToggleButton*) gtk_check_button_new();
		// gtk_toggle_button_set_mode(chk_menu, FALSE);
		gtk_toggle_button_set_active(chk_menu, cpu_mode->config->active_governors_menu == NULL || array_contains(cpu_mode->config->active_governors_menu, *govs));

		GtkWidget* lbl_governor = gtk_label_new(*govs);
		
		gtk_grid_insert_row(governor_list, 0);
		gtk_grid_attach(governor_list, (GtkWidget*) lbl_governor, 0, row + 1, 1, 1);
		gtk_grid_attach(governor_list, (GtkWidget*) chk_menu, 1, row + 1, 1, 1);
		gtk_grid_attach(governor_list, (GtkWidget*) chk_switch, 2, row + 1, 1, 1);
		
		govs++;
		row++;
	}
	free_array(governors);


    g_signal_connect(dialog, "response", G_CALLBACK (cpu_mode_config_dialog_response), cpu_mode);

	gtk_widget_show_all(dialog);

}
