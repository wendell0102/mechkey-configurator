#ifndef GUI_H
#define GUI_H

/*
 * gui.h - GTK3 graphical interface for mechkey-configurator
 *
 * Depends on: GTK 3.x
 *   Linux:  sudo apt install libgtk-3-dev
 *   macOS:  brew install gtk+3
 */

#include <gtk/gtk.h>
#include "hid_keyboard.h"
#include "rgb.h"
#include "macros.h"

/* ------------------------------------------------------------------ */
/* Application state                                                    */
/* ------------------------------------------------------------------ */

typedef struct {
    /* Device */
    hid_device_t *dev;
    uint16_t      vid;
    uint16_t      pid;

    /* Main window */
    GtkWidget *window;
    GtkWidget *notebook;      /* tabbed panels */
    GtkWidget *statusbar;
    guint      status_ctx;

    /* ---- RGB tab ---- */
    GtkWidget *rgb_colorbutton_all;   /* GtkColorButton for all-keys */
    GtkWidget *rgb_mode_combo;        /* ComboBox: mode selection    */
    GtkWidget *rgb_speed_scale;       /* Scale 0-255: effect speed   */
    GtkWidget *rgb_brightness_scale;  /* Scale 0-255                 */
    GtkWidget *rgb_key_id_spin;       /* SpinButton: key ID (0-255)  */
    GtkWidget *rgb_key_colorbutton;   /* color for single key        */

    /* ---- Macros tab ---- */
    GtkWidget *macro_id_spin;         /* macro ID to delete          */
    GtkWidget *macro_list_view;       /* TreeView showing macros     */
    GtkListStore *macro_store;

    /* ---- Shortcuts tab ---- */
    GtkWidget *sc_src_key_entry;      /* hex entry: src keycode      */
    GtkWidget *sc_src_mod_entry;      /* hex entry: src modifier     */
    GtkWidget *sc_dst_key_entry;      /* hex entry: dst keycode      */
    GtkWidget *sc_dst_mod_entry;      /* hex entry: dst modifier     */
    GtkWidget *sc_list_view;
    GtkListStore *sc_store;

    /* ---- Profiles tab ---- */
    GtkWidget *profile_slot_spin;     /* slot 0-7                    */

    /* ---- Device tab ---- */
    GtkWidget *dev_vid_entry;
    GtkWidget *dev_pid_entry;
    GtkWidget *dev_status_label;
    GtkWidget *dev_list_view;
    GtkListStore *dev_store;
} AppState;

/* ------------------------------------------------------------------ */
/* Public functions                                                     */
/* ------------------------------------------------------------------ */

/** Initialise GTK, build and show the main window. */
void gui_run(int *argc, char ***argv);

/** Post a message to the status bar. */
void gui_status(AppState *app, const char *fmt, ...);

#endif /* GUI_H */
