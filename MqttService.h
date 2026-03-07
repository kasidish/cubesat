#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "DataModel.h"
#include "SensorService.h"

class MqttService {
public:
    MqttService();
    void begin(SensorService* s);
    void update();
    bool isConnected();


private:
    void reconnect();
    void publishTelemetry();
    void callback(char* topic, byte* payload, unsigned int length);

#if ENABLE_MQTT_TLS
    WiFiClientSecure espClient;
#else
    WiFiClient espClient;
#endif

    PubSubClient client;
    SensorService* sensors;
    unsigned long lastPublishTime;
    const unsigned long PUBLISH_INTERVAL = 5000; // Publish every 5 seconds
};

#endif
