#include "TelemetryService.h"

TelemetryService::TelemetryService() : dataQueuePtr(nullptr) {
    sdMutex = xSemaphoreCreateMutex();
}

bool TelemetryService::begin(QueueHandle_t* q) {
    dataQueuePtr = q;

#if ENABLE_SD
    SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);
    if (!SD_MMC.begin("/sdcard", true)) {
        Serial.println("SD_MMC mount failed");
    } else {
        Serial.println("SD_MMC mounted");
        if (!SD_MMC.exists("/photos")) SD_MMC.mkdir("/photos");
        
        // CSV Header
        if (!SD_MMC.exists("/datalog.csv")) {
             File f = SD_MMC.open("/datalog.csv", FILE_WRITE);
             if (f) {
                 f.println("Timestamp,Mode,Vin(V),Iin(A),Pin(W),Vout(V),Iout(A),Pout(W),Efficiency(%),Latitude,Longitude,Satellites,ADC0,ADC1,ADC2,ADC3,SoC(%),adcSoC(%),Logic0,Logic1,Logic2,Logic3");
                 f.close();
             }
        }
    }
#endif

    xTaskCreatePinnedToCore(TelemetryService::task, "TelemetryTask", 4096, this, 1, NULL, 0);
    return true;
}

void TelemetryService::task(void* param) {
    TelemetryService* self = (TelemetryService*)param;
    for (;;) {
        self->loop();
    }
}

void TelemetryService::loop() {
    MeasurementData d;
    // Wait for data from SensorService
    if (dataQueuePtr && *dataQueuePtr && xQueueReceive(*dataQueuePtr, &d, pdMS_TO_TICKS(100)) == pdPASS) {
        latestTimestamp = String(d.timestamp);
        logToSerial(d); // Serial Studio
        logToSD(d);     // SD Card
    }
}

void TelemetryService::logToSerial(const MeasurementData& d) {
    const char* modeStr = (currentSystemMode == MODE_SENSOR) ? "SENSOR" : "SLEEP";
    Serial.printf("/*%s,%s,%.3f,%.6f,%.6f,%.3f,%.6f,%.6f,%.2f,%.6f,%.6f,%d,%d,%d,%d,%d,%.2f,%.2f,%.1f,%.1f,%.1f,%.1f*/\n",
        d.timestamp, modeStr,
        d.vin, d.iin, d.pin,
        d.vout, d.iout, d.pout,
        d.efficiency,
        d.lat, d.lng,
        d.satellites,
        d.adcValues[0], d.adcValues[1], d.adcValues[2], d.adcValues[3], d.battSoC,
        d.adcSoC, d.logicLevels[0], d.logicLevels[1], d.logicLevels[2], d.logicLevels[3]
    );
}

void TelemetryService::logToSD(const MeasurementData& d) {
#if ENABLE_SD
    if (xSemaphoreTake(sdMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        File f = SD_MMC.open("/datalog.csv", FILE_APPEND);
        if (f) {
            const char* modeStr = (currentSystemMode == MODE_SENSOR) ? "SENSOR" : "SLEEP";
            f.printf("%s,%s,%.3f,%.6f,%.6f,%.3f,%.6f,%.6f,%.2f,%.6f,%.6f,%d,%d,%d,%d,%d,%.2f,%.2f,%.1f,%.1f,%.1f,%.1f\n",
                d.timestamp, modeStr,
                d.vin, d.iin, d.pin,
                d.vout, d.iout, d.pout,
                d.efficiency,
                d.lat, d.lng,
                d.satellites,
                d.adcValues[0], d.adcValues[1], d.adcValues[2], d.adcValues[3], d.battSoC,
                d.adcSoC, d.logicLevels[0], d.logicLevels[1], d.logicLevels[2], d.logicLevels[3]
            );
            f.close();
        } else {
            Serial.println("SD Error: Failed to open /datalog.csv for append");
        }
        xSemaphoreGive(sdMutex);
    }
#endif
}
