//--------------------------------------------------------------
/*
 * Setup up PIN-Configuration for different MCU
 * 
 * Support for:
 *    - Arduino DUE
 *    - ESP8266
 *    - ESP32
 *    - Arduino UNO
 *    - Arduino MEGA  
 *    - Sanguino (ATmgega 644p & ATmega 1284p)
 * 
 * Copyright (c) by Philipp Gahtow, year 2022
*/

#if defined(__SAM3X8E__)    //SAM ARM Adruino DUE
#define DUE_MCU
#undef Z21VIRTUAL
#undef XPRESSNET

#elif defined(ESP8266) //ESP8266 or WeMos D1 mini
#define ESP8266_MCU
#define ESP_WIFI
#define ESP_OTA
#undef LnBufferUSB
#undef Z21VIRTUAL
#undef LAN
#undef DHCP
#undef WIFI
#undef S88N
#undef DCCGLOBALDETECTOR
#undef DALLASTEMPSENSE
#if defined BOOSTER_INT
#undef BOOSTER_EXT
#endif

#elif defined(ESP32)   //ESP32 Modul
#define ESP32_MCU
#define ESP_WIFI
#define ESP_OTA
#undef Z21VIRTUAL
#undef LAN
#undef DHCP
#undef WIFI

#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) //Arduino MEGA
#define MEGA_MCU
//#define SOFT_RESET    //for changed on LAN Interface
#undef Z21VIRTUAL

#elif defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644P__)  //Sanguino (other pins!)
#define SANGUINO_MCU
//#define SOFT_RESET    //for changed on LAN Interface
//ACHTUNG SS is on PIN3 (D2)!!!
#undef Z21VIRTUAL

#else //others Arduino UNO / NANO
#define UNO_MCU
//#define SOFT_RESET    //for changed on LAN Interface

#endif

//--------------------------------------------------------------
//Setting PIN CONFIG:
//--------------------------------------------------------------
#if defined(SANGUINO_MCU)
//Power:
#define Z21ResetPin 27  //RESET-Button-Pin bei Neustart betätigen um Standard IP zu setzten!
#define Z21ButtonPin Z21ResetPin  //Pin where the POWER-Button is conected
//DCC and Booster
#define DCCLed 25    //LED to show DCC active
#define DCCPin 12    //Pin for DCC sginal out
#define ShortLed 26     //LED to show Short
#define ShortExtPin 4  //Pin to detect Short Circuit of Booster (detect LOW)
#define GoExtPin 3   //Pin for GO/STOP Signal of Booster
#define ProgRelaisPin  23   //Pin for using Kehrschleifen-Modul
#define ACKSensePin 2 //Pin for ACK Comperator input
//Booster INT config:
#define GoIntPin 17   //Pin for inverted DCC Signal
#define ShortIntPin 13  //Pin for second Booster like TLE5206 (detect HIGH)
#define VAmpIntPin A4   //Input for Current sensor (CV read)
//XpressNet:
#define XNetTxRxPin  16    //XpressNet Control-Port for Send/Receive at MAX485
//LocoNet:
#define LNTxPin 15    //Sending Pin for LocoNet

//--------------------------------------------------------------
#elif defined(ESP8266_MCU)
/* Wemos D1 mini Pinout:
 * TX  IO, Serial        GPIO01       Debug TX
 * RX  IO, Serial        GPIO03       LocoNet RX
 * D0  IO                GPIO16       XpressNet Send/Receive
 * D1  IO, SCL           GPIO5        I2C Display
 * D2  IO, SDA           GPIO4        I2C Display
 * D3  IO, 10k Pull-up   GPIO0        LocoNet TX
 * D4  IO, 10k Pull-up,  GPIO2        BUILTIN_LED and Button
 * D5  IO, SCK           GPIO14       NDCC
 * D6  IO, MISO          GPIO12       XpressNet RX/TX
 * D7  IO, MOSI          GPIO13       DCC
 * D8  IO, 10k Pull-down, SS GPIO15   Prog-Enable
 * A0                                 Sense Input
 */
#define POWER_LED_INVERT    //invert the central Power Ouput Signal
//Power:
#define Z21ResetPin D4   //GPIO0 = RESET-Button-Pin bei Neustart betätigen um Standard IP zu setzten!
#define Z21ButtonPin Z21ResetPin  //Pin where the POWER-Button is conected
//DCC and Booster
#if defined(ADD_ACK_COMP) && !defined(BOOSTER_EXT)
  #define DCCLed 99       //not in use!
