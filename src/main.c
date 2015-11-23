#include <pebble.h>
#include <utils.h>

#define BUF_SIZE 32

#ifndef PBL_PLATFORM_APLITE
    static StatusBarLayer *s_status_bar;
    static Layer *s_battery_layer;
#endif

Window *main_window=NULL;
TextLayer *title_layer=NULL, *info_layer=NULL;

info_t info;
uint8_t selected_command=0;

// Buffers
char *info_buf=NULL;

void write_info() {
	text_layer_set_text(info_layer, info_buf);
}

void update() {
    
    if (info.show) {
        snprintf(info_buf, BUF_SIZE, "%s", info.text);
    } else {
        snprintf(info_buf, BUF_SIZE, "%s", COMMANDS[selected_command]);
    }
    
    clear_info(&info);
    
    write_info();
    
    // Force redraw
    layer_mark_dirty(window_get_root_layer(main_window));
}

void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
    // On click of select button
    
    send_command(selected_command);
    update();
}

void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
    // On long click of select button
    
    update();
}

void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
    // On click of up button
    
    selected_command = (selected_command + 1) % NUM_COMMANDS;
    update();
}

void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
    // On click of down button
    
    selected_command = (selected_command + NUM_COMMANDS - 1) % NUM_COMMANDS;
    update();
}

void back_single_click_handler(ClickRecognizerRef recognizer, void *context) {
}

void back_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
    switch(click_number_of_clicks_counted(recognizer)) {
        case 3:
            window_stack_pop(true);
            break;
    }
}

void config_provider(Window *window) {
    // Set the click configurations

    window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
//     window_long_click_subscribe(BUTTON_ID_SELECT, 500, select_long_click_handler, NULL);

    window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);

    window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);

    window_single_click_subscribe(BUTTON_ID_BACK, back_single_click_handler);
    window_multi_click_subscribe(BUTTON_ID_BACK, 2, 3, 750, true, back_multi_click_handler);
}

void main_window_load() {
    LOGI("Main window load");
    
    Layer *window_layer = window_get_root_layer(main_window);
    GRect bounds = layer_get_bounds(window_layer);
    
    int y_offset = 0;
    #ifdef PBL_PLATFORM_BASALT
        y_offset = 16;
    #endif
    #ifdef PBL_PLATFORM_CHALK
        y_offset = 32;
    #endif
    
    // Create the titel layer
	title_layer = text_layer_create(GRect(0, y_offset, bounds.size.w, 40));
	// Set the text, font, and text alignment for title layer
	text_layer_set_text(title_layer, "PC Remote");
	text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
	// Add the title layer to the window
	layer_add_child(window_get_root_layer(main_window), text_layer_get_layer(title_layer));
   
    // Info layer
    info_layer = text_layer_create(GRect(5, y_offset + 40, bounds.size.w - 10, 30));

    // Set the font and text alignment
    text_layer_set_font(info_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
    text_layer_set_text_alignment(info_layer, GTextAlignmentCenter);

    // Add the text layer to the window
    layer_add_child(window_layer, text_layer_get_layer(info_layer));
    
    #ifndef PBL_PLATFORM_APLITE 
        // Set up the status bar last to ensure it is on top of other Layers
        s_status_bar = status_bar_layer_create();
        layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar));
    
        // Show legacy battery meter
        s_battery_layer = layer_create(GRect(bounds.origin.x, bounds.origin.y, bounds.size.w, STATUS_BAR_LAYER_HEIGHT));
        layer_set_update_proc(s_battery_layer, battery_proc);
        layer_add_child(window_layer, s_battery_layer);
    #endif
}

void main_window_unload() {
    LOGI("Main window unload");
    
    // Destroy the layers
	text_layer_destroy(title_layer);
  	text_layer_destroy(info_layer);
    #ifndef PBL_PLATFORM_APLITE
        status_bar_layer_destroy(s_status_bar);
        layer_destroy(s_battery_layer);
    #endif
}

void menu_window_load() {
    LOGI("Menu window load");
}

void menu_window_unload() {
    LOGI("Menu window unload");
}

void main_window_appear() {
    LOGI("Main window appear");
    update();
}

void menu_window_appear() {
    LOGI("Menu window appear");
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {  
    update();
}

void handle_init(void) {
    
	// Create the main window and set handlers
    LOGI("Create main window");
	main_window = window_create();
    LOGI("Set main window handlers");
    window_set_window_handlers(main_window, (WindowHandlers) {
        .load = main_window_load,
        .appear = main_window_appear,
        .unload = main_window_unload,
    });
    
    // Set the click configuration provider
    LOGI("Set main window config provider");
    window_set_click_config_provider(main_window, (ClickConfigProvider) config_provider);
    
    // Init buffers
    LOGI("Init buffers");
    info_buf = malloc(BUF_SIZE);
    
    // Push the window
    LOGI("Push main window");
	window_stack_push(main_window, false);
    
    app_message_register_inbox_received(inbox_received_handler);
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void handle_deinit(void) {
    LOGI("Deinitialize");
    
    app_message_deregister_callbacks();
      
    // Destroy buffers
    free(info_buf);
    info_buf = NULL;
	
	// Destroy the windows
	window_destroy(main_window);
    LOGI("Done");
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
