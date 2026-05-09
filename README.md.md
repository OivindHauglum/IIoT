# IIoT in a Shoebox — The Visualizer

USN Bachelor 2026 — Øivind Hauglum

A compact, self-contained IIoT demonstration platform built around the ESP32-S3.
No external infrastructure. No installation. Plug in USB-A to USB-C and open `192.168.4.1`.

---

## Hardware

| Component | Interface | GPIO |
|-----------|-----------|------|
| Potentiometer | 12-bit ADC | GPIO7 |
| HX711 load cell | Bit-bang | GPIO5 (SCK), GPIO6 (DT) |
| DS18B20 temperature | 1-Wire | GPIO4 |
| VL53L1X ToF distance | I2C | GPIO8 (SDA), GPIO9 (SCL) |
| LED proximity indicator (×5) | GPIO | GPIO10–14 |

- **MCU:** ESP32-S3-DevKitC-1-N32R16V / N8R8
- **Communication:** USB RNDIS/NCM (no WiFi in production)
- **IP:** 192.168.4.1
- **Framework:** ESP-IDF v5.x + FreeRTOS

---

## GUI

Browser-based web application served from ESP32 flash (SPIFFS).
Covers live sensor visualisation, MING stack exercises, multiple-choice questions,
fault scenarios, and instructor guide.

The `gui/` folder contains:

| File | Description |
|------|-------------|
| `signalbehandling_widgets.html` | Interactive signal processing tools — unit circle, z-plane, IIR filter, live sensor signal demo |
| `iiot_dashboard_c6.html` | Early C6 prototype dashboard (WiFi, historical reference) |

---

## Signal Processing Widgets

Four standalone interactive HTML tools built to visualise concepts from
first-year engineering mathematics through advanced DSP:

1. **Unit circle** — Euler's formula, cosinus/sinus interactive
2. **IIR lowpass** — pole position in z-plane, frequency response
3. **Lowpass vs highpass** — mirror images in z-plane
4. **Live sensor signal** — raw ADC with noise, IIR filtering, spectrum

Open any `.html` file directly in Chrome — no server needed.

---

## Development History

| Phase | Hardware | Framework | Notes |
|-------|----------|-----------|-------|
| PoC | ESP32-C6 DevKitC | Arduino | WiFi softAP, VL53L0X, potmeter |
| Final | ESP32-S3 DevKitC-1 | ESP-IDF | USB RNDIS/NCM, 4 sensors |

---

## Status

Work in progress — USN Bachelor thesis, May 2026.

```
Firmware:     ESP-IDF, in progress
GUI:          HTML/JS, functional
Documentation: LaTeX, ~170 pages
```

---

*Natural curiosity is the one thing that cannot be engineered away.*
