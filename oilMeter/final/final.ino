#include <WiFi.h>
#include <Ethernet.h>
#include <PubSubClient.h>


#include "variables.h"
#include "defines.h"
#include "sensor.h"
#include "debug.h"
void callBack(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}


PubSubClient mqttClient(mqqtBroker , 1883 , callBack , espClient);

void setup() 
{
  Serial.begin(115200);
  log("start time : " + String(startTime));
  startTime = millis();
  log("start time : " + String(startTime));
  ++bootCount;
Serial.println("=================================");
Serial.println("bootCount:" + String(bootCount));
Serial.println("wifiFail:" + String(wifiFail));
Serial.println("mqqtFail:" + String(mqqtFail));
Serial.println("skipCount:"+ String(skipCount));
Serial.println("lastRunMicros:" + String(lastRunMicros));
Serial.println("oldValue:" + String(oldValue));
Serial.println("=================================");
  
  sleepy = TIME_TO_SLEEP;

  if ( bootCount > QUICK_SHOT)
    sleepy = ONE_HOUR;
 
  if ( !takeMeasurement())
  {
  sleepy = TIME_TO_SLEEP;
  }
  stopTime = millis();
  lastRunMicros = stopTime - startTime;
  log("stopTime time : " + String(stopTime));
  log("elapsed : " + String(lastRunMicros));
}

bool publish(int remaining)
{
    if (!SetupWifi())
    {
      log("FAILED TO SETUP WIFI");
      wifiFail++;
      return false;
    }

    if (!mqttClient.connect("client" , mqqtUser , password))
    {
      log("FAILED TO OPEN MQ");
      mqqtFail++;
      return false;
    }
   else
   {
    log("MQ open");
   }

    String msg= "{\"wifiFail\": \""+String(wifiFail)+"\", "+
                  "\"mqqtFail\": \""+String(mqqtFail)+"\", "+
                  "\"skipCount\": \""+String(skipCount)+"\", "+
                  "\"runTime\": \""+String(lastRunMicros)+"\", "+
                  "\"level\": \""+String(remaining)+"\", "+
                  "\"bootCount\": \""+String(bootCount)+"\", "+
                  "\"sleepDelay\": \""+String(sleepy)+"\"}";
log(msg);
    mqttClient.publish("esp/oil/liters" , msg.c_str()  , true);
    mqttClient.disconnect();
    skipCount = 0;
    return true;
 }
 
bool SetupWifi()
{
  //Serial.println("Connecting..");
    WiFi.begin(ssid, password);

  int failedCount = 0;
  
  while (WiFi.status() != WL_CONNECTED)
  {
    failedCount++;
    delay(500);

    if ( failedCount > 10)
    {

      return false;
    }
  }

//  Serial.println("");
 // Serial.println("WiFi connection Successful");
return true;
}


int calculateLitresRemaining(int level)
{
  int used = ( level * 61 * 185 ) / 1000;
  int remainingLitres = FULL - used;
  //Serial.println("Remaining liters : " + String(remainingLitres) );
  return remainingLitres;
}

void setupPins()
{
      pinMode(SENSOR , OUTPUT);
    pinMode(TRIGGER , OUTPUT);
    pinMode(ECHO , INPUT_PULLUP);
}

bool takeMeasurement()
{
  setupPins();
    

int zeroReadTime = 0;
 int distance = 0;
      while ( distance == 0)
      {
        distance = sensorRead();
        if( distance == 0 )
        {
          if ( zeroReadTime++ >=5)
            return false;
      
            log("ZERO DISTANCE");
       
        }
      }
   
    int litresRemaining = calculateLitresRemaining(distance);

    log("d : " + String(distance) );
    log("r : " + String(litresRemaining) );
    log("old : " + String(oldValue) );



    if ( ++skipCount == 5 || litresRemaining != oldValue || 1==1)
    {
        log("Sending new value");
        oldValue = litresRemaining;
        return publish(litresRemaining);
    }
    else
    {

     log("value not changed : " + String(skipCount));

      return true;
    }
}







void loop() {
  esp_sleep_enable_timer_wakeup(sleepy * 1000000ULL);
  #ifdef _DEBUG_
  Serial.println("Going to sleep for " + String(sleepy) +" seconds : iteration : " + String(bootCount));
  #endif
  //Go to sleep now
  esp_deep_sleep_start();
  }
