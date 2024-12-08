# ESP32 Multi-Service BLE

This repository contains Arduino IDE code for configuring an ESP32 with multi-service BLE (Bluetooth Low Energy) functionality. The ESP32 advertises multiple BLE characteristics and supports the following features:

## Features
- **GPS Data**: Periodic notifications with real-time location data.
- **SOS Alerts**: Real-time transmission and reception of emergency alerts.
- **Chat Messages**: RBi-directional chat communication.
- **Weather Updates**: Provide real-time weather data.

## BLE Services and Characteristics
- **Service UUID**: `12345678-1234-1234-1234-123456789abc`
- **Characteristics**:
  - **GPS**: `abcd1234-5678-1234-5678-abcdef123456`
  - **SOS**: `abcd1234-5678-1234-5678-abcdef654321`
  - **Chat**: `abcd1234-5678-1234-5678-abcdef987654`
  - **Weather**: `abcd1234-5678-1234-5678-abcdef111213`

## Prerequisites

### Hardware:
 - ESP32 Development Board
 - Data Cable to connect ESP32 to Power source
 - BLE-compatible mobile device (e.g., running AquaSafe mobile app)
   
### Software:
 - Arduino IDE
 - ESP32 Board Package for Arduino (Installation Guide)
 - BLE Scanner app (e.g., nRF Connect)

   
## How to Use
1. Clone the repository:
   ```bash
   git clone https://github.com/Lora-ResearchProject/ESP32_MultiService_BLE.git

2. Upload Code to ESP32
   - Open the .ino file from this repository in Arduino IDE.
   - Select the correct board and port:
      - Board: ESP32 Dev Module
      - Port: Select the appropriate COM port for your ESP32.
   - Compile and upload the code to the ESP32.
   - If upload fails:
      - Hold the BOOT button on the ESP32 while uploading.

3. Verify BLE Server Advertising
   - Use a BLE scanner app (e.g., nRF Connect).
   - Scan for available BLE devices.
   - Confirm that the ESP32 is advertising as ESP32-MultiService.
   - Ensure the service and characteristic UUIDs are visible in the scanner.

## Integration with AquaSafe Mobile App
The UUIDs in the ESP32 firmware must match those defined in the AquaSafe mobile app's ```lib/utils/constants.dart```.

For example:
```
static const String gpsCharacteristicUuid = "abcd1234-5678-1234-5678-abcdef123456";
static const String sosCharacteristicUuid = "abcd1234-5678-1234-5678-abcdef654321";
static const String chatCharacteristicUuid = "abcd1234-5678-1234-5678-abcdef987654";
static const String weatherCharacteristicUuid = "abcd1234-5678-1234-5678-abcdef111213";
```

## Testing with the AquaSafe Mobile App
  1. Run the AquaSafe app and confirm it connects to the BLE service.
  2. Test the following functionalities:
      - GPS Data:
        - Verify periodic notifications from the gpsCharacteristic.
      - SOS Alerts:
        - Trigger an SOS alert from the app and confirm reception on the ESP32.
        - Click SOS alerts card view and confirm transmition from ESP32 
      - Chat Messages:
        - Send and receive chat messages between the app and ESP32.
      - Weather Updates:
        - Confirm periodic weather notifications.

## Troubleshooting
  1. ESP32 Not Advertising:
      - Recheck the SERVICE_UUID and characteristic UUIDs in the code.
      - Restart the ESP32 and ensure it is powered correctly.

  2. Cannot Connect to ESP32:
      - Verify UUID synchronization between the ESP32 firmware and the AquaSafe app.
      - Check for physical connection issues with the ESP32.

  3. No Data Sent/Received:
      - Ensure the app calls the correct BLE characteristic with valid data format.
      - Check the Arduino Serial Monitor for incoming data logs on the ESP32.

    