#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

int sensorHumedad = 39;
int valvulaPin = 17;
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTTYPE    DHT11
#define DHTPIN 26
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

const char* ssid = "Samsung M31";
const char* password = "05190225";

const char* serverName = "http://192.168.16.7:8080/sistema_de_riego_api/measures/add";

void initializateAP(){
  WiFi.softAP(ssid, password);

  // Obtener dirección IP del punto de acceso (AP)
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

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

void httpPost(double measure, int sensor){
    if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);

    DynamicJsonDocument doc(1024);
    DynamicJsonDocument docSensor(1024);

    docSensor["id"] = sensor;
    
    doc["value"] = measure;
    doc["sensor"] = docSensor;
    
    String json;
    serializeJson(doc, json);


    http.addHeader("Content-Type", "application/json");
    http.addHeader("identification","1044607242");
    
    int httpResponseCode = http.POST(json);
    if (httpResponseCode > 0) {
      if(httpResponseCode == 201){
        digitalWrite(valvulaPin,HIGH);
        delay(5000);
        digitalWrite(valvulaPin,LOW);
      }
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
  //initializateAP();

  dht.begin();
  sensor_t sensorDHT;
  delayMS = sensorDHT.min_delay / 1000;
}
 
void loop() {  
  delay(10000);
  
  sensors_event_t eventDHT;
  
  dht.temperature().getEvent(&eventDHT);
  double temperature = eventDHT.temperature;
  Serial.println("Temperatura: "+String(temperature) + "°C");

  httpPost(temperature,1);
   
  dht.humidity().getEvent(&eventDHT);
  double humidity = eventDHT.relative_humidity;
  Serial.println("Humedad del aire: "+String(humidity) + "%");

  httpPost(humidity,2);
  
  double soilMoisture = map(analogRead(sensorHumedad), 0, 4095, 100, 0);
  Serial.println("Humedad de la tierra: "+String(soilMoisture) + "%");

  httpPost(soilMoisture,3);

  
  Serial.println();
  delay(10000);
}
