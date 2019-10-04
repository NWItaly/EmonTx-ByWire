#include <Ethernet.h>

/* OPEN ENERGY MONITOR */
const IPAddress myEmonData_serverIP(x, x, x, x); //IP server
const PROGMEM unsigned int myEmonData_serverPort = 80; //IP Port
const String myEmonData_apiKey = "<your api key>";//api key cloud
const PROGMEM byte myEmonData_node = 2;//identificate txShield
