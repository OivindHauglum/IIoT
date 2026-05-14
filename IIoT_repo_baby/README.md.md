# Visualize the Invisible

A playground project — born from a larger one.

This repository grew out of **The Visualizer — IIoT in a Shoebox**,
a USN Bachelor 2026 thesis project.
The original assignment was to build a compact IIoT training platform
for industrial operators. Some work is documented in
[IIoT_The_Visualizer](https://github.com/OivindHauglum/IIoT_The_Visualizer).

This repository is something else: a sandbox for exploring what happens
when the same idea — making invisible things visible and tangible —
is applied beyond its original scope.

---

## The Idea

The Visualizer teaches operators how data travels from a physical sensor
to a browser dashboard. It makes the abstract pipeline concrete.

That same principle applies everywhere signals, systems and mathematics
are taught. Formulas in textbooks are correct. They are also, frequently,
incomprehensible without something to interact with.

This repository is an experiment in making that interaction fast, simple
and cheap to produce — so it can reach as many people as possible,
regardless of background, age or institution.

No servers. No installation. One HTML file. Open in Chrome.

---

## Interactive Tools (gui/)

Standalone HTML files. Open directly in Chrome — fully offline.

| File | Subject | Description |
|------|---------|-------------|
| `signalbehandling_widgets.html` | Signal Processing | Unit circle, z-plane poles, IIR filter frequency response, live sensor signal with noise |
| `control_theory_dashboard.html` | Control Theory | PID controller simulation, step response (2nd order), Bode plot, root locus |
| `iiot_dashboard_c6.html` | IIoT Pipeline | Early C6 prototype — WiFi, historical reference (Feb 2026) |

---

## Firmware Examples (examples/)

Commented pedagogical code — not production firmware.
Each file explains the concept, the hardware connection,
and which GUI widget it connects to.

| File | Sensor | Concept |
|------|--------|---------|
| `potmeter_iir.c` | Potentiometer (ADC) | IIR filter, z-plane pole position |
| `ds18b20_statemachine.c` | DS18B20 temperature | Moore FSM, non-blocking firmware, thermal dynamics |
| `knock_sensor_ringbuffer.c` | SW-420 / KY-031 knock | Impulse response, high-pass vs low-pass, ring buffer |

---

## Sensor Expansion

The hardware platform has more GPIO pins than sensors are currently connected to.
Most expansions cost less than a textbook chapter.

**Knock / vibration sensor** — SW-420, KY-031, ADXL345 (< 50 NOK)
Connect to any GPIO. Students tap the sensor and watch impulse spikes appear.
Apply a high-pass filter: spikes survive. Apply a low-pass filter: spikes disappear.
The clearest possible demonstration of why LP vs HP matters.
Also directly relevant to industrial machine condition monitoring.

**DS18B20 + heater** — existing sensor + PWM resistor (< 30 NOK)
Add a power resistor as a heater. The temperature sensor becomes a PID feedback loop.
The step response curve in `control_theory_dashboard.html` becomes something
students can touch. Second-order thermal dynamics, in their hands.

**Microphone** — MAX4466 or INMP441 (< 80 NOK)
ADC or I2S audio input. Combined with JavaScript FFT in the browser,
students can see their own voice as a frequency spectrum.
The unit circle and z-plane widgets connect directly: those poles shape what you hear.

See `SENSOR_EXPANSION.md` for the full list with connection details.

---

## Expansion Possibilities

The barrier to adding new learning modules is primarily software, not hardware.
The hardware already exists.

With the right firmware and GUI, the same device can support:

- **Signal processing** — live filter effects on real sensor data
- **Control systems** — closed-loop PID with a physical process variable
- **Electronics** — Wheatstone bridge, ADC resolution, sensor calibration
- **Mathematics** — unit circle, Euler's formula, complex numbers
- **Communication systems** — modulation and noise visualised
- **Machine monitoring** — vibration, impulse detection, predictive maintenance

---

## Philosophy

This is not a commercial product. It is not a finished project.
It is a pragmatic argument that the tools available today make it
possible to build genuinely useful educational tools quickly,
cheaply and openly — if someone decides to do it.

The word *transcendence* means to climb over.
That is what this is for.

---

## Origin

Developed alongside a bachelor thesis, late evenings in May 2026,
while revisiting textbooks from old courses and wondering why
the formulas were still as frightening as they had always been.

The answer was obvious. The solution turned out to be a slider bar.

---

*Made with natural curiosity. Cannot be engineered away.*
