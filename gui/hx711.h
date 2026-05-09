#pragma once

#include <stdint.h>

// Init HX711 — setter opp GPIO
void hx711_init(void);

// Les råverdi fra HX711
int32_t hx711_read(void);

// Hent nåværende baseline (tare-verdi)
int32_t hx711_get_baseline(void);

// Sett kalibreringsfaktor (enheter per gram — bestemmes eksperimentelt)
void hx711_set_scale(float scale);

// Tare — les 10 målinger og sett som ny nullpunkt
void hx711_tare(void);

// Hent vekt i kilogram (med IIR-filter)
float hx711_get_weight_kg(void);

// Hent vekt i gram (med IIR-filter)
float hx711_get_weight_g(void);