#include <WiFi.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define SENSOR 16
#define TRIGGER 2
#define ECHO 4

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR int successCount = 0;

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

void setup() {


 //do the read here...
  takeMeasurement();

}

void publish(int remaining)
{
    
  Serial.println("Connecting to MQTT attempt "  + String(bootCount) );
    if (!mqttClient.connect("client" , mqqtUser , password))
      Serial.println("failed");
      else
      {
  Serial.println("connected");
//  else
    char val[8];char val1[8];

//itoa(remaining, val , 10);
itoa(++successCount, val , 10);
Serial.println("Success Count : " + String(successCount) + " - BootCount " + String(bootCount));
mqttClient.publish("esp/oil/liters" , val);

itoa(sleepy, val1 , 10);
mqttClient.publish("esp/oil/sleep" , val1);

itoa(bootCount, val , 10);
mqttClient.publish("esp/oil/bootCount" , val);

itoa(bootCount - successCount, val , 10);
mqttClient.publish("esp/oil/failCount" , val);

//Serial.println("published");
mqttClient.flush();
//Serial.println("flushed");
mqttClient.disconnect();
//Serial.println("disconnected");
      }
}


void SetupWifi()
{
  //Serial.println("Connecting..");
    WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }

//  Serial.println("");
  Serial.println("WiFi connection Successful");

}

void powerUpSensor()
{
//  Serial.println("Powering up sensor");
  digitalWrite(SENSOR , HIGH);
  delay(100);
}

void powerDownSensor()
{
//  Serial.println("Powering down sensor");
  digitalWrite(SENSOR , LOW);
  delay(100);
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
//  Serial.println(distance);

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

    SetupWifi();
    powerUpSensor();
    int distance = readFromSensor();
    int litresRemaining = calculateLitresRemaining(distance);
    powerDownSensor();
    publish(litresRemaining);
    Serial.println("#");
}

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */


void loop() {

++bootCount;
sleepy = TIME_TO_SLEEP;
if ( bootCount > 10)
  sleepy *= 10;
else if ( bootCount > 100)
  sleepy *= 100;
 else if ( bootCount > 1000)
   sleepy *= 1000;
   
  
 
esp_sleep_enable_timer_wakeup(sleepy * 1000000ULL);
  Serial.println("Going to sleep for " + String(sleepy) +" seconds : iteration : " + String(bootCount));

  //Go to sleep now
  esp_deep_sleep_start();
  }
