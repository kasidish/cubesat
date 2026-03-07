# ESP32-S3 OOP Cubesat Project

## Architecture Overview

The project follows an Object-Oriented Design (OOD) where each hardware component or functional block is encapsulated into a "Service" class.

### File Structure (Current)
- `CubesatProject.ino`: Main entry point, service initialization, and high-level loop.
- `DataModel.h`: Defines shared data structures (`MeasurementData`), system modes, and global configurations.
- `SensorService.h / .cpp`: Handles I2C sensors (INA226), GPS (NMEA), RTC, and ADC filtering.
- `TelemetryService.h / .cpp`: Handles data logging to SD Card (CSV) and Serial Studio output.
- `WebService.h / .cpp`: Provides a web dashboard with live JSON API and camera stream.
- `MqttService.h / .cpp`: Handles MQTT telemetry publishing and remote command reception.
- `CameraService.h / .cpp`: Manages the ESP32-S3 camera module and image capture.

## FreeRTOS Task Execution & Priority Workflow

The ESP32-S3 dual-core processor is utilized to separate high-frequency sensor acquisition from network and UI handling.

### Task Map & Priorities

| Task / Context   | Priority | Core | Service            | Behavior                                   |
| ---------------- | -------- | ---- | ------------------ | ------------------------------------------ |
| **SensorTask**   | 2 (High) | 0    | `SensorService`    | Preempts other tasks on Core 0 to sample.  |
| **TelemetryTask**| 1 (Low)  | 0    | `TelemetryService` | Runs in background when SensorTask sleeps. |
| **Arduino Loop** | 1 (Low)  | 1    | `Web` & `MQTT`     | Handles network traffic and user requests. |

### Execution Logic Diagram

```text
CORE 0 (Hardware & Storage)           CORE 1 (Connectivity & UI)
===========================           ==========================

 [SensorTask P2]                      [Arduino Loop P1]
       |                                     |
       |-- (1) Read I2C/GPS/ADC              |-- (A) Handle HTTP Clients
       |-- (2) Update Mutex Record --------->|-- (B) Fetch Mutex Data
       |-- (3) Push to Data Queue --         |-- (C) MQTT Keep-alive/Pub
       |                           |         |
   [Sleep 1s]                      |         v
       |                           |    (Continues Loop)
       v                           |
 [TelemetryTask P1] <--------------|
       |
       |-- (4) Pop from Data Queue
       |-- (5) Write to SD Card
       |-- (6) Output Serial Studio
       |
       v
 (Yields to SensorTask)
```

## Connectivity & Remote Access

The ESP32-S3 operates in **Dual WiFi Mode (AP + STA)** simultaneously to ensure accessibility in both field and lab environments.

### 1. Local Access (SoftAP Mode)
- **SSID:** `Cubesat_AP`
- **Password:** `12345678`
- **IP Address:** `192.168.4.1`
- **Usage:** Connect your PC/Phone directly to this WiFi to access the Dashboard when no router is available.

### 2. Network Access (STA Mode)
- **Config:** Set `WIFI_SSID` and `WIFI_PASS` in `DataModel.h`.
- **NTP Sync:** Automatically synchronizes the internal RTC with internet time upon connection.
- **MQTT:** Connects to the HiveMQ broker to stream data to the Cloud/Node-RED.

### 3. Node-RED & MQTT Integration
- **Broker:** `broker.hivemq.com` (Port 1883)
- **Telemetry Topic:** `cubesat/telemetry` (JSON payload)
- **Command Topic:** `cubesat/command` (To switch modes remotely)
- **Commands:**
  - `sensor`: Switch to Sensor Mode (Logging + Telemetry)
  - `camera`: Switch to Camera Mode (Prioritize Stream)
  - `sleep`: Enter Low Power Mode

## How to Test & Use

### Step 1: Configuration
1. Open `DataModel.h`.
2. Set your WiFi credentials.
3. Enable/Disable hardware features (GPS, INA226, RTC) using the `ENABLE_` switches.

### Step 2: Flashing
1. Select **ESP32S3 Dev Module** in Arduino IDE.
2. Ensure **PSRAM** is enabled (OPI/QSPI) for the Camera.
3. Upload the code.

### Step 3: Monitoring
1. **Serial Studio:** Open Serial Monitor (115200) and look for the `/* ... */` telemetry strings. Use Serial Studio to visualize these real-time.
2. **Web Dashboard:** Open a browser and go to `http://192.168.4.1` (or the STA IP).
   - View live sensor values.
   - View the live Camera feed.
   - Trigger manual SD Card captures.
3. **Node-RED:** Import the provided flow (in `datasheet esp32-s3 Pinout/nodered_flow.json`) to visualize data in the cloud.

## Data Flow Diagram (Logical)

```text
        Sensors (INA226, GPS, RTC, ADC)
                ↓
        [ SensorService ] (Core 0 Task)
        /               \
       ↓                 ↓
 [ Shared Mutex ]   [ Data Queue ]
       ↓                 ↓
 [ Web / MQTT ]    [ TelemetryService ] (Core 0 Task)
 (Arduino Loop)          ↓
                  [ SD Card / Serial ]
```
