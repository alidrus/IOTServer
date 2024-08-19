// vim: syntax=arduino autoindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "MyLED.h"

// Comment this line out if you don't want serial console messages
#define DEBUG_MODE

AsyncWebServer server(80);

MyLED led;

static const char* okResponse = "HTTP/1.1 200 OK";
static const char* contentHeaderJson = "Content-Type: application/json";
static const char* contentHeaderHtml = "Content-Type: text/html";
static const char* ledIsOnResponse = "{\"ledIsOn\": true}";
static const char* ledIsOffResponse = "{\"ledIsOn\": false}";

static const char* hostname = "iotserver";
static const char* ssid = "Doudou-IoT";
static const char* password = "0123206635";

static const char* index_html = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><link rel=\"icon\" href=\"data:,\"><title>LED Control</title><style>html {font-family: Helvetica;display: inline-block;margin: 0px auto;text-align: center;}.led-state {background-color: #16161a;color: #fffffe;border: 0;border-radius: 50%;font-weight: bold;font-size: 16pt;margin: auto;width: 150px;height: 150px;text-align: center;vertical-align: middle;line-height: 150px;}.is-on {background-color: #1974D2;}.led-switch {background-color: #2cb67d;border: 0;color: #fffffe;padding: 8px 20px;text-decoration: none;font-size: 12pt;margin: 2px;cursor: pointer;border-radius: 15px 50px;width: 150px;outline: none;}.turn-off {background-color: #72757e;}</style></head><body><h1>LED Control</h1><div id=\"ledState\" class=\"led-state\">LED is OFF</div><p><button id=\"ledButton\" class=\"led-switch\" onclick=\"ajaxCall()\">Turn ON</button></p><script>let statusObj = {ledIsOn: false,ajaxCallInProgress: false};const ids = {refreshInterval: null,};const updateStatus = function (responseText) {statusObj.ledIsOn = JSON.parse(responseText).ledIsOn;const buttonObj = document.getElementById('ledButton');const ledStateObj = document.getElementById('ledState');if (statusObj.ledIsOn) {buttonObj.classList.add('turn-off');buttonObj.innerText = 'Turn OFF';ledStateObj.classList.add('is-on');ledStateObj.innerText = 'LED is ON';} else {buttonObj.classList.remove('turn-off');buttonObj.innerText = 'Turn ON';ledStateObj.classList.remove('is-on');ledStateObj.innerText = 'LED is OFF';}};const ajaxCall = function (statusOnly = false) {let url = '/led';if (!statusOnly)url += ('/' + (statusObj.ledIsOn ? '0' : '1'));const xhttp=new XMLHttpRequest();xhttp.onreadystatechange = function() {if (this.readyState == 4 && this.status == 200)updateStatus(this.responseText);};xhttp.open('GET', url, true);xhttp.send();};window.onload = () => {ids.refreshInterval = setInterval(() => {if (statusObj.ajaxCallInProgress) {return;}statusObj.ajaxCallInProgress = true;ajaxCall(true);statusObj.ajaxCallInProgress = false;}, 100);};window.onbeforeunload = () => {clearInterval(ids.refreshInterval);};</script></body></html>";

void wifiSetup() {
    // Disconnect from WiFi
    WiFi.disconnect();

    delay(1000);

    // Set hostname
    /*
    if (!WiFi.setHostname(hostname)) {
        Serial.println("Failed to set Wifi hostname");
    } else {
        Serial.println("Set Wifi hostname");
    }

    // Set WiFi mode to station
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);  // This is a MUST!
    */

    // Connect in station mode
    WiFi.mode(WIFI_STA);

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

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("REQUEST: /");
        request->send(200, "text/html", index_html);
    });

    // Route for GET request to /led/0
    server.on("/led/1", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Serial.println("REQUEST: /led/1");
        led.turnOn(false);
        request->send(200, "text/html", led.isOn() ? ledIsOnResponse : ledIsOffResponse);
        Serial.println(String("RESPONSE: ") + (led.isOn() ? ledIsOnResponse : ledIsOffResponse));
    });

    // Route for GET request to /led/0
    server.on("/led/0", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Serial.println("REQUEST: /led/0");
        led.turnOff(false);
        request->send(200, "text/html", led.isOn() ? ledIsOnResponse : ledIsOffResponse);
        Serial.println(String("RESPONSE: ") + (led.isOn() ? ledIsOnResponse : ledIsOffResponse));
    });

    // Route for GET request to /led (ordering is important as this matches
    // like a regex so it has to come after /led/1 and /led/0)
    server.on("/led", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", led.isOn() ? ledIsOnResponse : ledIsOffResponse);
    });

    // Reply with a 404 error if request does not match anything else
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not found");
    });

    // Start the server
    server.begin();

#ifdef DEBUG_MODE
    // DEBUG messages on serial console
    Serial.println("Server started");
#endif

    // Turn of the built-in LED upon startup
    led.turnOff(true);
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        wifiSetup();
    }
}
