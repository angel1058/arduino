#include <WiFi.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define SENSOR 16
#define TRIGGER 2
#define ECHO 4

RTC_DATA_ATTR int bootCount = 0;

int FULL = 1400;

const char* ssid = "Phobos"; //Enter SSID
const char* password = "1058Dr460n!"; //Enter Password
const char* mqqtUser = "SONOFF"; //Enter SSID
const char* mqqtBroker = "192.168.1.97";

WiFiClient espClient;

void callBack(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

PubSubClient mqttClient(mqqtBroker , 1883 , callBack , espClient);

void setup() {

  Serial.begin(9600);
  
  pinMode(SENSOR , OUTPUT);
  pinMode(TRIGGER , OUTPUT);
  pinMode(ECHO , INPUT);

SetupWifi();
 //do the read here...
  takeMeasurement();

}

void publish(int remaining)
{
    
  Serial.println("Connecting to MQTT");
    if (mqttClient.connect("client" , mqqtUser , password))
  Serial.println("connected");
  else
    Serial.println("failed");
    char val[8];

itoa(remaining, val , 10);

mqttClient.publish("esp/oil/liters" , val);
Serial.println("published");
//client.flush();
Serial.println("flushed");
mqttClient.disconnect();
Serial.println("disconnected");

}


void SetupWifi()
{
  Serial.println("Connecting..");
    WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connection Successful");

}

void powerUpSensor()
{
  Serial.println("Powering up sensor");
  digitalWrite(SENSOR , HIGH);
  delay(1000);
}

void powerDownSensor()
{
  Serial.println("Powering down sensor");
  digitalWrite(SENSOR , LOW);
  delay(1000);
}

int readFromSensor()
{
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(20);
  digitalWrite(TRIGGER, LOW);
  long duration = pulseIn(ECHO, HIGH );
  //Calculate the dwqwistance:
  int distance = duration * 0.034 / 2;
  Serial.println(distance);

  return distance;
}

int calculateLitresRemaining(int level)
{
  int used = ( level * 61 * 185 ) / 1000;
  int remainingLitres = FULL - used;
  Serial.println("Remaining liters : " + String(remainingLitres) );
  return remainingLitres;
}

void takeMeasurement()
{
    powerUpSensor();
    int distance = readFromSensor();
    int litresRemaining = calculateLitresRemaining(distance);
    powerDownSensor();
    publish(litresRemaining);
}

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10800        /* Time ESP32 will go to sleep (in seconds) */


void loop() {

++bootCount;
esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * 1000000ULL);
  Serial.println("Going to sleep for " + String(TIME_TO_SLEEP) +" seconds : iteration : " + String(bootCount));

  //Go to sleep now
  esp_deep_sleep_start();
  }
