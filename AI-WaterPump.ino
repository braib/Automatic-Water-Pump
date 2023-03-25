#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define motorpin 4

#define sensorpin 34

#define WLAN_SSID       "your_wifi_name"             
#define WLAN_PASS       "your_wifi_password"       

#define AIO_SERVER      "io.adafruit.com" 
#define AIO_SERVERPORT  1883                   

#define AIO_USERNAME  "your_adafruitio_username"
#define AIO_KEY       "your_adafruitio_key"

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish sensorvalue = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/sensor");
Adafruit_MQTT_Publish motorstate = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/motor");

int sensor_value;
int Threshold_value = 700;  // you can change the values as your requirements

void MQTT_connect();

void setup()
{
  Serial.begin(115200);

  pinMode(motorpin, OUTPUT);
  
  Serial.println(); 
  Serial.println();
  Serial.println();
  
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
 
  sensor_value = 0;
}

void loop()
{
  MQTT_connect();

  sensor_value = analogRead(sensorpin);
  
  Serial.print("Moisture = ");
  Serial.println(sensor_value);

  if(!sensorvalue.publish(sensor_value))
  {
    Serial.println(F("Failed"));
  }
  else
  {
    Serial.println(F("Sent!"));
  }

  if(sensor_value >= Threshold_value)
  {
    digitalWrite(motorpin,HIGH);
    motorstate.publish("ON");
    Serial.println("The Motor is : ON");
  }
  else
  {
    digitalWrite(motorpin,LOW);
    motorstate.publish("OFF");
    Serial.println("The Motor is : OFF");
  }
  
  delay(15000);
}



void MQTT_connect()
{
  int8_t again;
  
  if (mqtt.connected())
  {
    return;
  }
  
  Serial.println("Connecting to Adafruit IO");
  
  uint8_t retry = 5;
  
  while ((again = mqtt.connect()) != 0)
  { 
       Serial.println(mqtt.connectErrorString(again));
       Serial.println("Retrying Adafruit connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  
       retry--;
       
       if (retry == 0)
       {
         while (1);
       }

  }
  Serial.println("Adafruit IO is Connected!");
}