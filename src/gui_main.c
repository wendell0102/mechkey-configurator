/*
 * gui_main.c - Entry point for the graphical interface of mechkey-configurator
 *
 * Compile with:  make gui
 * Run:           ./mechkey-gui
 */

#include "gui.h"

int main(int argc, char **argv) {
    gui_run(&argc, &argv);
    return 0;
}
