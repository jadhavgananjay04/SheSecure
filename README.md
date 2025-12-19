# SheSecure

# SheSecure Smart Pole

Women safety alert system with a handheld RF panic button (transmitter) and a smart pole (receiver) that acquires GPS, sends GSM SMS alerts, and triggers local audio/visual alarms.

## System at a Glance
- **Transmitter (panic button)**: 433MHz RF TX module with a push button and coin-cell power. Sends a fixed RF code when pressed.
- **Receiver (smart pole)**: Arduino UNO or ESP32 with 433MHz RX, GPS, GSM, buzzer, and LED. Validates RF code, grabs location, sends SMS to contacts, and plays alert tones.
- **Offline-first**: Works without internet; only GSM network is required for SMS.
- **Typical flow**: Panic press → RF signal → Receiver validates → Buzzer + LED on → GPS fix → SMS to configured numbers with coordinates.

## Features
- RF-triggered emergency alert (433MHz)
- GPS coordinate capture and validation
- GSM SMS to multiple contacts with delivery confirmation
- Local buzzer + LED alert patterns
- Cooldown/debounce to avoid spam (configurable)
- Serial debug commands for testing

## Hardware Bill of Materials
### Receiver (smart pole)
- Arduino UNO **or** ESP32
- 433MHz RF Receiver (RX470)
- NEO-6M GPS
- SIM800L GSM (separate 5V supply recommended)
- Active buzzer + red LED + 220Ω resistor
- Power: 9–12V input (UNO) or 5V/USB (ESP32) with stable 2A peak for GSM

### Transmitter (panic button)
- 433MHz RF Transmitter (TX470)
- Momentary push button
- 3V coin-cell battery and small enclosure
- Short wire antenna for better range

## Pin Connections
### Arduino UNO
```
RF RX: VCC 5V, GND GND, DATA D2
GPS:   VCC 5V, GND GND, TX D3 (SS), RX D4 (SS)
GSM:   VCC 5V (reg), GND GND, TX D5 (SS), RX D6 (SS)
Buzzer: VCC D7, GND GND
LED:    Anode D8 (220Ω), Cathode GND
```

### ESP32 (default mapping)
```
RF RX: VCC 3.3V, GND GND, DATA GPIO2
GPS:   VCC 3.3V, GND GND, TX GPIO16 (UART2), RX GPIO17 (UART2)
GSM:   VCC 5V ext, GND GND, TX GPIO18 (UART1), RX GPIO19 (UART1)
Buzzer: VCC GPIO4, GND GND
LED:    Anode GPIO5 (220Ω), Cathode GND
```

> Use the wiring diagrams in `wiring_diagrams/` for visuals.

## Firmware Layout
```
code/
├── arduino/
│   ├── individual_tests/ (rf/gps/gsm/buzzer/led)
│   └── shesecure_main.ino
└── esp32/
    ├── individual_tests/ (rf/gps/gsm/buzzer/led)
    └── shesecure_main.ino
libraries/ (RCSwitch, TinyGPS++, SoftwareSerial)
docs/ (setup_guide, troubleshooting, iot_expansion)
```

## Quick Start
1) Hardware: Wire modules per section above; insert SIM in SIM800L; ensure GSM has a stable 5V/2A source.  
2) Libraries: Install `RCSwitch`, `TinyGPS++`, and (for UNO) `SoftwareSerial`.  
3) Configure: Open `code/<board>/shesecure_main.ino` and set:
```cpp
String emergencyNumbers[] = {"+1234567890", "+0987654321"};
// If using RCSwitch, set your expected RF code (example):
const unsigned long PANIC_CODE = 123456;
```
4) Upload: Select the right board/port and upload `shesecure_main.ino`.  
5) Test modules: Run sketches in `individual_tests/` to verify RF, GPS lock, GSM SMS, buzzer, and LED.  
6) End-to-end test: Press the transmitter; confirm buzzer/LED, GPS fix, and SMS with coordinates.

## Operation Notes
- Keep the SIM card active and with SMS balance.
- Place the GPS antenna with sky view for a faster fix.
- Add 1000µF capacitor near SIM800L VCC and 0.1µF decoupling caps near modules.
- Use a short wire antenna (17.3 cm for 433MHz) on both TX and RX for range.
- Default debounce/cooldown is 30s; adjust in the main sketch if needed.

## Troubleshooting
- No GPS fix: Move outdoors, check antenna, wait for cold start.  
- GSM not responding: Verify 5V/2A supply, SIM PIN disabled, and network signal.  
- RF range is short: Add proper antenna length and reduce interference.  
- SMS missing coords: Ensure GPS has a valid fix before sending; check serial logs.  

## Future Ideas
- Firebase/IoT dashboard
- Multiple panic buttons per pole
- Geofencing and API links to emergency services
- Cloud SMS fallback
