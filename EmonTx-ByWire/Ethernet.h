#include "Config.h"

const PROGMEM unsigned int serverRequestTimeout = 30;

// Enter a MAC address for your controller below.
byte myMac[] = { 0xA8, 0x24, 0xA4, 0xE4, 0x62, 0x11 };
EthernetServer server(23);  // server to receive configuration
EthernetClient client;      // client to communicate with server

enum typeConfigData {
  unknown,
  ip,
  port,
  apikey,
  node
};

String printIP(IPAddress ip) {
  String result = "";
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    if (thisByte > 0) result += ".";
    result += ip[thisByte];
  }
  return result;
}  //printIP

void InitWebServer() {
  Sprintln(F("Start receive configuration"));
  server.begin();
}  //initWebServer

void ListenWebServer() {

  EthernetClient newClient = server.accept();

  if (newClient) {

    unsigned long lastConnectionTime = millis();

    digitalWrite(txLed, HIGH);
    Sprintln(F("Inizio ricezione: "));

    // typeConfigData typeEditing = unknown;
    bool confirm = false;
    String temp = "";

    while (!confirm && (newClient.available() > 0 || millis() - lastConnectionTime < serverRequestTimeout * 1000)) {
      if (newClient.available() > 0) {
        char thisChar = newClient.read();
        if (thisChar == 13) {
          confirm = true;
        } else if ((thisChar >= 65 && thisChar <= 90) || (thisChar >= 97 && thisChar <= 122) || (thisChar >= 48 && thisChar <= 57) || thisChar == 61 || thisChar == 46) {
          //A-Za-z0-9=.
          temp.concat(thisChar);
        }
      }
    }

    if (temp.startsWith("ip=")) {
      Wprintln(F("Editing IP"));
      temp = temp.substring(3);  //Remove "ip="
      IPAddress ip;

      if (!ip.fromString(temp)) {  // try to parse into the IPAddress
        Sprintln("UnParsable IP");
      } else {
        config.ip1 = ip[0];
        config.ip2 = ip[1];
        config.ip3 = ip[2];
        config.ip4 = ip[3];
        saveConfiguration();
        Sprint(F("New IP: "));
        Sprint(config.ip1);
        Sprint(F("."));
        Sprint(config.ip2);
        Sprint(F("."));
        Sprint(config.ip3);
        Sprint(F("."));
        Sprintln(config.ip4);
      }
    } else if (temp.startsWith("port=")) {
      Wprintln(F("Editing Port"));
      temp = temp.substring(5);
      int port = temp.toInt();
      if (port <= 0) {
        Sprintln("UnParsable Port");
      } else {
        config.port = port;
        saveConfiguration();
        Sprint(F("New port: "));
        Sprintln(config.port);
      }
    } else if (temp.startsWith("key=")) {
      Wprintln(F("Editing API Key"));
      temp = temp.substring(4);
      if (temp.length() != 32) {
        Sprintln(F("API Key not valid"));
      } else {
        Sprint(F("Original: "));
        Sprint(temp);
        Sprintln(F(";"));
        temp.toCharArray(config.apikey, 32 + 1);
        saveConfiguration();
        Sprint(F("New API key: "));
        Sprintln(config.apikey);
      }
    } else if (temp.startsWith("node=")) {
      Wprintln(F("Editing Node"));
      temp = temp.substring(5);
      byte node = temp.toInt();
      if (node <= 0) {
        Sprintln("UnParsable Node");
      } else {
        config.node = node;
        saveConfiguration();
        Sprint(F("New node: "));
        Sprintln(config.node);
      }
    } else if (temp == "init") {
      InitEEPROM();
      Wprintln(F("EEPROM initialized"));
    } else {
      Wprint(F("Not editing: "));
      Wprintln(temp);
    }

    digitalWrite(txLed, LOW);
    Sprintln(F("Fine ricezione!!!"));
    newClient.stop();

    Blink(2);
  }
}  //ListenWebServer

void ConnectionStart() {
  Sprint(F("My IP address: "));
  Sprint(printIP(Ethernet.localIP()));
  Sprintln(F("."));

  InitWebServer();
}  //ConnectionStart

void InitEthernet() {
  // start the Ethernet connection:
  if (Ethernet.begin(myMac) == 0) {
    Sprintln(F("Failed to configure Ethernet using DHCP."));
    // no point in carrying on, so do nothing forevermore:
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Sprintln(F("Ethernet shield was not found.  Sorry, can't run without hardware. :("));
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Sprintln(F("Ethernet cable is not connected."));
    }
  } else {
    ConnectionStart();
  }
}  //initEthernet

//Manage ethernet connection and reconnection
bool EthernetManager() {
  bool result = true;
  switch (Ethernet.maintain())  //Request a renewal from the DHCP server
  {
    case 1:
      //renewed fail
      Sprintln(F("Error: renewed fail"));
      result = false;
      break;

    case 2:
      //renewed success
      Sprintln(F("Renewed success"));
      ConnectionStart();
      break;

    case 3:
      //rebind fail
      Sprintln(F("Error: rebind fail"));
      result = false;
      break;

    case 4:
      //rebind success
      Sprintln(F("Rebind success"));
      ConnectionStart();
      break;

    default:
      //nothing happened
      break;
  }
  return result;
}  //EthernetManager

void BlinkError(String message, byte blink) {
  Sprintln(message);
  client.stop();
  Sprintln(F("Disconnected"));
  Blink(3);
}  //BlinkError