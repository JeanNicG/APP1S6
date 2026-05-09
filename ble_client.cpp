#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>

// UUIDs
const char* SERVICE_UUID = "9c257925-30df-414c-85ea-ac8b55388388";
const char* TX_UUID      = "6b4764ce-c028-4767-8fba-228675ccca55";
const char* RX_UUID      = "f335287f-ab4f-4ddb-8d41-a5cc10cf29ea";

BLEClient* client = nullptr;
BLERemoteCharacteristic* txChar = nullptr;
BLEAdvertisedDevice* serverDevice = nullptr;

// Handle incoming data
void onDataReceived(BLERemoteCharacteristic* chr, uint8_t* data, size_t len, bool isNotify) {
    Serial.print("📨 ");
    for (size_t i = 0; i < len; i++) Serial.print((char)data[i]);
    Serial.println();
}

// Scan result callback
class ScanCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice device) {
        if (device.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
            serverDevice = new BLEAdvertisedDevice(device);
            BLEDevice::getScan()->stop();
        }
    }
};

// Connection callback
class ClientCallbacks : public BLEClientCallbacks {
    void onConnect(BLEClient* c) { Serial.println("✅ Connected"); }
    void onDisconnect(BLEClient* c) { Serial.println("❌ Disconnected"); }
};

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting BLE Client...");

    BLEDevice::init("");
    
    BLEScan* scan = BLEDevice::getScan();
    scan->setAdvertisedDeviceCallbacks(new ScanCallbacks());
    scan->start(10);
}

void loop() {
    // Found device, connect to it
    if (serverDevice && !client) {
        Serial.println("Connecting...");
        
        client = BLEDevice::createClient();
        client->setClientCallbacks(new ClientCallbacks());
        client->connect(serverDevice);
        
        // Get TX characteristic for notifications
        BLERemoteService* service = client->getService(BLEUUID(SERVICE_UUID));
        if (service) {
            txChar = service->getCharacteristic(BLEUUID(TX_UUID));
            if (txChar && txChar->canNotify()) {
                txChar->registerForNotify(onDataReceived);
            }
        }
    }

    // Send data example (uncomment to use)
    // if (client && client->isConnected()) {
    //     BLERemoteCharacteristic* rxChar = client->getService(BLEUUID(SERVICE_UUID))->getCharacteristic(BLEUUID(RX_UUID));
    //     if (rxChar) {
    //         rxChar->writeValue((uint8_t*)"Hello", 5);
    //         Serial.println("📤 Sent: Hello");
    //     }
    //     delay(1000);
    // }

    delay(1000);
}
