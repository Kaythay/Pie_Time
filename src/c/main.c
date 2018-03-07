#include <pebble.h>

//TODO: separate main.c into multiple files, if possible

/* ------------------------------------------------------------- */
/* variable declarations */

static Window* main_w;
static Layer* canvas_layer;
static TextLayer* time_layer;
static GBitmap *pot, *current_flower, *next_flower;
static BitmapLayer *pot_layer, *flower_layer;
static TimeUnits precision;      //used to determine the precision of the time displayed on the watch
static int change_period;        //used to determine when to change the flower displayed on the watch
static int next_flower_index;    //index of current flower resource being displayed (ie. 0, 1, 2, or 3)
static uint32_t* flower_resource;

/* ------------------------------------------------------------- */
/* function declarations */

//time change handlers
static void update_time_handler(struct tm *tick_time, TimeUnits units_changed);
static void update_flower_handler(struct tm *tick_time, TimeUnits units_changed);

//helper function(s) for time changes
static struct tm* get_time();

//config change handler
static void inbox_received_handler(DictionaryIterator *it, void *context);

//helper function(s) for config changes
static TimeUnits to_time_units(int i);    //converts the int into the corresponding TimeUnits value
static char* time_format();               //returns the appropriate time format for the current precision value

//functions for updating display
static void update_time(struct tm *tick_time);
static void update_flower(struct tm *tick_time);
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
    
    //set default value(s)
    precision = SECOND_UNIT;
    change_period = 15;
    next_flower_index = 1;
    flower_resource = malloc(4*sizeof(*flower_resource));
    flower_resource[0] = RESOURCE_ID_TULIP_1;
    flower_resource[1] = RESOURCE_ID_TULIP_2;
    flower_resource[2] = RESOURCE_ID_TULIP_3;
    flower_resource[3] = RESOURCE_ID_TULIP_4;

    //before attaching handlers, initialize the time and the flower 
    //currently, both are initialized in update_time()
    update_time(get_time());
    
    //attach time handler with default precision
    tick_timer_service_subscribe(precision, update_time_handler);

    //attach config handler
    app_message_register_inbox_received(inbox_received_handler);
    // allow phone to transfer config data to watch app
    app_message_open(128, 128);
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
    current_flower = gbitmap_create_with_resource(RESOURCE_ID_TULIP_1);
    next_flower = gbitmap_create_with_resource(RESOURCE_ID_TULIP_2);
    
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
    gbitmap_destroy(current_flower);
    gbitmap_destroy(next_flower);
    bitmap_layer_destroy(flower_layer);
    layer_destroy(canvas_layer);
}

/* ---------------------------------------------------------------------------------- */
/* time change handlers */

static void update_time_handler(struct tm *tick_time, TimeUnits units_changed){
    update_time(tick_time);
}

static void update_flower_handler(struct tm *tick_time, TimeUnits units_changed){
    update_flower(tick_time);
}

/* ---------------------------------------------------------------------------------- */
/* helper functions for time change */

static struct tm* get_time(){
    time_t temp = time(NULL);
    return localtime(&temp);
}

/* ---------------------------------------------------------------------------------- */
/* config change handler */

static void inbox_received_handler(DictionaryIterator *it, void *context){
    //get data from the dictionary, convert it to the appropriate types
    Tuple* precision_t = dict_find(it, MESSAGE_KEY_TimePrecision);
    Tuple* change_period_t = dict_find(it, MESSAGE_KEY_FlowerChangePeriod);
    TimeUnits new_precision = to_time_units(precision_t->value->int32);
    int new_change_period = change_period_t->value->int32;
  
    if (precision != new_precision){
        precision = new_precision;
        tick_timer_service_unsubscribe();
        tick_timer_service_subscribe(precision, update_time_handler);
    }
    if (change_period == new_change_period){
        change_period = new_change_period;
    }
    return;
}

/* ---------------------------------------------------------------------------------- */
/* helper function(s) for config change */

static TimeUnits to_time_units(int i){
    if (i == 0) {
        return SECOND_UNIT;
    }
    if (i == 1) {
        return MINUTE_UNIT;
    }
    return HOUR_UNIT;
}

static char* time_format(){
    switch(precision){
        case SECOND_UNIT:
            return clock_is_24h_style() ? "%H:%M:%S" : "%I:%M:%S";
            break;
        case MINUTE_UNIT:
            return clock_is_24h_style() ? "%H:%M" : "%I:%M";
            break;
        default:
            return clock_is_24h_style() ? "%H" : "%I";
    }
}

/* ---------------------------------------------------------------------------------- */
/* functions that update the display */

static void update_time(struct tm *tick_time){
    //update the text to show the current time
    static char s_buffer[10];
    strftime(s_buffer, sizeof(s_buffer), time_format(), tick_time);    
    text_layer_set_text(time_layer, s_buffer);

    update_flower(tick_time);
}

static void update_flower(struct tm *tick_time){
    if ( (tick_time->tm_sec % change_period) == 0){
      gbitmap_destroy(current_flower);
      current_flower = next_flower;
      next_flower_index = (next_flower_index + 1) % 4;
      next_flower = gbitmap_create_with_resource(flower_resource[next_flower_index]);
      bitmap_layer_set_bitmap(flower_layer, current_flower);
    }
}

static void update_canvas(Layer * layer, GContext *ctx){
    struct tm* tick_time = get_time();
    update_time(tick_time);
    update_flower(tick_time);
}
