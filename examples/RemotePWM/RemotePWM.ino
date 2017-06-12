/**
 * Created by Park, Nil on 2017-06-12.
 * Last Revision: 2017-06-12
 *
 * This example reports a button or switch status to Vitcon IOT Server
 *
 * Attach WIFI-LINK to D0 and D1, Config WIFI-LINK access to Vitcon IOT Broker Server.
 * LED-LINK should be attached to D5 (PWM pin)
 *
 * You should install VitconCommon library prior to VitconIOT library.
 * The minimum interval calling BrokerComm.Run() function should be less than 2ms
 */

#include <VitconBrokerComm.h>

using namespace vitcon;

int pwm_value = 0;

/* A set of definition for IOT items */
#define ITEM_COUNT 2
void pwm_out(int32_t val) { pwm_value = (int)val; analogWrite(5, val); }
IOTItemInt pwm_status;
IOTItemInt pwm(pwm_out);
IOTItem *items[ITEM_COUNT] = { &pwm_status, &pwm };
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
  pwm_status.Set(pwm_value);
  comm.Run();
}