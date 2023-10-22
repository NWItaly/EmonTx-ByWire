#include <EEPROM.h>
#include <Ethernet.h>
#include "Commons.h"

const PROGMEM int initializedAddress = 0;
const PROGMEM int configAddress = 1;

struct MyConfig {
  //IP server
  byte ip1;
  byte ip2;
  byte ip3;
  byte ip4;

  int port;  //IP Port

  char apikey[33];  //api key cloud

  byte node;  //identificate txShield
};

MyConfig config;

MyConfig GetConfiguration() {
  MyConfig result;  //Variable to store custom object read from EEPROM.

  EEPROM.get(configAddress, result);

  if (result.ip1 == 0) {
    Sprintln(F("No configuration in EEPROM"));
  } else {
    Sprintln(F("Read data from EEPROM: "));
    Sprint(F("IP: "));
    Sprint(result.ip1);
    Sprint(F(":"));
    Sprint(result.ip2);
    Sprint(F(":"));
    Sprint(result.ip3);
    Sprint(F(":"));
    Sprintln(result.ip4);

    Sprint(F("Port: "));
    Sprintln(result.port);
    Sprint(F("Node: "));
    Sprintln(result.node);
    Sprint(F("API key: "));
    if (result.apikey[0] == 0) {
      Sprintln(F("No API key"));
    } else {
      Sprintln(result.apikey);
    }
  }

  return result;
}  //GetConfiguration

void saveConfiguration() {
  EEPROM.update(initializedAddress, 1);
  EEPROM.put(configAddress, config);
}  //saveConfiguration

void InitEEPROM() {

  Sprintln(F("initEEPROM"));
  ///Delete all data
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.update(i, 0);
  }
  EEPROM.update(initializedAddress, 1);  //Initialized

  //Default data
  config = { 192, 168, 0, 0, 80, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 2 };
  EEPROM.put(configAddress, config);

}  //initiEEPROM

void InitConfig() {

  bool initialized = false;
  EEPROM.get(initializedAddress, initialized);

  if (!initialized) {
    InitEEPROM();
  }
  config = GetConfiguration();

}  //initConfig