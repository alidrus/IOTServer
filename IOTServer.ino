// vim: syntax=arduino autoindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

#include <WiFi.h>
#include <NTPClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DHTesp.h>

#include "Hysteresis.h"
#include "IndexHtml.h"
#include "Pins.h"
#include "Secrets.h"

#include <IRremote.hpp>

// Comment this line out if you don't want serial console messages
#define DEBUG_MODE

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 28800, 60000);

AsyncWebServer server(80);

DHTesp dht;

ComfortState cf;

Hysteresis climateControl;

static const char* okResponse = "HTTP/1.1 200 OK";
static const char* contentHeaderJson = "Content-Type: application/json";
static const char* contentHeaderHtml = "Content-Type: text/html";
static const char* environmentResponse = "{\"ts\": \"%.1f\", \"t\": \"%.1f\", \"h\": \"%.1f\", \"hi\": \"%.1f\", \"dp\": \"%.1f\", \"cs\": \"%s\"}";

unsigned long lastSyncTime = millis();
unsigned long lastHysteresisTime = millis();

char stringBuffer[96];

void wifiSetup() {
    // Disconnect from WiFi
    WiFi.disconnect(true, true);

    delay(1000);

    // Set hostname
    WiFi.setHostname(WIFI_HOSTNAME);

    // Set WiFi mode to station
    WiFi.mode(WIFI_STA);

    // WiFi credentials
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

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

    dht.setup(DHT_PIN, DHTesp::DHT22);

    delay(1000);

    // Setup WiFi
    wifiSetup();

    timeClient.begin();

#ifdef DEBUG_MODE
    // DEBUG messages on serial console
    Serial.println("Starting server...");
#endif

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("REQUEST: /");

        request->send(200, "text/html; charset=UTF-8", indexHtml);
    });

    // Route for root /index.html
    server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("REQUEST: /index.html");

        request->send(200, "text/html; charset=UTF-8", indexHtml);
    });

    // Route for root /index.css
    server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("REQUEST: /index.css");

        request->send(200, "text/css; charset=UTF-8", indexCss);
    });

    // Route for root /index.css
    server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("REQUEST: /index.js");

        request->send(200, "text/javascript; charset=UTF-8", indexJs);
    });

    // Increase temperature
    server.on("/temperature/up", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Serial.println("REQUEST: /temperature/up");

        climateControl.incrementTargetDewPoint();

        request->send(200, "text/plain", "OK");
    });

    // Decrease temperature
    server.on("/temperature/down", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Serial.println("REQUEST: /temperature/down");

        climateControl.decrementTargetDewPoint();

        request->send(200, "text/plain", "OK");
    });

    // Route for GET request to /temperature
    server.on("/environment", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("REQUEST: /environment");

        TempAndHumidity dhtValues = dht.getTempAndHumidity();

        if (dht.getStatus() != 0) {
            request->send(503, "text/plain", "Service unavailable");
        } else {
            const float targetDewPoint = climateControl.getTargetDewPoint();
            const float heatIndex = dht.computeHeatIndex(dhtValues.temperature, dhtValues.humidity);
            const float dewPoint = dht.computeDewPoint(dhtValues.temperature, dhtValues.humidity);
            const float cr = dht.getComfortRatio(cf, dhtValues.temperature, dhtValues.humidity);

            String comfortStatus;

            switch(cf) {
                case Comfort_OK:
                    comfortStatus = "OK";
                    break;
                case Comfort_TooHot:
                    comfortStatus = "TooHot";
                    break;
                case Comfort_TooCold:
                    comfortStatus = "TooCold";
                    break;
                case Comfort_TooDry:
                    comfortStatus = "TooDry";
                    break;
                case Comfort_TooHumid:
                    comfortStatus = "TooHumid";
                    break;
                case Comfort_HotAndHumid:
                    comfortStatus = "HotAndHumid";
                    break;
                case Comfort_HotAndDry:
                    comfortStatus = "HotAndDry";
                    break;
                case Comfort_ColdAndHumid:
                    comfortStatus = "ColdAndHumid";
                    break;
                case Comfort_ColdAndDry:
                    comfortStatus = "ColdAndDry";
                    break;
                default:
                    comfortStatus = "Unknown";
                    break;
            };

            sprintf(stringBuffer, environmentResponse, targetDewPoint, dhtValues.temperature, dhtValues.humidity, heatIndex, dewPoint, comfortStatus);
            request->send(200, "application/json", stringBuffer);
        }
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

    // First run of ntp update (time sync)
    lastSyncTime = millis();
    timeClient.update();
}

void loop() {
    unsigned long timeElapsed = millis();

    if (WiFi.status() != WL_CONNECTED) {
        wifiSetup();
    }

    // Run every 60 seconds
    if ((timeElapsed - lastSyncTime) >= 60000) {
        lastSyncTime = timeElapsed;

        timeClient.update();
    }

    // Run every 20 seconds
    if ((timeElapsed - lastHysteresisTime) >= 20000) {
        lastHysteresisTime = timeElapsed;

        TempAndHumidity dhtValues = dht.getTempAndHumidity();
        const float dewPoint = dht.computeDewPoint(dhtValues.temperature, dhtValues.humidity);

        climateControl.monitorComfort(dewPoint);
    }
}
