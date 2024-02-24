/**************************************************************
 * Z21 Ethernet DCC Command Station
 * Copyright (c) 2015-2023 by Philipp Gahtow
***************************************************************
*
* This is a simple dcc command station that receive commands via Ethernet, XpressNet or LocoNet.
* It base on the Z21 ethernet protocol of ROCO

Unterstützte Funktionen/Protokolle:
 * NMRA DCC output (POM and Railcom Cutout with Detector)
 * support functions F0 - F68 and binary instructions
 * Z21 Ethernet over LAN and/or WLAN
 *  - support W5100 or ENC28 LAN Shield
 *  - support WLAN with ESP8266 over Serial
 * S88N feedback, with DCC
 * XpressNet with AUTO switch MASTER-SLAVE-MODE
 * LocoNet T/B with optional Slotserver
 * LocoNet LocoBuffer
 * RailCom Detector
 * separate Programming Rail
 * Display for System Status
 * multi Hardware Support:
 *  - support for ATmega 328p/644/1280/1284/2560 (NANO, UNO, MEGA, Sanguino)
 *  - support for ARM (DUE) (only: DCC, LAN, WLAN, S88)
 *  - support ESP8266 and ESP32
 *  --> a use of all functions/interfaces is only awayable on Arduino MEGA!!!

***************************************************************

- DCC Master Interface with Timer 2 by modifired CmdrArduino library by Philipp Gahtow
- Z21 LAN Protokoll mit W5100/ENC28 Ethernet Shield with z21.h library
- LAN HTTP Website on Port 80 to configure ethernet IP and S88 bus length
- ESP8266 WiFi Z21 LAN Untersützung with z21.h library and HTTP Webite
- fast S88N feedback (with Timer when LocoNet is disable)
- LocoNet at MEGA with Timer 5, normal Timer1 with Loconet.h library and Slotserver for FRED
- XpressNet (RS485) via LOOP-Function with XpressNetMaster.h library
- Relais for seperate program track
- Global Railcom Detector for MEGA on Serial3
- continus DCC for S88N and LocoNet-B
- support external LocoNet Booster over LocoNet-B Line
- use as LocoNet LocoBuffer over USB with JMRI and Rocrail (Arduino MEGA only)

***************************************************************

Softwareversion: */
#define Z21mobileSwVer 498
/*
---------------------------------------------------------------
changes:
15.04.2015  Abschaltung S88 Interface per Define (S88N)
16.04.2015  Aktualisierung Z21 LAN Protokoll V1.05 & Firmware-Version 1.26
17.04.2015  LN OPC_INPUT_REP msg von Belegmeldern über LAN_LOCONET_DETECTOR
20.04.2015  kurze/Lange DCC Adressen (1..99 kurz, ab 100 lang)
22.04.2015  Add in DCC Lib Function support F13 - F28
            Add Power Button with Reset (press at startup)
23.04.2015  Add LocoNet set Railpower (OPC_GPOFF, OPC_GPON, OPC_IDLE)
            Add LocoNet Slot Write (OPC_WR_SL_DATA)
            New Broadcast Msg (8 Bit) Z21 Protokoll V1.05 (Include LocoNet)
            Add LocoNet OPC_RQ_SL_DATA, OPC_UHLI_FUN, OPC_SW_REQ, OPC_SW_REP, OPC_SW_ACK, OPC_SW_STATE
28.04.2015  Add DCC CV Write and Decoder Reset Packet before CV-Programming            
04.07.2015  Add Support Sanguino (ATmega644p and ATmega1284p) =>MCU_config.h
10.07.2015  Change Timer for DCC Interface and S88 to support LocoNet for all MCU
            Add second Booster support (intenal/external)
21.07.2015  S88 max Module define hinzu und S88 HTTP Ausgabe angepasst
30.07.2015  Versionsnummer für Debug definiert
02.08.2015  DCC Accessory Befehl korrigiert
            PowerButton Input geändert von Pin 51 nach Pin 47
03.08.2015  DCC Decoder Funktion korrigiert
17.09.2015  S88 Timer Auswahl (MEGA = Timer3)
18.09.2015  ESP8266 WiFi Support; Z21 LAN über externe Library
23.09.2015  Überarbeitung LAN_LOCONET_DETECTOR
            Neues Kommando OPC_MULTI_SENSE
            DCC Dekoder ohne Timer4!
            Optionale Lok-Event-Informationen im LocoNet (reduzierung der Sendedaten)
03.10.2015  S88 verbessert -> Fehler in der S88 Modulanzahl korrigiert (Überlauf der Zählervariale)       
            LocoNet TX/RX Packetverarbeitung verbessert  
04.10.2015  ROCO EXT Booster Short mit Transistor (invertiert!) 
            Optimierung S88 Timer (Rechenoperationen und Seicherbedarf)              
10.10.2015  Anzeigen Reset Zentrale mittels binkenden LEDs   
13.10.2015  Rückmelder über LocoNet
            Senden von DCC Weichenschaltmeldungen auch über LocoNet         
            LAN Webseite angepasst für Smartphone Display
14.10.2015  Einstellung der Anzahl von S88 Modulen über WiFi
            Verbesserung der Kommunikation mit dem ESP    
04.11.2015  LocoNet Master- oder Slave-Mode auswählbar
19.12.2015  Support kombinierte UDP Paket für WLAN und LAN            
26.12.2015  Add Track-Power-Off after Service Mode 
20.02.2016  Speicherreduzierung wenn kein WLAN und LAN genutzt wird
            LocoNet Client Modus Kommunikation mit IB verbessert
            Extra Serial Debug Option für XpressNet
27.02.2016  Änderung Dekodierung DCC14 und DCC28
            Invertierung Fahrtrichtung DCC Decoder DIRF            
            LocoNet Slave-Mode ignoriere Steuerbefehle, wenn Slot = 0
02.06.2016  Baud für Debug und WiFi einstellbar
            Software Serial für WiFi wählbar (zB. für Arduino UNO)
            -> WiFi Modul Firmware ab v2.5
17.07.2016 Fix Network UDP Antwortport - Sende Pakete an Quellport zurück
25.07.2016 add busy message for XpressNet (MultiMaus update screen now)
Aug.2016   add Railcom Support and update DCCInterfaceMaster and Booster Hardware,
           support POM read over I2C with external MCU (GLOBALDETECTOR)
26.08.2016 add DHCP for Ethernet Shield      
21.11.2016 DCC: fix Railcom - still Problem with Startup: Analog-Power on the rails - Hardware change needed!
26.11.2016 LocoNet: add Uhlenbrock Intellibox-II F13 to F28 support
27.11.2016 LocoNet: fix Speed DIR in OPC_SL_RD_DATA in data byte to 0x80 = B10000000 and OPC_LOCO_DIRF remove invert
27.12.2016 Z21 add CV lesen am Programmiergleis
01.02.2017 add negative DCC output option and seperate this feature from RAILCOM
15.03.2017 fix narrowing conversation inside LNInterface.h
28.03.2017 external Booster active in ServiceMode when no internal Booster
24.04.2017 fix data lost on loconet - s88 timer3 block packets - deactivated
28.04.2017 add MultiMaus support for F13 to F20 without fast flashing
10.05.2017 add XpressNet information for loco speed and function and switch position change
11.05.2017 add internal Booster Short Detection over Current Sense Resistor
25.05.2017 add RailCom Global Reader for Arduino MEGA on Serial3 (POM CV read only)
19.06.2017 fix problems with Arduino UNO compiling
09.07.2017 fix problems when using without XpressNet
23.07.2017 add support for Arduino DUE
26.08.2017 add default speed step setting
09.01.2018 add POM Bit write
21.01.2018 optimize LocoNet Slot system - reduce RAM use
18.08.2018 add support for Arduino DUE XpressNet
02.11.2018 adjust Z21 WiFi data communication and rise up baud rate
22.11.2018 add support for Arduino ESP8266 (WiFi, DCC extern and intern without seperate prog track)
09.06.2019 add extra DCC-Output for S88 and LocoNet without Power-OFF and RailCom
12.04.2020 adjust problems with the Serial communication to ESP8266 WiFi
22.06.2020 remove DUE XpressNet statememts
28.07.2020 change Input Pins VoltInPin and TempPin set only to INPUT Mode.
29.07.2020 add ENC28J60 module - instead of w5100 Shield for MEGA only.
30.07.2020 central startup Railpower sync for all interfaces      
04.08.2020 fix size of data type "RailcomCVAdr"
24.10.2020 fix error inside LocoNetInterface with Master-Mode
29.10.2020 reduce timeout for LAN DHCP challange
30.10.2020 add Z21 Interface config file with template 
04.12.2020 fix LocoNet Client Mode, don't answer on Master commands!
           remove Ethernet LAN DHCP timeout when using UIP Library
05.01.2021 fix WiFi RX problem with long UDP Packet and zero Packets (LAN_LOCONET_DETECTOR)
           fix wrong Adr in LocoNet Slot data response, when doing a DISPATCH Adr > 127
06.01.2021 add new AREF with 1.1 Volt
07.07.2021 fix EEPROM Problem with ESP32 and ESP8266 when doing a commit
08.01.2021 use ESP DNS library only for ESP32 and set a seperate one for ESP8266
02.03.2021 fix Ethernet S88 Module change if DHCP is on
03.03.2021 fix big UDP ethernet receive when package is empty
17.03.2021 fix error with inactiv Debug on HTTP reading S88Module at Debug.print
           change EEPROM storage for ESP
           add firmware to EEPROM and full EEPROM reset when no firmware is found or when POWER pressed on start
           Serialnumber in Z21-APP show now the firmware version
06.04.2021 fix report LocoNet sensor messages to Z21-APP for display
18.05.2021 add LocoNet2 library to support LocoNet on ESP32
20.05.2021 fix some mistakes in LocoNet packet working
v4.92:--------------------------------------------------------------------------------
02.06.2021 fix wrong function notifyPower into notifyRailpower
           fix DCCLed calculation when flashing 
06.06.2021 fix wrong adr in LocoNet OPC_SW_REQ    
10.06.2021 fix power button doesn't change back to csNormal     
16.06.2021 add LocoNet drive direction invert for IntelliBox slave usage (#define LnInvDir)
18.06.2021 fix LocoNet Function 9-28 by Uhlenbrock (0xD4 0x20)
v4.93:--------------------------------------------------------------------------------
21.06.2021 add LAN Interface DHCP change over HTTPCONF
07.07.2021 fix ESP32 DCC Output and Pin config
30.09.2021 fix ethernet to not check port all the time
           fix enc28 ethernet shield bug for boardcast message send out
05.11.2021 add to handel Lok information request central and ask the master (DCC/LocoNet/XpressNet) 
           add new debug flag "REQUEST" for global lok, switch and cv requests
07.11.2021 fix usage without WLAN and LAN problem with Z21 header #defines in LNInterface.h
v4.94:--------------------------------------------------------------------------------
14.11.2021 fix LocoNet from LAN
15.11.2021 fix LocoNet reporting to z21 interface
16.11.2021 fix ethernet update the source port on each request
17.11.2021 add LocoNet Buffer USB connection
           add ACK Sense detection with LM357
v4.95:--------------------------------------------------------------------------------           
18.11.2021 add prio delay to LocoNet send
21.11.2021 fix missing feedback data from XpressNet to LocoNet
30.11.2021 add middle Booster Sense value
14.12.2021 get Booster Sense not when RailCom cutout is on
27.12.2021 add option for OLED Display that shows the configuration data
28.12.2021 fix software number with additional dot
30.12.2021 remove blocking when receiving WLAN data
           add sketch build time for Debug output and Display
06.01.2022 slow down sense reading only for ESP8266 to let WLAN work with ADC too
           add invert option for DCC/Power LED and Service Mode (Prog Relay) output
08.10.2022 fix Short Detection and ACK for WeMos D1 mini with voltage devider
v4.96:--------------------------------------------------------------------------------
13.01.2022 fix config request for WLAN-Interface that report 'OKOKOKOKOK' to DISPLAY -> add some wait
           fix slow S88 by removing the wait timing #define S88CLOCKTIME when using option: no timer (S88noT)
02.02.2022 add XpressNet support for ESP8266 and ESP32       
03.02.2022 fix error "endless loop", when getting loco data as Xpressnet Client
04.02.2022 for ESP MCU switch WLAN client off if connection fail (AP only) - strange errors!
10.02.2022 I2C Display SSD1306 Adresse kann automatisch ermittelt werden: "#define Z21DISPLAY FIND"
           change Slot-Server to 50x loco when using UNO without LAN and WiFi Module
22.02.2022 add I2C Display driver SH110X
23.02.2022 correct the output mA of sense reading for ESP8266 and ESP32, fix Uref for this MCU to 3.3V
v4.97:--------------------------------------------------------------------------------
11.04.2022 fix ESP OTA csTrackVoltageOff was not defined
           if LocoNet is not active S88 use Timer3 on MEGA by default, to have a better S88 timing
12.04.2022 deactivate unsupported interface on ESP8266 by default
13.04.2022 fix problems in CV reading inside DCCInterfaceMaster and display progress on display
           adjust internal short detection
14.04.2022 create display icon for service- ands short circuit mode
15.04.2022 add display icon for all power mode, auto remove config data after 50 sec. when running
16.04.2022 show WLAN Signal connection on display if using ESP8266 
           show send and receive data from LAN/WLAN on display
21.04.2022 fix Display output for AVR, add WLAN Signal from ESP interface         
25.04.2022 fix ESP AP not working, when client wifi is set but not connected/found  
           clean up software structure and remove legacy DCCDecoder.h and #define DECODER
26.04.2022 add Z21 Firmware 1.42 with additional loco function 29..32767 and DDCext
27.04.2022 modify AVR serial communication protokoll with ESP WLAN interface
28.04.2022 fix Display updates and ESP WLAN interface
02.05.2022 remove display RX/TX data transmit
           fix ESP serial wlan data decoding
           add WLANMaus CV read and write
v4.98:--------------------------------------------------------------------------------           
03.05.2022 change mA on display to A
07.05.2022 add XOR check for ESP LocoNet RX Interface
08.05.2022 OTA slow down XpressNet and LocoNet -> WiFi client had no connection, need to switch STA off!
11.05.2022 add at ESP the RSSI on website
18.05.2022 add setCVNackSC for XpressNet
           fix LocoNet Slave Loco Data Request feedback to Z21 and XpressNet
05.07.2022 remove #define SwitchFormat, now use the APP with RCN-213 switch!
07.07.2022 add APP setting for short Adr and repeat >F13
21.07.2022 add INA219 current sensor
23.07.2022 change INA219 settings (10 Bit, gain 320)
25.07.2022 clear up source code remove unused defines
31.07.2023 add pin to detect prog ACK pulse
31.08.2023 fix problem compiling when Ethernet is not used and a Display is on that shows WLAN IPAddress
08.09.2023 add Z21DISPLAY_CONTRAST to have longer OLED Display lifetime
           remove animation on display when programming
15.09.2023 add DCC ACK sense Value for different MCU
------------------------------------
*/
//--------------------------------------------------------------------------------------------
//Prepare Z21PG:
#include "CONFIG.h"     //Load individual Z21 central configuration
#include "Z21Include.h"    //load all Interfaces  #include "main/Z21Include.h"    //load all Interfaces
#include "DCCGlobal.h"  //DCC all Interface function
#include "Z21EEPROM.h"  //EEPROM/Flash constant store
//--------------------------------------------------------------
//--------------------------------------------------------------
//INIT all ports and interfaces:
void setup() {  
  //Reduce the reference Voltage to 1,1 Volt!
  #if defined(AREF_1V1) && !defined(ESP32_MCU) && !defined(ESP8266_MCU)
    analogReference(AREF_1V1); //EXTERNAL or INTERNAL1V1
  #endif  

  pinMode(DCCLed, INPUT);      //DCC Status LED "off" State
  pinMode(ShortLed, OUTPUT);    //Short Status LED
  digitalWrite(ShortLed, HIGH);    //Short LED showes working and Power up
  #if defined(ProgRelaisPin)
    pinMode(ProgRelaisPin, OUTPUT);       //ProgTrack-Relais
    #if defined(PROG_OUT_INVERT) 
      digitalWrite(ProgRelaisPin, HIGH);     //ProgTrack 
    #else
      digitalWrite(ProgRelaisPin, LOW);     //ProgTrack 
    #endif
  #endif  
  #if defined(BOOSTER_EXT)    //Booster (ROCO) external: 
    pinMode(ShortExtPin, INPUT_PULLUP);  //set short pin and Turn on internal Pull-Up Resistor
    pinMode(GoExtPin, OUTPUT);      //GO/STOP Signal
    digitalWrite(GoExtPin, BOOSTER_EXT_OFF);    //set STOP to Booster
  #endif
  #if defined(BOOSTER_INT)    //Booster2 internal:
    #if !defined(BOOSTER_INT_NDCC)
    pinMode(GoIntPin, OUTPUT);    //GO/STOP2 Signal
    digitalWrite(GoIntPin, BOOSTER_INT_OFF);   //set STOP to Booster2 invertet
    #endif
    #if defined(BOOSTER_INT_TLE5206) || defined(BOOSTER_EXT_CDE)
    pinMode(ShortIntPin, INPUT_PULLUP);  //set up short2 PIN and Turn on internal Pull-Up Resistor
    #endif
  #endif
  pinMode(Z21ResetPin, INPUT_PULLUP); //Turn on internal Pull-Up Resistor
  pinMode(Z21ButtonPin, INPUT_PULLUP); //Turn on internal Pull-Up Resistor

  #if defined(DebugBaud)
    Debug.begin(DebugBaud);
    #if defined(ESP8266_MCU) || defined(ESP32_MCU)
      Debug.println("/n");  //Zeilenumbruch einfügen
    #endif
  #endif

  #if defined(Z21DISPLAY)
    DisplaySetup();    //Display init
  #endif

  #if defined(MEGA_MCU)
  //pinMode(VAmSensePin, INPUT_PULLUP);  //AC 5A Sensor (for testing only)
  #if defined(AREF_1V1)
    pinMode(VoltIntPin, INPUT);  //Rail Voltage: Rail:100k - Sense - 4,7k - GND
    #if !defined(DALLASTEMPSENSE)
    pinMode(TempPin, INPUT);     //Temp.Resistor(15k)
    #endif
  #else
    pinMode(VoltIntPin, INPUT_PULLUP);  //Rail Voltage: Rail:100k - Sense - 4,7k - GND
    #if !defined(DALLASTEMPSENSE)
    pinMode(TempPin, INPUT_PULLUP);     //Temp.Resistor(15k)
    #endif
  #endif
  #endif

  #if defined(SOFT_RESET)   //Arduino UNO, MEGA, Sangduino ONLY!
    MCUSR = 0;
    wdt_disable();
  #endif

  #if defined(ESP_WIFI)
    FIXSTORAGE.begin(EESize);  //init EEPROM
  #endif

  #if defined(LOCONET) && (defined(ESP8266_MCU) || defined(ESP32_MCU)) 
    //rewrite Debug Baud rate!
    LocoNetHw_init();
  #endif
  
  #if defined(Z21DISPLAY)
  DisplayBoot(5); //Boot up bar...
  #endif

  //Check Firmware in EEPROM:
  EEPROM_Setup();   //init Z21 EEPROM defaults

  #if defined(ESP_WIFI)
    ESPSetup();   //ESP8266 and ESP32 WLAN Setup
  #endif

  #if defined(DebugBaud)
    Debug.print(F("Z21PG v"));
    Debug.print(String(Z21mobileSwVer).substring(0,1));
    Debug.print(".");
    Debug.print(String(Z21mobileSwVer).substring(1));
    Debug.print("-");
    Debug.print(bitRead(FIXSTORAGE.read(EEPROMRCN213), 2));

    #if defined(UNO_MCU)
    Debug.print(F(" UNO"));
    #elif defined(MEGA_MCU)
    Debug.print(F(" MEGA"));
    #elif defined(SANGUINO_MCU)
    Debug.print(F(" SANGUINO"));
    #elif defined(DUE_MCU)
    Debug.print(F(" DUE"));
    #elif defined(ESP8266_MCU)
    Debug.print(F(" ESP8266"));
    #elif defined(ESP32_MCU)
    Debug.print(F(" ESP32"));
    #endif

    #if defined(DCC)
      Debug.print(".DCC");
    #endif
    #if defined (BOOSTER_INT_NDCC)
      if (FIXSTORAGE.read(EEPROMRailCom) == 0x01)
        Debug.print(".RAILCOM");
    #endif
    #if defined(ACKBOOSTER)
      Debug.print(".ACK_BOOST");
    #endif
    #if defined(AREF_1V1)
      Debug.print(".AREF_1V1");
    #endif
  #endif

  #if defined(Z21DISPLAY)
  DisplayBoot(10);
  #endif

  #if defined(XPRESSNET)  
    #if defined(ESP8266_MCU) || defined(ESP32_MCU)
      #if defined(FS14)
      XpressNet.setup(Loco14, XNetSerialPin, XNetTxRxPin);    //Initialisierung XNet Serial und Send/Receive-PIN  
      #elif defined(FS28)
      XpressNet.setup(Loco28, XNetSerialPin, XNetTxRxPin);    //Initialisierung XNet Serial und Send/Receive-PIN  
      #else
      XpressNet.setup(Loco128, XNetSerialPin, XNetTxRxPin);    //Initialisierung XNet Serial und Send/Receive-PIN  
      #endif
    #else //other AVR MCU:
      #if defined(FS14)
      XpressNet.setup(Loco14, XNetTxRxPin);    //Initialisierung XNet Serial und Send/Receive-PIN  
      #elif defined(FS28)
      XpressNet.setup(Loco28, XNetTxRxPin);    //Initialisierung XNet Serial und Send/Receive-PIN  
      #else
      XpressNet.setup(Loco128, XNetTxRxPin);    //Initialisierung XNet Serial und Send/Receive-PIN  
      #endif
    #endif  
    XpressNet.setPower(Railpower);  //send Railpowerinformation to XpressNet
  #endif

  #if defined(ESP_WIFI)
  delay(10);
  #endif

  #if defined(Z21DISPLAY)
  DisplayBoot(20);
  #endif

  #if defined(INA219)
  Wire.begin();
  Wire.setClock(400000);  //400kHz für die I2C Übertragungsrate 
  #endif
  
  #if defined(INA219)
    if(!ina219.init()){
      #if defined(DebugBaud)
        Debug.println(F("INA219 not found!"));
      #endif
    }
    /* Set ADC Mode for Bus and ShuntVoltage
      * Mode *            * Res / Samples *       * Conversion Time *
      BIT_MODE_9        9 Bit Resolution             84 µs
      BIT_MODE_10       10 Bit Resolution            148 µs  
      BIT_MODE_11       11 Bit Resolution            276 µs
      BIT_MODE_12       12 Bit Resolution            532 µs  (DEFAULT)
      SAMPLE_MODE_2     Mean Value 2 samples         1.06 ms
      SAMPLE_MODE_4     Mean Value 4 samples         2.13 ms
      SAMPLE_MODE_8     Mean Value 8 samples         4.26 ms
      SAMPLE_MODE_16    Mean Value 16 samples        8.51 ms     
      SAMPLE_MODE_32    Mean Value 32 samples        17.02 ms
      SAMPLE_MODE_64    Mean Value 64 samples        34.05 ms
      SAMPLE_MODE_128   Mean Value 128 samples       68.10 ms
      */
    ina219.setADCMode(BIT_MODE_9);    //more speed!
    ina219.setShuntSizeInOhms(senseResist);     //setting shunt value
    ina219.setPGain(PG_320);         //Default setting range (320mV full range, ~16 A max. current, ~4mA resolution with 20 mOhm shunt)
  #endif

  //first read a value to mesure the power on the track
  #if defined(BOOSTER_INT_CURRENT_SHORT_DETECT)
      do {
        VAmpINT = getRailmA();   
        delay(2);
      }
      while (VAmpINT > 65000);  //read until INA219 is started up!
      #if defined(DEBUG)
        Debug.print(F(",mA:"));
        Debug.print(VAmpINT);
      #endif
  #endif
  
  #if defined(DCC)
    //setup the DCC signal:
    #if defined(BOOSTER_INT_NDCC)
      #if defined(FS14)
        #if defined(ADD_ACK_COMP)
          dcc.setup(DCCPin, GoIntPin, DCC14, Railpower, DCC_ACK_SENSE_VALUE, ACKSensePin); 
        #else  
          dcc.setup(DCCPin, GoIntPin, DCC14, Railpower, DCC_ACK_SENSE_VALUE); 
        #endif  
      #elif defined(FS28)
        #if defined(ADD_ACK_COMP)
          dcc.setup(DCCPin, GoIntPin, DCC28, Railpower, DCC_ACK_SENSE_VALUE, ACKSensePin); 
        #else  
          dcc.setup(DCCPin, GoIntPin, DCC28, Railpower, DCC_ACK_SENSE_VALUE); 
        #endif  
      #else
        #if defined(ADD_ACK_COMP)
          dcc.setup(DCCPin, GoIntPin, DCC128, Railpower, DCC_ACK_SENSE_VALUE, ACKSensePin); 
        #else  
          dcc.setup(DCCPin, GoIntPin, DCC128, Railpower, DCC_ACK_SENSE_VALUE);
        #endif  
      #endif
    #else
      #if defined(FS14)    
        #if defined(ADD_ACK_COMP)
          dcc.setup(DCCPin, 0, DCC14, Railpower, DCC_ACK_SENSE_VALUE, ACKSensePin);  //no NDCC and no RAILCOM
        #else  
          dcc.setup(DCCPin, 0, DCC14, Railpower, DCC_ACK_SENSE_VALUE);  //no NDCC and no RAILCOM
        #endif  
      #elif defined(FS28)    
        #if defined(ADD_ACK_COMP)
          dcc.setup(DCCPin, 0, DCC28, Railpower, DCC_ACK_SENSE_VALUE, ACKSensePin);  //no NDCC and no RAILCOM
        #else  
          dcc.setup(DCCPin, 0, DCC28, Railpower, DCC_ACK_SENSE_VALUE);  //no NDCC and no RAILCOM
        #endif  
      #else
        #if defined(ADD_ACK_COMP)
          dcc.setup(DCCPin, 0, DCC128, Railpower, DCC_ACK_SENSE_VALUE, ACKSensePin);  //no NDCC and no RAILCOM
        #else  
          dcc.setup(DCCPin, 0, DCC128, Railpower, DCC_ACK_SENSE_VALUE);  //no NDCC and no RAILCOM
        #endif  
      #endif
    #endif  
    
    //for CV reading over RAILCOM activate i2c communication:
    #if defined(DCCGLOBALDETECTOR)
      #if defined(MEGA_MCU)
      RailComSetup(); //init the Serial interface for receiving RailCom
      #endif
    #endif
    //extra DCC Output for S88 or LocoNet
    #if defined(additionalOutPin)
      #if (!defined(LAN) && defined (UNO_MCU)) || defined(MEGA_MCU) || defined(DUE_MCU) || defined(ESP_WIFI)
        dcc.enable_additional_DCC_output(additionalOutPin);
        #if defined(DebugBaud)
          Debug.print(F(".addOutput"));
        #endif
      #endif
    #endif
  #endif

  #if defined(DebugBaud)
    Debug.println();  //finish config line!
    Debug.print(F("Build: ")); 
    Debug.println(comp_date);
  #endif

  #if defined(Z21DISPLAY)
  DisplayBoot(30);
  #endif

  #if defined(LAN)
  //Want to RESET Ethernet to default IP?
  if ((digitalRead(Z21ButtonPin) == LOW) || (not( (FIXSTORAGE.read(EELANip) == 10) /*Class A */ || (FIXSTORAGE.read(EELANip) == 172) /*Class B */ || (FIXSTORAGE.read(EELANip) == 192)  /*Class C */  ))) {
      
      EEPROM_Load_Defaults();
      
      bool LEDstate = true;
      while (digitalRead(Z21ButtonPin) == LOW) {  //Wait until Button - "UP"
        #if defined(DEBUG)
          Debug.print("."); 
        #endif  
        delay(200);   //Flash:
        if (LEDstate) {
          pinMode(DCCLed, OUTPUT);  
          #if defined(POWER_LED_INVERT) 
            digitalWrite(DCCLed, LOW);
          #else
            digitalWrite(DCCLed, HIGH);
          #endif
          digitalWrite(ShortLed, LEDstate); //ON
          LEDstate = false;
        }
        else {
          pinMode(DCCLed, INPUT);
          digitalWrite(ShortLed, LEDstate);
          LEDstate = true;
        }
      }
      #if defined(DEBUG)
          Debug.println();  //new line!
      #endif  
      pinMode(DCCLed, INPUT); //DCC LED is in "off" State
      digitalWrite(ShortLed, LOW);    //Short LED is in "off" State
  }
  if (FIXSTORAGE.read(EELANDHCP) == 1)
    LAN_DHCP = true;
  LAN_ip[0] = FIXSTORAGE.read(EELANip);
  LAN_ip[1] = FIXSTORAGE.read(EELANip+1);
  LAN_ip[2] = FIXSTORAGE.read(EELANip+2);
  LAN_ip[3] = FIXSTORAGE.read(EELANip+3);
  #endif

  #if defined(S88N)
    SetupS88();    //S88 Setup 
  #endif 

  #if defined(Z21DISPLAY)
  DisplayBoot(40);
  #endif

  #if defined(LOCONET)
    LNsetup();      //LocoNet Interface init
    LNsetpower();   //send Railpowerinformation to LocoNet
  #endif

  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
    z21.setPower(Railpower);  //send Railpowerinformation to Z21 Interface
  #endif

  #if defined(Z21DISPLAY)
  DisplayBoot(50);
  #endif

  #if defined(ESP_WIFI)
    #if defined(ESP_HTTPCONF)
      #if defined(DEBUG)
      Debug.print(F("Server Begin..."));
      #endif
      ESPWebserver.begin(); //Start the HTTP server
      #if defined(DEBUG)
      Debug.println("OK");
      #endif
    #endif
    #if defined(DEBUG)
    Debug.print(F("Start UDP listener..."));
    #endif
    Udp.begin(z21Port);   //open Z21 port
    #if defined(DEBUG)
    Debug.println("OK");
    #endif
    #if defined(ESP32)
    yield();
    #endif
    
    #if defined(ESP_OTA)
      #if defined(DEBUG)
      Debug.print(F("Init OTA..."));
      #endif
      ESP_OTA_Init();
      #if defined(DEBUG)
      Debug.println("OK");
      #endif
    #endif
  #endif

  #if defined(Z21DISPLAY)
  DisplayBoot(60);
  #endif

  #if defined(LAN)
    // start the Ethernet and UDP:
    delay(120); //wait for Ethernet Shield to get up (need longer to boot!)

    #if defined(ENC28)
     /* Disable SD card */
    pinMode(SDSSPIN, OUTPUT);
    digitalWrite(SDSSPIN, HIGH);
    #endif

    Ethernet.init(LANSSPIN);  //SS-Pin Most Arduino shields
    
    if (LAN_DHCP) {
      #if defined(DEBUG)
          Debug.print(F("IP over DHCP..."));  
      #endif 
      #if defined(ENC28)
      if (Ethernet.begin(mac) == 0) { //ENC28 UIP Lib doesn't have timeout option!
      #else 
      if (Ethernet.begin(mac,LANTimeoutDHCP,2000) == 0) { //Trying to get an IP address using DHCP
      #endif  
        #if defined(DEBUG)
          Debug.println(F("fail!"));  
        #endif
        LAN_DHCP = false;   //DHCP not found!
      }
      else {
        //Save IP that receive from DHCP
        LAN_ip = Ethernet.localIP();
        if (LAN_DHCP) {
          //Store IP in EEPROM:
          FIXSTORAGE.FIXMODE(EELANip, LAN_ip[0]);
          FIXSTORAGE.FIXMODE(EELANip+1, LAN_ip[1]);
          FIXSTORAGE.FIXMODE(EELANip+2, LAN_ip[2]);
          FIXSTORAGE.FIXMODE(EELANip+3, LAN_ip[3]);
          #if defined(ESP_WIFI)
          FIXSTORAGE.commit();
          #endif
          #if defined(DEBUG)
          Debug.println("SAVE");  
          #endif
        }
        #if defined(DEBUG)
        else Debug.println("OK");  
        #endif
      }
    }  
    if (!LAN_DHCP) { //kein DHCP:
      // initialize the Ethernet device not using DHCP:
      Ethernet.begin(mac,LAN_ip);  //set IP and MAC  
    }
    Udp.begin(z21Port);  //UDP Z21 Port 21105
    
  //#if defined(Z21MT)
  //UdpMT.begin(34472);   //UDP Maintenance Tool 21106
  //#endif

    #if defined(HTTPCONF)
      server.begin();    //HTTP Server
    #endif
  #endif

  #if defined(Z21DISPLAY)
  DisplayBoot(70);
  #endif

  #if defined(DEBUG)
    #if defined(LAN)
    if (LAN_DHCP)
      Debug.print(F("DHCP "));
    Debug.print(F("Eth IP: "));
    Debug.println(LAN_ip);
    #endif
    #if defined(S88N)
      Debug.print(F("S88: "));
      Debug.println(S88Module);
    #endif
    #if !defined(DUE_MCU) //not for the DUE!
      Debug.print(F("RAM: "));
      Debug.println(freeRam());  
    #endif
  #endif

  #if defined(Z21DISPLAY)
  DisplayBoot(80);
  #endif

  #if defined(DALLASTEMPSENSE) 
  sensors.begin();
  sensors.getAddress(insideThermometer, 0);
  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, DALLAS_RESOLUTION);
  sensors.setWaitForConversion(false);  //kein Warten auf die Umwandlung!

  sensors.requestTemperatures();  //1. Abfrage der Temperatur
  #endif

  #if defined(Z21DISPLAY)
  DisplayBoot(90);
  #endif

  #if defined(WIFI)
  WLANSetup();    //Start ESP WLAN
  #endif 

  #if defined(Z21DISPLAY)
    DisplayBoot(100);
    DisplayReady = true;
    DisplayUpdateRailPower(true);  //Aktualisierung!
    #if defined(BOOSTER_INT_MAINCURRENT)
      notifyz21getSystemInfo(0); //SysInfo an alle BC Clients senden!
    #endif
  #endif

  //init ready:
  globalPower(csNormal);          //Start up the Railpower
  digitalWrite(ShortLed, LOW);    //Short LED goes off - we are ready to work!
}
//--------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//run the state machine to update all interfaces
void loop() {

  updateLedButton();     //DCC Status LED and Button

  #if defined(DCC)
  ShortDetection();  //handel short on rail to => power off
  dcc.update();    //handel Rail Data
  
    #if defined(DCCGLOBALDETECTOR) && defined(MEGA_MCU)
    RailComRead();  //check RailCom Data
    #endif
    
  #endif

  #if (defined(HTTPCONF) && defined(LAN)) || ((defined(ESP8266_MCU) || defined(ESP32_MCU)) && defined(ESP_HTTPCONF))
    Webconfig();    //Webserver for Configuration
  #endif
  
  #if defined(S88N)
    notifyS88Data();    //R-Bus geänderte Daten 1. Melden
  #endif  
  
  #if defined(XPRESSNET)  
    XpressNet.update(); //XpressNet Update
  #endif

  #if defined(LOCONET)
    LNupdate();      //LocoNet update
  #endif

  #if defined(DCC)
    dcc.update();    //handel Rail Data second time!
  #endif
  
  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
    Z21LANreceive();   //Z21 LAN Decoding
  #endif
  /*
  #if defined(LAN) && defined(Z21MT)
    Z21MTreceive();
  #endif
  */
  #if defined(LAN)
     if (LAN_DHCP) {
        Ethernet.maintain(); //renewal of DHCP leases
     }
  #endif  

  #if defined(ESP_OTA)
    ArduinoOTA.handle();
  #endif

}
//--------------------------------------------------------------------------------------------
