# IOTServer
## Proof Of Concept

![Screenshot 1](/images/screenshot1.png)
![Screenshot 2](/images/screenshot2.png)


### Introduction

The ESP8266 is a low-cost Wi-Fi microchip, with a full TCP/IP stack and
microcontroller capability, produced by Espressif Systems in Shanghai, China
(Source: [Wikipedia](https://en.wikipedia.org/wiki/ESP8266)). It provides a
fairly low cost entry into the field of IoT.

The purpose of this POC (Proof Of Concept) is to demonstrate a fast Web Server
running on the ESP8266 that is able to control a switch. Since we're not
interested in complicated wiring diagrams for this experiment, we will instead
use the built-in LED of the NodeMCU and imagine that to be a light (or some
other device you would like to control using IoT).


### Requirements

In order to compile and run this test, you will need:
* [NodeMCU v3 Wireless WiFi Module](http://www.nodemcu.com/index_en.html)
* [Arduino IDE](https://www.arduino.cc/en/Main/Software) or [Arduino CLI](https://github.com/arduino/arduino-cli)

If you want to make changes to the HTML or JS served by the IOTServer, you will
need [NodeJS](https://nodejs.org/en/) installed in order to run the *printify*
script which will produce code you can copy and paste into  *IOTServer.ino*.


### Making Changes to HTML and JS

If you wish to make changes to the HTML and/or JS served by the web server, you
will need to edit the files *web/page.html* and *web/page.js* and then run
`./printify` to convert it into minified code embedded in a `client.println()`
statement.

In order to change the contents of the page you will need to replace the line
that comes directly after
```cpp
    // Serve the HTML page
```
