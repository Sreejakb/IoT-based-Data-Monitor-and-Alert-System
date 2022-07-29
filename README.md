# IoT-based-Data-Monitor-and-Alert-System
Monitor the Temperature, Humidity, Air Quality, LPG, Carbon Monoxide, Smoke and detect Fire in the surrounding environment. It can send alert to Telegram based on the limit set in the telegram app using telegram bot. Telegram app can also be used to monitor the values and download the CSV File from the Bolt IoT Cloud using the date and time entered by the user. The data can be monitored and visualized on the Bolt IoT Cloud Platform. Web Server can also be used to monitor and change the threshold values of the parameters.

<b><h3>Things used in this project</b></h3>

<h4><b>Hardware components</h4></b>

Bolt WiFi Module

ESP8266 development board

DHT-22 Sensor

MQ-135 Sensor

MQ-2 Sensor

Flame/Fire Sensor

20×4 LCD with I2C module

7 Red LEDS

150 Ohm Resistors – 7

Active Buzzer Module

CD4051 Single 8-channel Multiplexer/Demultiplexer IC

74HC595 8-bit Serial to Parallel Shift Register IC

Breadboard

Jumper Wires

<h4><b>Software, Apps and online services</h4></b>

Arduino IDE, Telegram Messaging App, Web Browser, Bolt IoT Cloud


<h4><b>Hardware setup</h4></b>

Step 1) Connect the DHT-22 Temperature and Humidity Sensor with the NodeMCU ESP8266 and Bolt IoT Module as per the following:

Vcc (DHT-22) is connected to 3.3 V (Bolt IoT Module)

Gnd (DHT-22) is connected to Gnd (Bolt IoT Module)

Data (DHT-22) is connected to D5 (NodeMCU ESP8266)

Step 2) Connect the MQ-135 Air Quality Sensor with the NodeMCU ESP8266 and Bolt IoT Module as per the following:

Vcc (MQ-135) is connected to 3.3 V (Bolt IoT Module)

Gnd (MQ-135) is connected to Gnd (Bolt IoT Module)

A0 (MQ-135) is connected to Channel 1 (CD4051 IC)

Step 3) Connect the MQ-2 LPG, Carbon Monoxide and Smoke Sensor with the NodeMCU ESP8266 and Bolt IoT Module as per the following:

Vcc (MQ-2) is connected to 3.3 V (Bolt IoT Module)

Gnd (MQ-2) is connected to Gnd (Bolt IoT Module)

A0 (MQ-2) is connected to Channel 0 (CD4051 IC)

Step 4) Connect the Fire Sensor with the NodeMCU ESP8266 and Bolt IoT Module as per the following:

Vcc (Rain sensor) is connected to 3.3 V (Bolt IoT Module)

Gnd (Rain sensor) is connected to Gnd (Bolt IoT Module)

D0 (Rain sensor) is connected to D0 (NodeMCU ESP8266)

Step 5) Connect each 7 LEDs to 150 Ohm Resistor which are then connected QA, QB, QC, QD, QE, QF, QG of SN74HC595 IC.

Step 6) Connect the 20×4 LCD with I2C module with the NodeMCU ESP8266 and Bolt IoT Module as per the following:

Vcc (I2C Module) is connected to 5 V (Bolt IoT Module)

Gnd (I2C Module) is connected to Gnd (Bolt IoT Module)

SCL (I2C Module) is connected to D1 (NodeMCU ESP8266)

SDA (I2C Module) is connected to D2 (NodeMCU ESP8266)

Step 7) Connect the SN74HC595 IC with the NodeMCU ESP8266 and Bolt IoT Module as per the following:

Vcc (SN74HC595 IC) is connected to 3.3 V (Bolt IoT Module)

SRCLR’ (SN74HC595 IC) is connected to 3.3 V (Bolt IoT Module)

Gnd (SN74HC595 IC) is connected to Gnd (Bolt IoT Module)

OE’ (SN74HC595 IC) is connected to Gnd (Bolt IoT Module)

SER (SN74HC595 IC) is connected to D6 (NodeMCU ESP8266)

RCLK (SN74HC595 IC) is connected to D7 (NodeMCU ESP8266)

SRCLK (SN74HC595 IC) is connected to D8 (NodeMCU ESP8266)

Step 8) Connect the Buzzer with the NodeMCU ESP8266 and Bolt IoT Module as per the following:

Vcc (Buzzer) is connected to 3.3 V (Bolt IoT Module)

Gnd (Buzzer) is connected to Gnd (Bolt IoT Module)

I/O (Buzzer) is connected to D3 (NodeMCU ESP8266)

Step 9) Connect the CD4051 IC with the NodeMCU ESP8266 and Bolt IoT Module as per the following:

Vdd (CD4051 IC) is connected to 3.3 V (Bolt IoT Module)

Gnd, Input B, Input C, Inhibit, Vss, Vee (CD4051 IC) is connected to Gnd (Bolt IoT Module)

Common in/out (CD4051 IC) is connected to A0 (NodeMCU ESP8266)

Input A (CD4051 IC) is connected to D4 (NodeMCU ESP8266)

Step 10) Bolt IoT Module with NodeMCU ESP8266 as per the following:

5V (Bolt IoT Module) is connected to 5 V (LCD I2C Module)

3.3V(Bolt IoT Module) is connected to 3.3V (NodeMCU ESP8266)

Gnd (Bolt IoT Module) is connected to Gnd (NodeMCU ESP8266)

RX (Bolt IoT Module) is connected to TX (NodeMCU ESP8266)

TX (Bolt IoT Module) is connected to RX (NodeMCU ESP8266)

<b>Video link of explanation and demonstration:</b> 

Before uploading the code, add the following libraries:

boltiot-arduino-helper – https://github.com/Inventrom/boltiot-arduino-helper

DHT-sensor-library – https://github.com/adafruit/DHT-sensor-library.git

LiquidCrystal library – https://github.com/arduino-libraries/LiquidCrystal

MQ135 – https://github.com/Phoenix1747/MQ135.git

MQ-2 sensor library – https://github.com/labay11/MQ-2-sensor-library.git

Fastbot – https://github.com/GyverLibs/FastBot.git

ESPAsyncTCP – https://github.com/me-no-dev/ESPAsyncTCP.git

ESPAsyncWebServer – https://github.com/me-no-dev/ESPAsyncWebServer.git
