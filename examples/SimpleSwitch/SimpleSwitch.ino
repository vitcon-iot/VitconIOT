/**
 * Created by Park, Nil on 2017-04-27.
 * Last Revision: 2017-06-12
 *
 * This example reports a button or switch status to Vitcon IOT Server
 *
 * Attach WIFI-LINK to D0 and D1, Config WIFI-LINK access to Vitcon IOT Broker Server.
 * RELAY-LINK should be attached to D2
 *
 * You should install VitconCommon library prior to VitconIOT library.
 * The minimum interval calling BrokerComm.Run() function should be less than 2ms
 */

#include <VitconBrokerComm.h>

using namespace vitcon;

/* A set of definition for IOT items */
#define ITEM_COUNT 2
void relay_out(bool val) { digitalWrite(2, val); }
IOTItemBin relay_status;
IOTItemBin relay(relay_out);
IOTItem *items[ITEM_COUNT] = { &relay_status, &relay };
/* A set of definition for IOT items */

/* IOT server communication manager */
const char device_id[] = ""; // Change device_id to yours
BrokerComm comm(&Serial, device_id, items, ITEM_COUNT);
/* IOT server communication manager */

void setup() {
  Serial.begin(250000);
  pinMode(2, OUTPUT);
  comm.SetInterval(200);
}

void loop() {
  relay_status.Set(digitalRead(2));
  comm.Run();
}
