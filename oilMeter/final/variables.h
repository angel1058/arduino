#ifndef variables
#define variables

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR int wifiFail = 0;
RTC_DATA_ATTR int mqqtFail = 0;
RTC_DATA_ATTR int skipCount= 0;
RTC_DATA_ATTR unsigned long lastRunMicros = 0;
RTC_DATA_ATTR int oldValue = 0;

int sleepy = 0;
const char* ssid = "Phobos"; //Enter SSID
const char* password = "1058Dr460n!"; //Enter Password
const char* mqqtUser = "SONOFF"; //Enter SSID
const char* mqqtBroker = "192.168.1.97";

WiFiClient espClient;

unsigned long startTime;
unsigned long stopTime;

int ReadCount = 10;
int cnt = 0;

#endif
