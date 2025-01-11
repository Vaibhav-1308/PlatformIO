#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiManager.h> // Library to handle Wi-Fi credentials dynamically

// Function prototypes for handling HTTP requests
void handleRoot();
void handleInfo();

WebServer server(80); // Create an HTTP server object on port 80

void setup() {
    Serial.begin(115200); // Initialize Serial Monitor for debugging
    pinMode(12, OUTPUT);  // Configure GPIO pin 12 as an output pin

    // Start WiFiManager
    WiFiManager wm;

    // Configure the Access Point settings for WiFiManager
    const char *apSSID = "ESP32-Config";       // Temporary AP SSID for configuration
    const char *apPassword = "12345678";       // Temporary AP password
    wm.setTimeout(180);                        // Set timeout for configuration portal (seconds)

    // Start WiFiManager Access Point for user configuration
    if (!wm.autoConnect(apSSID, apPassword)) { // Start AP mode and wait for configuration
        Serial.println("Failed to connect or timeout, restarting...");
        delay(3000);
        ESP.restart();                         // Restart ESP32 if configuration fails
    }

    // At this point, the ESP32 is connected to a network or is in AP mode
    Serial.println("Wi-Fi connected!");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("Password: ");
    Serial.println(WiFi.psk());
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Define server routes
    server.on("/", handleRoot);        // Handle requests to the root URL
    server.on("/info", handleInfo);    // Display current SSID and Password
    server.onNotFound([]() {           // Handle undefined URLs
        server.send(404, "text/html", "Page Not Found - 404 error");
    });

    // Start the HTTP server
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient(); // Handle incoming client requests
}

void handleRoot() {
    // HTML content for the web page
    String htmlPage = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>ESP32 Web Server</title>
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
                background-color: #007BFF;
                color: white;
            }
            button:hover {
                background-color: #0056b3;
            }
        </style>
    </head>
    <body>
        <h1>Welcome to ESP32 Web Server</h1>
        <p>Click the button below to view Wi-Fi credentials</p>
        <a href="/info"><button>View Wi-Fi Info</button></a>
    </body>
    </html>
    )rawliteral";

    server.send(200, "text/html", htmlPage); // Send HTML content to the client
}

void handleInfo() {
    // Display Wi-Fi credentials dynamically
    String infoPage = "<!DOCTYPE html><html><head><title>Wi-Fi Info</title></head><body>";
    infoPage += "<h1>Wi-Fi Info</h1>";
    infoPage += "<p><strong>SSID:</strong> " + WiFi.SSID() + "</p>";
    infoPage += "<p><strong>Password:</strong> " + WiFi.psk() + "</p>";
    infoPage += "<p><strong>IP Address:</strong> " + WiFi.localIP().toString() + "</p>";
    infoPage += "<a href=\"/\"><button>Back</button></a>";
    infoPage += "</body></html>";

    server.send(200, "text/html", infoPage); // Send Wi-Fi info as HTML
}
