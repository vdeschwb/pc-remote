#include <pebble.h>
#include <utils.h>

void set_info(info_t *info, char *text, uint8_t clearable_after) {
    info -> text = text;
    if (clearable_after == 0) {
        info -> clearable_counter = 0;
    } else {
        info -> clearable_counter = clearable_after + 1;
    }
    info -> show = true;
}

void clear_info(info_t *info) {
    if (info -> clearable_counter == 1) {
        info -> show = false;
    } else {
        info -> clearable_counter--;
    }
}

bool persist_data(uint16_t *data) {
    LOGI("Persist data...");
    bool succ=true;
    for (int i=0; i<data[0]+1; i+=2) {
        int code_1 = persist_write_int(STORAGE_BASE_KEY + DATA_KEY + i, data[i] << 16 | data[i+1]);
        succ &= (code_1 == 0);
        LOGI("%d", code_1);    
    }
    return succ;
}

void load_data(uint16_t *data) {
    LOGI("Loading data...");
    bool static_data = true;
    if (static_data) {
        uint16_t STATICDATA[2*23+1] = {23, 10, 0, 50, 3, 30, 3, 60, 0, 120, 7, 75, 7, 75, 7, 60, 10, 75, 6, 70, 0, 70, 4, 90, 3, 150, 9, 90, 3, 120, 7, 20, 0, 75, 4, 120, 9, 180, 15, 90, 12, 120, 0, 50, 6, 0, 0};
        memcpy(data, STATICDATA, sizeof(STATICDATA));
    } else {
        uint16_t len = persist_read_int(STORAGE_BASE_KEY + DATA_KEY) >> 16;
        int32_t tmp;
        for (int i=0; i<len+1; i+=2) {
            tmp = persist_read_int(STORAGE_BASE_KEY + DATA_KEY + i);
            data[i] = tmp >> 16;
            data[i+1] = tmp & 0xFFFF;
        }
    }
    LOGI("Done.");
}

void send_command(int value) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    if (!iter) {
        // Error creating outbound message
        return;
    }

    dict_write_int(iter, COMMAND_KEY, &value, sizeof(int), true);
    dict_write_end(iter);

    app_message_outbox_send();
}
