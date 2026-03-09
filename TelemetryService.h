#ifndef TELEMETRY_SERVICE_H
#define TELEMETRY_SERVICE_H

#include "DataModel.h"
#include <FS.h>
#include <SD_MMC.h>

// SD_MMC Pins (1-bit mode)
#define SD_MMC_CMD 38
#define SD_MMC_CLK 39
#define SD_MMC_D0  40

class TelemetryService {
public:
    TelemetryService();
    bool begin(QueueHandle_t* q);

private:
    static void task(void* param);
    void loop();
    void logToSerial(const MeasurementData& d);
    void logToSD(const MeasurementData& d);

    QueueHandle_t* dataQueuePtr;
    SemaphoreHandle_t sdMutex;
    String latestTimestamp; // Last known timestamp
};

#endif
