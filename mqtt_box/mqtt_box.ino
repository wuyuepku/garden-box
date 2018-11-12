#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID       "network"
#define WLAN_PASS       "password"

#define MQTT_SERVER      "wuyue98.cn"
#define MQTT_SERVERPORT  1883
#define MQTT_USERNAME    "username"
#define MQTT_KEY         "password"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_USERNAME, MQTT_KEY);
Adafruit_MQTT_Subscribe light = Adafruit_MQTT_Subscribe(&mqtt, "test/light", MQTT_QOS_0);

void lightcallback(char *data, uint16_t len) {
    Serial.print("Hey we're in a light callback, the value is: ");
    Serial.println(data);
}

void setup() {
    Serial.begin(115200);
    delay(10);
    Serial.println("MQTT demo");
    
    Serial.print("Connecting to "); Serial.println(WLAN_SSID);
    WiFi.begin(WLAN_SSID, WLAN_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    
    Serial.println("WiFi connected");
    Serial.print("IP address: "); Serial.println(WiFi.localIP());
    
    light.setCallback(lightcallback);
    mqtt.subscribe(&light);
}

void loop() {
    MQTT_connect();
    mqtt.processPackets(10000);
    if(!mqtt.ping()) {  // ping the server to keep the mqtt connection alive
        mqtt.disconnect();
    }
}

void MQTT_connect() {
    int8_t ret;
    
    if (mqtt.connected()) { return; }
    Serial.print("Connecting to MQTT... ");
    
    uint8_t retries = 3;
    while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
        Serial.println(mqtt.connectErrorString(ret));
        Serial.println("Retrying MQTT connection in 10 seconds...");
        mqtt.disconnect();
        delay(10000);  // wait 10 seconds
        retries--;
        if (retries == 0) {
            // basically die and wait for WDT to reset me
            while (1);
        }
    }
    
    Serial.println("MQTT Connected!");
}
