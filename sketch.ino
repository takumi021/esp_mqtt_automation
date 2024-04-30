#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const char* topic_temp = "home/temperature";
const char* topic_light1 = "home/light1/state";
const char* topic_light2 = "home/light2/state";
const char* topic_light3 = "home/light3/state";
const char* topic_light4 = "home/light4/state";
const char* topic_buzzer = "home/buzzer/state";

const int light1Pin = 12;
const int light2Pin = 13;
const int light3Pin = 14;
const int light4Pin = 15;
const int buzzerPin = 5;
const int dhtPin = 4;

#define DHTTYPE DHT11
DHT_Unified dht(dhtPin, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);


void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0'; 
  String strPayload = String((char*)payload);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(strPayload);

  if (strPayload == "ON") {
    if (String(topic)==topic_light1){
      digitalWrite(light1Pin, LOW);
    } else if (String(topic)==topic_light2){
      digitalWrite(light2Pin, LOW);
    } else if (String(topic)==topic_light3){
      digitalWrite(light3Pin, LOW);
    } else if (String(topic)==topic_light4){
      digitalWrite(light4Pin, LOW);
    } else if (String(topic)==topic_buzzer){
      digitalWrite(buzzerPin, LOW);
    }
  } else if (strPayload == "OFF") {
    if (String(topic)==topic_light1){
      digitalWrite(light1Pin, HIGH);
    } else if (String(topic)==topic_light2){
      digitalWrite(light2Pin, HIGH);
    } else if (String(topic)==topic_light3){
      digitalWrite(light3Pin, HIGH);
    } else if (String(topic)==topic_light4){
      digitalWrite(light4Pin, HIGH);
    } else if (String(topic)==topic_buzzer){
      digitalWrite(buzzerPin, HIGH);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT... ");

    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
  
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(topic_light1);
      client.subscribe(topic_light2);
      client.subscribe(topic_light3);
      client.subscribe(topic_light4);
      client.subscribe(topic_buzzer);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  
  pinMode(light1Pin, OUTPUT);
  pinMode(light2Pin, OUTPUT);
  pinMode(light3Pin, OUTPUT);
  pinMode(light4Pin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(dhtPin, INPUT);
  
  dht.begin();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();
  
  // Reading temperature or humidity takes about 250 milliseconds!
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  } else {
    String temperatureString = String(event.temperature);
    client.publish(topic_temp, temperatureString.c_str());
    Serial.print("Published temperature: ");
    Serial.println(event.temperature);
  }
  
  delay(1000);
}
