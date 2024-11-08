# ATTENTION: This README.md file is VERY outdated. The interface no longer looks like what's described below and the project has been upgraded to use ESP32 instead.


# IOTServer
## Climate Control Web Interface


### Introduction

ESP32 is a series of low-cost, low-power system-on-chip microcontrollers with integrated Wi-Fi and dual-mode Bluetooth.
(Source: [Wikipedia entry](https://en.wikipedia.org/wiki/ESP32)). It provides a
fairly low cost entry into the field of IoT.

This project creates a fairly low cost solution to control an air conditioner to prevent the compressor from running non-stop and consumig excessive electricity.

### Requirements

In order to compile and run this test, you will need:
* Mac OS or Linux (I have only tested this on an Ubuntu 24.04 machine)
* [ESP32 microcontroller board](https://www.espressif.com/en/products/socs/esp32)
* [Arduino IDE](https://www.arduino.cc/en/Main/Software) or [Arduino CLI](https://github.com/arduino/arduino-cli)

If you want to make changes to the HTML or JS served by the IOTServer, you will
need [NodeJS](https://nodejs.org/en/) installed in order to run the *printify*
script which will produce code you can copy and paste into  *IOTServer.ino*.


### Making Changes to HTML and JS

If you wish to make changes to the HTML and/or JS served by the web server, you
will need to edit the files *web/page.html* and *web/page.js* and then run
`./printify` to convert it into minified code embedded in a `client.println()`
statement.

In order to change the contents of the page you will need to edit
*IOTServer.ino* and replace the line that comes directly after the following
comment:
```cpp
    // Serve the HTML page
```


### Uploading To NodeMCU

When you are ready to run the code on the NodeMCU, you can compile and
upload it using Arduino IDE. If you have `arduino-cli` installed you can simply
run `make upload` on the command-line and the code will be uploaded to the
NodeMCU.

If your NodeMCU serial device is not found you may need to edit the *Makefile*
and change `/dev/tty.usbserial-1410` to the exact device your system detects.
