cubesat.cpp is FreeRTOS by use ESP-IDF C++ just only for INA226 Sensor test

esp32cubesat.c is Arduino IDE (FreeRTOS) by use ESP32-S3 for All system design (OV2640, INA226, GPS, SD Card, RTC, WiFi, Web Server) ;
by using c (embeded c) coding

if u want to test ESP32-S3 for All system design (OV2640, INA226, GPS, SD Card, RTC, WiFi, Web Server) ; by using c (embeded c) coding
use esp32cubesat.c

if u want to test INA226 Sensor test by using ESP-IDF C++ coding
use cubesat.cpp

then if it have some error or bug, fix it and test again

use socker header pin 30 for all cubesat system ;

A (A1 → A15) :
Pin	Net/Signal what it use for?
A1	GND_STACK	ground
A2	VBAT_STACK	bat 7.4V send to Powerboard
A3	GND_STACK	ground (return current of VBAT)
A4	VBAT_STACK	parallel add current/reduce voltage drop
A5	GND_STACK	ground
A6	3V3_STACK	3.3V from Powerboard send to all board
A7	GND_STACK	ground
A8	3V3_STACK	parallel add current
A9	GND_STACK	ground
A10	I2C_SDA (GPIO1)	go to INA226/RTC/sensor
A11	I2C_SCL (GPIO2)	go to INA226/RTC/sensor
A12	GPS_RX (GPIO21)	ESP receive data (connect to GPS TX)
A13	GPS_TX (GPIO47)	ESP send (connect to GPS RX)
A14	PWR_EN / BUCK_EN (optional) command to open/close buck or open system (if you want)
A15	GND_STACK	ground

B (B1 → B15) :
Pin	Net/Signal	what it use for?
B1	GND_STACK	ground
B2	VBAT_STACK	parallel add current/reduce voltage drop
B3	GND_STACK	ground
B4	5V_USB_STACK (optional/NC) send 5V from charge (if you don't use it, leave it blank)
B5	GND_STACK	ground
B6	3V3_STACK	add pin 3.3V again
B7	GND_STACK	ground
B8	SPI_SCK (spare)	for sensor board
B9	SPI_MOSI (spare)	for sensor board
B10	SPI_MISO (spare)	for sensor board
B11	SPI_CS1 (spare)	for sensor board
B12	SPI_CS2 (spare)	for sensor board
B13	INT1 (spare) interrupt from sensor/RTC
B14	INT2 (spare) interrupt
B15	GPIO spare / SYS_RST (optional)	reset or other signal