#!/usr/bin/env bash
unset GIO_MODULE_DIR
unset GDK_PIXBUF_MODULEDIR
unset GDK_PIXBUF_MODULE_FILE
unset GTK_EXE_PREFIX
unset GTK_IM_MODULE_FILE
unset GTK_PATH
unset GSETTINGS_SCHEMA_DIR
unset SNAP_LIBRARY_PATH

export QT_QPA_PLATFORM=xcb
unset WAYLAND_DISPLAY

exec "${TODO_GUI_BIN:-./build/frontend/todo_gui}" "$@"
