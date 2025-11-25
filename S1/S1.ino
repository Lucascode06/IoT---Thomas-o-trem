#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const byte LDR_PIN = 34;
const byte LED_PIN = 19;

const char* SSID = "FIESC_IOT_EDU";
const char* PASS = "8120gv08";

WiFiClientSecure client;
PubSubClient mqtt(client);

const char* URL = "e6607d8d5fec40a9974cfc1552a13e2f.s1.eu.hivemq.cloud";
const int PORT = 8883;

const char* MQTT_USER = "schmidt_s1";
const char* MQTT_PASS = "ED6p<kLo>49:bZ&Fh2Kd";

const char* TOPICO_ILUMINACAO = "S1/iluminacao";

//RECEBE MENSAGENS
void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem;

  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  Serial.print("Recebido em ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(mensagem);

  if (strcmp(topic, TOPICO_ILUMINACAO) == 0) {
    if (mensagem == "acender") {
      digitalWrite(LED_PIN, HIGH);
    } else if (mensagem == "apagar") {
      digitalWrite(LED_PIN, LOW);
    }
  }
}

// CONECTA MQTT
void conectaMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Conectando ao HiveMQ... ");

    String clientId = "ESP32-S1-";
    clientId += String(random(0xffff), HEX);

    if (mqtt.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println("Conectado!");
      mqtt.subscribe(TOPICO_ILUMINACAO);
    } else {
      Serial.println("Falhou. Tentando novamente...");
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

  Serial.print("Conectando ao Wi-Fi ");
  WiFi.begin(SSID, PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" Conectado!");

  client.setInsecure();
  mqtt.setServer(URL, PORT);
  mqtt.setCallback(callback);

  conectaMQTT();
}

void loop() {
  if (!mqtt.connected()) {
    conectaMQTT();
  }

  mqtt.loop();

  // LER LDR
  int leituraLDR = analogRead(LDR_PIN);
  Serial.print("LDR: ");
  Serial.println(leituraLDR);

  // acender se estiver acima de 3500
  if (leituraLDR > 3500) {
    digitalWrite(LED_PIN, HIGH);
    mqtt.publish(TOPICO_ILUMINACAO, "acender");
  } else {
    digitalWrite(LED_PIN, LOW);
    mqtt.publish(TOPICO_ILUMINACAO, "apagar");
  }

  delay(1000);
}



