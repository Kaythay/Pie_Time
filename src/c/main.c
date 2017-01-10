#include <pebble.h>
#include "main.h"

static Window *s_main_w;
static TextLayer *s_time_layer;

static void main_w_load(Window *window);
static void main_w_unload(Window *window);
static void init();
static void deinit();

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