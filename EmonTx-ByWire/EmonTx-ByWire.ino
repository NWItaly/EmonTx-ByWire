#include "Config.h"
#include <DallasTemperature.h>
#include <EmonLib.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <avr/pgmspace.h>
#include <SPI.h>
#include <avr/io.h>
#include <avr/wdt.h>
#define Reset_AVR() wdt_enable(WDTO_30MS); while(1) {}

#define TEST 0
#if TEST == 1
#define Sprintln(a) Serial.println(a)
#define Sprint(a) Serial.print(a)
#define Cprintln(a) Serial.println(a); client.println(a)
#define Cprint(a) Serial.print(a); client.print(a)
#else
#define Sprintln(a)
#define Sprint(a)
#define Cprintln(a) client.println(a)
#define Cprint(a) client.print(a)
#endif

/* NET */
// Enter a MAC address for your controller below.
byte myMac[] = { 0xA8, 0x24, 0xA4, 0xE4, 0x62, 0x16 };
EthernetClient client; // client to communicate with server
unsigned long lastConnectionTime = 0; // last time you connected to the server, in milliseconds
byte postingInterval = 10; // delay between updates, in seconds
byte failedAttempts = 0;
const byte retryBeforeReboot = 10;//approximately 10 minutes

#define EMON_Enabled 1
#if EMON_Enabled == 1

EnergyMonitor emon[] = { EnergyMonitor(), EnergyMonitor(), EnergyMonitor(), EnergyMonitor() };
const PROGMEM int emonPinVoltage = 0;
const PROGMEM float myEmonData_calibrationCurrent = 60.606; // 58.18 – 63.02
//const PROGMEM float myEmonData_calibrationVoltage = 268.97; // (230V x 13) / (9V x 1.2) = 276.9 Calibration for UK AC-AC adapter 77DB-06-09
const PROGMEM float myEmonData_calibrationVoltage = 234.26; //  Calibration for EU AC-AC adapter 77DE-06-09
//const float myEmonData_calibrationVoltage = 130.0; //Calibration for US AC-AC adapter 77DA-10-09
const PROGMEM float myEmonData_phaseShiftVoltage = 1.7;
const PROGMEM unsigned int myEmonData_wavelengths = 20;
const PROGMEM unsigned int myEmonData_timeout = 2000;

#endif // EMON_Enabled == 1

/* TxShield */
const byte txPinCurrent[] = { A1, A2, A3, A4 };
const byte txPinCount(sizeof(txPinCurrent) / sizeof(byte));
const byte txLed = 9;

/* Temperature */
#define DS18B20_Enabled 1
#if DS18B20_Enabled == 1
	const byte DS18B20_Pin = 7;// Data wire is plugged into port 4 on the Arduino
	// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
	OneWire oneWire(DS18B20_Pin);
	DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature. 
#endif
byte DS18B20_Resolution = 11; //Precision data read

/* PROGRAMM */
void setup() {

	//  turn on serial (for debugging)
	Serial.begin(9600);
	while (!Serial) {
		; // wait for serial port to connect. Needed for Leonardo only
	}
	
	// Setup indicator LED
	Sprintln(F("Setup LED to inform when it sends data."));
	pinMode(txLed, OUTPUT);
	digitalWrite(txLed, LOW);	
		
	// start the Ethernet connection:
	if (Ethernet.begin(myMac) == 0) {
		Sprintln(F("Failed to configure Ethernet using DHCP."));
		// no point in carrying on, so do nothing forevermore:
		delay(5000);
		//Reboot();
	}
	else {
		ConnectionStart();
	}

#if EMON_Enabled == 1

	// Calculate if there is an ACAC adapter on analog input 0
//	double vrms = calc_rms(0, 1780) * (myEmonData_calibrationVoltage * (3.3 / 1024));
//	Sprint("RMS Voltage on AC-AC  is: ~");
//	Sprint((int)vrms);
//	Sprintln("V");

	Sprint(F("Emon pin: '"));
	Sprint(txPinCount);
	Sprintln(F("'"));
	for (int index = 0; index < txPinCount; index++) {
//		if (vrms > 90) {
//			emon[index].voltage(emonPinVoltage, vrms, myEmonData_phaseShiftVoltage);
//		}
//		else {
			emon[index].voltage(emonPinVoltage, myEmonData_calibrationVoltage, myEmonData_phaseShiftVoltage);
//		}
		emon[index].current(txPinCurrent[index], myEmonData_calibrationCurrent);
		Sprint(F("Configure Emon input: '"));
		Sprint(index);
		Sprintln(F("'"));
	}
#endif
	
	// Start up the dallas/onewire library
#if DS18B20_Enabled == 1
	Sprintln(F("Waiting 6 seconds before search devices DS18B20."));
	delay(6000);
	sensors.begin();
	DeviceAddress tmp_address;
	byte numberOfDevices = sensors.getDeviceCount();
	// locate devices on the bus
	Sprint(F("Locating devices..."));
	Sprint(F("Found "));
	Sprint(numberOfDevices);
	Sprintln(F(" devices."));

	for (int i = 0; i<numberOfDevices; i++)
	{
		sensors.getAddress(tmp_address, i);
		sensors.setResolution(tmp_address, DS18B20_Resolution);
	}
#else
	Sprintln(F("DS18B20 disabled"));
#endif

	Sprintln(F("Setup ends"));
}//setup

