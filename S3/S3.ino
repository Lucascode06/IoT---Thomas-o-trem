#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient client;
PubSubClient mqtt(client);

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

const int PORT = 1883;
const String URL = "test.mosquitto.org";
const String broker_user = "";
const String broker_pass = "";

const String MyTopic = "gui_fischer";
const String OtherTopic = "gu_schi";

const int LED_PIN = 2;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(SSID.c_str(), PASS.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado ao WiFi!");

  mqtt.setServer(URL.c_str(), PORT);
  mqtt.setCallback(callback);
  while (!mqtt.connected()) {
    String ID = "S1-" + String(random(0xffff), HEX);
    if (mqtt.connect(ID.c_str(), broker_user.c_str(), broker_pass.c_str())) {
      Serial.println("\nConectado ao broker!");
      mqtt.subscribe(MyTopic.c_str());
    } else {
      Serial.print(".");
      delay(200);
    }
  }
}

void loop() {
  mqtt.loop();
  if (Serial.available() > 0) {
    String mensagem = "guilherme: " + Serial.readStringUntil('\n');
    mqtt.publish(OtherTopic.c_str(), mensagem.c_str());
  }
  delay(2000);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem = "";
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  Serial.print("Recebido: ");
  Serial.println(mensagem);

  if (mensagem == "LED_ON") {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED aceso!");
  } else if (mensagem == "LED_OFF") {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED apagado!");
  }
}
