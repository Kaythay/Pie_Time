#include <stdlib.h>
#include <pebble.h>

//TODO: separate main.c into multiple files, if possible

/* ------------------------------------------------------------- */
/* variable declarations */

static Window* main_w;
static Layer* canvas_layer;
static TextLayer* time_layer;
static GBitmap *pot, **flower_resource;
static BitmapLayer *pot_layer, *flower_layer;
static TimeUnits precision;    //used to determine the precision of the time displayed on the watch
static int change_period;      //used to determine when to change the flower displayed on the watch
static int curr_flower_index;  //index of flower that is currently being displayed, where index corresponds to its position in flower_resource
static time_t last_repaint;    //used to determine when change_period amount of time has passed

/* ------------------------------------------------------------- */
/* function declarations */

//time change handler
static void update_time_handler(struct tm *tick_time, TimeUnits units_changed);

//config change handler
static void inbox_received_handler(DictionaryIterator *it, void *context);

//helper function(s) for time changes and config changes
static int stoi(char* c_str);
static TimeUnits to_time_units(int i);    //converts the int into the corresponding TimeUnits value
static char* time_format();               //returns the appropriate time format for the current precision value

//functions for updating display
static void update_time(time_t curr_time);
static void update_flower(time_t curr_time);
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
    last_repaint = -1;
    curr_flower_index = 0;
    //initialize the bitmaps
    flower_resource = malloc(4*sizeof(*flower_resource));
    flower_resource[0] = gbitmap_create_with_resource(RESOURCE_ID_TULIP_1);
    flower_resource[1] = gbitmap_create_with_resource(RESOURCE_ID_TULIP_2);
    flower_resource[2] = gbitmap_create_with_resource(RESOURCE_ID_TULIP_3);
    flower_resource[3] = gbitmap_create_with_resource(RESOURCE_ID_TULIP_4);
    
    //before attaching handlers, initialize the time and the flower 
    //currently, both are initialized in update_time()
    update_time(time(NULL));
    
    //attach time handlers with default precision
    tick_timer_service_subscribe(precision, update_time_handler);

    //attach config handler
    app_message_register_inbox_received(inbox_received_handler);
    // allow phone to transfer config data to watch app
    app_message_open(128, 128);
    app_message_get_context();
}

static void deinit(){
    window_destroy(main_w);
    free(flower_resource);
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
    text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
    text_layer_set_text_color(time_layer, GColorFromRGB(255, 170, 0));
    text_layer_set_background_color(time_layer, GColorFromRGBA(0, 0, 0, 0));    //set background to transparent
    
    //format the flower layer
    bitmap_layer_set_compositing_mode(flower_layer, GCompOpSet);
    
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
    gbitmap_destroy(flower_resource[0]);
    gbitmap_destroy(flower_resource[1]);
    gbitmap_destroy(flower_resource[2]);
    gbitmap_destroy(flower_resource[3]);
    bitmap_layer_destroy(flower_layer);
    layer_destroy(canvas_layer);
}

/* ---------------------------------------------------------------------------------- */
/* time change handler */

static void update_time_handler(struct tm *tick_time, TimeUnits units_changed){
    update_time(mktime(tick_time));
}

/* ---------------------------------------------------------------------------------- */
/* config change handler */

static void inbox_received_handler(DictionaryIterator *it, void *context){
    //get data from the dictionary, convert it to the appropriate types
    Tuple* precision_t = dict_find(it, MESSAGE_KEY_TimePrecision);
    Tuple* change_period_t = dict_find(it, MESSAGE_KEY_FlowerChangePeriod);
    int new_precision = to_time_units( stoi(precision_t->value->cstring) );
    int new_change_period = stoi(change_period_t->value->cstring);

    printf("Selected precision index is : %d", stoi(precision_t->value->cstring));
    printf("Selected change period is : %d", new_change_period);

    if (precision != new_precision){
        precision = new_precision;
        tick_timer_service_unsubscribe();
        tick_timer_service_subscribe(precision, update_time_handler);
        printf("Precision updated to index : %d", stoi(precision_t->value->cstring));
    }
    if (change_period != new_change_period){
        change_period = new_change_period;
        printf("Change period updated to : %d", change_period);
    }
    return;
}

/* ---------------------------------------------------------------------------------- */
/* helper function(s) for time and config change */

//assumes the given c_str contains only digit characters (ie. 0,1,2,...,9)
static int stoi(char* c_str){
    int val = 0;
    while (*c_str != '\0'){
        val *= 10;
        val += *c_str - '0';
        c_str++;
    }
    return val;
}

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
        case MINUTE_UNIT:
            return clock_is_24h_style() ? "%H:%M" : "%I:%M";
        default:
            return clock_is_24h_style() ? "%H" : "%I";
    }
}

/* ---------------------------------------------------------------------------------- */
/* functions that update the display */

static void update_time(time_t curr_time){
    //update the text to show the current time
    struct tm * tick_time = localtime(&curr_time);
    static char s_buffer[10];
    strftime(s_buffer, sizeof(s_buffer), time_format(), tick_time);    
    text_layer_set_text(time_layer, s_buffer);

    update_flower(curr_time);
}

static void update_flower(time_t curr_time){
    if ( (curr_time - last_repaint) >= change_period){
        //determine which flower to display
        curr_flower_index = (curr_flower_index + 1) % 4;
        bitmap_layer_set_bitmap(flower_layer, flower_resource[curr_flower_index]);
        last_repaint = curr_time;
        printf("Updated flower at %d with index %d", last_repaint, curr_flower_index);
    }
    return;
}

static void update_canvas(Layer * layer, GContext *ctx){
    time_t temp = time(NULL);
//     struct tm *tick_time = localtime(&temp);
    update_time(temp);
    update_flower(temp);
}