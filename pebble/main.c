#include <pebble.h>
static Window *window;
static TextLayer *hello_layer;
static char msg[5000];
static char minT[5000];
static char maxT[5000];
int count_time;

//Send message to middleware

void tick_handler(struct tm* tick_time, TimeUnits units_changed){
//     text_layer_set_text(auto_layer, "auto!");
      if (count_time == 0){
        count_time = 5;
        DictionaryIterator *iter; 
        app_message_outbox_begin(&iter);
        int key = 9;
        Tuplet value = TupletCString(key, "auto"); 
        dict_write_tuplet(iter, &value); 
        app_message_outbox_send();
      }else{
        count_time--;
      }
}

/* This is called when the select button is clicked */
void select_click_handler(ClickRecognizerRef recognizer, void *context) {
   count_time = 0;
    tick_timer_service_subscribe(SECOND_UNIT, (TickHandler)tick_handler);
  
}

void up_click_handler(ClickRecognizerRef recognizer, void *context) {
   tick_timer_service_unsubscribe();
   const uint16_t count = click_number_of_clicks_counted(recognizer);
  if(count == 1){
    //text_layer_set_text(hello_layer, "Fahrenheit"); 
    DictionaryIterator *iter; 
    app_message_outbox_begin(&iter);
    int key = 2;
  // send the message "hello?" to the phone, using key #0 
    Tuplet value = TupletCString(key, "f"); 
    dict_write_tuplet(iter, &value); 
    app_message_outbox_send();
  } else if(count == 2){
   // text_layer_set_text(hello_layer, "Show Average"); 
    DictionaryIterator *iter; 
    app_message_outbox_begin(&iter);
    int key = 3;
    Tuplet value = TupletCString(key, "avg"); 
    dict_write_tuplet(iter, &value);
    app_message_outbox_send();
  } else if (count == 3){
   // text_layer_set_text(hello_layer, "Red Light"); 
    DictionaryIterator *iter; 
    app_message_outbox_begin(&iter);
    int key = 6;
    Tuplet value = TupletCString(key, "ulong"); 
    dict_write_tuplet(iter, &value);
    app_message_outbox_send();
  }else if (count == 4) {
    //text_layer_set_text(hello_layer, "show min"); 
    DictionaryIterator *iter; 
    app_message_outbox_begin(&iter);
    int key = 4;
    Tuplet value = TupletCString(key, "min"); 
    dict_write_tuplet(iter, &value);
    app_message_outbox_send();
  } else if (count == 5) {
   // text_layer_set_text(hello_layer, "predict temperature"); 
    DictionaryIterator *iter; 
    app_message_outbox_begin(&iter);
    int key = 10;
    Tuplet value = TupletCString(key, "pd"); 
    dict_write_tuplet(iter, &value);
    app_message_outbox_send(); 
  }
  
}


void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  tick_timer_service_unsubscribe();
   const uint16_t count = click_number_of_clicks_counted(recognizer);
  if(count == 1){
    //text_layer_set_text(hello_layer, "Count");
    DictionaryIterator *iter; 
    app_message_outbox_begin(&iter);
    int key = 8;
// send the message "hello?" to the phone, using key #0 
    Tuplet value = TupletCString(key, "click"); 
    dict_write_tuplet(iter, &value); 
    app_message_outbox_send();
  }if(count == 2){
    //text_layer_set_text(hello_layer, "Stand By!");
    DictionaryIterator *iter; 
    app_message_outbox_begin(&iter);
    int key = 0;
    Tuplet value = TupletCString(key, "s"); 
    dict_write_tuplet(iter, &value); 
    app_message_outbox_send();
  } else if (count == 3){
   // text_layer_set_text(hello_layer, "Green Light"); 
    DictionaryIterator *iter; 
    app_message_outbox_begin(&iter);
    int key = 7;
    Tuplet value = TupletCString(key, "dlong"); 
    dict_write_tuplet(iter, &value);
    app_message_outbox_send();
  }else if (count == 4) {
    //text_layer_set_text(hello_layer, "show max"); 
    DictionaryIterator *iter; 
    app_message_outbox_begin(&iter);
    int key = 5;
    Tuplet value = TupletCString(key, "max"); 
    dict_write_tuplet(iter, &value);
    app_message_outbox_send();
  }
  
}

void out_sent_handler(DictionaryIterator *sent, void *context) {
  // outgoing message was delivered -- do nothing
}
 
void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
// outgoing message failed
  text_layer_set_text(hello_layer, "Error out!"); 
}

