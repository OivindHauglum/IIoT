#include "hx711.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "freertos/portmacro.h"

#define HX711_DT   5
#define HX711_SCK  6

// Kalibrering — juster baseline ved å lese råverdi uten last
// Juster scale ved å legge kjent vekt og beregne: scale = (råverdi - baseline) / kjent_gram
static int32_t baseline = 254700;
static float   scale    = 430.0f;  // enheter per gram — kalibreres eksperimentelt

// IIR-filter for vekt: alpha=0.2 gir treg men stabil verdi
// Øk alpha (mot 1.0) for raskere respons, senk for mer filtrering
#define WEIGHT_ALPHA 0.6f
static float weight_filtered = 0.0f;
static int   weight_init     = 0;

void hx711_init(void)
{
    gpio_set_direction(HX711_DT, GPIO_MODE_INPUT);
    gpio_set_direction(HX711_SCK, GPIO_MODE_OUTPUT);
}

int32_t hx711_read(void)
{
    int32_t data = 0;
    int timeout  = 100000;

    while (gpio_get_level(HX711_DT) == 1) {
        if (--timeout == 0) return 0;
    }

    portDISABLE_INTERRUPTS();

    for (int i = 0; i < 24; i++) {
        gpio_set_level(HX711_SCK, 1);
        esp_rom_delay_us(2);
        data <<= 1;
        if (gpio_get_level(HX711_DT)) data++;  // les mens SCK er høy
        gpio_set_level(HX711_SCK, 0);
        esp_rom_delay_us(2);
    }

    // Gain = 128 — 1 ekstra puls
    gpio_set_level(HX711_SCK, 1);
    esp_rom_delay_us(2);
    gpio_set_level(HX711_SCK, 0);
    esp_rom_delay_us(2);

    portENABLE_INTERRUPTS();

    if (data & 0x800000) data |= 0xFF000000;
    return data;
}

int32_t hx711_get_baseline(void) { return baseline; }
void    hx711_set_scale(float s) { scale = s; }

// Tare: les 10 målinger og sett som ny nullpunkt
void hx711_tare(void)
{
    int32_t sum = 0;
    for (int i = 0; i < 10; i++) sum += hx711_read();
    baseline = sum / 10;
    weight_init = 0;  // nullstill filter
}

float hx711_get_weight_kg(void)
{
    // Middel over 5 prøver for å redusere korttidsstøy
    int32_t sum = 0;
    for (int i = 0; i < 5; i++) sum += hx711_read();
    int32_t raw = sum / 5;

    float weight_raw = (float)(raw - baseline) / scale / 1000.0f;

    // IIR-filter — initialiser med første gyldige verdi
    if (!weight_init) {
        weight_filtered = weight_raw;
        weight_init = 1;
    } else {
        weight_filtered = WEIGHT_ALPHA * weight_raw
                        + (1.0f - WEIGHT_ALPHA) * weight_filtered;
    }

    return weight_filtered;
}

float hx711_get_weight_g(void)
{
    return hx711_get_weight_kg() * 1000.0f;
}
