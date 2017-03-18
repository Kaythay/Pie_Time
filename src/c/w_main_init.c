#include <pebble.h>
#include "src/c/w_main_init.h"

TextLayer *s_time_layer;

void tick_handler(struct tm *tick_time, TimeUnits units_changed){
    update_time();
}

void update_time(){
    //get the current time
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    
    //update the text to show the time
    static char s_buffer[8];
    strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);    
    text_layer_set_text(s_time_layer, s_buffer);
}

void main_w_load(Window *window){
    //create a layer to show the time on?
    Layer *w_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(w_layer);
    
    //format time layer
    s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58,52), bounds.size.w, 50));
    text_layer_set_text(s_time_layer, "time");
    
    
    //add layers
    layer_add_child(w_layer, text_layer_get_layer(s_time_layer));
}

void main_w_unload(Window *window){
    text_layer_destroy(s_time_layer);
}