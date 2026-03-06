Architecture Overview :

/src
 ├── main.cpp
 ├── config.h
 ├── core/
 │    ├── AppController.h
 │    ├── AppController.cpp
 │
 ├── services/
 │    ├── SensorService.h / .cpp
 │    ├── CameraService.h / .cpp
 │    ├── SDService.h / .cpp
 │    ├── WebService.h / .cpp
 │    ├── TelemetryService.h / .cpp
 │
 ├── models/
 │    ├── SharedData.h

Runtime Flow (FreeRTOS) :

1. main.cpp
   - Initialize Serial (115200)
   - Initialize WiFi (STA Mode for MQTT/Web, AP Mode for configuration)
   - Initialize MQTT (connect to HiveMQ)
   - Create SharedData (with Mutex for thread safety)
   - Create Tasks:
     - SensorService (Priority 2, Core 1)
     - TelemetryService (Priority 1, Core 1)
     - WebService (Priority 1, Core 0)
   - Start FreeRTOS Scheduler (handled automatically by ESP32)

2. SensorService Task
   - Loop every 2 seconds
   - Read INA226, GPS , RTC
   - Package data into SensorData struct
   - Update SharedData using Mutex lock
   - Sleep (vTaskDelay)

3. TelemetryService Task
   - Loop every 1 second
   - Read latest data from SharedData
   - Format data as JSON
   - Print JSON to Serial (for Serial Studio)
   - Sleep (vTaskDelay)

4. WebService Task
   - Runs on Core 0
   - Handle :
        - GET / ; Simple HTML page (live values)
        - GET /json ; JSON API response
   - Reads from SharedData (thread-safe)
   - No blocking operations inside handler

| Service          | Priority | Core | Purpose                    |
| ---------------- | -------- | ---- | -------------------------- |
| SensorService    | 2        | 1    | Real-time data acquisition |
| TelemetryService | 1        | 1    | Serial JSON output         |
| WebService       | 1        | 0    | HTTP request handling      |

Data Flow Diagram (Logical) :

        Sensors (INA226, GPS)
                ↓
        SensorService
                ↓
        SharedData (Mutex Protected)
                ↓
 ┌──────────────────────────────┬
 ↓                              ↓
TelemtryService               WebService
 (Serial JSON)                 (HTTP API)

---

## Node-RED Dashboard Configuration

This project includes a pre-configured Node-RED flow to display telemetry data, calculate battery percentage, and control the operational modes of the Cubesat (Sensor Mode, Camera Mode, Sleep Mode).

### How to Import the Flow:
1. Open the file `nodered_flow.json` in this repository (or copy its contents directly).
2. Open your Node-RED editor (usually `http://localhost:1880`).
3. Click the menu button ☰ (top right corner).
4. Select **Import**.
5. Paste the JSON contents into the grey box.
6. Click the red **Import** button.
7. Place the new flow on your workspace and click the red **Deploy** button (top right corner).

### Prerequisites:
- Ensure you have the `node-red-dashboard` node installed via the Palette Manager (Menu > Manage palette > Install). Without this, the UI nodes will appear as unrecognized (`unknown: ui_...`).
- The flow uses the public HiveMQ broker (`broker.hivemq.com`) on port `1883`. Ensure your ESP32 is configured to match this broker in `DataModel.h`.
