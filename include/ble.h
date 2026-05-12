/**
 * @file        ble.h
 * @author      jeannicolas gosselin and anahi mongelos toledo
 * @date        May 12, 2026
 * @brief       This is the BLE header file
 * @cite        This code is based on the BLE examples provided by the ESP32 Arduino library, 
 *              with modifications for our specific use case.
 */
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