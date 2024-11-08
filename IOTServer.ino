// vim: syntax=arduino autoindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

#include <WiFi.h>
#include <NTPClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DHTesp.h>
#include <IRremote.h>
#include "IndexHtml.h"

// Comment this line out if you don't want serial console messages
//#define DEBUG_MODE

// Infrared remote carrier frequency
#define IR_CARRIER_FREQUENCY  38


int dhtPin = 17;

int irPin = 4;

float thermostatSetting = 27;

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 28800, 60000);

AsyncWebServer server(80);

DHTesp dht;

ComfortState cf;

IRsend irsend(irPin);

static const char* okResponse = "HTTP/1.1 200 OK";
static const char* contentHeaderJson = "Content-Type: application/json";
static const char* contentHeaderHtml = "Content-Type: text/html";
static const char* environmentResponse = "{\"ts\": \"%.1f\", \"t\": \"%.1f\", \"h\": \"%.1f\", \"hi\": \"%.1f\", \"dp\": \"%.1f\", \"cs\": \"%s\"}";

static const char* hostname = "iotserver";
static const char* ssid = "Doudou-IoT";
static const char* password = "0123206635";

unsigned long lastSyncTime = millis();

// Raw IR timings
const uint16_t txPowerToggle[] = {
    9788, 9676, 9812, 9680, 4652, 2408, 432, 296, 436, 868,
    436, 868, 432, 296, 432, 872, 432, 296, 436, 296,
    432, 296, 432, 300, 432, 296, 432, 868, 436, 296,
    432, 296, 436, 868, 432, 300, 432, 296, 436, 292,
    432, 872, 432, 296, 436, 296, 432, 296, 432, 296,
    436, 868, 436, 296, 432, 868, 436, 296, 432, 872,
    432, 296, 432, 300, 432, 292, 436, 296, 432, 300,
    432, 296, 432, 296, 436, 296, 432, 296, 436, 864,
    436, 296, 436, 296, 432, 296, 432, 300, 432, 296,
    432, 296, 436, 296, 432, 868, 436, 296, 432, 296,
    436, 296, 432, 296, 432, 296, 436, 1100, 204, 292,
    436, 296, 432, 868, 436, 296, 432, 300, 432, 868,
    432, 300, 432, 868, 436, 868, 432, 868, 436, 296,
    432, 868, 436, 868, 436, 20048, 4652
};
const uint16_t txFanMode[] = {
    9788, 9676, 9816, 9676, 4656, 2404, 436, 296, 436, 864,
    436, 868, 436, 296, 432, 868, 436, 328, 404, 288,
    440, 296, 432, 296, 436, 292, 436, 868, 436, 296,
    432, 296, 436, 864, 436, 296, 436, 296, 436, 324,
    404, 864, 436, 868, 436, 296, 432, 868, 436, 868,
    436, 296, 432, 296, 436, 864, 436, 296, 436, 864,
    440, 292, 436, 296, 432, 296, 436, 328, 400, 328,
    404, 292, 436, 296, 432, 296, 436, 296, 432, 868,
    436, 296, 432, 296, 436, 292, 436, 292, 436, 296,
    436, 296, 432, 296, 436, 864, 436, 296, 436, 328,
    400, 296, 436, 868, 432, 868, 436, 868, 436, 292,
    436, 296, 432, 872, 432, 328, 404, 292, 436, 868,
    436, 296, 432, 868, 436, 296, 432, 868, 436, 868,
    436, 292, 436, 868, 436, 20044, 4656
};
const uint16_t txCoolingMode[] = {
    9788, 9680, 9812, 9680, 4652, 2408, 432, 300, 428, 868,
    436, 868, 436, 300, 428, 872, 432, 296, 432, 300,
    428, 300, 432, 300, 432, 868, 432, 300, 428, 300,
    432, 300, 428, 872, 432, 300, 432, 296, 432, 300,
    428, 300, 432, 868, 432, 300, 432, 300, 428, 300,
    428, 872, 432, 300, 432, 868, 432, 300, 432, 868,
    432, 300, 432, 300, 432, 296, 432, 296, 432, 300,
    432, 296, 432, 300, 428, 300, 432, 296, 432, 872,
    432, 300, 428, 300, 432, 300, 428, 300, 428, 300,
    432, 300, 428, 300, 432, 868, 432, 300, 432, 296,
    432, 300, 428, 304, 428, 300, 428, 872, 432, 300,
    432, 300, 428, 868, 432, 300, 432, 296, 432, 872,
    432, 300, 432, 868, 432, 300, 432, 868, 432, 300,
    432, 868, 432, 300, 432, 20052, 4652
};

char stringBuffer[96];

void wifiSetup() {
    // Disconnect from WiFi
    WiFi.disconnect(true, true);

    delay(1000);

    // Set hostname
    WiFi.setHostname(hostname);

    // Set WiFi mode to station
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

    dht.setup(dhtPin, DHTesp::DHT22);

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
        thermostatSetting += 0.5;
        request->send(200, "text/plain", "OK");
    });

    // Decrease temperature
    server.on("/temperature/down", HTTP_GET, [] (AsyncWebServerRequest *request) {
        thermostatSetting -= 0.5;
        request->send(200, "text/plain", "OK");
    });

    // Route for GET request to /tx/cooling
    server.on("/mode/cooling", HTTP_GET, [] (AsyncWebServerRequest *request) {
        // Send txCoolingMode
        irsend.sendRaw(txCoolingMode, sizeof(txCoolingMode) / sizeof(txCoolingMode[0]), IR_CARRIER_FREQUENCY);
        request->send(200, "text/plain", "OK");
    });

    // Route for GET request to /tx/fan
    server.on("/mode/fan", HTTP_GET, [] (AsyncWebServerRequest *request) {
        // Send txFanMode
        irsend.sendRaw(txFanMode, sizeof(txFanMode) / sizeof(txFanMode[0]), IR_CARRIER_FREQUENCY);
        request->send(200, "text/plain", "OK");
    });

    // Route for GET request to /temperature
    server.on("/environment", HTTP_GET, [](AsyncWebServerRequest *request) {
        TempAndHumidity dhtValues = dht.getTempAndHumidity();

        if (dht.getStatus() != 0) {
            request->send(503, "text/plain", "Service unavailable");
        } else {
            float heatIndex = dht.computeHeatIndex(dhtValues.temperature - 2, dhtValues.humidity);
            float dewPoint = dht.computeDewPoint(dhtValues.temperature - 2, dhtValues.humidity);
            float cr = dht.getComfortRatio(cf, dhtValues.temperature - 2, dhtValues.humidity);

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

            sprintf(stringBuffer, environmentResponse, thermostatSetting, dhtValues.temperature - 2, dhtValues.humidity, heatIndex, dewPoint, comfortStatus);
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

void loop() {
    unsigned long timeElapsed = millis();

    if (WiFi.status() != WL_CONNECTED) {
        wifiSetup();
    }

    // Run ntp update every 1 seconds
    if ((timeElapsed - lastSyncTime) >= 1000) {
        lastSyncTime = timeElapsed;
        timeClient.update();
    }
}
