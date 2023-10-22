#define TEST 0             //1 = Use Serial
#define DEBUG 0            //1 = Only 1 loop
#define EMON_Enabled 1     //1 = Read and send consumption data
#define DS18B20_Enabled 1  //1 = Read and send temperature

#define Reset_AVR() \
  wdt_enable(WDTO_30MS); \
  while (1) {}

#if TEST == 1
#define Sprintln(a) Serial.println(a)
#define Sprint(a) Serial.print(a)
#define SPrint(a, b) Serial.print(a, b)
#define Cprintln(a) \
  Serial.println(a); \
  client.println(a)
#define Cprint(a) \
  Serial.print(a); \
  client.print(a)
#define CPrint(a, b) \
  Serial.print(a, b); \
  client.print(a, b)
#define Wprintln(a) \
  Serial.println(a); \
  server.println(a)
#define Wprint(a) \
  Serial.print(a); \
  server.print(a)
#else
#define Sprintln(a)
#define Sprint(a)
#define SPrint(a, b)
#define Cprintln(a) client.println(a)
#define Cprint(a) client.print(a)
#define CPrint(a, b) client.print(a, b)
#define Wprintln(a) server.println(a)
#define Wprint(a) server.print(a)
#endif

/* TxShield */
const PROGMEM byte txLed = 9;

void Blink(byte errorNumber) {
  for (int index = 0; index < errorNumber; index++) {
    digitalWrite(txLed, HIGH);
    delay(500);
    digitalWrite(txLed, LOW);
    delay(500);
  }
}  //Blink