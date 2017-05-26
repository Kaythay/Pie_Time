#include <pebble.h>

/* ------------------------------------------------------------- */
/* variable declarations */

static Window *main_w;
static TextLayer *time_layer;
static GBitmap *pot, *flower;
static BitmapLayer *pot_layer, *flower_layer;

/* ------------------------------------------------------------- */
/* function declarations */

//functions for handling updating the time shown on-screen
static void minute_change_handler(struct tm *tick_time, TimeUnits units_changed);
static void update_time();

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
    
    //attach time handlers
    tick_timer_service_subscribe(MINUTE_UNIT, minute_change_handler);
}

static void deinit(){
    window_destroy(main_w);
}

/* ----------------------------------------------------------------------------------- */
/* functions related to to the main window */

static void main_w_load(Window *window){
    //get main window
    Layer *w_layer = window_get_root_layer(window);
    //get main window bounds
    GRect bounds = layer_get_bounds(w_layer);
    
    //format time layer
    time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58,52), bounds.size.w, 50));
    text_layer_set_text(time_layer, "0:00");
    text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
    text_layer_set_background_color(time_layer, GColorFromRGBA(0, 0, 0, 0));    //set background to transparent
    
    //format bitmaps
    //draw the flower
    flower = gbitmap_create_with_resource(RESOURCE_ID_TULIP_4);
    flower_layer = bitmap_layer_create(GRect(35, 20, 70, 85));
    bitmap_layer_set_compositing_mode(flower_layer, GCompOpSet);
    bitmap_layer_set_bitmap(flower_layer, flower);
    //draw the pot
    pot = gbitmap_create_with_resource(RESOURCE_ID_POT_CLAY);
    pot_layer = bitmap_layer_create(GRect(47,105, 50, 48));
    bitmap_layer_set_compositing_mode(pot_layer, GCompOpSet);
    bitmap_layer_set_bitmap(pot_layer, pot);
    
    //add layers
    layer_add_child(w_layer, bitmap_layer_get_layer(flower_layer));
    layer_add_child(w_layer, bitmap_layer_get_layer(pot_layer));
    layer_add_child(w_layer, text_layer_get_layer(time_layer));
}

static void main_w_unload(Window *window){
    //destroy all
    text_layer_destroy(time_layer);
    gbitmap_destroy(pot);
    bitmap_layer_destroy(pot_layer);
}

/* ---------------------------------------------------------------------------------- */
/* functions related to to the time layer */

static void minute_change_handler(struct tm *tick_time, TimeUnits units_changed){
    update_time();
}

static void update_time(){
    //get the current time
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    
    //update the text to show the current time
    static char s_buffer[8];
    strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);    
    text_layer_set_text(time_layer, s_buffer);
}