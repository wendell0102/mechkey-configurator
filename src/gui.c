#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "gui.h"

/* ------------------------------------------------------------------ */
/* Helpers                                                              */
/* ------------------------------------------------------------------ */

void gui_status(AppState *app, const char *fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    gtk_statusbar_pop(GTK_STATUSBAR(app->statusbar), app->status_ctx);
    gtk_statusbar_push(GTK_STATUSBAR(app->statusbar), app->status_ctx, buf);
}

static void on_device_connect(GtkWidget *widget, gpointer data) {
    AppState *app = (AppState *)data;
    const char *vid_s = gtk_entry_get_text(GTK_ENTRY(app->dev_vid_entry));
    const char *pid_s = gtk_entry_get_text(GTK_ENTRY(app->dev_pid_entry));

    uint16_t vid = (uint16_t)strtoul(vid_s, NULL, 16);
    uint16_t pid = (uint16_t)strtoul(pid_s, NULL, 16);

    if (app->dev) hid_keyboard_close(app->dev);

    app->dev = hid_keyboard_open(vid, pid);
    if (app->dev) {
        app->vid = vid;
        app->pid = pid;
        gui_status(app, "Connected to %04X:%04X", vid, pid);
        gtk_label_set_markup(GTK_LABEL(app->dev_status_label),
                             "<span color='green' weight='bold'>Connected</span>");
    } else {
        gui_status(app, "Failed to open %04X:%04X", vid, pid);
        gtk_label_set_markup(GTK_LABEL(app->dev_status_label),
                             "<span color='red' weight='bold'>Disconnected</span>");
    }
}

/* ------------------------------------------------------------------ */
/* RGB Handlers                                                         */
/* ------------------------------------------------------------------ */

static void on_rgb_apply_all(GtkWidget *widget, gpointer data) {
    AppState *app = (AppState *)data;
    if (!app->dev) return;

    GdkRGBA color;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(app->rgb_colorbutton_all), &color);

    rgb_set_all(app->dev, (uint8_t)(color.red * 255),
                          (uint8_t)(color.green * 255),
                          (uint8_t)(color.blue * 255));
    gui_status(app, "RGB: Applied color to all keys");
}

static void on_rgb_mode_changed(GtkWidget *widget, gpointer data) {
    AppState *app = (AppState *)data;
    if (!app->dev) return;

    int mode = gtk_combo_box_get_active(GTK_COMBO_BOX(app->rgb_mode_combo));
    uint8_t speed = (uint8_t)gtk_range_get_value(GTK_RANGE(app->rgb_speed_scale));

    rgb_set_mode(app->dev, (rgb_mode_t)mode, speed);
    gui_status(app, "RGB: Mode set to %s (speed %u)", rgb_mode_name(mode), speed);
}

/* ------------------------------------------------------------------ */
/* UI Construction                                                      */
/* ------------------------------------------------------------------ */

static GtkWidget *create_rgb_tab(AppState *app) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 15);

    /* All keys section */
    GtkWidget *frame_all = gtk_frame_new("Global RGB");
    GtkWidget *box_all = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(box_all), 10);

    app->rgb_colorbutton_all = gtk_color_button_new();
    GtkWidget *btn_apply = gtk_button_new_with_label("Apply to All Keys");
    g_signal_connect(btn_apply, "clicked", G_CALLBACK(on_rgb_apply_all), app);

    gtk_box_pack_start(GTK_BOX(box_all), gtk_label_new("Color:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_all), app->rgb_colorbutton_all, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box_all), btn_apply, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(frame_all), box_all);
    gtk_box_pack_start(GTK_BOX(vbox), frame_all, FALSE, FALSE, 0);

    /* Mode section */
    GtkWidget *frame_mode = gtk_frame_new("Lighting Effects");
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);

    app->rgb_mode_combo = gtk_combo_box_text_new();
    for(int i=0; i<RGB_MODE_COUNT; i++)
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(app->rgb_mode_combo), rgb_mode_name(i));
    gtk_combo_box_set_active(GTK_COMBO_BOX(app->rgb_mode_combo), 0);
    g_signal_connect(app->rgb_mode_combo, "changed", G_CALLBACK(on_rgb_mode_changed), app);

    app->rgb_speed_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 255, 1);
    gtk_range_set_value(GTK_RANGE(app->rgb_speed_scale), 128);

    app->rgb_brightness_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 255, 1);
    gtk_range_set_value(GTK_RANGE(app->rgb_brightness_scale), 255);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Effect Mode:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->rgb_mode_combo, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Effect Speed:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->rgb_speed_scale, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Brightness:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->rgb_brightness_scale, 1, 2, 1, 1);

    gtk_container_add(GTK_CONTAINER(frame_mode), grid);
    gtk_box_pack_start(GTK_BOX(vbox), frame_mode, FALSE, FALSE, 0);

    return vbox;
}

static GtkWidget *create_device_tab(AppState *app) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    app->dev_vid_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(app->dev_vid_entry), "046D");
    app->dev_pid_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(app->dev_pid_entry), "C335");

    GtkWidget *btn_conn = gtk_button_new_with_label("Connect to Keyboard");
    g_signal_connect(btn_conn, "clicked", G_CALLBACK(on_device_connect), app);

    app->dev_status_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(app->dev_status_label), "<i>Not connected</i>");

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Vendor ID (hex):"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->dev_vid_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Product ID (hex):"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->dev_pid_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_conn, 0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Status:"), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->dev_status_label, 1, 3, 1, 1);

    gtk_box_pack_start(GTK_BOX(vbox), grid, FALSE, FALSE, 0);

    GtkWidget *info = gtk_label_new("Common VIDs: Logitech=046D, Corsair=1B1C, Redragon=04F2");
    gtk_style_context_add_class(gtk_widget_get_style_context(info), "dim-label");
    gtk_box_pack_start(GTK_BOX(vbox), info, FALSE, FALSE, 0);

    return vbox;
}

void gui_run(int *argc, char ***argv) {
    gtk_init(argc, argv);

    AppState *app = g_new0(AppState, 1);

    app->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(app->window), "MechKey Configurator");
    gtk_window_set_default_size(GTK_WINDOW(app->window), 600, 500);
    g_signal_connect(app->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(app->window), main_vbox);

    /* Tabs */
    app->notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(main_vbox), app->notebook, TRUE, TRUE, 0);

    gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook),
                             create_device_tab(app), gtk_label_new("Connection"));
    gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook),
                             create_rgb_tab(app), gtk_label_new("RGB Lighting"));
    gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook),
                             gtk_label_new("Macro management (WIP)"), gtk_label_new("Macros"));
    gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook),
                             gtk_label_new("Key remapping (WIP)"), gtk_label_new("Keymap"));

    /* Status bar */
    app->statusbar = gtk_statusbar_new();
    app->status_ctx = gtk_statusbar_get_context_id(GTK_STATUSBAR(app->statusbar), "gui");
    gtk_box_pack_start(GTK_BOX(main_vbox), app->statusbar, FALSE, FALSE, 0);

    gui_status(app, "Welcome. Please connect your keyboard.");

    gtk_widget_show_all(app->window);
    gtk_main();

    if (app->dev) hid_keyboard_close(app->dev);
    g_free(app);
}
