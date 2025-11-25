#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClientSecure client;
PubSubClient mqtt(client);

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

const int PORT = 8883;
const String URL = "e6607d8d5fec40a9974cfc1552a13e2f.s1.eu.hivemq.cloud";

const String broker_user = "kauan_trem";
const String broker_pass = "ED6p<kLo>49:bZ&Fh2Kd";

const String MyTopic = "trem";

const int ledVerde = 18;
const int ledVermelho = 21;


void setup() {
  Serial.begin(115200);
  Serial.println("Conectando ao WiFi");
  WiFi.begin(SSID, PASS);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado!");
  client.setInsecure();
  Serial.println("Conectando ao broker...");
  mqtt.setServer(URL.c_str(),PORT);
  while(!mqtt.connected()){
    String ID = "S1-";
    ID += String(random(0xffff),HEX);
    mqtt.connect(ID.c_str(),broker_user.c_str(),broker_pass.c_str());
    delay(200);
    Serial.print(".");
  }
  mqtt.subscribe("trem");
  mqtt.setCallback(callback);
  Serial.println("\n Conectado ao broker com sucesso!");

  pinMode(ledVerde, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
}

void loop() {
  mqtt.loop();
  delay(10);
}

void callback(char* topic, byte* payload, unsigned int length){
  String mensagem = "";
  for(int i = 0; i < length; i++){
    mensagem += (char)payload[i];
  }
  Serial.print("Recebido: ");
  Serial.println(mensagem);

  int msg = mensagem.toInt();

  if (msg > 0){
    digitalWrite(ledVerde, HIGH);
    digitalWrite(ledVermelho, LOW);
  } else if(msg < 0){
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledVermelho, HIGH);
  } else{
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledVermelho, LOW);
  }
}
