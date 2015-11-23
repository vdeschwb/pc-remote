#pragma once
#include <pebble.h>

#define NUM_COMMANDS 4

#define STORAGE_BASE_KEY 22456709
#undef RELEASE

#ifdef RELEASE
    #pragma message "---- COMPILING IN RELEASE MODE - NO LOGGING WILL BE AVAILABLE ----"
    #define LOGI(...)
    #define LOG(...)
    #define WARN(...)
    #define ERROR(...)
    #undef APP_LOG
    #define APP_LOG(...)
#else
    #define LOGI(...) ((void) APP_LOG(APP_LOG_LEVEL_DEBUG, __VA_ARGS__))
    #define LOG(...) app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
    #define WARN(...) app_log(APP_LOG_LEVEL_WARNING, __FILE__, __LINE__, __VA_ARGS__)
    #define ERROR(...) app_log(APP_LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#endif

// Storage keys
typedef enum {
    DISPLAY_STYLE_KEY=0,
    BUZZ_BEFORE_NEXT_TYPE_KEY=1,
    BUZZ_BEFORE_NEXT_VALUE_KEY=2,
    FPS_KEY=3,
    DATA_KEY=4,
    COMMAND_KEY=42
} storage_key_e;

typedef enum {TRAIN, PRESENT} mode_e;

typedef enum {COUNTDOWN, TIMER} display_style_e;

typedef enum {OFF, ABSOLUTE, RELATIVE} buzz_before_next_type_t;

typedef struct {
    storage_key_e key;
    void *value;
} config_t;

typedef struct {
    char* text;
    uint8_t clearable_counter;
    bool show;
} info_t;

void set_info(info_t *, char *, uint8_t);

void clear_info(info_t *);

bool persist_data(uint16_t *);

void load_data(uint16_t *);

void send_command(int);

static char* COMMANDS[NUM_COMMANDS] = {"UP", "DOWN", "LEFT", "RIGHT"};

#ifndef PBL_PLATFORM_APLITE
    static void battery_proc(Layer *layer, GContext *ctx) {
      // Emulator battery meter on Aplite
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_draw_rect(ctx, GRect(126, 4, 14, 8));
      graphics_draw_line(ctx, GPoint(140, 6), GPoint(140, 9));

      BatteryChargeState state = battery_state_service_peek();
      int width = (int)(float)(((float)state.charge_percent / 100.0F) * 10.0F);
      graphics_context_set_fill_color(ctx, GColorWhite);
      graphics_fill_rect(ctx, GRect(128, 6, width, 4), 0, GCornerNone);
    }
#endif