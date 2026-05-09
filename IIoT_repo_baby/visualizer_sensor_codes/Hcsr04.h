#pragma once
#include "esp_err.h"

esp_err_t hcsr04_init(void);

// temp_c: romtemperatur fra DS18B20 for kompensasjon
// Send -1000.0f om temperatur ikke er tilgjengelig
float hcsr04_read_cm(float temp_c);
