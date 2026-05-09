#include <Arduino.h>
#include "sensors.h"
#include "ble.h"

Sensors sensors;
BLEUart ble;

void setup() {
  Serial.begin(115200);
  delay(500);
  ble.begin("JN-StationMeteo");
  sensors.init();
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
    "Light:%d%% Humidity:%.1f%% Temp:%.1f°C Pressure:%.1fkPa WindSpeed:%.2fm/s WindDirection:%d° RainLevel:%.2fmm",
    lightLevel, th.humidity, th.temperature, pressure, wind.speed, wind.direction, rainLevel);
  
  if (ble.isConnected()) {
    ble.sendString(buffer);
  }
  
  // Serial output for debugging
  Serial.println(buffer);
  
  delay(10);
}