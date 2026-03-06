#ifndef SENSOR_SERVICE_H
#define SENSOR_SERVICE_H

#include "DataModel.h"
#include <Wire.h>
#include <TinyGPS++.h>
#include <RTClib.h>
#include "INA226.h"

// I2C Pins
#define I2C_SDA 41
#define I2C_SCL 42

// GPS Pins
#define GPS_RX_PIN 21 // GPS TX -> ESP RX
#define GPS_TX_PIN 47 // GPS RX -> ESP TX

// ADC Pins
const int ADC_PINS[4] = {3, 14, 1, 2};

class SensorService {
public:
    SensorService();
    void begin();
    MeasurementData getLatestData(); 
    void syncTime(struct tm* timeinfo);

private:
    static void task(void* param);
    void loop();
    void makeTimestamp(char* out, size_t outSize);

    INA226 ina_in; // 0x41
    INA226 ina_out; // 0x44
    RTC_DS3231 rtc;
    TinyGPSPlus gps;
    HardwareSerial gpsSerial;

    bool inaInOK;
    bool inaOutOK;
    bool rtcOK;

    MeasurementData latest;
    SemaphoreHandle_t mutex;
    QueueHandle_t* dataQueuePtr; // Pointer to the main queue
    
    // GPS NMEA Parsing State
    String nmeaSentence;
    int totalSatsInView;
    String snrDetails;

    // Runtime re-check / filter state
    float adcFiltered[4];
    unsigned long bootTimeMs;
    
public:
    void setDataQueue(QueueHandle_t* q) { dataQueuePtr = q; }
};

#endif
