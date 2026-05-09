#ifndef POTMETER_H
#define POTMETER_H

// Init
void potmeter_init(void);

// Raw ADC-verdi
int potmeter_read_raw(void);

// Spenning i Volt
float potmeter_read_volt(void);

#endif