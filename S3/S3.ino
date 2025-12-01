#include <ESP32Servo.h>
#include <WiFi.h>
#include <PubSubClient.h>


#define TRIG 5
#define ECHO 18
#define SERVO1 13
#define SERVO2 12
#define LED_ILUM 23
#define LED_R 22
#define LED_G 21
#define LED_B 19


Servo servo1;
Servo servo2;


// WiFi
const char* SSID = "FIESC_IOT_EDU";
const char* PASS = "8120gv08";


// MQTT
WiFiClientSecure client;
PubSubClient mqtt(client);


const char* URL = "e6607d8d5fec40a9974cfc1552a13e2f.s1.eu.hivemq.cloud";
const int PORT = 8883;


const char* MQTT_USER = "fischer_s3";
const char* MQTT_PASS = "ED6p<kLo>49:bZ&Fh2Kd";



const char* TOPIC_PRESENCA = "S3/Presenca";
const char* TOPIC_SERVO1 = "S3/Servo1";
const char* TOPIC_SERVO2 = "S3/Servo2";



const char* TOPIC_ILUM = "S1/iluminacao";


// Conecta WiFi
void conectaWiFi() {
  WiFi.begin(SSID, PASS);
  Serial.print("Conectando WiFi");


  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }


  Serial.println("\nWiFi OK!");
}


// Conecta MQTT
void conectaMQTT() {
  mqtt.setServer(URL, PORT);
  mqtt.setCallback(callback);


  Serial.print("Conectando MQTT");


  while (!mqtt.connected()) {
    String id = "S3-" + String(random(9999));


    if (mqtt.connect(id.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println("\nMQTT OK!");


      mqtt.subscribe(TOPIC_SERVO1);
      mqtt.subscribe(TOPIC_SERVO2);
      mqtt.subscribe(TOPIC_ILUM);


    } else {
      Serial.print(".");
      delay(500);
    }
  }
}


// Mede distância
long medirDistancia() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);


  long tempo = pulseIn(ECHO, HIGH);
  long distancia = tempo * 0.034 / 2;


  return distancia;
}


// Callback MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";


  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }


  Serial.print("Recebido ");
  Serial.print(topic);
  Serial.print(" => ");
  Serial.println(msg);


  if (strcmp(topic, TOPIC_SERVO1) == 0) {
    servo1.write(msg.toInt());
  }


  if (strcmp(topic, TOPIC_SERVO2) == 0) {
    servo2.write(msg.toInt());
  }


  if (strcmp(topic, TOPIC_ILUM) == 0) {
    digitalWrite(LED_ILUM, msg == "acender" ? HIGH : LOW);
  }
}


// Função LED RGB
void setarLED(bool r, bool g, bool b) {
  digitalWrite(LED_R, r);
  digitalWrite(LED_G, g);
  digitalWrite(LED_B, b);
}


void setup() {
  Serial.begin(115200);


  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LED_ILUM, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);


  servo1.attach(SERVO1);
  servo2.attach(SERVO2);


  // LED azul: conectando
  setarLED(false, false, true);


  conectaWiFi();
  conectaMQTT();


  // LED verde: pronto
  setarLED(false, true, false);
}


void loop() {
  if (!mqtt.connected()) {
    conectaMQTT();
  }
  mqtt.loop();


  long dist = medirDistancia();
  bool detectou = (dist < 20);


  String status = detectou ? "1" : "0";
  mqtt.publish(TOPIC_PRESENCA, status.c_str());


  Serial.print("Distância: ");
  Serial.print(dist);
  Serial.print(" cm => Presença: ");
  Serial.println(status);


  // LED RGB
  if (detectou) {
    setarLED(true, false, false);  // Vermelho
  } else {
    setarLED(false, true, false);  // Verde
  }


  delay(300);
}
