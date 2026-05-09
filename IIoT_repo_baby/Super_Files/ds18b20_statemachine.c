/* ============================================================
   ds18b20_statemachine.c
   DS18B20 temperature sensor — non-blocking Moore state machine

   Pedagogical example — The Visualizer / Visualize the Invisible
   USN Bachelor 2026 — Øivind Hauglum

   Concepts demonstrated:
     - Moore finite state machine (FSM)
     - Non-blocking embedded firmware pattern
     - Why alpha = 1.0 for DS18B20:
       the thermal time constant (30–90s in still air) is
       far larger than the digital sampling interval (750ms),
       so the physical process is its own low-pass filter.

   Connection to GUI:
     control_theory_dashboard.html — "Step Response" page.
     The DS18B20 connected to a heater or cooler would show
     a real second-order thermal step response.
     Adjust zeta and observe — that curve is your room.

   Connection to signal processing:
     signalbehandling_widgets.html — alpha = 1.0 means pole
     at z = 0, the filter does nothing. The physics does the
     filtering instead.

   Hardware:
     - DS18B20 data → GPIO4
     - 4.7kΩ pull-up resistor between GPIO4 and 3.3V (required)
   ============================================================ */

#include <stdbool.h>
#include "esp_timer.h"
#include "esp_log.h"
/* Include your chosen 1-Wire / DS18B20 library here */

static const char *TAG = "ds18b20";

/* ---- State machine states ---- */
typedef enum {
    DS18B20_IDLE,        /* Waiting — ready to start conversion  */
    DS18B20_CONVERTING,  /* Hardware converting for 750ms        */
    DS18B20_READ_RESULT  /* Read result and store                 */
} ds18b20_state_t;

static ds18b20_state_t state       = DS18B20_IDLE;
static int64_t         conv_start  = 0;
static float           last_temp   = 25.0f; /* Last valid reading */

/* Conversion time: DS18B20 requires 750ms at 12-bit resolution */
#define CONVERSION_TIME_US 750000

void ds18b20_task_tick(void) {
    /*  Call this function from your main loop or FreeRTOS task.
        It never blocks — each call takes microseconds.

        The HTTP handler reads last_temp at any time without
        waiting. This is the key difference from a blocking read:
        the web interface stays responsive even during conversion.

        This is a Moore machine: output (last_temp) depends only
        on the stored state, not on the timing of incoming requests.
        See harris2015digital ch.3 for formal Moore/Mealy theory. */

    switch (state) {

        case DS18B20_IDLE:
            /* Start a new temperature conversion */
            /* ds18b20_start_conversion(); */
            conv_start = esp_timer_get_time();
            state = DS18B20_CONVERTING;
            break;

        case DS18B20_CONVERTING:
            /* Poll timer — do not block */
            if ((esp_timer_get_time() - conv_start) >= CONVERSION_TIME_US) {
                state = DS18B20_READ_RESULT;
            }
            break;

        case DS18B20_READ_RESULT: {
            /* float t = ds18b20_read_temperature(); */
            float t = 23.5f; /* Replace with actual read */

            /* Sentinel value check: -1000 means read failed.
               Serve last valid value rather than propagating
               an invalid reading to the GUI dashboard. */
            if (t > -100.0f) {
                last_temp = t;
            } else {
                ESP_LOGW(TAG, "Invalid reading — serving cached value %.1f", last_temp);
            }
            state = DS18B20_IDLE;
            break;
        }
    }
}

/* Called by HTTP handler — never blocks */
float ds18b20_get_temperature(void) {
    return last_temp;
}

/* ============================================================
   EXPANSION IDEA — Control Theory Demo

   Connect a small heater (e.g. power resistor) or Peltier
   element to a PWM GPIO. Use the DS18B20 as feedback.

   This creates a real closed-loop temperature control system:

     setpoint → PID controller → heater PWM → room temp → DS18B20

   Students can observe:
     - P-only: fast response but steady-state error (room ≠ setpoint)
     - PI: eliminates offset but may overshoot
     - PID: fast, no offset, damped — but sensitive to Kd if noisy

   The step response from the control_theory_dashboard.html
   becomes something they can touch. That is the point.
   ============================================================ */
