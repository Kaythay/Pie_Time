#include <pebble.h>

/* ------------------------------------------------------------- */
/* variable declarations */

static Window *main_w;
static Layer *canvas_layer;
static TextLayer *time_layer;
static GBitmap *pot, *flower1, *flower2, *flower3, *flower4;
static BitmapLayer *pot_layer, *flower_layer;

/* ------------------------------------------------------------- */
/* function declarations */

//time change handlers
static void minute_change_handler(struct tm *tick_time, TimeUnits units_changed);
// static void hour_change_handler(struct tm *tick_time, TimeUnits units_changed);

//functions for updating display
static void update_time();
static void update_flower();
// static uint32_t get_flower_resource(int time);
static void update_canvas(Layer * layer, GContext *ctx);

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
    
    //initialize the time and the flower
    update_time();
//     update_flower();
    
    //attach time handlers
    tick_timer_service_subscribe(SECOND_UNIT, minute_change_handler);
//     tick_timer_service_subscribe(SECOND_UNIT, hour_change_handler);
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
    
    //create layers
    canvas_layer = layer_create(bounds);
    time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58,52), bounds.size.w, 50));
    flower_layer = bitmap_layer_create(GRect(35, 20, 70, 85));
    pot_layer = bitmap_layer_create(GRect(47,105, 50, 48));
    
    //format time layer
    text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
    text_layer_set_background_color(time_layer, GColorFromRGBA(0, 0, 0, 0));    //set background to transparent
    
    //format the flower layer
    bitmap_layer_set_compositing_mode(flower_layer, GCompOpSet);
    
    //initialize the gbitmaps
    flower1 = gbitmap_create_with_resource(RESOURCE_ID_TULIP_1);
    flower2 = gbitmap_create_with_resource(RESOURCE_ID_TULIP_2);
    flower3 = gbitmap_create_with_resource(RESOURCE_ID_TULIP_3);
    flower4 = gbitmap_create_with_resource(RESOURCE_ID_TULIP_4);
    
    //draw the pot
    pot = gbitmap_create_with_resource(RESOURCE_ID_POT_CLAY);
    bitmap_layer_set_compositing_mode(pot_layer, GCompOpSet);
    bitmap_layer_set_bitmap(pot_layer, pot);
    
    //setup canvas procedure
    layer_set_update_proc(canvas_layer, update_canvas);
    
    //add layers
    layer_add_child(w_layer, bitmap_layer_get_layer(flower_layer));
    layer_add_child(w_layer, bitmap_layer_get_layer(pot_layer));
    layer_add_child(w_layer, canvas_layer);
    layer_add_child(w_layer, text_layer_get_layer(time_layer));
}

static void main_w_unload(Window *window){
    //destroy all
    text_layer_destroy(time_layer);
    gbitmap_destroy(pot);
    bitmap_layer_destroy(pot_layer);
    gbitmap_destroy(flower1);
    gbitmap_destroy(flower2);
    gbitmap_destroy(flower3);
    gbitmap_destroy(flower4);
    bitmap_layer_destroy(flower_layer);
    layer_destroy(canvas_layer);
}

/* ---------------------------------------------------------------------------------- */
/* time change handlers */

static void minute_change_handler(struct tm *tick_time, TimeUnits units_changed){
    update_time();
}

// static void hour_change_handler(struct tm *tick_time, TimeUnits units_changed){
//     update_flower();
// }

/* ---------------------------------------------------------------------------------- */
/* functions that update the display */

static void update_time(){
    //get the time
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    
    //update the text to show the current time
    static char s_buffer[10];
    strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M:%S" : "%I:%M:%S", tick_time);    
    text_layer_set_text(time_layer, s_buffer);
    
    update_flower();
}

static void update_flower(){
    //get the time
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    
    //determine which flower to display
    //TODO: optimize?
    int hour = tick_time->tm_sec;
    if (hour == 0){
        bitmap_layer_set_bitmap(flower_layer, flower1);
    } else if (hour == 15){
        bitmap_layer_set_bitmap(flower_layer, flower2);
    } else if (hour == 30){
        bitmap_layer_set_bitmap(flower_layer, flower3);
    } else if (hour == 45){
        bitmap_layer_set_bitmap(flower_layer, flower4);
    }
}

static void update_canvas(Layer * layer, GContext *ctx){
    
}