
// for operation it firstly connects to the flat 202 2.4g wifi router then it broadcast the server

#include <Arduino.h>        // Base library for Arduino framework
#include <WiFi.h>           // Library for Wi-Fi functionality
#include <WebServer.h>      // Library to create a simple HTTP server
#include <ESPmDNS.h>        // Library for mDNS (Multicast DNS) functionality
#include <WiFiManager.h>    // Library to manage Wi-Fi connections

// Function prototypes for handling HTTP requests
void handleRoot();
void handlePin();

WebServer server(80);                     // Create an HTTP server object on port 80

void setup() {
    pinMode(12, OUTPUT);                  // Configure GPIO pin 12 as an output pin
    Serial.begin(115200);                 // Initialize Serial Monitor for debugging
    Serial.println("Welcome to ESP32 with WiFiManager!"); 

    // Create a WiFiManager object
    WiFiManager wifiManager;

    // Automatically connect or start AP for configuration
    if (!wifiManager.autoConnect("ESP32-AP", "12345678")) { 
        Serial.println("Failed to connect and hit timeout");
        ESP.restart(); // Restart if unable to connect
    }

    // Print connected Wi-Fi details
    Serial.println("Connected to Wi-Fi!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());       // Print the assigned IP address

    // Start mDNS service
    if (!MDNS.begin("esp32")) {           // Initialize mDNS responder with "esp32" hostname
        Serial.println("Error starting MDNS responder!");
    } else {
        Serial.println("MDNS responder started");
    }

    // Define server routes
    server.on("/", handleRoot);            // Handle requests to the root URL ("/")
    server.on("/control", handlePin);      // Handle requests to the "/control" URL
    server.onNotFound([]() {               // Handle all other undefined URLs
        server.send(404, "text/html", "Page Not Found - 404 error");
    });

    // Start the HTTP server
    server.begin();                        // Begin the HTTP server
    Serial.println("HTTP server started"); // Print confirmation message
}

void loop() {
    server.handleClient();                 // Handle incoming client requests
}

void handleRoot() {
    // HTML content for the web page
    String htmlPage = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>ESP32 Buzzer Buttons</title>
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
        <h1>Welcome to ESP32 Server</h1>
        <h2>Buzzer On-Off Buttons</h2>
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

    server.send(200, "text/html", htmlPage); // Send HTML content to the client
}

void handlePin() {
    // Retrieve the "data" parameter from the URL
    String input = server.arg("data");
    if (input == "1") {                    // If the input is "1", turn on the LED
        digitalWrite(12, HIGH);            // Set GPIO pin 12 HIGH (LED ON)
        server.send(200, "application/json", "{\"status\":\"1\"}"); // Respond with JSON status
    } else if (input == "0") {             // If the input is "0", turn off the LED
        digitalWrite(12, LOW);             // Set GPIO pin 12 LOW (LED OFF)
        server.send(200, "application/json", "{\"status\":\"0\"}"); // Respond with JSON status
    } else {                               // If input is invalid
        server.send(400, "application/json", "{\"error\":\"Invalid Input\"}"); // Respond with error
    }
}
