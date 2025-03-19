#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc"

#define GPS_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef123456"
#define SOS_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef654321"
#define CHAT_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef987654"
#define WEATHER_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef111213"
#define HOTSPOT_CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef111214"

BLECharacteristic *gpsCharacteristic;
BLECharacteristic *sosCharacteristic;
BLECharacteristic *chatCharacteristic;
BLECharacteristic *weatherCharacteristic;
BLECharacteristic *hotspotCharacteristic;

bool deviceConnected = false;

unsigned long lastReceivedTime = 0;
const unsigned long timeoutDuration = 10000;
unsigned long lastMessageTime = 0;
const unsigned long messageInterval = 10000;

// Sample SOS alerts (This can be updated dynamically based on real conditions)
String sosAlerts = "[{\"id\":\"004-0000\",\"l\":\"8.01713-79.96301\",\"s\":1},"
                    "{\"id\":\"004-0000\",\"l\":\"8.01713-79.96301\",\"s\":1},"
                    "{\"id\":\"004-0000\",\"l\":\"8.01714-79.96302\",\"s\":1}]";


// Simulated weather percentage for testing (Change to dynamic later)
String mockWeatherResponse = "{\"id\":\"005|Uf6rVNA\", \"w\":90}";

// Sample hotspot response
String hotspotData = "[{\"hotspotId\":11,\"latitude\":-33.9189,\"longitude\":151.2353},"
                         "{\"hotspotId\":10,\"latitude\":11.667,\"longitude\":92.7358},"
                         "{\"hotspotId\":9,\"latitude\":43.0642,\"longitude\":141.3469}]";

// List of simulated chat messages
String mockChatMessage = "{\"id\":\"005|UfS1SHv\",\"m\":7}";

// int currentMessageIndex = 0; 

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
  String dataBuffer = "";

   void onWrite(BLECharacteristic *pCharacteristic) override {
    String receivedData = pCharacteristic->getValue();

    if (receivedData.length() > 0) {
      dataBuffer += receivedData;
      lastReceivedTime = millis();

      if (receivedData.endsWith("}")) {
        Serial.print("✅ GPS Data received: ");
        Serial.println(dataBuffer);

        dataBuffer = "";
      }
    }
  }
};

// Callbacks for SOS characteristic
class SOSCharacteristicCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    String value = pCharacteristic->getValue();
    if (value.length() > 0) {
      Serial.print("🚨 SOS Received: ");
      Serial.println(value);
    }
  }

  void onRead(BLECharacteristic *pCharacteristic) override {
    Serial.println("📩 Fetch SOS Alerts request received");
    pCharacteristic->setValue(sosAlerts.c_str()); 
    Serial.print("📤 SOS Alerts sent sent to mobile: ");
    Serial.println(sosAlerts);
  }
};

// Callbacks for Chat characteristic
class ChatCharacteristicCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    String value = pCharacteristic->getValue();
    
    if (value.length() > 0) {
      Serial.print("📜 Chat Message received from Mobile App: ");
      Serial.println(value);
    }
  }
};

// Callbacks for Weather characteristic
class WeatherCharacteristicCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    String value = pCharacteristic->getValue();
    
    if (value.length() > 0) {
      Serial.print("🌦️ Weather Request received: ");
      Serial.println(value);
      
      // Simulate a weather response with the stored percentage
      weatherCharacteristic->setValue(mockWeatherResponse.c_str());
      
      Serial.print("📡 Weather Response Sent: ");
      Serial.println(mockWeatherResponse);
    }
  }
};

// Callbacks for hotspot characteristic
class HotspotCharacteristicCallback : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pCharacteristic) override {
    Serial.println("📩 Fetch Hotspot Data request received");

    // 🔹 Set the BLE characteristic value
    pCharacteristic->setValue(hotspotData.c_str());
    Serial.println("📡 Hotspot Data Sent Successfully!");
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
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  chatCharacteristic->addDescriptor(new BLE2902());
  chatCharacteristic->setCallbacks(new ChatCharacteristicCallback());

  // Weather Characteristic
  weatherCharacteristic = pService->createCharacteristic(
    WEATHER_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  weatherCharacteristic->addDescriptor(new BLE2902());
  weatherCharacteristic->setCallbacks(new WeatherCharacteristicCallback());

  // Hotspot Characteristic (NEW)
  hotspotCharacteristic = pService->createCharacteristic(
    HOTSPOT_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  hotspotCharacteristic->addDescriptor(new BLE2902());
  hotspotCharacteristic->setCallbacks(new HotspotCharacteristicCallback());

  // Start service and advertising
  pService->start();

  BLEDevice::setMTU(250);

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("🌐 Waiting for a client connection...");
}

void loop() {
  if (deviceConnected) {
    // Check if data hasn't been received within the timeout duration
    if (millis() - lastReceivedTime > timeoutDuration) {
      Serial.println("⚠️ No GPS data received from Mobile App for 10 seconds...");  // Avoid spamming the log
      delay(5000);
    } 

    // Check if 10 seconds have passed and send the chat notification
    if (millis() - lastMessageTime >= messageInterval) {
      // Send chat notification every 10 seconds
      Serial.print("📤 Sending chat notification: ");
      Serial.println(mockChatMessage);

      chatCharacteristic->setValue(mockChatMessage.c_str());
      chatCharacteristic->notify();

      lastMessageTime = millis();  // Reset the timer after sending the message
    }
  } else {
    delay(1000);
  }
}