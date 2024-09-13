#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <U8g2lib.h>
#include <Wire.h>

#include "config.h"

U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display


const char* ssid = SSID;
const char* password = PASSWORD;
const char* mqtt_server = "192.168.3.26";

WiFiClient espClient;
PubSubClient client(espClient); // MQTT client

void callback(char* topic, byte* payload, unsigned int length) {
  // led1 -> built-in LED 1 0 
  if (strcmp(topic, "led1") == 0) {
    if ((char)payload[0] == '0') {
      digitalWrite(LED_BUILTIN, LOW);
    } else {
      digitalWrite(2, HIGH);
    }
  }

  // text -> oled 1306
  if (strcmp(topic, "text") == 0) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 20, (char*)payload);
    u8g2.sendBuffer();
  }
}

void setup() {
  Serial.begin(115200);

  Wire.begin();
  u8g2.begin();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
}


void reconect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe("led1");
      client.subscribe("text");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}



void loop() {
 
  if (!client.connected()) {
    reconect();
  }
  client.loop();
  
  if(millis()%1000 == 0){
    int value = analogRead(32);
    client.publish("pot", String(value).c_str());
  }

}

