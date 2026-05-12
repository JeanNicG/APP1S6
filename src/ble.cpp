/**
 * @file        ble.cpp
 * @author      jeannicolas gosselin and anahi mongelos toledo
 * @date        May 12, 2026
 * @brief       This is the BLE implementation file
 * @cite        This code is based on the BLE examples provided by the ESP32 Arduino library, 
 *              with modifications for our specific use case.
 */
#include "ble.h"

// https://www.uuidgenerator.net/
const char* BLEUart::SERVICE_UUID = "9c257925-30df-414c-85ea-ac8b55388388";
const char* BLEUart::RX_UUID      = "f335287f-ab4f-4ddb-8d41-a5cc10cf29ea";
const char* BLEUart::TX_UUID      = "6b4764ce-c028-4767-8fba-228675ccca55";

// Callback for Server Connection
class MyServerCallbacks : public BLEServerCallbacks {
    BLEUart* _bleUart;
public:
    MyServerCallbacks(BLEUart* ble) : _bleUart(ble) {}
    void onConnect(BLEServer* pServer) {
        _bleUart->_deviceConnected = true;
    };
    void onDisconnect(BLEServer* pServer) {
        _bleUart->_deviceConnected = false;
    }
};

// Callback for Receiving Data
class MyRxCallbacks : public BLECharacteristicCallbacks {
public:
    void onWrite(BLECharacteristic* pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0) {
            Serial.println("--- BLE Received ---");
            for (int i = 0; i < rxValue.length(); i++) {
                Serial.print(rxValue[i]);
            }
            Serial.println("\n--------------------");
        }
    }
};

BLEUart::BLEUart() : _pServer(NULL), _deviceConnected(false), _oldDeviceConnected(false) {}

void BLEUart::begin(const char* deviceName) {
    BLEDevice::init(deviceName);

    _pServer = BLEDevice::createServer();
    _pServer->setCallbacks(new MyServerCallbacks(this));

    BLEService* pService = _pServer->createService(SERVICE_UUID);

    _pTxCharacteristic = pService->createCharacteristic(
        TX_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    _pTxCharacteristic->addDescriptor(new BLE2902());

    _pRxCharacteristic = pService->createCharacteristic(
        RX_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    _pRxCharacteristic->setCallbacks(new MyRxCallbacks());

    pService->start();

    _pServer->getAdvertising()->addServiceUUID(SERVICE_UUID);
    _pServer->getAdvertising()->start();
}

void BLEUart::update() {
    // Handling disconnection/re-advertising
    if (!_deviceConnected && _oldDeviceConnected) {
        delay(500); 
        _pServer->startAdvertising();
        Serial.println("Restarted Advertising");
        _oldDeviceConnected = _deviceConnected;
    }
    // Handling connection
    if (_deviceConnected && !_oldDeviceConnected) {
        _oldDeviceConnected = _deviceConnected;
    }
}

void BLEUart::sendValue(uint8_t value) {
    if (_deviceConnected) {
        _pTxCharacteristic->setValue(&value, 1);
        _pTxCharacteristic->notify();
    }
}

void BLEUart::sendString(const char* str) {
    if (_deviceConnected) {
        std::string value(str);
        _pTxCharacteristic->setValue(value);
        _pTxCharacteristic->notify();
    }
}

bool BLEUart::isConnected() {
    return _deviceConnected;
}