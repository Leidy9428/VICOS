/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "Casilimas";
const char* password = "ABUELITO9428";
const char* mqtt_server = "192.168.0.20";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// SENSOR DE TEMP Y HUM DHT11
#include <DHT11.h>
int pin=4; //PIN D2
DHT11 dht11(pin); 


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("/prueba/measure", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  int err;
  float temp, humi;
  char stempC [10];
  char humiC [10];
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    
    if((err=dht11.read(humi, temp))==0)
    {
      Serial.print("temperature:");
      Serial.print(temp);
      Serial.print(" humidity:");
      Serial.print(humi);
      Serial.println(); 
      //++value;
      //String data = (String("{") + String("\"id_device\":") + String(value)+ String(",\"temp\":") + String(value+2)+ String(", \"hum\": ") + String(value+3)+ String("}"));
      //char data = (String("{") + String("\"id_device\":") + String(1)+ String(",\"temp\":") + String(temp)+ String(", \"hum\": ") + String(humi)+ String("}"));
      
      //snprintf (msg, 50, data);

      dtostrf(temp, 4, 2, stempC); //number, width, decimal places, buffer
      dtostrf(humi, 4, 2, humiC); //number, width, decimal places, buffer

      Serial.println(stempC);
      Serial.println(humiC);
      
      snprintf(msg, 50, "{\"id_device\":%d,\"temp\":%s,\"hum\":%s}" , 1, stempC, humiC);
      //snprintf(msg, 50, "{\"id_device\":%d,\"temp\":%d,\"hum\":%d}" , 1, temp, humi);
      //snprintf (msg, 50, data);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("/prueba/measure", msg);
    }
    else
    {
      Serial.println();
      Serial.print("Error No :");
      Serial.print(err);
      Serial.println();    
    }
   
  }
}
