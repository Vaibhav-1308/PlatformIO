#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
// #include <NetworkClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

void handleRoot();
void readTemp();
void handlePin();
void collectHandler();

const char *ssid = "Realme8";
const char *password = "1234567890";

WebServer server(80);
WiFiManager wifiManager;
void setup()
{
  pinMode(12, OUTPUT);
  Serial.begin(115200);
  Serial.println("Welcome to ESP32 Tut1");
  // wifiManager.autoConnect("AP-NAME", "AP-PASSWORD");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32"))
  {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/inline", []()
            { server.send(200, "text/plain", "this works as well"); });
  server.on("/temp", readTemp);
  server.on("/control", handlePin);
  server.on("/collect", HTTP_POST, collectHandler);
  server.onNotFound([]()
                    { server.send(404, "text/html", "Page Not found - 404 error"); });
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();
  delay(2); // allow the cpu to switch to other tasks
}

void handleRoot()
{
  server.send(200, "text/html", "I am root <b>handler</b>");
}
void readTemp()
{
  int temp = 28.30;
  String message = "Current Temperature is: ";
  message += temp;
  message += " Deg C";
  server.send(200, "text/html", message);
}
void handlePin()
{
  String input = server.arg("data");
  if (input == "1")
  {
    digitalWrite(12, HIGH);
    server.send(200, "application/json", "{\"status\":\"1\"}" );
  }
  else
  {
    digitalWrite(12, LOW);
    server.send(200, "application/json", "{\"status\":\"0\"}" );
  }

}

void collectHandler() {
  /// String input = server.arg();
  // String resp = server.raw();
}