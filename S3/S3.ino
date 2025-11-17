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

WiFiClient client;
PubSubClient mqtt(client);



const char* SSID = "FIESC_IOT_EDU";
const char* PASS = "8120gv08";

const char* BROKER = "test.mosquitto.org";
int PORT = 1883;

const char* TOPIC_PRESENCA = "S3/Presenca";    
const char* TOPIC_SERVO1   = "S3/Servo1";      
const char* TOPIC_SERVO2   = "S3/Servo2";      
const char* TOPIC_ILUM     = "S1/Ilum";        



void conectaWiFi() {
  WiFi.begin(SSID, PASS);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nWiFi OK!");
}

void conectaMQTT() {
  mqtt.setServer(BROKER, PORT);
  mqtt.setCallback(callback);

  Serial.print("Conectando MQTT");
  while (!mqtt.connected()) {
    String ID = "S3-" + String(random(9999));
    if (mqtt.connect(ID.c_str())) {
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



long lerDistancia() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duracao = pulseIn(ECHO, HIGH);
  long distancia = duracao * 0.034 / 2;

  return distancia;
}



void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++)
    msg += (char)payload[i];

  Serial.print("Mensagem recebida ");
  Serial.print(topic);
  Serial.print(" => ");
  Serial.println(msg);

  if (String(topic) == TOPIC_SERVO1) {
    int ang = msg.toInt();
    servo1.write(ang);
  }

  if (String(topic) == TOPIC_SERVO2) {
    int ang = msg.toInt();
    servo2.write(ang);
  }


  if (String(topic) == TOPIC_ILUM) {
    if (msg == "1") digitalWrite(LED_ILUM, HIGH);
    else digitalWrite(LED_ILUM, LOW);
  }
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


  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, HIGH);

  conectaWiFi();
  conectaMQTT();

  
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, LOW);
}



void loop() {
  if (!mqtt.connected()) conectaMQTT();
  mqtt.loop();

  long d = lerDistancia();

 
  bool detectou = (d < 20);

  String msg = detectou ? "1" : "0";
  mqtt.publish(TOPIC_PRESENCA, msg.c_str());

  Serial.print("Distância: ");
  Serial.print(d);
  Serial.print(" cm => Presença: ");
  Serial.println(msg);


  if (detectou) {
    digitalWrite(LED_R, HIGH);  
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, LOW);
  } else {
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, HIGH);  
    digitalWrite(LED_B, LOW);
  }

  delay(300);
}
