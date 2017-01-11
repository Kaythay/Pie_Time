#include <pebble.h>
#include "main.h"

static Window *s_main_w;
static TextLayer *s_time_layer;

static void main_w_load(Window *window);
static void main_w_unload(Window *window);
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void update_time();
static void init();
static void deinit();

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
    update_time();
}

static void update_time(){
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    
    static char s_buffer[8];
    strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
        
    text_layer_set_text(s_time_layer, s_buffer);
}

static void main_w_load(Window *window){
    Layer *w_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(w_layer);
    
    s_time_layer = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(58,52), bounds.size.w, 50));
    
    text_layer_set_text(s_time_layer, "time");
    
    layer_add_child(w_layer, text_layer_get_layer(s_time_layer));


}

static void main_w_unload(Window *window){
    text_layer_destroy(s_time_layer);
}

static void init(){
    s_main_w = window_create();
    window_set_window_handlers(s_main_w, (WindowHandlers){
        .load = main_w_load,
        .unload = main_w_unload
    });
    
    window_stack_push(s_main_w, true);
    
        update_time();
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(){
    window_destroy(s_main_w);
}

int main(){
    init();
    app_event_loop();
    deinit();
    return 0;
}