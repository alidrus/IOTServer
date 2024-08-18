// vim: syntax=arduino autoindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include "MyLED.h"

// Comment this line out if you don't want serial console messages
#define DEBUG_MODE

WiFiServer server(80);

MyLED led;

static const char* okResponse = "HTTP/1.1 200 OK";
static const char* contentHeaderJson = "Content-Type: application/json";
static const char* contentHeaderHtml = "Content-Type: text/html";
static const char* ledIsOnResponse = "{\"ledIsOn\": true}";
static const char* ledIsOffResponse = "{\"ledIsOn\": false}";

static const char* hostname = "iotserver";
static const char* ssid = "Doudou-IoT";
static const char* password = "0123206635";

void wifiSetup() {
    // Disconnect from WiFi
    WiFi.disconnect();

    delay(1000);

    // Connect in station mode
    WiFi.mode(WIFI_STA);

    // Set WiFi mode to station
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);  // This is a MUST!

    // Set hostname
    if (!WiFi.setHostname(hostname)) {
        Serial.println("Failed to set Wifi hostname");
    } else {
        Serial.println("Set Wifi hostname");
    }

    // WiFi credentials
    WiFi.begin(ssid, password);

#ifdef DEBUG_MODE
    // DEBUG messages on serial console
    Serial.print("Connecting Wifi");
#endif

    int counter = 0;
    while (WiFi.status() != WL_CONNECTED) {
        if (counter > 10) {
            return;
        }

#ifdef DEBUG_MODE
        Serial.print(".");
#endif

        delay(500);

        counter++;
    }

#ifdef DEBUG_MODE
    // DEBUG messages on serial console
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("MAC address: ");
    Serial.println(WiFi.macAddress());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("ESP32 Hostname: ");
    Serial.println(WiFi.getHostname());
    Serial.println("The IOT Server can be accessed via a browser at the following URL:");
    Serial.print("http://");
    Serial.println(WiFi.localIP());
#endif
}

void setup() {
#ifdef DEBUG_MODE
    // Connect to serial console for DEBUG messages
    Serial.begin(115200);
#endif

    // Setup WiFi
    wifiSetup();

    // Set the pin mode for the built in LED port to OUTPUT
    pinMode(LED_BUILTIN, OUTPUT);

#ifdef DEBUG_MODE
    // DEBUG messages on serial console
    Serial.println("Starting server...");
#endif

    // Start the server
    server.begin();

#ifdef DEBUG_MODE
    // DEBUG messages on serial console
    Serial.println("Server started");
#endif

    // Turn of the built-in LED
    led.turnOff(true);
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        wifiSetup();

        return;
    }

    // Create a client connection object
    WiFiClient client = server.available();

    // Quit the loop if it didn't work (better luck next time)
    if (!client) {
        return;
    }

    while (client.connected()) {
        if (client.available()) {
            // A client has connected! 
#ifdef DEBUG_MODE
            // DEBUG messages on serial console
            Serial.print("New client connected");
#endif
            // Read the client request into a string
            String request = client.readStringUntil('\n');

#ifdef DEBUG_MODE
            // DEBUG messages on serial console
            Serial.print("Request: ");
            Serial.println(request);
#endif

            client.flush();
            client.println(okResponse);

            // If it is an AJAX call, serve the JSON content and quit the loop
            if (request.indexOf("/led") != -1) {
                if (request.indexOf("/led/1") != -1) {
                    led.turnOn(true);
                }

                if (request.indexOf("/led/0") != -1) {
                    led.turnOff(true);
                }

                client.println(contentHeaderJson);
                client.println("");
                client.println(led.isOn() ? ledIsOnResponse : ledIsOffResponse);
            } else {
                // This is not an AJAX call, so we will serve HTML instead
                client.println(contentHeaderHtml);
                client.println("");

                // Serve the HTML page
                client.println("<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><link rel=\"icon\" href=\"data:,\"><title>LED Control</title><style>html {font-family: Helvetica;display: inline-block;margin: 0px auto;text-align: center;}.led-state {background-color: #16161a;color: #fffffe;border: 0;border-radius: 50%;margin: auto;width: 150px;height: 150px;text-align: center;vertical-align: middle;line-height: 150px;}.is-on {background-color: #1974D2;}.led-switch {background-color: #2cb67d;border: 0;color: #fffffe;padding: 16px 40px;text-decoration: none;font-size: 30px;margin: 2px;cursor: pointer;border-radius: 15px 50px;width: 150px;outline: none;}.turn-off {background-color: #72757e;}</style></head><body><h1>LED Control</h1><div id=\"ledState\" class=\"led-state\">LED is OFF</div><p><button id=\"ledButton\" class=\"led-switch\" onclick=\"ajaxCall()\">ON</button></p><script>let statusObj = {ledIsOn: false};const ids = {refreshInterval: null,};const updateStatus = function (responseText) {statusObj.ledIsOn = JSON.parse(responseText).ledIsOn;const buttonObj = document.getElementById('ledButton');const ledStateObj = document.getElementById('ledState');if (statusObj.ledIsOn) {buttonObj.classList.add('turn-off');buttonObj.innerText = 'OFF';ledStateObj.classList.add('is-on');ledStateObj.innerText = 'LED is ON';} else {buttonObj.classList.remove('turn-off');buttonObj.innerText = 'ON';ledStateObj.classList.remove('is-on');ledStateObj.innerText = 'LED is OFF';}};const ajaxCall = function (statusOnly = false) {let url = '/led';if (!statusOnly)url += ('/' + (statusObj.ledIsOn ? '0' : '1'));const xhttp=new XMLHttpRequest();xhttp.onreadystatechange = function() {if (this.readyState == 4 && this.status == 200)updateStatus(this.responseText);};xhttp.open('GET', url, true);xhttp.send();};window.onload = () => {ajaxCall(true);ids.refreshInterval = setInterval(() => {ajaxCall(true);}, 1000);};window.onbeforeunload = () => {clearInterval(ids.refreshInterval);};</script></body></html>");

                // The HTTP response ends with another blank line
                client.println();
            }

            client.stop();
        }
    }
}
