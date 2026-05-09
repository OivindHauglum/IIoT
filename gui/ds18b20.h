#pragma once

#include <stdint.h>

// Init sensor
void ds18b20_init(void);

// Temperatur i Celsius
float ds18b20_read_temp_c(void);

// Temperatur i Fahrenheit (valgfritt)
// float ds18b20_read_temp_f(void);