/**
 * Created by Park, Nil on 2017-04-27.
 * Last Revision: 2017-06-12
 *
 * This example reports a button or switch status to Vitcon IOT Server
 *
 * Attach WIFI-LINK to D0 and D1, Config WIFI-LINK access to Vitcon IOT Broker Server.
 * NTC-LINK should be attached to A1
 *
 * You should install VitconCommon library prior to VitconIOT library.
 * The minimum interval calling BrokerComm.Run() function should be less than 2ms
 */

#include <VitconBrokerComm.h>
#include <VitconNTCLink.h>

using namespace vitcon;

NTCLink ntc;

/* A set of definition for IOT items */
#define ITEM_COUNT 1
IOTItemFlo temperature;
IOTItem *items[ITEM_COUNT] = { &temperature };
/* A set of definition for IOT items */

/* IOT server communication manager */
const char device_id[] = ""; // Change device_id to yours
BrokerComm comm(&Serial, device_id, items, ITEM_COUNT);
/* IOT server communication manager */

void setup() {
  Serial.begin(250000);
  comm.SetInterval(200);
}

void loop() {
  temperature.Set(ntc.GetTemperature(analogRead(A1)));
  comm.Run();
}
