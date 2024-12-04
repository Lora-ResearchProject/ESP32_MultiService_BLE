#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// UUIDs for the service and characteristics
#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc"

#define GPS_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef123456"
#define SOS_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef654321"
#define CHAT_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef987654"
#define WEATHER_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef111213"

// Global variables
BLECharacteristic *gpsCharacteristic;
BLECharacteristic *sosCharacteristic;
BLECharacteristic *chatCharacteristic;
BLECharacteristic *weatherCharacteristic;

bool deviceConnected = false;

unsigned long lastReceivedTime = 0;
const unsigned long timeoutDuration = 10000;  // 10 seconds timeout duration

// Sample SOS alerts (This can be updated dynamically based on real conditions)
String sosAlerts = "[{\"id\":\"004-0000\",\"l\":\"8.01713-79.96301\",\"s\":1},"
                    "{\"id\":\"004-0000\",\"l\":\"8.01713-79.96301\",\"s\":1},"
                    "{\"id\":\"004-0000\",\"l\":\"8.01714-79.96302\",\"s\":1}]";

// Callbacks for BLE server
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) override {
    deviceConnected = true;
    Serial.println("Client connected");
  }

  void onDisconnect(BLEServer *pServer) override {
    deviceConnected = false;
    Serial.println("Client disconnected");
    BLEDevice::startAdvertising();
  }
};

// Callbacks for GPS characteristic
class GPSCharacteristicCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    String value = pCharacteristic->getValue();
    if (value.length() > 0) {
      lastReceivedTime = millis();
      Serial.print("GPS Data received: ");
      Serial.println(value);
      pCharacteristic->setValue("");
    }
  }
};

// Callbacks for SOS characteristic
class SOSCharacteristicCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    String value = pCharacteristic->getValue();
    if (value.length() > 0) {
      Serial.print("SOS Alert received from Mobile App: ");
      Serial.println(value);
    }
  }

  void onRead(BLECharacteristic *pCharacteristic) override {
    Serial.println("Fetch SOS Alerts request received");
    pCharacteristic->setValue(sosAlerts.c_str());   // Send the SOS alerts (stored in sosAlerts) back to the client
    Serial.print("SOS Alerts sent sent to mobile: ");
    Serial.println(sosAlerts);
  }
};

// Callbacks for Chat characteristic
class ChatCharacteristicCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    String value = pCharacteristic->getValue();
    if (value.length() > 0) {
      Serial.print("Chat Message received from Mobile App: ");
      Serial.println(value);
    }
  }
};

// Callbacks for Weather characteristic
class WeatherCharacteristicCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    String value = pCharacteristic->getValue();
    if (value.length() > 0) {
      Serial.print("Weather Data received from Mobile App: ");
      Serial.println(value);
    }
  }
};

void setup() {
  Serial.begin(115200);

  // Initialize BLE
  BLEDevice::init("ESP32-MultiService");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // GPS Characteristic
  gpsCharacteristic = pService->createCharacteristic(
    GPS_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
  gpsCharacteristic->addDescriptor(new BLE2902());
  gpsCharacteristic->setCallbacks(new GPSCharacteristicCallback());

  // SOS Characteristic
  sosCharacteristic = pService->createCharacteristic(
    SOS_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
  sosCharacteristic->addDescriptor(new BLE2902());
  sosCharacteristic->setCallbacks(new SOSCharacteristicCallback());

  // Chat Characteristic
  chatCharacteristic = pService->createCharacteristic(
    CHAT_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
  chatCharacteristic->addDescriptor(new BLE2902());
  chatCharacteristic->setCallbacks(new ChatCharacteristicCallback());

  // Weather Characteristic
  weatherCharacteristic = pService->createCharacteristic(
    WEATHER_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  weatherCharacteristic->addDescriptor(new BLE2902());
  weatherCharacteristic->setCallbacks(new WeatherCharacteristicCallback());

  // Start service and advertising
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("Waiting for a client connection...");
}

void loop() {
  if (deviceConnected) {
    // Check if data hasn't been received within the timeout duration
    if (millis() - lastReceivedTime > timeoutDuration) {
      Serial.println("No GPS data received from Mobile App for 10 seconds...");
      delay(5000);  // Avoid spamming the log
    }
  } else {
    delay(1000);
  }
}