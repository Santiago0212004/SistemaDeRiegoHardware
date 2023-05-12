#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

int sensorHumedad = 4;
int valvulaPin = 17;
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTTYPE    DHT11
#define DHTPIN 26
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

const char* ssid = "BARSING";
const char* password = "05190225";

const char* serverName = "http://tu_servidor/mediciones";



void connectWifi(){
    WiFi.begin(ssid, password);

    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print("Connecting to ");
      Serial.println(ssid);
    }
    
    Serial.println("-------------------------------");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.println("-------------------------------");
}

void hhtpPost(){
    if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);

    // Build JSON object with measurement data
    DynamicJsonDocument doc(1024);
    //doc["temperature"] = temperature;
    //doc["airHumidity"] = airHumidity;
    //doc["soilHumidity"] = soilHumidity;
    String json;
    serializeJson(doc, json);

    // Send HTTP POST request
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(json);
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error in HTTP request: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
}

void setup() {
  pinMode(valvulaPin,OUTPUT);
  Serial.begin(115200);

  delay(1000);
  
  connectWifi();

  dht.begin();
  sensor_t sensorDHT;
  delayMS = sensorDHT.min_delay / 1000;
}
 
void loop() {  
  delay(2000);

  digitalWrite(valvulaPin,HIGH);
  sensors_event_t eventDHT;
  
  dht.temperature().getEvent(&eventDHT);
  Serial.println("Temperatura: "+String(eventDHT.temperature) + "°C");
  
  dht.humidity().getEvent(&eventDHT);
  Serial.println("Humedad del aire: "+String(eventDHT.relative_humidity) + "%");

  
  int humedadSuelo = map(analogRead(sensorHumedad), 0, 4095, 100, 0);
  Serial.println("Humedad de la tierra: "+String(humedadSuelo) + "%");

  digitalWrite(valvulaPin,LOW);
  
  Serial.println();
}
