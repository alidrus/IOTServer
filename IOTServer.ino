// vim: syntax=arduino autoindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

#include <WiFi.h>
#include <ESPmDNS.h>
#include <NTPClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DHTesp.h>

#include "Hysteresis.h"
#include "IndexHtml.h"
#include "Pins.h"
#include "Secrets.h"
#include "Calibration.h"

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
static const char* environmentResponse = "{\"ts\": \"%.2f\", \"t\": \"%.2f\", \"h\": \"%.2f\", \"hi\": \"%.2f\", \"dp\": \"%.2f\", \"cs\": \"%s\", \"co\": %s}";

char stringBuffer[112];

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
            Serial.println("Unable to connect to WiFi!");
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

    // Set up mDNS
    if (!MDNS.begin(WIFI_HOSTNAME)) {
        // Reboot!
        Serial.println("Unable to set up mDNS!");

        return;
    }
}

void webServerSetup() {
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
            const float temperature = dhtValues.temperature + TEMPERATURE_CALIBRATION;
            const float heatIndex = dht.computeHeatIndex(temperature, dhtValues.humidity);
            const float dewPoint = dht.computeDewPoint(temperature, dhtValues.humidity);
            const float cr = dht.getComfortRatio(cf, temperature, dhtValues.humidity);
            const float compressorIsOn = climateControl.getCompressorIsOn();

            String comfortStatus;

            switch(cf) {
                case Comfort_OK:
                    comfortStatus = "OK";
                    break;
                case Comfort_TooHot:
                    comfortStatus = "Too Hot";
                    break;
                case Comfort_TooCold:
                    comfortStatus = "Too Cold";
                    break;
                case Comfort_TooDry:
                    comfortStatus = "Too Dry";
                    break;
                case Comfort_TooHumid:
                    comfortStatus = "Too Humid";
                    break;
                case Comfort_HotAndHumid:
                    comfortStatus = "Hot and Humid";
                    break;
                case Comfort_HotAndDry:
                    comfortStatus = "Hot and Dry";
                    break;
                case Comfort_ColdAndHumid:
                    comfortStatus = "Cold and Humid";
                    break;
                case Comfort_ColdAndDry:
                    comfortStatus = "Cold and Dry";
                    break;
                default:
                    comfortStatus = "Unknown";
                    break;
            };

            sprintf(stringBuffer, environmentResponse, targetDewPoint, temperature, dhtValues.humidity, heatIndex, dewPoint, comfortStatus, compressorIsOn ? "true" : "false");

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
}

void setup() {
#ifdef DEBUG_MODE
    // Connect to serial console for DEBUG messages
    Serial.begin(115200);
#endif

    delay(1500);

    // Setup WiFi
    wifiSetup();

    delay(500);

    dht.setup(DHT_PIN, DHTesp::DHT22);

    delay(250);

    // Setup Web Server
    webServerSetup();

    delay(250);

    // Begin time client
    timeClient.begin();

    delay(500);

    // Create Tasks
    xTaskCreate(multiTask, "Multiple Task", 4000, NULL, 1, NULL);
}

void multiTask(void *parameters) {
    int counter = 0;

    unsigned long rawTime, hours;

    while (true) {
        // Check WiFi health and reconnect if needed
        if (WiFi.status() != WL_CONNECTED) {
            wifiSetup();
        }

        // Only perform timesync when connected to WiFi
        if (WiFi.status() == WL_CONNECTED) {
            timeClient.update();
        }

        // Run climate control monitoring every 20 seconds
        if (counter % 20 == 0) {
            rawTime = timeClient.getEpochTime();

            hours = (rawTime % 86400L) / 3600;

            TempAndHumidity dhtValues = dht.getTempAndHumidity();

            const float temperature = dhtValues.temperature + TEMPERATURE_CALIBRATION;
            const float dewPoint = dht.computeDewPoint(temperature, dhtValues.humidity);

            climateControl.monitorComfort(dewPoint);
        }

        counter += 1;

        // Idle for the next second
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void loop() {}
