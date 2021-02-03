#include <WiFi.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define SENSOR 16
#define TRIGGER 2
#define ECHO 4

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR int successCount = 0;
RTC_DATA_ATTR unsigned long lastRunMicros = 0;
#define uS_TO_S_FACTOR 1000000ULL  
#define TIME_TO_SLEEP  60        


int FULL = 1400;
int sleepy = 0;
const char* ssid = "Phobos"; //Enter SSID
const char* password = "1058Dr460n!"; //Enter Password
const char* mqqtUser = "SONOFF"; //Enter SSID
const char* mqqtBroker = "192.168.1.97";

WiFiClient espClient;

void callBack(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

PubSubClient mqttClient(mqqtBroker , 1883 , callBack , espClient);
unsigned long startTime;
unsigned long stopTime;

void setup() {
  startTime = millis();
  ++bootCount;
  sleepy = TIME_TO_SLEEP;
  if ( bootCount > 10)
    sleepy = 60*60*3;
 
   takeMeasurement();

  lastRunMicros = stopTime - startTime;
}

void publish(int remaining)
{
      
    if (!mqttClient.connect("client" , mqqtUser , password))
    {
      stopTime = millis();
      return;
    }
    char val[8];char val1[8];

    ++successCount;

  String msg= "{\"runTime\": \""+String(lastRunMicros)+"\", \"level\": \""+String(remaining)+"\", \"bootCount\": \""+String(bootCount)+"\", \"failCount\": \""+String(bootCount - successCount)+"\", \"sleepDelay\": \""+String(sleepy)+"\"}";

  mqttClient.publish("esp/oil/liters" , msg.c_str());
  mqttClient.disconnect();
  stopTime = millis();
}


void SetupWifi()
{
  //Serial.println("Connecting..");
    WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
 //   Serial.print(".");
  }

//  Serial.println("");
 // Serial.println("WiFi connection Successful");

}

void powerUpSensor()
{
//  Serial.println("Powering up sensor");
  digitalWrite(SENSOR , HIGH);
}

void powerDownSensor()
{
//  Serial.println("Powering down sensor");
  digitalWrite(SENSOR , LOW);
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
  //Serial.println("Remaining liters : " + String(remainingLitres) );
  return remainingLitres;
}

void takeMeasurement()
{
    Serial.begin(115200);
    Serial.print("!");
  
    pinMode(SENSOR , OUTPUT);
    pinMode(TRIGGER , OUTPUT);
    pinMode(ECHO , INPUT);

    powerUpSensor();
    SetupWifi();
    int distance = readFromSensor();
    powerDownSensor();
  Serial.println("d : " + String(distance) );
    int litresRemaining = calculateLitresRemaining(distance);
  Serial.println("r : " + String(litresRemaining) );

    publish(litresRemaining);
//    Serial.println("#");
}


void loop() {


   
  
 
esp_sleep_enable_timer_wakeup(sleepy * 1000000ULL);
//  Serial.println("Going to sleep for " + String(sleepy) +" seconds : iteration : " + String(bootCount));

  //Go to sleep now
  esp_deep_sleep_start();
  }
