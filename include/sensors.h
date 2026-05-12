/**
 * @file        sensors.h
 * @author      jeannicolas gosselin and anahi mongelos toledo
 * @date        May 12, 2026
 * @brief       This is the sensor header file
 */
#pragma once
#include <Arduino.h>
#include <math.h>
#include <Dps310.h>

struct TempHumidity {
    float humidity;
    float temperature;
};

struct WindSpeedDirection {
    float speed;
    float direction;
};

class Sensors {
public:
    void init();
    int readLightLevel();
    TempHumidity readTempHumid();
    float readPressure();
    WindSpeedDirection readWindSpeedDirection();
    float readRainLevel();
private:
    Dps310 dps310PressureSensor;
};
