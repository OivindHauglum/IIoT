#include "history.h"
#include "cJSON.h"
#include "esp_timer.h"
#include <string.h>
#include <stdlib.h>

// =====================
// RINGBUFFER
// =====================
static history_entry_t buffer[HISTORY_SIZE];
static int   head  = 0;   // neste skriveposisjon
static int   count = 0;   // antall gyldige entries

void history_init(void)
{
    memset(buffer, 0, sizeof(buffer));
    head  = 0;
    count = 0;
}

void history_push(float pot, float weight,
                  float temp, float dist)
{
    buffer[head].timestamp_ms =
        esp_timer_get_time() / 1000LL;  // µs → ms
    buffer[head].pot    = pot;
    buffer[head].weight = weight;
    buffer[head].temp   = temp;
    buffer[head].dist   = dist;

    head = (head + 1) % HISTORY_SIZE;
    if (count < HISTORY_SIZE) count++;
}

char *history_to_json(void)
{
    cJSON *root    = cJSON_CreateObject();
    cJSON *samples = cJSON_CreateArray();

    // Start fra eldste entry
    int start = (count < HISTORY_SIZE)
                ? 0
                : head;  // head peker på eldste når full

    for (int i = 0; i < count; i++) {
        int idx = (start + i) % HISTORY_SIZE;
        cJSON *entry = cJSON_CreateObject();
        cJSON_AddNumberToObject(entry, "t",
            (double)buffer[idx].timestamp_ms);
        cJSON_AddNumberToObject(entry, "pot",
            buffer[idx].pot);
        cJSON_AddNumberToObject(entry, "weight",
            buffer[idx].weight);
        cJSON_AddNumberToObject(entry, "temp",
            buffer[idx].temp);
        cJSON_AddNumberToObject(entry, "dist",
            buffer[idx].dist);
        cJSON_AddItemToArray(samples, entry);
    }

    cJSON_AddItemToObject(root, "samples", samples);
    cJSON_AddNumberToObject(root, "count", count);

    char *json = cJSON_Print(root);
    cJSON_Delete(root);
    return json;  // caller må free()
}
