#include <pebble.h>

/* ------------------------------------------------------------- */
/* variable declarations */

static Window *main_w;
static TextLayer *time_layer;
static Layer *canvas_layer;

/* ------------------------------------------------------------- */
/* function declarations */

//functions for handling updating the time shown on-screen
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void update_time();

//functions for drawing on the canvas
static void canvas_update_proc(Layer * layer, GContext * cntxt);

//functions for properly initializing the main window
static void main_w_load(Window *window);
static void main_w_unload(Window *window);

static void init();
static void deinit();

/* -------------------------------------------------------------------------------------- */

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

/* ----------------------------------------------------------------------------------- */
/* functions related to to the main window */

static void main_w_load(Window *window){
    //get window bounds
    Layer *w_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(w_layer);
    
    //format time layer
    time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58,52), bounds.size.w, 50));
    text_layer_set_text(time_layer, "time");
    
    //format canvas layer
    canvas_layer = layer_create(bounds);
    layer_set_update_proc(canvas_layer, canvas_update_proc);
    
    //add layers
    layer_add_child(w_layer, text_layer_get_layer(time_layer));
    layer_add_child(w_layer, canvas_layer);
}

static void main_w_unload(Window *window){
    text_layer_destroy(time_layer);
}

/* ---------------------------------------------------------------------------------- */
/* functions related to to the time layer */

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

/* ---------------------------------------------------------------------------------- */
/* functions related to to the canvas layer */

static void canvas_update_proc(Layer * layer, GContext * cntxt){
    graphics_context_set_stroke_color(cntxt, GColorDarkCandyAppleRed);
    graphics_context_set_fill_color(cntxt, GColorCeleste);
    graphics_context_set_stroke_width(cntxt, 8);
    graphics_context_set_antialiased(cntxt, true);
    
    //draw a line
    GPoint start = GPoint(10,10);
    GPoint end = GPoint(10,30);
    graphics_draw_line(cntxt, start, end);
}