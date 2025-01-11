#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <Update.h>
#include <DNSServer.h>

// Function prototypes for controlling HTTP requests
void handleRoot();
void handlePin();

const char *apSSID = "ESP32_Access_Point"; // Access Point SSID
const char *apPassword = "12345678";       // Access Point Password (at least 8 characters)

WebServer server(80);

void setup() {
    pinMode(12, OUTPUT);          // Configure GPIO pin 12 as an OUTPUT pin
    Serial.begin(115200);         // Initialize serial monitor
    Serial.println("Starting ESP32 Access Point...");

    // Configure ESP32 as an Access Point
    WiFi.softAP(apSSID, apPassword); // Create Wi-Fi Access Point
    IPAddress IP = WiFi.softAPIP();  // Get IP address of the Access Point
    Serial.print("\nTo Connect to the ESP32 use the credentials given below:\n");
    Serial.print("SSID of ESP32 is ");
    Serial.print(apSSID);
    Serial.print("\nPassword of ESP32 is ");
    Serial.println(apPassword);
    Serial.print("\nAccess Point IP: ");
    Serial.println(IP);              // Print the Access Point's IP address

    // Start mDNS service
    if (!MDNS.begin("esp32")) {
        Serial.println("Error starting mDNS responder");
    } else {
        Serial.println("MDNS responder started");
    }

    // Define server routes
    server.on("/", handleRoot);          // Handle requests to the root URL ("/")
    server.on("/control", handlePin);    // Handle request to the "/control" URL
    server.onNotFound([]() {
        server.send(404, "text/html", "Page Not Found - 404 Error");
    });

    // Start HTTP server
    server.begin();                      // Begin the HTTP server
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();               // Handle incoming client requests
}

void handleRoot() {
    // Define an HTML page as a string using a raw string literal for better readability
    String htmlPage = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8"> <!-- Set character encoding to UTF-8 -->
        <meta name="viewport" content="width=device-width, initial-scale=1.0"> <!-- Ensure responsive design -->
        <title>ESP32 Buzzer Buttons</title> <!-- Title of the webpage -->
        <style>
            /* Inline CSS styles for the webpage */
            body {
                font-family: Arial, sans-serif; /* Set font style */
                text-align: center; /* Center align the content */
                margin-top: 20%; /* Add top margin to center content vertically */
            }
            button {
                font-size: 20px; /* Set button font size */
                padding: 10px 20px; /* Add padding to buttons */
                margin: 10px; /* Add margin between buttons */
                cursor: pointer; /* Change cursor to pointer on hover */
                border: none; /* Remove default border */
                border-radius: 5px; /* Add rounded corners to buttons */
            }
            #onButton {
                background-color: #28a745; /* Green background for "On" button */
                color: white; /* White text color */
            }
            #offButton {
                background-color: #dc3545; /* Red background for "Off" button */
                color: white; /* White text color */
            }
        </style>
    </head>
    <body>
        <h1>Welcome to ESP32 Server</h1> <!-- Main heading of the webpage -->
        <h2>Buzzer On-Off Buttons</h2> <!-- Subheading of the webpage -->
        <!-- Button to turn the buzzer "On", calls sendRequest with parameter 1 -->
        <button id="onButton" onclick="sendRequest(1)">On</button>
        <!-- Button to turn the buzzer "Off", calls sendRequest with parameter 0 -->
        <button id="offButton" onclick="sendRequest(0)">Off</button>
        <script>
            // JavaScript function to send a request to the server when a button is clicked
            function sendRequest(state) {
                // Use the Fetch API to send a GET request to the server
                fetch(`/control?data=${state}`) // Append the state value to the "data" parameter
                    .then(response => response.json()) // Parse the server's response as JSON
                    .then(data => console.log(data)) // Log the response to the console
                    .catch(err => console.error('Error:', err)); // Handle errors by logging them
            }
        </script>
    </body>
    </html>
    )rawliteral";

    // Send the HTML page as the response to the client's request
    server.send(200, "text/html", htmlPage);
}


void handlePin() {
    // Retrieve the "Data" Parameters from the URL
    String input = server.arg("data");
    if (input == "1") {
        digitalWrite(12, HIGH);
        server.send(200, "application/json", "{\"status\":\"1\"}"); // Respond with JSON status
    } else if (input == "0") {
        digitalWrite(12, LOW);
        server.send(200, "application/json", "{\"status\":\"0\"}"); // Respond with JSON status
    } else {
        server.send(400, "application/json", "{\"error\":\"Invalid Input\"}"); // Respond with error
    }
}
