#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

const char* ssid     = "WIR-Guest";
const char* password = "Guest@WIRgroup";
const char* resource = "https://maker.ifttt.com/trigger/bme680_readings/with/key/qLBZzDxUbpVr2rEdb4JBX";
const char* server = "maker.ifttt.com";

unsigned long previousIFTTTRequestMillis = 0;  // will store last time LED was updated
// constants won't change:
const long IFTTTRequestInterval = 60000;  // interval at which to blink (milliseconds)

#define SEALEVELPRESSURE_HPA (1013.25)

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    float temperature;
    float humidity;
    float pressure;
    String barcode;
} struct_message;

// Create a struct_message called myData
struct_message myData;


AsyncWebServer webserver(80);
void initWifi() { 
  Serial.print(ssid);
  WiFi.begin(ssid, password);  

  int timeout = 10 * 4; 
  while(WiFi.status() != WL_CONNECTED  && (timeout-- > 0)) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");

  if(WiFi.status() != WL_CONNECTED) {
     Serial.println("Failed to connect, going back to sleep");
  }
 
  Serial.print(millis()); 
  Serial.println(WiFi.localIP());
}
void makeIFTTTRequest() { 
  Serial.print(server);
  
  WiFiClient client;
  int retries = 5;
  while(!!!client.connect(server, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if(!!!client.connected()) {
    Serial.println("Failed to connect...");
  }
  
  Serial.print("Request resource: "); 
  Serial.println(resource);

  
  String jsonObject = String("{\"value1\":\"") + myData.temperature + "\",\"value2\":\"" + (myData.pressure)
                      + "\",\"value3\":\"" + myData.humidity + "\"}";
                      
  
  client.println(String("POST ") + resource + " HTTP/1.1");
  client.println(String("Host: ") + server); 
  client.println("Connection: close\r\nContent-Type: application/json");
  client.print("Content-Length: ");
  client.println(jsonObject.length());
  client.println();
  client.println(jsonObject);
        
  int timeout = 5 * 10;            
  while(!!!client.available() && (timeout-- > 0)){
    delay(100);
  }
  if(!!!client.available()) {
    Serial.println("No response...");
  }
  while(client.available()){
    Serial.write(client.read());
  }
  
  Serial.println("\nclosing connection");
  client.stop(); 
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Temperature: ");
  Serial.println(myData.temperature);
  Serial.print("Humidity: ");
  Serial.println(myData.humidity);
  Serial.print("Pressure: ");
  Serial.println(myData.pressure);
  Serial.println();
  Serial2.print("temp.txt=\"" + String(myData.temperature) + "\"");
Serial2.write(0xFF);
Serial2.write(0xFF);
Serial2.write(0xFF);

Serial2.print("humidity.txt=\"" + String(myData.humidity) + "\"");
Serial2.write(0xFF);
Serial2.write(0xFF);
Serial2.write(0xFF);

Serial2.print("pressure.txt=\"" + String(myData.pressure) + "\"");
Serial2.write(0xFF);
Serial2.write(0xFF);
Serial2.write(0xFF);
unsigned long currentMillis = millis();

Serial.println("Scanned barcode: " + String(myData.barcode));
    Serial2.print("GM67.txt=\"" + String(myData.barcode) + "\"");
    Serial2.write(0xFF);
Serial2.write(0xFF);
Serial2.write(0xFF);
  }


void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  Serial2.begin(9600);
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packet info
  

WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  webserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! I am ESP32.");
  });

  AsyncElegantOTA.begin(&webserver);    // Start ElegantOTA
  webserver.begin();
  Serial.println("HTTP server started");

  initWifi();

}

void loop() {
  unsigned long currentMillis = millis();

String link = ("https://docs.google.com/spreadsheets/d/1N3kxCGWEZqtjcR8O7tVOAubeztbV7kMpSJ_jtvDsXmI/edit?usp=sharing");
  Serial2.print("qr0.txt=\"" + String(link) + "\"");
Serial2.write(0xFF);
Serial2.write(0xFF);
Serial2.write(0xFF);
delay(5000);
  // Handle incoming web requests
  AsyncElegantOTA.loop();

  // Send IFTTT request every 3 seconds
  if (currentMillis - previousIFTTTRequestMillis >= IFTTTRequestInterval) {
    // Save the last time IFTTT request was made
    previousIFTTTRequestMillis = currentMillis;
    makeIFTTTRequest();
    
}
}