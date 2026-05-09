/* ============================================================
   potmeter_iir.c
   Potentiometer (ADC) with first-order IIR filter

   Pedagogical example — The Visualizer / Visualize the Invisible
   USN Bachelor 2026 — Øivind Hauglum

   Concept demonstrated:
     - Analogue-to-digital conversion (ADC)
     - First-order IIR low-pass filter
     - How alpha controls the trade-off between
       noise rejection and response speed

   Hardware:
     - Potentiometer wiper → GPIO7 (ADC1_CHANNEL_6)
     - VCC → 3.3V, GND → GND

   Connection to GUI:
     signalbehandling_widgets.html — "Signal & Noise" page
     Move the alpha slider to see this filter in action.
   ============================================================ */

#include "driver/adc.h"
#include "esp_log.h"

static const char *TAG = "potmeter";

/* IIR filter coefficient.
   Lower alpha = more smoothing, slower response.
   Higher alpha = faster response, more noise.

   Sensor     alpha   Reason
   --------   -----   ------
   Potmeter   0.30    Responsive to hand movement
   ToF        0.30    Moderate distance smoothing
   Load cell  0.05    Heavy mechanical noise
   DS18B20    1.00    Thermal process is slow enough
*/
#define ALPHA 0.30f

static float filtered_value = 0.0f;

void potmeter_init(void) {
    /* Configure ADC1 channel 6 (GPIO7) */
    adc1_config_width(ADC_WIDTH_BIT_12);           /* 12-bit: 0-4095 */
    adc1_config_channel_atten(ADC1_CHANNEL_6,
                               ADC_ATTEN_DB_12);   /* 0-3.3V range   */
    ESP_LOGI(TAG, "Potmeter initialised on GPIO7");
}

float potmeter_read_filtered(void) {
    int raw = adc1_get_raw(ADC1_CHANNEL_6);

    /* First-order IIR (Exponential Moving Average):
       y[n] = alpha * x[n] + (1 - alpha) * y[n-1]

       This is a discrete low-pass filter with pole at z = (1 - alpha).
       See z-plane widget in signalbehandling_widgets.html for
       a visual explanation of what this means in frequency domain. */
    filtered_value = ALPHA * (float)raw + (1.0f - ALPHA) * filtered_value;

    return filtered_value;
}

int potmeter_read_raw(void) {
    return adc1_get_raw(ADC1_CHANNEL_6);
}
