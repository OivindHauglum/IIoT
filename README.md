# IIoT undervisningsplattform (ESP32-C6)

Lokal undervisningsrigg som kjører helt uten internett.

## Mål oppfylt

- ESP32-C6 kjører som **WiFi Access Point**.
- Innebygget HTTP-server leverer `index.html`.
- WebSocket sender sanntidsdata hvert **100 ms**.
- Sensorer:
  - 12-bit ADC (potmeter)
  - I2C VL53L0X (TOF)
- IIR-filter med justerbar `alpha` via endpoint `/alpha`.
- Frontend viser:
  - Chart.js tidsserier
  - Canvas-gauge
  - CSS-animert MING-stack flow
  - 9 undervisningssider

## Struktur

- `src/main.cpp` – firmware (AP, web, websockets, sensors, IIR)
- `data/index.html` – dashboard + undervisningssider
- `platformio.ini` – build-oppsett og biblioteker

## Dataformat

WebSocket JSON payload:

```json
{
  "potRaw": 0,
  "potFilt": 0,
  "tofRaw": 0,
  "tofFilt": 0
}
```

## Endepunkter

- `GET /` – dashboard
- `GET /alpha` – nåværende alpha
- `POST /alpha?value=0.35` – oppdater alpha (`0 < alpha <= 1`)

## Kjøring (PlatformIO)

```bash
pio run
pio run -t upload
pio run -t uploadfs
pio device monitor
```

## Nettverk

- SSID: `IIoT-ESP32-C6`
- Passord: `iiotclass`
- HTTP: `http://192.168.4.1`
- WS: `ws://192.168.4.1:81/`

## Skalering (videre)

- MQTT broker lokalt
- Lokal flow-engine
- Ringbuffer logging
- Hall-of-fame persistent storage
- Offline Chart.js assets
