# H6 — Spectrum Analyzer

WiFi/BLE spectrum visualization and channel energy detection on ESP32-C6.

## Overview

Real-time RF spectrum analyzer that:
- Scans all 13 WiFi channels for energy levels
- Detects nearby BLE devices and signal strength
- Displays ASCII spectrum visualization via Serial
- Tracks channel utilization over time
- Identifies busiest frequency bands

## Hardware

| Component | Connection | Role |
|-----------|------------|------|
| ESP32-C6 DevKit | Main board | WiFi + BLE dual-mode radio |

## Features

- **WiFi energy detection**: RSSI-based power measurement per channel
- **BLE scanning**: Device discovery with RSSI and name extraction
- **Channel history**: Rolling average over 64 samples
- **ASCII visualization**: Bar graph output for channel energy
- **Busiest channel identification**: Automatic peak detection

## Serial Output

```
┌──────── WiFi Channel Energy (dBm) ────────┐
│ Ch 1 [ -85.2] ################              │
│ Ch 6 [ -42.1] ######################################
│ Ch 11[-55.3] ############################### │
└───────────────────────────────────────────┘

── BLE Devices Detected: 5 ──
  AA:BB:CC:DD:EE:FF [ -65] ############## Sensor
```

## Build & Flash

```bash
arduino-cli compile --fqbn esp32:esp32:esp32c6 firmware/
arduino-cli upload --fqbn esp32:esp32:esp32c6 --port /dev/ttyUSB0 firmware/
```

## Legal Disclaimer

**IMPORTANT: Read before use.**

This project is provided for **educational and authorized security testing purposes only**.

### Authorization Requirements
- You MUST have explicit written permission from the network owner before using this tool
- Unauthorized interception of network communications is illegal under federal and state laws
- This tool should ONLY be used on networks you own or have written authorization to test

### Legal Framework
- **Computer Fraud and Abuse Act (CFAA)**: Unauthorized access to computer systems is a federal crime
- **Wiretap Act (18 U.S.C. § 2511)**: Interception of electronic communications without consent is illegal
- **State Laws**: Many states have additional computer crime and wiretapping statutes
- **GDPR/CCPA**: Data collection may be subject to privacy regulations

### Acceptable Use
- Testing security of your own networks
- Authorized penetration testing with written scope
- Academic research in controlled lab environments
- Security education and training

### Prohibited Use
- Intercepting communications on networks you don't own
- Attacking infrastructure without authorization
- Any activity that violates applicable laws or regulations
- Commercial use without proper licensing

### No Warranty
This software is provided "AS IS" without warranty of any kind. The author is not responsible for any misuse or damage caused by this software.

### Responsible Disclosure
If you discover vulnerabilities using this tool, follow responsible disclosure practices:
1. Report to the vendor/owner privately
2. Allow reasonable time for remediation
3. Do not exploit beyond proof of concept

## License

MIT