void in_received_handler(DictionaryIterator *received, void *context) {
  int key = 0;
  Tuple *text_tuple = NULL;
//   Tuple *text_tuple1 = NULL;
//   Tuple *text_tuple2 = NULL;
  
  for (key = 0; key < 11; key++) {
//     if (key == 3) {
//       text_tuple = dict_find(received, key);
//     }else if (key == 4) {
//       text_tuple1 = dict_find(received, key);
//     }else if (key == 5) {
//       text_tuple2 = dict_find(received, key);
//       break;
//     }
//     else {
      text_tuple = dict_find(received, key);
      if (!text_tuple) continue;
      else break;
    }

    
  
//     if (text_tuple && text_tuple1 && text_tuple2) {
//       if (text_tuple->value && text_tuple1->value && text_tuple2->value) {
//         layers = true;
//       strcpy(msg, text_tuple->value->cstring);
//       strcpy(minT, text_tuple1->value->cstring);
//       strcpy(maxT, text_tuple2->value->cstring);
//       text_layer_set_text(avg_layer, msg);
//       text_layer_set_text(min_layer, minT);
//       text_layer_set_text(max_layer, maxT);
//       }
      
//     }

    if (text_tuple) {
      if (text_tuple->value) {
      // put it in this global variable 

        strcpy(msg, text_tuple->value->cstring);
      } else {
        strcpy(msg, "no value!");
      }
      text_layer_set_text(hello_layer, msg);
    }
    else {
      text_layer_set_text(hello_layer, "no message!");
    }
  
}
  
void in_dropped_handler(AppMessageResult reason, void *context) {
  // incoming message dropped
  text_layer_set_text(hello_layer, "Error in!");
}


/* this registers the appropriate function to the appropriate button */ 
void config_provider(void *context) {
  uint16_t delay_ms = 2000;
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
//   window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 10,0, true, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_multi_click_subscribe(BUTTON_ID_UP, 2, 10,0, true, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_multi_click_subscribe(BUTTON_ID_DOWN, 2, 10,0, true, down_click_handler);
//   window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window); 
  GRect bounds = layer_get_bounds(window_layer);
   hello_layer = text_layer_create((GRect) { .origin = { 0, 72 },
                                            .size = { bounds.size.w, 20 } }); 
  text_layer_set_overflow_mode(hello_layer, GTextOverflowModeTrailingEllipsis); 
  
//   avg_layer = text_layer_create((GRect) { .origin = { 0, 25 },
//                                             .size = { bounds.size.w, 20 } }); 
//   max_layer = text_layer_create((GRect) { .origin = { 0, 50 },
//                                             .size = { bounds.size.w, 20 } });
//   min_layer = text_layer_create((GRect) { .origin = { 0, 75 },
//                                             .size = { bounds.size.w, 20 } });
  text_layer_set_text(hello_layer, "Hello World"); 
  text_layer_set_text_alignment(hello_layer, GTextAlignmentCenter); 
  text_layer_set_font(hello_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  
//   text_layer_set_text(avg_layer, "Hello world!"); 
//   text_layer_set_text_alignment(avg_layer, GTextAlignmentCenter); 
//   text_layer_set_font(avg_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  
//   text_layer_set_text(max_layer, "Hello world!"); 
//   text_layer_set_text_alignment(max_layer, GTextAlignmentCenter); 
//   text_layer_set_font(max_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  
//   text_layer_set_text(min_layer, "Hello world!"); 
//   text_layer_set_text_alignment(min_layer, GTextAlignmentCenter); 
//   text_layer_set_font(min_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  
  layer_add_child(window_layer, text_layer_get_layer(hello_layer));
//   layer_add_child(window_layer, text_layer_get_layer(max_layer));
//   layer_add_child(window_layer, text_layer_get_layer(min_layer));
//   layer_add_child(window_layer, text_layer_get_layer(avg_layer));

}
static void window_unload(Window *window) { 
  text_layer_destroy(hello_layer);
}




static void init(void) {
  window = window_create(); 
  window_set_window_handlers(window, (WindowHandlers) { .load = window_load, .unload = window_unload, });
  // need this for adding the listener 
  window_set_click_config_provider(window, config_provider);
  
  // for registering AppMessage handlers 
  app_message_register_inbox_received(in_received_handler); 
  app_message_register_inbox_dropped(in_dropped_handler); 
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler); 
  
  const uint32_t inbound_size = 1024;
  const uint32_t outbound_size = 1024; 
  app_message_open(inbound_size, outbound_size);
  
  const bool animated = true;
  window_stack_push(window, animated); 
}

static void deinit(void) { 
  window_destroy(window);
}

int main(void) { 
  init();
  app_event_loop();
  deinit(); 
}