#else
  #define DCCLed D4       //GPIO2 = LED to show DCC active (LED_BUILTIN)
#endif  
#define DCCPin D7       //GPIO  = Pin for DCC sginal out
#define ShortLed 99     //D1 = LED to show Short
//Supported external Booster:
#if defined(BOOSTER_EXT)
  #undef BOOSTER_INT
  #define GoExtPin  D5    //Pin for GO/STOP Signal of Booster
  #define ShortExtPin D8  //Pin to detect Short Circuit of Booster (detect LOW)
  #define ACKSensePin -1 //Pin for ACK Comperator input
#else
  //Booster INT config:
  #define GoIntPin D5     //GPIO14 = Pin for inverted DCC Signal  
  #define ProgRelaisPin D8  //GPIO14 = Pin for using Kehrschleifen-Modul
  #if defined(ADD_ACK_COMP)
    #define ACKSensePin D4 //Pin for ACK Comperator input
  #else
    #define ACKSensePin -1 //Pin for ACK Comperator input
  #endif
#endif
#define VAmpIntPin A0   //ADC Input for Current sensor (CV read)
//XpressNet
#define XNetSerialPin D6  //GPIO12 = XpressNet RX/TX data line
#define XNetTxRxPin D0    //GPIO16 = XpressNet Control-Port for Send/Receive at MAX485
//LocoNet
#define LNTxPin D3      //D3 = Sending Pin for LocoNet //Default Pin: LN_TX_PORT D5 (GPIO14)

//--------------------------------------------------------------
#elif defined(ESP32_MCU)
//Power:
#define Z21ResetPin 36  //RESET-Button-Pin bei Neustart betätigen um Standard IP zu setzten!
#define Z21ButtonPin Z21ResetPin  //Pin where the POWER-Button is conected
//DCC and Booster
#define DCCLed 32    //LED to show DCC active
#define DCCPin 33    //Pin for DCC sginal out
#define additionalOutPin 16 //Pin for true DCC Output without Shutdown adn RailCom
#define ShortLed 27     //LED to show Short
#define ShortExtPin 4  //Pin to detect Short Circuit of Booster (detect LOW)
#define GoExtPin  15   //Pin for GO/STOP Signal of Booster
#define ProgRelaisPin  26   //Pin for using Kehrschleifen-Modul
#define ACKSensePin -1 //Pin for ACK Comperator input
//Booster INT config:
#define GoIntPin 25   //Pin for inverted DCC Signal
//#define ShortIntPin 35  //Pin for second Booster like TLE5206 (detect HIGH)
#define VAmpIntPin 34   //Input for Current sensor (CV read)
#define TempPin 13     //Temp.Sense_resistor (15k) with 46k Pull-Up or DALLAS
#define VoltIntPin 39  //Rail Voltage: Rail:100k - Sense - 4,7k - GND
//XpressNet
#define XNetSerialPin 10  //XpressNet RX/TX data line
#define XNetTxRxPin  9    //XpressNet Control-Port for Send/Receive at MAX485
//LocoNet
#define LNTxPin 21    //Sending Pin for LocoNet
#define LNRxPin 19    //Receiving Pin for LocoNet


//--------------------------------------------------------------
#elif defined(UNO_MCU) //Arduino UNO
//Power:
#define Z21ResetPin 10  //RESET-Button-Pin bei Neustart betätigen um Standard IP zu setzten!
#define Z21ButtonPin Z21ResetPin  //Pin where the POWER-Button is conected
//DCC and Booster
#define DCCLed 3    //LED to show DCC active
#define DCCPin 6    //Pin for DCC sginal out
#define additionalOutPin 11 //Pin for true DCC Output without Shutdown adn RailCom
#define ShortLed 45     //LED to show Short
#define ShortExtPin 5  //Pin to detect Short Circuit of Booster (detect LOW)
#define GoExtPin  A4   //Pin for GO/STOP Signal of Booster
#define ProgRelaisPin  A5   //Pin for using Kehrschleifen-Modul
#define ACKSensePin 2 //Pin for ACK Comperator input
//Booster INT config:
#define GoIntPin 4   //Pin for inverted DCC Signal
#define ShortIntPin 2  //Pin for second Booster like TLE5206 (detect HIGH)
#define VAmpIntPin A4   //Input for Current sensor
//XpressNet
#define XNetTxRxPin  9    //XpressNet Control-Port for Send/Receive at MAX485
//LocoNet
#define LNTxPin 7    //Sending Pin for LocoNet

