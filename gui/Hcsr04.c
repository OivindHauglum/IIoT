#include "hcsr04.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "HCSR04";

#define TRIG_PIN   15
#define ECHO_PIN   16
#define TIMEOUT_US 30000

// Median-filter: 5 målinger, forkast høyeste og laveste, middel av de 3 midterste
#define SAMPLES 5

// IIR-filter
#define DIST_ALPHA 0.3f
static float dist_filtered = 0.0f;
static int   dist_init     = 0;

esp_err_t hcsr04_init(void)
{
    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(TRIG_PIN, 0);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
    ESP_LOGI(TAG, "HC-SR04 init OK (TRIG=%d ECHO=%d)", TRIG_PIN, ECHO_PIN);
    return ESP_OK;
}

// Én rå måling i mikrosekunder
static int hcsr04_single_us(void)
{
    gpio_set_level(TRIG_PIN, 0);
    esp_rom_delay_us(2);
    gpio_set_level(TRIG_PIN, 1);
    esp_rom_delay_us(10);
    gpio_set_level(TRIG_PIN, 0);

    int timeout = TIMEOUT_US;
    while (gpio_get_level(ECHO_PIN) == 0) {
        esp_rom_delay_us(1);
        if (--timeout <= 0) return -1;
    }

    int duration = 0;
    timeout = TIMEOUT_US;
    while (gpio_get_level(ECHO_PIN) == 1) {
        esp_rom_delay_us(1);
        duration++;
        if (--timeout <= 0) return -1;
    }

    return duration;
}

// Enkel boble-sort for 5 elementer
static void sort5(float arr[5])
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 - i; j++) {
            if (arr[j] > arr[j+1]) {
                float tmp = arr[j];
                arr[j]    = arr[j+1];
                arr[j+1]  = tmp;
            }
        }
    }
}

// Hovedfunksjon med temperaturkompensasjon og median-filter
// temp_c: romtemperatur fra DS18B20 (send -1000 om ikke tilgjengelig)
float hcsr04_read_cm(float temp_c)
{
    // Lydhastighet basert på temperatur
    // v(m/s) = 331.3 + 0.606 * T
    // µs/cm  = 10000 / v(m/s)
    float us_per_cm;
    if (temp_c > -100.0f && temp_c < 60.0f) {
        float v = 331.3f + 0.606f * temp_c;
        us_per_cm = 10000.0f / v;
        ESP_LOGD(TAG, "Lydhastighet %.1f m/s (%.2f us/cm) ved %.1f C",
                 v, us_per_cm, temp_c);
    } else {
        // Fallback til standard 20°C = 343 m/s
        us_per_cm = 58.3f;
    }

    // Ta SAMPLES målinger med 60ms pause (anbefalt i datasheet)
    float samples[SAMPLES];
    int valid = 0;

    for (int i = 0; i < SAMPLES; i++) {
        int us = hcsr04_single_us();
        if (us > 0) {
            float cm = (float)us / us_per_cm;
            if (cm >= 2.0f && cm <= 400.0f) {
                samples[valid++] = cm;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(60));
    }

    if (valid == 0) return -1.0f;

    // Median-filter: sorter og bruk midterste verdier
    float raw;
    if (valid >= 3) {
        sort5(samples);  // sorter gyldige verdier
        // Bruk middel av de midterste (forkast høyeste og laveste)
        float sum = 0;
        for (int i = 1; i < valid - 1; i++) sum += samples[i];
        raw = sum / (valid - 2);
    } else {
        // For få målinger — bruk middel
        float sum = 0;
        for (int i = 0; i < valid; i++) sum += samples[i];
        raw = sum / valid;
    }

    // IIR-filter
    if (!dist_init) {
        dist_filtered = raw;
        dist_init = 1;
    } else {
        dist_filtered = DIST_ALPHA * raw + (1.0f - DIST_ALPHA) * dist_filtered;
    }

    return dist_filtered;
}
