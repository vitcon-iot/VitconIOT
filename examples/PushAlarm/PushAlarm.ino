/**
 * Created by Park, Nil on 2017-06-30.
 * Last Revision: 2017-07-05
 *
 * This example creates a push alarm to user smartphone.
 *
 * Attach WIFI-LINK to D0 and D1, Config WIFI-LINK access to Vitcon IOT Broker Server.
 * NTC-LINK should be attached to A1
 * TACT-LINK should be attached to D8, D9
 *
 * You should install VitconCommon and VitconLink library prior to VitconIOT library.
 * The minimum interval calling BrokerComm.Run() function should be less than 2ms
 */

#include <VitconBrokerComm.h>
#include <VitconNTC.h>

using namespace vitcon;

VitconNTC ntc;

/* A set of definition for IOT items */
#define ITEM_COUNT 2
IOTItemPsh push;
IOTItemFlo temperature;
IOTItem *items[ITEM_COUNT] = { &temperature, &push };
/* A set of definition for IOT items */

/* IOT server communication manager */
const char device_id[] = ""; // Change device_id to yours
BrokerComm comm(&Serial, device_id, items, ITEM_COUNT);
/* IOT server communication manager */

#define PIN_LED   13

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(250000);
  comm.SetInterval(200);
}

bool overtemp = false;
bool btnpushed = false;
void loop() {
  float temp = ntc.GetTemperature(analogRead(A1));
  if (!overtemp && temp >= 30.0) // Enter over-temperature condition. Set push alarm once.
  {
    push.Set("Temperature exceeded 30 celsius degree.");
    overtemp = true;
    digitalWrite(PIN_LED, HIGH);
  }
  if (overtemp && temp <= 29.0) // Exit over-temperature condition.
  {
    overtemp = false;
    digitalWrite(PIN_LED, LOW);
  }
  
  bool b = digitalRead(8);
  if( !btnpushed && b ) // If D8 port is HIGH, Set push alarm once.
  {
    push.Set("D8 button pushed");
    btnpushed = true;
  }
  if( btnpushed && !b)
  {
    btnpushed = false;
  }
  temperature.Set(temp);
  comm.Run();
}