//--------------------------------------------------------------
#else  //Arduino MEGA or DUE
//Power:
#define Z21ResetPin 47  //RESET-Button-Pin bei Neustart betätigen um Standard IP zu setzten!
#define Z21ButtonPin Z21ResetPin  //Pin where the POWER-Button is conected
//DCC and Booster
#define DCCLed 39    //LED to show DCC active
#define DCCPin 3    //Pin for DCC sginal out
#define additionalOutPin 11 //Pin for true DCC Output without Shutdown adn RailCom
#define ShortLed 45     //LED to show Short
#define ShortExtPin 5  //Pin to detect Short Circuit of Booster (detect LOW)
#define GoExtPin  12   //Pin for GO/STOP Signal of Booster
#define ProgRelaisPin  A5   //Pin for using Kehrschleifen-Modul
#define ACKSensePin 2 //Pin for ACK Comperator input
//Booster INT config:
#define GoIntPin 12   //Pin for inverted DCC Signal
#define ShortIntPin 41  //Pin for second Booster like TLE5206 (detect HIGH)
#define VAmpIntPin A0   //Input for Current sensor (CV read)
//#define VAmSensePin A8 //ACS712 5A Sensor (for testing only)
#define VoltIntPin A10  //Rail Voltage: Rail:100k - Sense - 4,7k - GND
#define TempPin A11     //Temp.Sense_resistor (15k) with 46k Pull-Up or DALLAS
//XpressNet
#define XNetTxRxPin  9    //XpressNet Control-Port for Send/Receive at MAX485
//LocoNet
#define LNTxPin 7    //Sending Pin for LocoNet

#endif

//--------------------------------------------------------------
//Dallas Temperatur Sensor (MEGA only):
#if defined(DALLASTEMPSENSE) && defined(MEGA_MCU)
#define ONE_WIRE_BUS TempPin
#else
#undef DALLASTEMPSENSE
#endif

//--------------------------------------------------------------
//S88 Singel Bus:
#if defined(ESP8266_MCU)
#undef S88N
#endif

#if defined(S88N)
#if defined(ESP32_MCU)
    //Eingänge:
  #define S88DataPin 13      //S88 Data IN
    //Ausgänge:
  #define S88ClkPin 12      //S88 Clock
  #define S88PSPin 14       //S88 PS/LOAD
  #define S88ResetPin 27    //S88 Reset
#else
    //Eingänge:
  #define S88DataPin A7      //S88 Data IN
    //Ausgänge:
  #define S88ClkPin A6      //S88 Clock
  #define S88PSPin A2       //S88 PS/LOAD
  #define S88ResetPin A3    //S88 Reset
#endif
#endif

//--------------------------------------------------------------
//LAN-Interface:
#define LANSSPIN 10   //Chip Select Pin of most Ethernet Shields
#define SDSSPIN 4     //Chip Select Pin SD-Card Reader on Ethernet Shield

//--------------------------------------------------------------
//Wifi-Interface:
#if defined (WIFI)
//Serialport:
#ifndef WLAN  //WLAN defined
  #if defined(MEGA_MCU) || defined(DUE_MCU) //MCU check: Arduino MEGA
  #define WLAN Serial2
  #elif defined(Z21VIRTUAL)
  #define WLAN SoftSerial
  #else
  #define WLAN Serial
  #endif  //END MCU check
#endif  //END WLAN defined

#if defined(Z21VIRTUAL) //Default Serial Baud Rate 1200,2400,4800,9600,14400,19200,28800,38400,57600,115200 
//for Arduino UNO only:
#define WIFISerialBaud 38400
#define RXvWiFi 12  //RX-PIN Soft Serial for Arduino UNO WiFi use  
#define TXvWiFi 13  //TX-PIN Soft Serial for Arduino UNO WiFi use  
#else
#define WIFISerialBaud 500000 //new with Z21_ESPArduinoUDP_v2.90 or higher!
#endif //END Z21VIRTUAL

#endif //End WiFi

//--------------------------------------------------------------
//Remove Display because of Pins already in use!
#if defined(Z21DISPLAY) && defined(UNO_MCU)
#undef DISPLAY
#endif
