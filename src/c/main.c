#include <pebble.h>

/* ------------------------------------------------------------- */
/* variable declarations */

static Window *main_w;
static TextLayer *time_layer;
static Layer *canvas_layer;
static GBitmap *pot;
static BitmapLayer *pot_layer;

/* ------------------------------------------------------------- */
/* function declarations */

//functions for handling updating the time shown on-screen
static void minute_change_handler(struct tm *tick_time, TimeUnits units_changed);
static void date_change_handler(struct tm *tick_time, TimeUnits units_changed);
static void update_time();

//functions for drawing on the canvas
static void update_graphics();
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
    
    //attach time handlers
    tick_timer_service_subscribe(MINUTE_UNIT, minute_change_handler);
    tick_timer_service_subscribe(DAY_UNIT, date_change_handler);
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
    
    //format canvas layer
    canvas_layer = layer_create(bounds);
    layer_set_update_proc(canvas_layer, canvas_update_proc);
    
    //format bitmaps
    pot = gbitmap_create_with_resource(RESOURCE_ID_CLAY_POT);
    pot_layer = bitmap_layer_create(GRect(30,80, 95, 75));
    bitmap_layer_set_compositing_mode(pot_layer, GCompOpSet);
    bitmap_layer_set_bitmap(pot_layer, pot);
    
    //add layers
    layer_add_child(w_layer, text_layer_get_layer(time_layer));
    layer_add_child(w_layer, canvas_layer);
    layer_add_child(w_layer, bitmap_layer_get_layer(pot_layer));
}

static void main_w_unload(Window *window){
    //destroy all
    text_layer_destroy(time_layer);
    layer_destroy(canvas_layer);
    gbitmap_destroy(pot);
    bitmap_layer_destroy(pot_layer);
}

/* ---------------------------------------------------------------------------------- */
/* functions related to to the time layer */

static void minute_change_handler(struct tm *tick_time, TimeUnits units_changed){
    update_time();
}

static void date_change_handler(struct tm *tick_time, TimeUnits units_changed){
    update_graphics();
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

/* ---------------------------------------------------------------------------------- */
/* functions related to to the canvas layer */

static void update_graphics(){
    //update graphics layer
    layer_mark_dirty(canvas_layer);
}
    
static void canvas_update_proc(Layer * layer, GContext * cntxt){
    //get information about the dimensions of the watch face
//     GRect canvas_bounds = layer_get_bounds(layer);
//     uint16_t middle_x = canvas_bounds.size.w / 2, //the x-coordinate for the middle of the watch
//              middle_y = canvas_bounds.size.w / 2, //the y-coordinate
//              radius = middle_x - 10;
    
    //format graphics
    graphics_context_set_stroke_color(cntxt, GColorDarkCandyAppleRed);
    graphics_context_set_fill_color(cntxt, GColorCeleste);
    graphics_context_set_stroke_width(cntxt, 4);
    graphics_context_set_antialiased(cntxt, true);
    
    //draw a circle
//     GPoint center = GPoint(middle_x, middle_y);
//     graphics_fill_circle(cntxt, center, radius);
    
    //draw the hour hand
//     GPoint hour_start = GPoint(middle_x,middle_y), hour_end = GPoint(middle_x, middle_x + (radius / 2));
//     graphics_draw_line(cntxt, hour_start, hour_end);
    
//     //draw the minute hand
//     GPoint min_start = GPoint(middle_x,middle_y), min_end = GPoint(middle_x,middle_x + radius);
//     graphics_draw_line(cntxt, min_start, min_end);   
}