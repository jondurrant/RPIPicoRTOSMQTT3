# RPIPicoRTOSMQTT
RPI Pico WIFI via ESP-01S, LWESP, FreeRTOS, and LWESPMQTT example

Demo code for RPI Pico using ESP-01S for wifi connection over uart.  Pico responds to a ping message and responds with a pong message.

Nothing clever in the code just wrapping the libraries into some C++ classes and demonstrating working example.

# Hardware
+ Raspberry Pico
+ ESP-01S (1Mb) running ESP-AT 2.1

## Connections:
PICO Uart0 on GPIO12 and 13

|pico|function|ESP-01S| function|
|----|--------|-------|---------|
|G12 | TX     | RX    | RX      |
|G13 | RX     | TX    | TX      |
|3.3 | 3.3v   | 3.3   | 3.3v    |
|GND |GND     | GND   | GND     |
|3.3 | 3.3v   | EN    | via 10k res|


## ESP-01S
I got very cheap pack of ESP-01S but they came with 1Mb flash. Latest version of ESP-AT, version 2.2, requires 2MB. 

Fortunatly Jandrassy has built a working 2.1 firmware for 1MB. This is on github at: https://github.com/jandrassy/WiFiEspAT

To reliably upload the firmware to ESP-01S requires the RST IO0 to get pulled to zero and the released. Some further inform on how to do this at: https://blog.tiga.tech/flashing-esphome-firmware-to-esp-01s/

I used the RPI 4 to upload the firmware and the esptool.py. I had to drop frequency to 40m and change upload mode to dout in the download.config file. Then use the command
esptool.py --port /dev/ttyS0 write_flash @download.config

# Dependancies
## C++
+ [lwesp](https://github.com/MaJerle/lwesp): library for talking to ESP-AT over serial
+ [FreeRTOS-Kernel](https://github.com/search?q=freertos#:~:text=FreeRTOS/FreeRTOS-Kernel): dependency for lwesp

I failed to get FreeRTOS MQTT libraries to be stable so have dropped back to using LWESP library. This doesn't have MQTT over SSL which I would have liked.

Configuration for the libraries are within the src/port folders. They are complex libraries and need proting for the Pico.

## Python3
I've used the Paho python client to send a ping. See the py folder.


# Cloning and build
git clone --recurse-submodules https://github.com/jondurrant/RPIPicoRTOSMQTT

main.cpp include "Credentials.env" for the Wifi AP name and passwd, MQTT Host, Port, Username and password. I've not included this file in the repo. Please create one looking something like this:

``` C++
#define SID "SID"
#define PASSWD "PWD"

#define MQTTHOST "IP"
#define MQTTPORT 1883
#define MQTTUSER "jon"
#define MQTTPASSWD "PWD"
```


## To build:
``` bash
cd RPIPicoRTOSMQTT
mkdir build
cd build
cmake ..
make
make install (only work if you used my config setup in the Pico Intro course as does SWD upload of code direct from make).
```


# MQTT Server
I'm using Mosquitto on a RPI. Followed the docker install guide at: https://docs.cedalo.com/streamsheets/2.4/installation/

## Clients
Two users are setup on Mosquitto: 
+ jon - test identity for the pico
+ mbp - test identity for my python ping code

## Topic Structure
I've tried to condence literals a bit, nervous on memory usage. 

+TNG/<ID>/ - THING / ID, same as user name in my example
+TNG/<ID>/LC/ON - LC is lifecycle and annouces connection
+TNG/<ID>/LC/OFF -  For disconnection
+TNG/<ID>/TPC - TOPIC, for messaging to a specific thing

Example assuming you keep the pico as "jon"
+ TNG/jon/TPC/PING - ping re	quest sent to jon
+ TNG/jon/TPC/PONG - pong as response to the ping




# To Do List
+ Make code into a reusable library
+ Add support for a twin data structure like AWS Shadow
+ SSL Support
+ Mutual Authentication
