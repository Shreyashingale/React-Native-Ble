#include <Arduino.h>
#include <NimBLEDevice.h>

static NimBLEServer* pServer;
static NimBLECharacteristic* pWriteCharacteristic;
static NimBLECharacteristic* pNotifyCharacteristic;

class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        Serial.printf("Client connected: %s\n", connInfo.getAddress().toString().c_str());
        pServer->updateConnParams(connInfo.getConnHandle(), 24, 48, 0, 180);
    }

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        Serial.println("Client disconnected - Restart advertising");
        NimBLEDevice::startAdvertising();
    }
} serverCallbacks;

class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        Serial.printf("Received from app: %s\n", pCharacteristic->getValue().c_str());
    }

    void onSubscribe(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo, uint16_t subValue) override {
        Serial.printf("Client subscribed to notifications on: %s\n", pCharacteristic->getUUID().toString().c_str());
    }
} chrCallbacks;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE UART server");

    NimBLEDevice::init("ESP32-NimBLE-UART");
    NimBLEDevice::setSecurityAuth(false, false, true);

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(&serverCallbacks);

    NimBLEService* pService = pServer->createService("6e400001-b5a3-f393-e0a9-e50e24dcca9e");

    // Write Characteristic from App → ESP32
    pWriteCharacteristic = pService->createCharacteristic(
        "6e400002-b5a3-f393-e0a9-e50e24dcca9e",
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
    );
    pWriteCharacteristic->setCallbacks(&chrCallbacks);

    // Read/Notify Characteristic from ESP32 → App
    pNotifyCharacteristic = pService->createCharacteristic(
        "6e400003-b5a3-f393-e0a9-e50e24dcca9e",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    pNotifyCharacteristic->setCallbacks(&chrCallbacks);

    pService->start();

    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setName("ESP32-BLE-UART");
    pAdvertising->addServiceUUID("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
    pAdvertising->start();

    Serial.println("BLE UART service advertising...");
}

void loop() {
    static unsigned long lastSend = 0;
    if (pServer->getConnectedCount() > 0 && millis() - lastSend > 5000) {
        lastSend = millis();
        std::string message = "ESP32 says hi!";
        pNotifyCharacteristic->setValue(message);
        pNotifyCharacteristic->notify();
        Serial.printf("Notified: %s\n", message.c_str());
    }
}
