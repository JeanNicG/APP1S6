#include <Arduino.h>
#include "sensors.h"
#include "ble.h"

#define RX_PIN 13
#define TX_PIN 14

Sensors sensors;
BLEUart ble;

void setup() {
  Serial.begin(115200);
  delay(500);
  ble.begin("JN-StationMeteo");
  sensors.init();
  Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
}

void loop() {
  ble.update();
  
  int lightLevel = sensors.readLightLevel();
  TempHumidity th = sensors.readTempHumid();
  float pressure = sensors.readPressure();
  WindSpeedDirection wind = sensors.readWindSpeedDirection();
  float rainLevel = sensors.readRainLevel();
  // Build data string to send
  char buffer[150];
  snprintf(buffer, sizeof(buffer), 
    "Light:%dLx Humidity:%.1f%% Temp:%.1f°C Pressure:%.1fkPa WindSpeed:%.2fKm/h WindDirection:%.1f° RainLevel:%.2fmm",
    lightLevel, th.humidity, th.temperature, pressure, wind.speed, wind.direction, rainLevel);
  
  if (ble.isConnected()) {
    ble.sendString(buffer);
  }
  // Debug
  Serial.println(buffer);

  // UART
  while (Serial1.available()) {
    char incoming = Serial1.read();
    if (incoming == 'R') {
      Serial.println("Base send R");
      Serial1.printf("UART_TX: %s\n", buffer);
    }
  }
  delay(10);
}