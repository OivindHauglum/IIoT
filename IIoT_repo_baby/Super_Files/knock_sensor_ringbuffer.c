/* ============================================================
   knock_sensor_ringbuffer.c
   Knock / vibration sensor (SW-420 or KY-031) with ring buffer

   Pedagogical example — The Visualizer / Visualize the Invisible
   USN Bachelor 2026 — Øivind Hauglum

   Concepts demonstrated:
     - Digital interrupt-driven event detection
     - Ring buffer as a sliding time window (retention policy)
     - Signal spikes and transient events
     - Why high-pass filtering is useful: knock IS the high-frequency
       component. A low-pass filter would erase exactly what we want.

   Connection to GUI:
     signalbehandling_widgets.html — "LP vs HP" page.
     Knock = impulse signal. High-frequency energy. High-pass filter
     passes it. Low-pass filter kills it. That is the point.

   Hardware options (drop-in):
     - SW-420 vibration sensor → any GPIO (digital out)
     - KY-031 knock sensor      → any GPIO (digital out)
     - ADXL345 accelerometer   → I2C (more resolution, same idea)
     - MPU-6050 IMU             → I2C (full 6-axis, same ring buffer)

   GPIO: change KNOCK_PIN to any available GPIO
   ============================================================ */

#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "knock";

#define KNOCK_PIN       GPIO_NUM_17
#define RING_SIZE       300    /* 5 minutes at 1Hz — same as main sensors */
#define DEBOUNCE_US     50000  /* 50ms debounce — ignore rapid re-triggers */

/* ---- Ring buffer ---- */
typedef struct {
    int64_t timestamp_us;   /* When the knock occurred          */
    uint32_t count;         /* Knock count in this window       */
} knock_event_t;

static knock_event_t ring[RING_SIZE];
static int           ring_head   = 0;
static int           ring_count  = 0; /* Total knocks ever recorded */
static int64_t       last_knock  = 0;

/* ---- ISR — called on each knock ---- */
static void IRAM_ATTR knock_isr(void *arg) {
    int64_t now = esp_timer_get_time();

    /* Debounce: ignore if too soon after last event */
    if ((now - last_knock) < DEBOUNCE_US) return;

    last_knock = now;
    ring_count++;

    /* Write to ring buffer (overwrites oldest entry) */
    ring[ring_head].timestamp_us = now;
    ring[ring_head].count = ring_count;
    ring_head = (ring_head + 1) % RING_SIZE;
}

void knock_sensor_init(void) {
    memset(ring, 0, sizeof(ring));

    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << KNOCK_PIN),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .intr_type    = GPIO_INTR_POSEDGE,   /* Trigger on rising edge */
    };
    gpio_config(&cfg);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(KNOCK_PIN, knock_isr, NULL);

    ESP_LOGI(TAG, "Knock sensor initialised on GPIO%d", KNOCK_PIN);
}

/* Return total knock count — for JSON endpoint */
uint32_t knock_get_count(void) { return ring_count; }

/* Return knocks in last N seconds — for dashboard graph */
uint32_t knock_get_recent(int seconds) {
    int64_t cutoff = esp_timer_get_time() - (int64_t)seconds * 1000000;
    uint32_t count = 0;
    for (int i = 0; i < RING_SIZE; i++) {
        if (ring[i].timestamp_us > cutoff) count++;
    }
    return count;
}

/* ============================================================
   EXPANSION IDEAS — What a knock sensor unlocks

   1. SIGNAL PROCESSING demo:
      Plot raw knock events in GUI — students see impulse spikes.
      Apply high-pass filter: spikes survive.
      Apply low-pass filter: spikes disappear.
      That is the clearest possible demonstration of LP vs HP.

   2. MACHINE CONDITION MONITORING:
      Mount on a motor, pump or bearing housing.
      Knock rate increases before mechanical failure.
      This is real predictive maintenance — in a shoebox.
      Industrial operators recognise this immediately.

   3. ACOUSTICS AND RESONANCE (physics / engineering):
      Tap different materials. Compare resonance decay times.
      Add an ADXL345 for full vibration spectrum.

   4. STRUCTURAL MONITORING (civil engineering context):
      Attach to a surface. Monitor impact events over time.
      Ring buffer = sliding window = retention policy concept.
      Same mental model as InfluxDB — just 50kr of hardware.

   Similar sensors, minimal code change:
     MPU-6050 IMU     → I2C, full 6-axis vibration + orientation
     ADXL345          → I2C/SPI, high-resolution vibration
     Piezo disc       → ADC, analogue — needs threshold detection
     Microphone (MAX4466) → ADC, sound → FFT possible in JS GUI
   ============================================================ */
