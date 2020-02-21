#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include <Arduino.h>
#include <analogWrite.h>

//WIFI 
const char* ssid = "Redmi";
const char* password = "123123123";
const char* mqttServer = "192.168.43.95";
const int mqttPort = 1883;
 
WiFiClient espClient;
PubSubClient client(espClient);

StaticJsonDocument<512> doc;

//LED STRIP
// Red, green, and blue pins for PWM control
const int redPin = 13;     // 13 corresponds to GPIO13
const int greenPin = 12;   // 12 corresponds to GPIO12
const int bluePin = 14;    // 14 corresponds to GPIO14

// Setting PWM frequency, channels and bit resolution
const int freq = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
// Bit resolution 2^8 = 256
const int resolution = 8;

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  //char JSONMessage[] = " {\"SensorType\": \"Temperature\", \"Value\": 10}"; //Original message
    
  //StaticJsonBuffer<300> JSONBuffer;
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
   DeserializationError error =  deserializeJson(doc, payload, length);
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  // Fetch values.
  //
  // Most of the time, you can rely on the implicit casts.
  // In other case, you can do doc["time"].as<long>();
  float hum = doc["hum"];
  Serial.println();
  Serial.print("hum: ");
  Serial.println(hum);
  Serial.println();
  
  if (hum <= 86 ){
    //azul
    ledcWrite(redChannel, 0);
    ledcWrite(greenChannel, 0);
    ledcWrite(blueChannel, 255);
  }
  else if (hum >= 87 && hum <= 88  ){
    //azulado
    ledcWrite(redChannel, 20);
    ledcWrite(greenChannel, 200);
    ledcWrite(blueChannel, 255);
  }
  else if (hum >= 89 && hum <= 90  ){
    //verde
    ledcWrite(redChannel, 0);
    ledcWrite(greenChannel, 255);
    ledcWrite(blueChannel, 0);
  }
  else if (hum >= 91 && hum <= 92  ){
    //amarillo
    ledcWrite(redChannel, 255);
    ledcWrite(greenChannel, 255);
    ledcWrite(blueChannel, 0);
  } 
  else{
    //rojo
    ledcWrite(redChannel, 255);
    ledcWrite(greenChannel, 0);
    ledcWrite(blueChannel, 0);
  }
  Serial.println("-----------------------");
 
}
 
void setup() {
 
  Serial.begin(115200);
  //LED STRIP
  // put your setup code here, to run once:
  // configure LED PWM functionalitites
  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);
  
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP32Client")) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  client.subscribe("/prueba/measure");
 
}
 
void loop() {
  client.loop();
}
