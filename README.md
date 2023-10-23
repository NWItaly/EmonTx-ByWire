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
  1. la variabile di compilazione TEST serve per abilitare/disabilitare l'utilizzo della porta seriale a scopo di test e ridurre il peso dello sketch nella versione di produzione.
  1. la variabile di compilazione DEBUG serve per fermare il loop al primo ciclo.
  1. la variabile EMON_Enabled serve per abilitare/disabilitare tutte le funzioni riguardanti l'EmonTx Shield.
  1. la variabile DS18B20_Enabled serve per abilitare/disabilitare tutte le funzioni riguardanti il sensore della temperatura.
  1. Per tutte le stringhe "statiche" utilizzo la funzione F() per ridurre l'utilizzo della memoria.

## Funzionamento:

### Lampeggio led

1. **Led fisso**: Ricezione dati di configurazione.
1. **Lampeggio 1 volta**: Invio dati con successo.
1. **Lampeggio 2 volta**: Ricezione dati con successo.
1. **Lampeggio 3 volte**: Impostazioni server non configurate.
    Dopo 10 tentativi si riavvia per caricare le impstazioni dalla EPROM.
1. **Lampeggio 4 volte**: Errore connessione al server.
1. **Lampeggio 5 volte**: Errore connessione LAN.
    Dopo 10 tentativi si riavvia per cercare di riconnettersi alla LAN.

### Invio configurazione

E' possibile cambiare le informazioni del server utilizzando Telnet.  
I dati modificabili sono:
1. `ip=x.x.x.x`: IP del server (EmonCMS) 
1. `port=x`: La porta su cui inviare i dati (80 default)
1. `key=xxxxxxxxxxxxxxxxxxxxxxxxxxxxx`: API Key (32 caratteri)
1. `node=x`: Numero del nodo (2 default, max 255)
1. `init`: svuota i dati nella EEPROM

Per installare Telnet su Windows 10: `pkgmgr /iu:"TelnetClient"`.  
Da linea di comando aprire un terminale Telnet: `telnet 192.168.x.x` (specificare).  
Da questo momento ogni carattere digitato verrà inviato all'arduino.    Sull'EmonTxShield il led di stato rimarrà accesso fino alla fine alla chiusura del terminale Telnet o fino al timeout che è di **30 secondi**.  
La ricezione dei dati sarà indicata da **2 lampeggi** del led.  
E' possibile modificare **un solo** valore per volta.  
Esempi:  
`ip=192.168.0.2`  
`port=80`  
`node=2`  
`key=abcdefghilmnopqrstuvz`

## Link esterni:

  Arduino: https://www.arduino.cc/  
  OpenEnergyMonitor (Emoncms): https://openenergymonitor.org/
