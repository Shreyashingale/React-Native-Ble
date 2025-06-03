#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

void setup() {
  Serial.begin(115200);

  // Initialize BLE and set device name
  BLEDevice::init("CorSense-ESP32");

  // Create a BLE Server
  BLEServer *pServer = BLEDevice::createServer();

  // Create a BLE Service
  BLEService *pService = pServer->createService(BLEUUID((uint16_t)0x180C)); // Example UUID

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("BLE device is now advertising as 'CorSense-ESP32'");
}

void loop() {
  // nothing to do here
}
