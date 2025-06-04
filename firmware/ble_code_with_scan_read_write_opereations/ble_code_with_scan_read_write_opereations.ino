#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>  // <-- required for BLE2902 descriptor


#define SERVICE_UUID        "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_RX   "6e400002-b5a3-f393-e0a9-e50e24dcca9e" // write
#define CHARACTERISTIC_TX   "6e400003-b5a3-f393-e0a9-e50e24dcca9e" // read/notify

static unsigned long lastTime = 0;

BLECharacteristic *txCharacteristic;
bool deviceConnected = false;

class ServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("BLE device connected");
    }

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("BLE device disconnected");
      pServer->startAdvertising();  // restart advertising
    }
};

class CharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      Serial.print("Received from phone: ");
      Serial.println(value.c_str());

      // Optional: Echo back
      if (deviceConnected) {
        txCharacteristic->setValue("ACK: " + value);
        txCharacteristic->notify();
      }
    }
};

void setup() {
  Serial.begin(115200);

  BLEDevice::init("ESP32-BLE");
  BLEServer *pServer = BLEDevice::createServer();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setScanResponse(true); // Optional: allows longer name in response
  pAdvertising->setMinPreferred(0x06); // Recommended
  pAdvertising->setMinPreferred(0x12); // Recommended
  BLEAdvertisementData advData;
  advData.setName("ESP32-BLE");  // <- THIS is the key!
  pAdvertising->setAdvertisementData(advData);
  pServer->setCallbacks(new ServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  // TX (ESP32 -> Phone)
  txCharacteristic = pService->createCharacteristic(
                       CHARACTERISTIC_TX,
                       BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ
                     );
  txCharacteristic->addDescriptor(new BLE2902());

  // RX (Phone -> ESP32)
  BLECharacteristic *rxCharacteristic = pService->createCharacteristic(
                                          CHARACTERISTIC_RX,
                                          BLECharacteristic::PROPERTY_WRITE
                                        );
  rxCharacteristic->setCallbacks(new CharacteristicCallbacks());

  pService->start();
  pServer->getAdvertising()->start();

  Serial.println("BLE ready and advertising");
}

void loop() {
  // Example: send periodic notification

  if (deviceConnected && millis() - lastTime > 5000) {
    lastTime = millis();
    txCharacteristic->setValue("Hello from ESP32");
    txCharacteristic->notify();
  }
}
