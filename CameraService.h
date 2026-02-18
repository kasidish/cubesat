#ifndef CAMERA_SERVICE_H
#define CAMERA_SERVICE_H

#include "DataModel.h"
#include "esp_camera.h"

// Camera Pins
#define CAM_PWDN  -1
#define CAM_RESET -1
#define CAM_XCLK  15
#define CAM_SIOD  4
#define CAM_SIOC  5
#define CAM_Y9    16
#define CAM_Y8    17
#define CAM_Y7    18
#define CAM_Y6    12
#define CAM_Y5    10
#define CAM_Y4    8
#define CAM_Y3    9
#define CAM_Y2    11
#define CAM_VSYNC 6
#define CAM_HREF  7
#define CAM_PCLK  13

class CameraService {
public:
    CameraService();
    bool begin();
    
    // Request a capture. If SD is available, it will be saved by TelemetryService.
    void triggerCapture(); 
    
    // Get latest framebuffer (thread safe access). 
    // IMPORTANT: caller MUST call returnFrame(fb) or esp_camera_fb_return(fb)
    camera_fb_t* getFrame();
    void returnFrame(camera_fb_t* fb);

    bool isCaptureRequested();
    void clearCaptureRequest();

private:
    static void task(void* param);
    
    SemaphoreHandle_t mutex;
    volatile bool captureRequested;
};

#endif
