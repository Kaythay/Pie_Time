#include <pebble.h>

static Window *main_w;
static TextLayer *time_layer;

//functions for handling updating the time shown on-screen
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void update_time();

//functions for properly initializing the main window
static void main_w_load(Window *window);
static void main_w_unload(Window *window);

static void init();
static void deinit();

int main(){
    init();
    app_event_loop();
    deinit();
    return 0;
}

static void init(){
    //create the main window, attach handlers
    main_w = window_create();
    window_set_window_handlers(main_w, (WindowHandlers){
        .load = main_w_load,
        .unload = main_w_unload
    });
    
    window_stack_push(main_w, true);
    
    //initialize the time
    update_time();
    
    //attach time handler
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(){
    window_destroy(main_w);
}


static void main_w_load(Window *window){
    //create a layer to show the time on?
    Layer *w_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(w_layer);
    
    //format time layer
    time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58,52), bounds.size.w, 50));
    text_layer_set_text(time_layer, "time");
    
    
    //add layers
    layer_add_child(w_layer, text_layer_get_layer(time_layer));
}

static void main_w_unload(Window *window){
    text_layer_destroy(time_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
    update_time();
}

static void update_time(){
    //get the current time
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    
    //update the text to show the time
    static char s_buffer[8];
    strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);    
    text_layer_set_text(time_layer, s_buffer);
}