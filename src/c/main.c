#include <pebble.h>
#include "src/c/init.h"
#include "src/c/w_main_init.h"

int main(){
    init();
    app_event_loop();
    deinit();
    return 0;
}