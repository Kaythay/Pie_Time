#include <pebble.h>
#include "src/c/init.h"
#include "src/c/w_main_init.h"

void init(){
    //create the main window, attach handlers
    s_main_w = window_create();
    window_set_window_handlers(s_main_w, (WindowHandlers){
        .load = main_w_load,
        .unload = main_w_unload
    });
    
    window_stack_push(s_main_w, true);
    
    //initialize the time
    update_time();
    
    //attach time handler
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

void deinit(){
    window_destroy(s_main_w);
}