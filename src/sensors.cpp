/**
 * @file        sensor.cpp
 * @author      jeannicolas gosselin and anahi mongelos toledo
 * @date        May 12, 2026
 * @brief       This is the sensor implementation file
 */
#include "sensors.h"

#define LIGHT_SENSOR_PIN 34
#define WIND_SPEED_PIN 27
#define WIND_DIRECTION_PIN 35
#define RAIN_SENSOR_PIN 23 
#define LIGHT_MAX_RAW 2350
#define LIGHT_MAX_LUX 203
#define DHT_PIN 16

volatile int wind_pulse_count = 0;
volatile int rain_pulse_count = 0;

void IRAM_ATTR wind_speed_isr() {
    wind_pulse_count++;
}

void IRAM_ATTR rain_level_isr() {
    rain_pulse_count++;
}

void Sensors::init() {
    analogSetPinAttenuation(LIGHT_SENSOR_PIN, ADC_11db);
    dps310PressureSensor.begin(Wire);
    attachInterrupt(digitalPinToInterrupt(WIND_SPEED_PIN), wind_speed_isr, FALLING);
    attachInterrupt(digitalPinToInterrupt(RAIN_SENSOR_PIN), rain_level_isr, FALLING);
}

int Sensors::readLightLevel() {
    int raw_light = analogRead(LIGHT_SENSOR_PIN);
    int light = map(raw_light, 0, LIGHT_MAX_RAW, 0, LIGHT_MAX_LUX);
    return light;
}

TempHumidity Sensors::readTempHumid() {
    int i, j;
    int duree[42];
    unsigned long pulse;
    byte data[5];
    float humidite;
    float temperature;
    int broche = DHT_PIN;

    delay(2000);
    
    pinMode(broche, OUTPUT_OPEN_DRAIN);
    digitalWrite(broche, HIGH);
    delay(250);
    digitalWrite(broche, LOW);
    delay(20);
    digitalWrite(broche, HIGH);
    delayMicroseconds(40);
    pinMode(broche, INPUT_PULLUP);
    
    while (digitalRead(broche) == HIGH);
    i = 0;

    do {
            pulse = pulseIn(broche, HIGH);
            duree[i] = pulse;
            i++;
    } while (pulse != 0);
    
    if (i != 42) 
        Serial.printf(" Erreur timing \n"); 

    for (i=0; i<5; i++) {
        data[i] = 0;
        for (j = ((8*i)+1); j < ((8*i)+9); j++) {
        data[i] = data[i] * 2;
        if (duree[j] > 50) {
            data[i] = data[i] + 1;
        }
        }
    }

    if ( (data[0] + data[1] + data[2] + data[3]) != data[4] ) 
        Serial.println(" Erreur checksum");

    humidite = data[0] + (data[1] / 256.0);
    temperature = data [2] + (data[3] / 256.0);
    return {humidite, temperature};
}

float Sensors::readPressure() {
    float pressure;
    uint8_t oversampling = 7;
    int16_t ret;
    
    ret = dps310PressureSensor.measurePressureOnce(pressure, oversampling);
    
    if (ret != 0) {
        Serial.print("Pressure measurement failed: ");
        Serial.println(ret);
        return 0.0;
    }
    
    return pressure/1000.0;
}

WindSpeedDirection Sensors::readWindSpeedDirection() {
    // Read and reset wind pulse count 
    int pulse_count = wind_pulse_count;
    wind_pulse_count = 0;
    
    // 1 pulse = 2.4Km/h
    float wind_speed = pulse_count * 2.4;
    
    // Read wind direction
    int raw_direction = analogRead(WIND_DIRECTION_PIN);
    
    const int adc_values[16]  = {3142, 1623, 1845, 335,  372,  263,   738,   506,   1148,  978,   2520,  2396,   3780,  3309,   3548,  2810};
    const float angles[16]    = {0.0,  22.5, 45.0, 67.5, 90.0, 112.5, 135.0, 157.5, 180.0, 202.5, 225.0, 247.5, 270.0, 292.5, 315.0, 337.5};
    float direction_deg = 0.0;
    int min_diff = 4096;
    
    for (int i = 0; i < 16; i++) {
        int diff = abs(raw_direction - adc_values[i]);
        if (diff < min_diff) {
            min_diff = diff;
            direction_deg = angles[i];
        }
    }
    
    return {wind_speed, direction_deg};
}

float Sensors::readRainLevel() {
    return rain_pulse_count * 0.2794;
}