# EmonTx-ByWire
**Arduino + EmonTxShield + EthernetShield**

Lo scopo di questo progetto è utilizzare la rete ethernet al posto del Wifi (ESP8266).

## Hardware:
  1. Arduino/Genuino Uno
  2. EmonTx Shield
  3. Ethernet shield
  4. DS18B20 (opzionale)
  
## Software:
  1. Arduino IDE per programmare e caricare il codice sulla scheda.
  2. la variabile di compilazione TEST serve per abilitare/disabilitare l'utilizzo della porta seriale a scopo di test e ridurre il peso dello sketch nella versione di produzione
  3. la variabile EMON_Enabled serve per abilitare/disabilitare tutte le funzioni riguardanti l'EmonTx Shield.
  4. la variabile DS18B20_Enabled serve per abilitare/disabilitare tutte le funzioni riguardanti il sensore della temperatura.
  5. Per tutte le stringhe "statiche" utilizzo la funzione F() per ridurre l'utilizzo della memoria.
  
## Link esterni:
  Arduino: https://www.arduino.cc/ 
  OpenEnergyMonitor (Emoncms): https://openenergymonitor.org/
