#pragma once
#include <stdint.h>

// Antall samples i ringbufferet
// 300 samples @ 1Hz = 5 minutter historikk
// Minnebruk: 300 * 28 bytes = ~8.4KB
#define HISTORY_SIZE 300

typedef struct {
    int64_t timestamp_ms;  // ms siden boot
    float   pot;
    float   weight;
    float   temp;
    float   dist;
} history_entry_t;

// Initialiser ringbuffer
void history_init(void);

// Legg til ny sample
void history_push(float pot, float weight, 
                  float temp, float dist);

// Serialiser hele bufferet til JSON-streng
// Returnerer heap-allokert streng — caller må free()
char *history_to_json(void);
