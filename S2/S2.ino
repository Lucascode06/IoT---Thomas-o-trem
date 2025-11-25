#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClientSecure client;
PubSubClient mqtt(client);

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

const int PORT = 8883;
const String URL = "e6607d8d5fec40a9974cfc1552a13e2f.s1.eu.hivemq.cloud";

const String broker_user = "lucas_s2";
const String broker_pass = "ED6p<kLo>49:bZ&Fh2Kd";

const String p1 = "Presenca1";
const String p2 = "Presenca2";
const String ilum = "ilum";

const int presenca1_ECHO = 19;
const int presenca1_TRIGG = 18;
const int presenca2_ECHO = 27;
const int presenca2_TRIGG = 26;


const int LED = 14;


void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem = "";
  
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }
  
  if (strcmp(topic, ilum.c_str()) == 0) {
    if (mensagem == "Iluminação Baixa!") {
      digitalWrite(LED, HIGH);
    } else if (mensagem == "Iluminação Adequada!") {
      digitalWrite(LED, LOW);
    } else {
      Serial.print("Erro: ");
      Serial.println(mensagem);
    }
  }
}

// Mede distância presença 1
long lerDistanciap1() {
  digitalWrite(presenca1_TRIGG, LOW);
  delayMicroseconds(2);
  digitalWrite(presenca1_TRIGG, HIGH);
  delayMicroseconds(10);
  digitalWrite(presenca1_TRIGG, LOW);
  
  long duracao = pulseIn(presenca1_ECHO, HIGH);
  long distanciap1 = duracao * 349.24 / 2 / 10000;
  
  return distanciap1;
}

// Mede distância presença 2
long lerDistanciap2() {
  digitalWrite(presenca2_TRIGG, LOW);
  delayMicroseconds(2);
  digitalWrite(presenca2_TRIGG, HIGH);
  delayMicroseconds(10);
  digitalWrite(presenca2_TRIGG, LOW);
  
  long duracao = pulseIn(presenca2_ECHO, HIGH);
  long distanciap2 = duracao * 349.24 / 2 / 10000;
  
  return distanciap2;
}

void setup() {
  Serial.begin(115200);
  

  pinMode(presenca1_TRIGG, OUTPUT);
  pinMode(presenca1_ECHO, INPUT);
  pinMode(presenca2_TRIGG, OUTPUT);
  pinMode(presenca2_ECHO, INPUT);
  pinMode(LED, OUTPUT);
  
  // Conecta WiFi
  Serial.println("Conectando ao WiFi");
  WiFi.begin(SSID, PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  
  Serial.println("\nConectado!");
  Serial.println("IP:");
  Serial.print(WiFi.localIP());
  
  // Configura MQTT
  client.setInsecure();
  Serial.println("\nConectando ao broker");
  mqtt.setServer(URL.c_str(), PORT);
  
  while (!mqtt.connected()) {
    String ID = "S2-";
    ID += String(random(0xffff), HEX);
    mqtt.connect(ID.c_str(), broker_user.c_str(), broker_pass.c_str());
    delay(200);
    Serial.print(".");
  }
  
  mqtt.subscribe(p1.c_str());
  mqtt.subscribe(p2.c_str());
  mqtt.unsubscribe("#");
  mqtt.subscribe(ilum.c_str());
  mqtt.setCallback(callback);
  
  Serial.println("\nConectado ao broker com sucesso!");
}

void loop() {
  // Verifica WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conectando ao WiFi");
    WiFi.begin(SSID, PASS);
    
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(200);
    }
    
    Serial.println("\nConectado!");
    Serial.println("IP:");
    Serial.print(WiFi.localIP());
  }
  
  // Lê sensores
  long distanciap1 = lerDistanciap1();
  long distanciap2 = lerDistanciap2();
  
  Serial.printf("Distancia 1: %d| Distancia 2: %d\n", distanciap1, distanciap2);
  
  // Publica se detectou presença
  if (distanciap1 <= 10) {
    Serial.println("Publicado p1");
    mqtt.publish(p1.c_str(), "p1_Estado_01");
  }
  
  if (distanciap2 <= 10) {
    Serial.println("Publicado p2");
    mqtt.publish(p2.c_str(), "p2_Estado_01");
  }
  
  mqtt.loop();
  delay(500);
}

