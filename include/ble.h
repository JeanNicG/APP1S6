#pragma once
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>

class BLEUart {
public:
    BLEUart();
    void begin(const char* deviceName);
    void update();
    void sendValue(uint8_t value);
    void sendString(const char* str);
    bool isConnected();

private:
    BLEServer* _pServer;
    BLECharacteristic* _pTxCharacteristic;
    BLECharacteristic* _pRxCharacteristic;
    bool _deviceConnected;
    bool _oldDeviceConnected;

    static const char* SERVICE_UUID;
    static const char* RX_UUID;
    static const char* TX_UUID;

    friend class MyServerCallbacks;
    friend class MyRxCallbacks;
};