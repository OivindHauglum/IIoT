# Sensor Expansion Guide

The hardware platform is a starting point, not a fixed product.
The ESP32-S3 has more GPIO pins than sensors currently connected.
Most of what is listed below requires minimal firmware changes
and costs less than a textbook chapter.

---

## Currently Implemented

| Sensor | Interface | Concept demonstrated |
|--------|-----------|---------------------|
| Potentiometer | ADC (GPIO7) | Analogue-to-digital conversion, IIR low-pass filter |
| HX711 + load cell | Bit-bang (GPIO5/6) | Wheatstone bridge, delta-sigma ADC, mechanical noise |
| DS18B20 | 1-Wire (GPIO4) | Digital serial protocol, thermal dynamics, state machine |
| VL53L1X | I2C (GPIO8/9) | Time-of-flight measurement, laser physics |
| LED ×5 | GPIO (GPIO10–14) | Proximity indicator, real-time edge response |

---

## Drop-In Sensor Expansions

These sensors connect with minimal wiring and small firmware additions.
All are compatible with existing I2C, ADC or GPIO infrastructure.

### Knock / Vibration — SW-420, KY-031, ADXL345

**Subject:** Signal processing — impulse response, high-pass vs low-pass

A knock sensor produces impulse spikes in the signal.
This is the clearest possible demonstration of why high-pass filters
exist: the spike IS the high-frequency energy.
Apply a low-pass filter and the spike disappears.
Apply a high-pass filter and only the spike remains.

Students can physically tap the sensor and watch the GUI respond.
The abstraction becomes physical.

**Also useful for:** machine condition monitoring, predictive maintenance,
structural impact detection. Industrial operators recognise this immediately
— it is what accelerometers on pump housings actually do.

**Connection cost:** < 50 NOK. Interface: GPIO interrupt or ADC.

---

### Temperature + Heating Element — DS18B20 + PWM resistor

**Subject:** Control theory — closed-loop PID, step response

The DS18B20 is already implemented. Add a small power resistor
(or Peltier element) driven by a PWM GPIO as a heater.

This creates a real closed-loop temperature control system:

```
Setpoint → PID controller → heater PWM → thermal process → DS18B20 → feedback
```

Students can:
- Observe P-only control: fast but steady-state error remains
- Add integral: offset disappears, overshoot may appear
- Add derivative: damps oscillation, sensitive to noise

The step response curve in `control_theory_dashboard.html` becomes
something they can touch. The second-order thermal system they simulated
is now in their hands.

**Connection cost:** < 30 NOK. Interface: PWM GPIO + existing DS18B20.

---

### Microphone — MAX4466, INMP441

**Subject:** Signal processing — frequency spectrum, FFT, filtering

A microphone connected to ADC produces a real audio signal.
Combined with JavaScript FFT in the browser GUI, students can:

- See their own voice as a frequency spectrum
- Observe how filtering affects the spectrum in real time
- Understand why sampling rate matters (Nyquist theorem)

The unit circle and z-plane widgets in `signalbehandling_widgets.html`
connect directly: those poles and zeros shape what you hear.

**Connection cost:** < 80 NOK. Interface: ADC (MAX4466) or I2S (INMP441).

---

### Soil Moisture / Conductivity

**Subject:** Industrial sensing, calibration, non-linearity

A resistive soil moisture sensor demonstrates:
- Non-linear sensor response (needs calibration curve)
- Environmental drift (temperature affects resistance)
- Why calibration matters in industrial measurement

Relevant for agriculture, water management, environmental monitoring.
Connects directly to ADC — same code as potentiometer.

**Connection cost:** < 20 NOK. Interface: ADC.

---

### HC-SR04 Ultrasonic (already evaluated)

**Subject:** Signal processing — temperature compensation, sensor fusion

Already integrated as fallback for VL53L1X.
Demonstrates temperature compensation: speed of sound varies with
temperature, DS18B20 corrects the distance calculation in real time.

Students can warm the sensor with their hand and watch the corrected
and uncorrected distance values diverge. Sensor fusion made visible.

---

### Flex Sensor / Strain Gauge Variant

**Subject:** Electronics — Wheatstone bridge, strain measurement

Same principle as the existing load cell but in a different form factor.
Bend the flex sensor and measure resistance change.
Demonstrates piezoresistive effect and bridge circuits physically.

**Connection cost:** < 60 NOK. Interface: ADC with voltage divider.

---

## The Broader Point

The hardware platform already exists. The firmware infrastructure
already exists. The browser GUI already exists.

Adding a new sensor is mostly a question of:
1. Which GPIO is available (GPIO15, GPIO16, GPIO17, GPIO18...)
2. Which interface does it use (ADC, I2C, GPIO, SPI)
3. What concept does it demonstrate best

The cost of expanding the platform is low.
The pedagogical reach of each addition is wide.

That is the argument this repository exists to make.

---

*See `examples/` for commented firmware code.*
*See `gui/` for interactive browser tools that connect to each concept.*
