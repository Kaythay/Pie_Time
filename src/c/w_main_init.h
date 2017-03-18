#pragma once

#include <pebble.h>

Window *s_main_w;

//functions for properly initializing the main window
void main_w_load(Window *window);
void main_w_unload(Window *window);

//functions for handling updating the time shown on-screen
void tick_handler(struct tm *tick_time, TimeUnits units_changed);
void update_time();