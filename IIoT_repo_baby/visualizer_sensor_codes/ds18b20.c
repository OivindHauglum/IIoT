#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "ds18b20.h"

#define DS18B20_GPIO GPIO_NUM_4

// IIR-filter: alpha=0.1 — temperatur endrer seg sakte, så lav alpha er riktig
// Gir svært stabile verdier i GUI uten merkbar forsinkelse
#define TEMP_ALPHA 1.0f
static float temp_filtered = 0.0f;
static int   temp_init     = 0;

static void ow_low()     { gpio_set_direction(DS18B20_GPIO, GPIO_MODE_OUTPUT); gpio_set_level(DS18B20_GPIO, 0); }
static void ow_release() { gpio_set_direction(DS18B20_GPIO, GPIO_MODE_INPUT); }
static int  ow_read()    { return gpio_get_level(DS18B20_GPIO); }

static int ow_reset()
{
    ow_low();
    esp_rom_delay_us(480);
    ow_release();
    esp_rom_delay_us(70);
    int presence = !ow_read();
    esp_rom_delay_us(410);
    return presence;
}

static void ow_write_bit(int bit)
{
    ow_low();
    if (bit) { esp_rom_delay_us(6); ow_release(); esp_rom_delay_us(64); }
    else     { esp_rom_delay_us(60); ow_release(); esp_rom_delay_us(10); }
}

static int ow_read_bit()
{
    int bit;
    ow_low(); esp_rom_delay_us(6);
    ow_release(); esp_rom_delay_us(9);
    bit = ow_read();
    esp_rom_delay_us(55);
    return bit;
}

static void ow_write_byte(uint8_t byte)
{
    for (int i = 0; i < 8; i++) { ow_write_bit(byte & 0x01); byte >>= 1; }
}

static uint8_t ow_read_byte()
{
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) { byte >>= 1; if (ow_read_bit()) byte |= 0x80; }
    return byte;
}

void ds18b20_init()
{
    gpio_set_direction(DS18B20_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(DS18B20_GPIO, GPIO_PULLUP_ONLY);
}

float ds18b20_read_temp_c()
{
    static int         measuring   = 0;
    static TickType_t  start_time  = 0;

    // Start ny måling
    if (!measuring) {
        if (!ow_reset()) return -1000.0f;
        ow_write_byte(0xCC);  // Skip ROM
        ow_write_byte(0x44);  // Convert T
        start_time = xTaskGetTickCount();
        measuring  = 1;
        return -1000.0f;
    }

    // Vent 750ms konverteringstid
    if (xTaskGetTickCount() - start_time < pdMS_TO_TICKS(750))
        return -1000.0f;

    // Les resultat
    measuring = 0;
    if (!ow_reset()) return -1000.0f;
    ow_write_byte(0xCC);
    ow_write_byte(0xBE);

    uint8_t temp_l = ow_read_byte();
    uint8_t temp_h = ow_read_byte();
    int16_t raw    = (temp_h << 8) | temp_l;
    float   temp   = (float)raw / 16.0f;

    // Sanity check — forkast åpenbart ugyldige verdier
    if (temp < -55.0f || temp > 125.0f) return -1000.0f;

    // IIR-filter — initialiser med første gyldige måling
    if (!temp_init) {
        temp_filtered = temp;
        temp_init = 1;
    } else {
        temp_filtered = TEMP_ALPHA * temp + (1.0f - TEMP_ALPHA) * temp_filtered;
    }

    return temp_filtered;
}
