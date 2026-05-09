//Vektkalibrering for HX711 og lastcelle
// Følg instruksjonene i Serial Monitor

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "hx711.h"

void app_main(void)
{
    hx711_init();
    vTaskDelay(pdMS_TO_TICKS(500));

    ESP_LOGI("CAL", "=== HX711 KALIBRERING ===");
    ESP_LOGI("CAL", "Steg 1: Fjern all vekt fra lastcellen");
    vTaskDelay(pdMS_TO_TICKS(3000));

    // Tare
    hx711_tare();
    ESP_LOGI("CAL", "Tare OK. Baseline = %ld", hx711_get_baseline());

    ESP_LOGI("CAL", "Steg 2: Legg en KJENT vekt på lastcellen (f.eks. 100g)");
    ESP_LOGI("CAL", "Venter 5 sekunder...");
    vTaskDelay(pdMS_TO_TICKS(5000));

    // Les råverdi med kjent vekt
    int32_t sum = 0;
    for (int i = 0; i < 10; i++) sum += hx711_read();
    int32_t raw_with_weight = sum / 10;

    int32_t diff = raw_with_weight - hx711_get_baseline();
    ESP_LOGI("CAL", "Råverdi med vekt: %ld", raw_with_weight);
    ESP_LOGI("CAL", "Differanse:        %ld", diff);

    // Beregn scale for ulike kjente vekter
    float known_g = 100.0f;  // <-- endre til din kjente vekt i gram
    float scale = (float)diff / known_g;

    ESP_LOGI("CAL", "=== RESULTAT ===");
    ESP_LOGI("CAL", "Sett scale = %.2f i hx711.c", scale);
    ESP_LOGI("CAL", "Verifiserer...");

    hx711_set_scale(scale);

    while (1) {
        float g = hx711_get_weight_g();
        ESP_LOGI("CAL", "Vekt: %.1f g (skal vaere %.0f g)", g, known_g);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}