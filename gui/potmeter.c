#include "potmeter.h"
#include "driver/adc.h"

#define ADC_CHANNEL ADC1_CHANNEL_6  // GPIO7
#define ADC_MAX     4095.0f
#define VREF        3.3f

// IIR-filter: alpha=0.3 — rask nok for potmeter, men filtrerer ADC-støy
#define POT_ALPHA 1.0f
static float pot_filtered = 0.0f;
static int   pot_init     = 0;

void potmeter_init(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);
}

int potmeter_read_raw(void)
{
    return adc1_get_raw(ADC_CHANNEL);
}

float potmeter_read_volt(void)
{
    // Middel over 4 prøver for å redusere ADC-støy
    int sum = 0;
    for (int i = 0; i < 4; i++) sum += adc1_get_raw(ADC_CHANNEL);
    float raw_volt = ((float)(sum / 4) / ADC_MAX) * VREF;

    // IIR-filter
    if (!pot_init) {
        pot_filtered = raw_volt;
        pot_init = 1;
    } else {
        pot_filtered = POT_ALPHA * raw_volt + (1.0f - POT_ALPHA) * pot_filtered;
    }

    return pot_filtered;
}
