
/* 
Here,(AP mode)
    It acts as a Hot spot and connects with the device by wifi 
    showing the SSID and displays the Password to connect 
*/


#include <Arduino.h>        // Base library for Arduino framework
#include <WiFi.h>           // Library for Wi-Fi functionality
#include <WebServer.h>      // Library to create a simple HTTP server
#include <ESPmDNS.h>        // Library for mDNS (Multicast DNS) functionality
#include <WiFiManager.h>    // Library to manage Wi-Fi connections
#include <DNSServer.h>
#include <Update.h>

// Ultrasonic sensor pins
const int trigPin = 38;
const int echoPin = 37;
const int L1 = 6;  //Pin for Led Red / Led 1
const int L2 = 18;  //Pin for Led Green / Led 2
const int L3 = 17;
const int L4 = 16;
const int Bz1 = 5;

const char *apSSID = "ESP32_Access_Point"; // Access Point SSID
const char *apPassword = "12345678";       // Access Point Password (at least 8 characters)

long duration;
int distance;

// HTTP server object
WebServer server(80);

// Function prototypes
void handleRoot();
void handleDistance();
float getDistance();

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  pinMode(L3,OUTPUT); //Onboard Led
  pinMode(L1, OUTPUT);  //Showing output on Led 1
  pinMode(L2, OUTPUT);  // Showing output on Led 2
  pinMode(L4, OUTPUT);
    Serial.begin(115200);
    Serial.println("Welcome to ESP32 Ultrasonic Sensor with WiFiManager!"); 

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
        Serial.println("Error starting MDNS responder!");
    } else {
        Serial.println("MDNS responder started");
    }

    // Define server routes
    server.on("/", handleRoot);
    server.on("/distance", handleDistance);
    server.onNotFound([]() {
        server.send(404, "text/html", "Page Not Found - 404 error");
    });

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
}

void handleRoot() {
    // HTML content for the web page
    String htmlPage = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>ESP32 Ultrasonic Sensor</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                text-align: center;
                margin-top: 20%;
            }
            h1 {
                font-size: 2.5rem;
                color: #333;
            }
            p {
                font-size: 1.5rem;
                color: #555;
            }
        </style>
    </head>
    <body>
        <h1>Ultrasonic Sensor Distance</h1>
        <p id="distance">Loading...</p>
        <script>
            function updateDistance() {
                fetch('/distance')
                    .then(response => response.json())
                    .then(data => {
                        document.getElementById('distance').innerText = `Distance: ${data.distance} cm`;
                    })
                    .catch(err => {
                        document.getElementById('distance').innerText = 'Error fetching distance';
                        console.error('Error:', err);
                    });
            }

            setInterval(updateDistance, 1000); // Update distance every second
        </script>
    </body>
    </html>
    )rawliteral";

    server.send(200, "text/html", htmlPage);
}

void handleDistance() {
    float distance = getDistance(); // Get distance from ultrasonic sensor
    String jsonResponse = "{\"distance\":\"" + String(distance, 2) + "\"}";
    server.send(200, "application/json", jsonResponse);
}


float getDistance() {
    // Trigger the ultrasonic sensor
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Measure the echo time
    long duration = pulseIn(echoPin, HIGH);

    // Calculate distance in cm (speed of sound = 343 m/s)
    float distance = (duration * 0.0343) / 2.0;

      Serial.print("Distance: ");
  Serial.println(distance);

  if(distance <= 5) {
    digitalWrite(L1, HIGH);   
    delay(300);              
    digitalWrite(L1, LOW);    
    delay(300);  
  } else if (distance > 5 && distance <= 10) {
    digitalWrite(L2, HIGH);   
    delay(300);              
    digitalWrite(L2, LOW);    
    delay(300);  
  } else if (distance > 10 && distance <= 15) {  
    digitalWrite(L3, HIGH);   
    delay(300);              
    digitalWrite(L3, LOW);    
    delay(300);     
  } else if (distance > 15 && distance <= 20) {
    digitalWrite(L4, HIGH);   
    delay(300);              
    digitalWrite(L4, LOW);    
    delay(300);
  } else if (distance > 20 && distance <= 29) {
    digitalWrite(L4, HIGH);   
    delay(300);              
    digitalWrite(L4, LOW);    
    delay(300);
   } else if (distance > 30) {
    digitalWrite(Bz1, LOW);   
    delay(500);              
    digitalWrite(Bz1, HIGH);    
    delay(500);   
}

 delay(100);

    return distance;
}


/* 

Platformio.ini File code


; PlatformIO Project Configuration File
;
;   Build options: build flags, source filter
;   Upload options: custom upload port, speed and extra flags
;   Library options: dependencies, extra library storages
;   Advanced options: extra scripting
;
; Please visit documentation for the other options and examples
; https://docs.platformio.org/page/projectconf.html

[env:esp32-s3-devkitc-1]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
upload_port = COM5
monitor_dtr = 0 ; added for the serial monitor as given suggestions on the community page.
monitor_rts = 0
monitor_speed = 115200
lib_deps = https://github.com/tzapu/WiFiManager
	        ESPmDNS
	        NetworkClient
	        WiFiManager


*/