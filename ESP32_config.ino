#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// UUIDs for the service and characteristics
#define SERVICE_UUID        "12345678-1234-1234-1234-123456789abc" // General Service UUID

#define GPS_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef123456" // GPS Data
#define SOS_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef654321" // SOS Alerts
#define CHAT_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef987654" // Chat Messages
#define WEATHER_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef111213" // Weather Updates

BLECharacteristic *gpsCharacteristic;
BLECharacteristic *sosCharacteristic;
BLECharacteristic *chatCharacteristic;
BLECharacteristic *weatherCharacteristic;

bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Client connected");
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Client disconnected");
    BLEDevice::startAdvertising();
  }
};

void setup() {
  Serial.begin(115200);
  
  // Initialize BLE
  BLEDevice::init("ESP32-MultiService");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create a BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // GPS Characteristic
  gpsCharacteristic = pService->createCharacteristic(
                        GPS_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
  gpsCharacteristic->addDescriptor(new BLE2902());

  // SOS Characteristic
  sosCharacteristic = pService->createCharacteristic(
                        SOS_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
  sosCharacteristic->addDescriptor(new BLE2902());

  // Chat Characteristic
  chatCharacteristic = pService->createCharacteristic(
                        CHAT_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
  chatCharacteristic->addDescriptor(new BLE2902());

  // Weather Characteristic
  weatherCharacteristic = pService->createCharacteristic(
                            WEATHER_CHARACTERISTIC_UUID,
                            BLECharacteristic::PROPERTY_READ |
                            BLECharacteristic::PROPERTY_NOTIFY
                          );
  weatherCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("Waiting for a client connection...");
}

void loop() {
  if (deviceConnected) {
    // Example: Notify GPS data periodically
    String gpsData = "{\"id\": \"123-0000\", \"l\": \"80.12321-13.32432\"}";
    gpsCharacteristic->setValue(gpsData.c_str());
    gpsCharacteristic->notify();
    Serial.println("GPS data sent: " + gpsData);

    delay(5000); // Delay for demonstration
  }
}
