#include "variables.h"
#include "defines.h"

void powerUp()
{
    #ifdef _DEBUG_SENSOR_
    Serial.print("powering up..." + String(SENSOR));
    #endif
    digitalWrite(SENSOR , HIGH);
    delay(100);
    #ifdef _DEBUG_SENSOR_
    Serial.println("...done");
    #endif
}

void powerDown()
{
    #ifdef _DEBUG_SENSOR_
    Serial.print("powering dowm...");
    #endif
    digitalWrite(SENSOR , LOW);
    #ifdef _DEBUG_SENSOR_
    Serial.println("..done");
    #endif
}

float getDistance()
{

    digitalWrite(TRIGGER, LOW);
    delayMicroseconds(6);
    digitalWrite(TRIGGER, HIGH);
    delayMicroseconds(12);
    digitalWrite(TRIGGER, LOW);
    int duration = pulseIn(ECHO, HIGH );
    float distance = duration * 0.034 / 2;
    
      
    return  distance;
}



int count( const int arr[] , int v)
{
  int c = 0;
  for (int t = 0 ; t < ReadCount ; t++)
  {
    if ( arr[t] == v)
      c++;
  }
  return c;
}

int sensorRead()
{
  #ifdef _DEBUG_SENSOR_
  Serial.println("Reading...");
  #endif
  powerUp();
  int failCount = 0;
  int tmp = ReadCount;
  int values[ReadCount];
  int oldValue;
  while ( tmp > 0)
  {
    int d = 0;
    while ( d ==0)
    {
      d = getDistance();
      if ( ++failCount == 5)
        d = oldValue;
    }
     #ifdef _DEBUG_SENSOR_
  Serial.println("sensorRead:" + String(d));
  #endif
    values[tmp - 1] = d;
    oldValue = d;
    tmp--;
  }

  int max = 0;
  int maxVal = 0;
  for (int t = 0 ; t < ReadCount ; t++)
  {
    int tmpVal = count(values , values[t]);
    if ( tmpVal > max)
    {
      maxVal = values[t];
      max = tmpVal;
    }
  }
  
  powerDown();
  #ifdef _DEBUG_SENSOR_
  Serial.println("Read...");
  #endif
  return maxVal;
}
