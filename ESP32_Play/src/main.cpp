#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WiFiManager.h>

void handleRoot();
void handlePin();

const char *ssid = "Flat 202_2.4G";
const char *password = "8432224001";

WebServer server(80);

void setup() {
    pinMode(12, OUTPUT);
    Serial.begin(115200);
    Serial.println("Welcome to ESP32");

    // Connect to Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Start mDNS
    if (!MDNS.begin("esp32")) {
        Serial.println("Error starting MDNS responder!");
    } else {
        Serial.println("MDNS responder started");
    }

    // Define server routes
    server.on("/", handleRoot);
    server.on("/control", handlePin);
    server.onNotFound([]() {
        server.send(404, "text/html", "Page Not Found - 404 error");
    });

    // Start the server
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
}

void handleRoot() {
    String htmlPage = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>On-Off Buttons</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                text-align: center;
                margin-top: 20%;
            }
            button {
                font-size: 20px;
                padding: 10px 20px;
                margin: 10px;
                cursor: pointer;
                border: none;
                border-radius: 5px;
            }
            #onButton {
                background-color: #28a745;
                color: white;
            }
            #offButton {
                background-color: #dc3545;
                color: white;
            }
        </style>
    </head>
    <body>
        <h1>Simple On-Off Buttons</h1>
        <button id="onButton" onclick="sendRequest(1)">On</button>
        <button id="offButton" onclick="sendRequest(0)">Off</button>
        <script>
            function sendRequest(state) {
                fetch(`/control?data=${state}`)
                    .then(response => response.json())
                    .then(data => console.log(data))
                    .catch(err => console.error('Error:', err));
            }
        </script>
    </body>
    </html>
    )rawliteral";
    server.send(200, "text/html", htmlPage);
}

void handlePin() {
    String input = server.arg("data");
    if (input == "1") {
        digitalWrite(12, HIGH);
        server.send(200, "application/json", "{\"status\":\"1\"}");
    } else if (input == "0") {
        digitalWrite(12, LOW);
        server.send(200, "application/json", "{\"status\":\"0\"}");
    } else {
        server.send(400, "application/json", "{\"error\":\"Invalid Input\"}");
    }
}
