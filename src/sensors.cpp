#include "sensors.h"

#define LIGHT_SENSOR_PIN 34
#define WIND_SPEED_PIN 27
#define WIND_DIRECTION_PIN 35
#define RAIN_SENSOR_PIN 23 
#define LIGHT_MAX_RAW 2300
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
    dps310PressureSensor.begin(Wire);
    // Interrupt to count wind speed and rain level 
    attachInterrupt(digitalPinToInterrupt(WIND_SPEED_PIN), wind_speed_isr, FALLING);
    attachInterrupt(digitalPinToInterrupt(RAIN_SENSOR_PIN), rain_level_isr, FALLING);
}

int Sensors::readLightLevel() {
    int raw_light = analogRead(LIGHT_SENSOR_PIN);
    int light = map(raw_light, 0, LIGHT_MAX_RAW, 0, 100); 
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
    
    // 1 pulse = 0.67m/s
    float wind_speed = pulse_count * 0.67;
    
    // Read wind direction
    int raw_direction = analogRead(WIND_DIRECTION_PIN);
    int direction_deg = map(raw_direction, 0, 4095, 0, 360);
    return {wind_speed, direction_deg};
}

float Sensors::readRainLevel() {
    return rain_pulse_count * 0.2794;
}