#include <pebble.h>
#include "main.h"

static Window *s_main_w;
static void main_w_load(Window *window);
static void main_w_unload(Window *window);


static void main_w_load(Window *window){
    
}

static void main_w_unload(Window *window){
    
}

static void init(){
    s_main_w = window_create();
    window_set_window_handlers(s_main_w, (WindowHandlers){
        .load = main_w_load,
        .unload = main_w_unload
    });
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