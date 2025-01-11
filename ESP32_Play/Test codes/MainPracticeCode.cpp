#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <ESPmDNS.h>
#include <WebServer.h>


// Function prototypes for controlling HTTP requests
void handleRoot();
void handlePin();


const char *ssid = "Flat 202_2.4G"; // WiFi SSID 
const char *password = "8432224001"; // WiFi Password

WebServer server(80);

void setup(){
    pinMode(12, OUTPUT); // Configure G{IO pin 12 as an OUTPUT pin
    Serial.begin(115200); // Initialize serial monitor  
    Serial.println("Welcome to ESP32 Logan Bro"); // Printing welcome message

    WiFi.mode(WIFI_STA); // Set Wi-Fi mode to Station (Client mode)
    WiFi.begin(ssid, password); // Connecting to the wifi with the provided credential 
    Serial.println(""); 

    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid); // Print the SSID of the connected network
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); // Print the ESP32's assigned IP address

    // Start mDNS service 
    if (!MDNS.begin("ESP32")){
        Serial.println("Error starting MDNS responder");
    } else {
        Serial.println("MDNS responder started");
    }

    // Define server routes 
    server.on("/", handleRoot); // Handle requests to the root URL ("/")
    server.on("/control", handlePin); // Handle request to the "/control" URL
    server.onNotFound([](){
        server.send(404, "text/html", "Page Not Found - 404 Error"); 
    });

    // starting HTTP server 
    server.begin(); // Begin the HTTP server
    Serial.println("HTTP server started"); // Print configuration message

}

    void loop(){
        server.handleClient(); // Handle incoming client request
    }

    void handlePin() {

        // Retrieve the "Data" Parameters from the URL
        String input = server.arg("data");
        if (input == "1") {
        digitalWrite(12, HIGH);
        server.send(200, "application/json", "{\"status\":\"1\"}"); // Respond with JSON status
    } else if (input == "0"){
        digitalWrite(12, LOW);
        server.send(200, "application/json", "{\"error\":\"0\"}");
    } else {
        server.send(400, "application /json", "{\"error\":\"Invalid Input\"}"); // Respond with error
    }
     
    }