void loop() {
	
	// if you're not connected, and at least <postingInterval> seconds have
	// passed sinceyour last connection, then connect again and send data:
	if (EthernetManager() == false) {
		failedAttempts++;
	}
	if (!client.connected() && (millis() - lastConnectionTime > postingInterval * 1000)) {
		
		bool sendData = true;
		digitalWrite(txLed, HIGH);//turn on indicator LED
		sendData = SendData();
		digitalWrite(txLed, LOW);//turn off indicator LED

		if (sendData) {
			// note the time that the connection was made:
			lastConnectionTime = millis();
			failedAttempts = 0;
		}
		else {
			failedAttempts++;//Count how many error happens
		}
				
	}
	else if (client.connected() && (millis() - lastConnectionTime > postingInterval * 1000 *2)) {
		client.stop();
		failedAttempts++;
	}
	
	//Check error and Reboot
	if (failedAttempts >= retryBeforeReboot) {
		//Reboot();
		ErrorBlink();
		Reset_AVR();
	}
	/*Sprintln("stop running for debug");
	while (true);*/
}//loop

//Manage ethernet connection and reconnection
bool EthernetManager() {
	bool result = true;
	switch (Ethernet.maintain())
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
}//EthernetManager

String printIP(IPAddress ip) {
	String result = "";
	for (byte thisByte = 0; thisByte < 4; thisByte++) {
		// print the value of each byte of the IP address:
		if (thisByte > 0) result += ".";
		result += ip[thisByte];		
	}
	return result;
}//printIP

void ConnectionStart() {
	Sprint(F("My IP address: "));
	Sprint(printIP(Ethernet.localIP()));
	Sprintln(F("."));
}//ConnectionStart

bool SendData() {
	bool result = false;
	if (client.connect(myEmonData_serverIP, myEmonData_serverPort)) {
		// if there's a successful connection:
		Sprintln(F("Connecting..."));
		// send the HTTP GET request:
		Cprint("GET /emoncms/input/post.json?apikey=");
		Cprint(myEmonData_apiKey);
		if (myEmonData_node != 255) {
			Cprint("&node=");
			Cprint(myEmonData_node);
		}

		Cprint("&json={");

#if EMON_Enabled == 1
		//Loop for every sensor
		for (int index = 0; index < txPinCount; index++) {

			if (index > 0) {
				Cprint(",");
			}

			emon[index].calcVI(myEmonData_wavelengths, myEmonData_timeout); // Calculate all. No.of wavelengths, time-out
			Cprint("\"realPower");
			Cprint(index); 
			Cprint("\":");
			Cprint(emon[index].realPower);

			Cprint(",\"apparentPower");
			Cprint(index);
			Cprint("\":");
			Cprint(emon[index].apparentPower);

			Cprint(",\"Irms");
			Cprint(index);
			Cprint("\":");
			Cprint(emon[index].Irms);

			Cprint(",\"powerFactor");
			Cprint(index);
			Cprint("\":");
			Cprint(emon[index].powerFactor);

			Cprint(",\"Vrms");
			Cprint(index);
			Cprint("\":");
			Cprint(emon[index].Vrms);
		}
#endif

#if EMON_Enabled == 1 && DS18B20_Enabled == 1
		Cprint(",");
#endif

#if DS18B20_Enabled == 1
		// call sensors.requestTemperatures() to issue a global temperature request to all devices on the bus
		sensors.requestTemperatures();

		DeviceAddress tmp_address;
		int numberOfDevices = sensors.getDeviceCount();
		//Loop for temperature device
		for (int index = 0; index<numberOfDevices; index++)
		{
			
			sensors.getAddress(tmp_address, index);
			float tempC = sensors.getTempC(tmp_address);

			if (tempC < 125.0 && tempC > -55.0) {
				if (index > 0) Cprint(",");
				Cprint("\"Temperature");
				Cprint(index);
				Cprint("\":");
				Cprint(tempC);
			}			
		}
#endif

		Cprintln("} HTTP/1.1");
		Cprintln("Host:emoncms.org");
		Cprintln("User-Agent: Arduino-ethernet");
		Cprintln("Connection: close");
		Cprintln();
		client.flush();

		Sprintln(F("Data sends"));

		// if there's incoming data from the net connection. Send it out the serial port.
		while (client.connected()) {
			if (client.available()) {
				char c = client.read();
				Sprint(c);
			}
		}

		// if there's no net connection through the loop, then stop the client:
		if (!client.connected()) {
			Sprintln();
			Sprintln(F("Disconnecting..."));
			client.stop();
		}

		result = true;
	}
	else {
		// if you couldn't make a connection:
		Sprintln(F("Connection failed"));
		client.stop();
		Sprintln(F("Disconnected"));
		ErrorBlink();
	}
	return result;
}//SendData

void ErrorBlink() {
	for (int index = 0; index < 5; index++) {
		digitalWrite(txLed, HIGH);
		delay(250);
		digitalWrite(txLed, LOW);
		delay(250);
	}
}//ErrorBlink

//double calc_rms(int pin, int samples)
//{
//	unsigned long sum = 0;
//	for (int i = 0; i<samples; i++) // 178 samples takes about 20ms
//	{
//		int raw = (analogRead(0) - 512);
//		sum += (unsigned long)raw * raw;
//	}
//	double rms = sqrt((double)sum / samples);
//	return rms;
//}//calc_rms
