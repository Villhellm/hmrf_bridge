# 433mhz ethernet bridge

## Required software:
1. MQTT Broker ([mosquitto](https://mosquitto.org/) is recommended)
2. Arduino IDE or similar

## Required Hardware:
1. Arduino Uno: https://amzn.to/2u3yN52
2. Ethernet Shield: https://amzn.to/2VT291R
3. 433mhz RF Receiver: https://amzn.to/2CkvbA5

You can substitute these components for cheaper items from china. This brings the cost down to about $12-$15.


![Schematic](https://raw.githubusercontent.com/Villhellm/README_images/master/rf_bridge_schematic.png)

Input your network information in the provided variables and upload the sketch to your arduino.

All 433mhz messages will be published to the topic **'HMRF_Bridge/state'** in a hexidecimal string.

This bridge is meant to be used with services such as [Node-Red](https://nodered.org/) or [Home-Assistant](https://www.home-assistant.io/)
