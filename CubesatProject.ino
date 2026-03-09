#include <Arduino.h>
#include "DataModel.h"
#include "SensorService.h"
#include "TelemetryService.h"
#include "WebService.h"
#include "MqttService.h"

// Global Services
SensorService sensorService;
TelemetryService telemetryService;
WebService webService;
MqttService mqttService;

// System Mode State
OperationMode currentSystemMode = MODE_SENSOR; // Default to sensor mode

// Shared Queue
QueueHandle_t dataQueue;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("ESP32-S3 OOP Cubesat");

    // Initialize Shared Queue
    dataQueue = xQueueCreate(10, sizeof(MeasurementData));
    if (dataQueue == NULL) {
        Serial.println("Error creating data queue!");
    }
    
    // Inject Queue into SensorService
    sensorService.setDataQueue(&dataQueue);

    // Initialize Services
    
    // 1. Sensors (I2C, GPS)
    sensorService.begin();
    
    // 2. MQTT Service
    mqttService.begin(&sensorService);

    // 3. Telemetry (SD Card) - Depends on Queue
    telemetryService.begin(&dataQueue);

    // 4. Web Service - Depends on Sensors and MQTT
    webService.begin(&sensorService, &mqttService);
}

void loop() {
    // Web Server is not purely async in this simple implementation, 
    // it needs a handleClient loop.
    webService.update();
    mqttService.update();
    vTaskDelay(pdMS_TO_TICKS(10));
}
