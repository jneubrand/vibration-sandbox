#include <pebble.h>
#include "selection_layer.h"

static Window *window;
static TextLayer *text_layer;

static char str_on_time[5] = "1";
static char str_off_time[5] = "1";
static char str_repeat[4] = "1";
static uint16_t num_on_time = 100;
static uint16_t num_off_time = 100;
static uint16_t num_repeat = 1;

static Layer *selection_win;

static void run_vibrations() {
  static uint32_t segments[20];
  for (uint8_t i = 0; i < num_repeat; i++) {
    segments[i*2] = num_on_time;
    segments[i*2 + 1] = num_off_time;
  }
  VibePattern pat = {
    .durations = segments,
    .num_segments = num_repeat * 2,
  };
  vibes_enqueue_custom_pattern(pat);
}

static char* selection_handle_get_text(int index, void *context) {
  if (index == 0) {
    return str_on_time;
  } else if (index == 1) {
    return str_off_time;
  } else {
    return str_repeat;
  }
}

static void selection_handle_complete(void *context) {
  run_vibrations();
}

static void selection_handle_increment(int index, uint8_t clicks, void *context) {
  if (index == 0) {
    num_on_time += 1;
    if (num_on_time > 500) num_on_time = 500;
    snprintf(str_on_time, 400, "%i", num_on_time);
  } else if (index == 1) {
    num_off_time += 1;
    if (num_off_time > 500) num_off_time = 500;
    snprintf(str_off_time, 400, "%i", num_off_time);
  } else if (index == 2) {
    num_repeat += 1;
    if (num_repeat > 10) num_repeat = 10;
    snprintf(str_repeat, 3, "%i", num_repeat);
  }
}

static void selection_handle_decrement(int index, uint8_t clicks, void *context) {
  if (index == 0) {
    num_on_time -= 1;
    if (num_on_time < 1) num_on_time = 1;
    snprintf(str_on_time, 400, "%i", num_on_time);
  } else if (index == 1) {
    num_off_time -= 1;
    if (num_off_time < 1) num_off_time = 1;
    snprintf(str_off_time, 400, "%i", num_off_time);
  } else if (index == 2) {
    num_repeat -= 1;
    if (num_repeat < 1) num_repeat = 1;
    snprintf(str_repeat, 3, "%i", num_repeat);
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) {
    .origin = { 0, (bounds.size.h - 60) / 2 },
    .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "On | Off | Repeat");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  snprintf(str_on_time, 400, "%i", num_on_time);
  snprintf(str_off_time, 400, "%i", num_off_time);
  snprintf(str_repeat, 3, "%i", num_repeat);

  selection_win = selection_layer_create(GRect((bounds.size.w - 120 - 6) / 2, (bounds.size.h - 40 + 25) / 2, 120 + 6, 40), 3);
  selection_layer_set_cell_width(selection_win, 0, 40);
  selection_layer_set_cell_width(selection_win, 1, 40);
  selection_layer_set_cell_width(selection_win, 2, 40);
  selection_layer_set_cell_padding(selection_win, 3);

  selection_layer_set_inactive_bg_color(selection_win, GColorBlack);
  selection_layer_set_active_bg_color(selection_win, GColorVividCerulean);

  selection_layer_set_click_config_onto_window(selection_win, window);
  selection_layer_set_callbacks(selection_win, NULL, (SelectionLayerCallbacks) {
    .get_cell_text = selection_handle_get_text,
    .complete = selection_handle_complete,
    .increment = selection_handle_increment,
    .decrement = selection_handle_decrement,
  });
  layer_add_child(window_get_root_layer(window), selection_win);
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
