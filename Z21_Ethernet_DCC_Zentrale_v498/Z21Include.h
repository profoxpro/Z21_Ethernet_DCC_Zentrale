//--------------------------------------------------------------------------------------------
#define FIND 0x00   //Display Mode - search for Address!

//**************************************************************
//Setup up PIN-Configuration for different MCU (UNO/MEGA/Sanduino)
#include "MCU_config.h"

//--------------------------------------------------------------------------------------------
//Load everything that is used:
//**************************************************************
//Report Upload Sketch Time:
#if defined(DebugBaud) || defined(Z21DISPLAY) || defined(HTTPCONF) || defined(ESP_WIFI)
const char* comp_date = __DATE__ " " __TIME__;    //sketch was compiled
#endif

//**************************************************************
//LocoNet LocoBuffer-USB -> deaktiviere alle DEBUG Funktionen!
#if (defined(LnBufferUSB) && defined(LOCONET)) || (defined(UNO_MCU) && defined(XPRESSNET))
#undef DebugBaud
#undef Debug
#undef DEBUG
#undef DEBUG_WLAN_CONFIG 
#undef REPORT   
#undef REQEST   
#undef LnDEB    
#undef XnDEB    
#undef Z21DEBUG 
#undef Z21DATADEBUG
#undef Z21SYSTEMDATADEBUG  
#else 
#undef LnBufferUSB    //deaktiviere!
#endif

//**************************************************************
//Firmware store in EEPROM:
#define EEPROMSwVerMSB 0
#define EEPROMSwVerLSB 1

/**************************************************************
Serial Debug Output:*/
#ifdef DebugBaud
  #ifndef Debug
    #define Debug Serial  //Interface for Debugging
  #endif
#endif

/**************************************************************
XpressNet Library:*/
#ifdef XPRESSNET
#include <XpressNetMaster.h>
#endif

/**************************************************************
LocoNet Library:*/
#ifdef LOCONET
#if defined(ESP32_MCU)  //use LocoNet2 Library
  #include "LNSerial.h"
  LocoNetBus bus;
  #include <LocoNetESP32.h>
  LocoNetESP32 locoNetPhy(&bus, LNRxPin, LNTxPin, 0);
  LocoNetDispatcher parser(&bus);
  
#elif defined(ESP8266_MCU)
  #include "LNHwSerial.h"
  
#else
  #include <LocoNet.h>
#endif
#endif

/**************************************************************
BOOSTER EXT:*/
#ifdef BOOSTER_EXT
#define BOOSTER_EXT_ON HIGH
#define BOOSTER_EXT_OFF LOW
#endif

/**************************************************************
BOOSTER INT:*/
#ifdef BOOSTER_INT
//#define BOOSTER_INT_ON LOW    //only for old Mode without RAILCOM support over NDCC!
//#define BOOSTER_INT_OFF HIGH  //only for old Mode without RAILCOM support over NDCC!
#define BOOSTER_INT_NDCC    //for new RAILCOM Booster3R - GoIntPin activate inverted booster signal
#endif

/**************************************************************
DCC Master to create a DCC Signal:*/
#include <DCCPacketScheduler.h>   //DCC Interface Library

//**************************************************************

void AllLocoData(uint16_t adr, uint8_t data[]); //so function can be found!

//Power functions:
static void globalPower (byte state);
static uint16_t getRailmA();
static uint16_t getRailVolt();

//--------------------------------------------------------------
// certain global XPressnet status indicators
#define csNormal 0x00 // Normal Operation Resumed ist eingeschaltet
#define csEmergencyStop 0x01 // Der Nothalt ist eingeschaltet
#define csTrackVoltageOff 0x02 // Die Gleisspannung ist abgeschaltet
#define csShortCircuit 0x04 // Kurzschluss
#define csServiceMode 0x08 // Der Programmiermodus ist aktiv - Service Mode
byte Railpower = csTrackVoltageOff;   //State of RailPower at Startup
bool Z21ButtonLastState = false;    //store last value of the Push Button for GO/STOP

/**************************************************************
ESP OTA Upload:*/
#if defined(ESP_OTA)
#include <ArduinoOTA.h>
#include "ESP_OTA.h"
#endif

//**************************************************************
#if defined(Z21VIRTUAL)  
#include <SoftwareSerial.h>
SoftwareSerial SoftSerial(TXvWiFi, RXvWiFi); // init Soft Serial
#undef LAN    //LAN nicht zulassen - Doppelbelegung der Signalleitungen!
#undef HTTPCONF
#undef LOCONET
#endif

//**************************************************************
#if defined(LAN)      //W5100 LAN Interface Library

  #define HTTPCONF //activate Website to configure DHCP, IP Adress and Number of S88 Bus Module

  #if !defined(MEGA_MCU)
  #undef ENC28
  #endif

  #if defined(ENC28)  //MEGA MCU only!
  #include <UIPEthernet.h>
  #else                                               
  #include <SPI.h>         // needed for Arduino versions later than 0018
  #include <Ethernet.h>
  #include <EthernetUdp.h>         // UDP library
  #endif
#else  
  //need class for variable type "IPAddress" (WLANlocalIP) what is showing WLAN IP on Display!
  #if defined(WIFI) && defined(Z21DISPLAY)
    #include <IPAddress.h>
  #endif
#endif

#if defined(ESP_WIFI)   //ESP8266 or ESP32
#if defined(ESP8266_MCU)
  #include <ESP8266WiFi.h>
  //#include <ESP8266WebServer.h>
  #if defined ESP_OTA
  #include <ESP8266mDNS.h>  //OTA update
  #endif
#elif defined(ESP32_MCU)  
  #include <WiFi.h>
  #include <WiFiAP.h>   //optional?
  #if defined(ESP_OTA)
  #include <ESPmDNS.h>  //ESP32 only, OTA update
  #endif
#endif  
#include <WiFiClient.h> 
#include <WiFiUdp.h>
#endif

//**************************************************************
//Configure the analog 1.1V refenence voltage:
#ifndef AREF_1V1
  #if defined(EXTERNAL_UREF_1V1)
  #define AREF_1V1 EXTERNAL   //externe 1.1 Volt
  #elif defined(MEGA_MCU)   
  #define AREF_1V1 INTERNAL1V1  //Arduino MEGA internal 1,1 Volt
  #elif defined(UNO_MCU)  
  #define AREF_1V1 INTERNAL   //Arduino UNO (equal to 1.1 volts on the ATmega168 or ATmega328P)
  #elif defined(ESP32_MCU) | defined(ESP8266_MCU)   //ESP Modul
  #define AREF_1V1      //always 1,1 Volt
  #endif
  //other Arduino's 5,0 Volt internal refence!
#endif

//**************************************************************
//Z21 LAN Protokoll:
#if defined(LAN) || defined (WIFI) || defined(ESP_WIFI) 
#include <z21.h> 
z21Class z21;
#else 
//make LocoNet work when no WLAN and LAN is used!
#if defined(LOCONET)
#include <z21header.h>
#endif
#endif

//**************************************************************
#if defined(DUE_MCU)
#include <DueFlashStorage.h>
DueFlashStorage Flash;
#define FIXSTORAGE Flash
#define FIXMODE write

#elif defined(ESP32_MCU)  //use NVS on ESP32!
#include <z21nvs.h>
z21nvsClass NVSEEPROMZ21;
#define FIXSTORAGE NVSEEPROMZ21
#define FIXMODE write

#else
#include <EEPROM.h>   //EEPROM - to store number of S88 Module and LAN IP
#define FIXSTORAGE EEPROM
  #if defined(ESP8266_MCU) || defined(ESP32_MCU)
  #define FIXMODE write
  #else
  #define FIXMODE update
  #endif
#endif

#if defined(ESP8266_MCU) || defined(ESP32_MCU)
//--> EEPROM Konfiguration ESP:
#define EESize 767    //Größe des EEPROM
#define EEStringMaxSize 40   //Länge String im EEPROM
//Client:
#define EEssidLength 200       //Länge der SSID
#define EEssidBegin 201        //Start Wert
#define EEpassLength 232        //Länge des Passwort
#define EEpassBegin 233        //Start Wert
//AP:
#define EEssidAPLength 264       //Länge der SSID AP
#define EEssidAPBegin 265        //Start Wert
#define EEpassAPLength 298        //Länge des Passwort AP
#define EEpassAPBegin 300        //Start Wert
#define EEkanalAP 299          //Kanal AP
#endif
//--> Allgemeine EEPROM Konfiguration:
//S88:
#define EES88Moduls 38  //Adresse EEPROM Anzahl der Module für S88
//LAN:
#define EELANDHCP 39  //aktiviert DHCP auf dem LAN Interface
#define EELANip 40    //Startddress im EEPROM für die IP

//---------------------------------------------------------------
#if defined(LAN)  //W5100 LAN Udp Setup:
EthernetUDP Udp;    //UDP for communication with APP/Computer (Port 21105)
#if defined(Z21MT)
EthernetUDP UdpMT;  //UDP to Z21 Maintenance Tool (Port 21106)
#include "Z21Maintenance.h"
#endif

//---------------------------------------------------------------
boolean LAN_DHCP = false;   //LAN DHCP Modus aus
// The IP address will be dependent on your local network:
IPAddress LAN_ip(192, 168, 0, 111);   //Werkseinstellung ist: 192.168.0.111
/*LAN MAC configuration: */
#ifndef LANmacB2
#define LANmacB2 0xEF
#endif
#ifndef LANmacB1
#define LANmacB1 0xFE
#endif
#ifndef LANmacB0
#define LANmacB0 0xED
#endif
static byte mac[] = { 0x84, 0x2B, 0xBC, LANmacB2, LANmacB1, LANmacB0 };
/*LAN DHCP configuration: */
#ifndef LANTimeoutDHCP
#define LANTimeoutDHCP 5000 //Timeout to wait for a DHCP respose
#endif
#if defined(HTTPCONF) //W5100 LAN config Website:
EthernetServer server(80);  // (port 80 is default for HTTP):
#endif
#endif    //LAN end
//---------------------------------------------------------------
#if defined(ESP_WIFI)
#ifndef ESP_HTTPCONF
#define ESP_HTTPCONF
#endif
#ifndef SssidAP
  #if defined(ESP32_MCU)
  #define SssidAP "Z21_ESP32_Central"   // Default Z21 AP (SSID)
  #else
  #define SssidAP "Z21_ESP_Central"   // Default Z21 AP (SSID)
  #endif
#endif
#ifndef SpassAP
#define SpassAP "12345678"  // Default Z21 network password
#endif
#ifndef SkanalAP
#define SkanalAP 3          // Default Kanal des AP
#endif
//AP default Z21 IP
IPAddress AP_ip(192, 168, 0, 111);   //Werkseinstellung ist: 192.168.0.111
IPAddress AP_sb(255, 255, 255, 0);   //Werkseinstellung Subnet

WiFiUDP Udp;
#if defined(ESP_HTTPCONF) //Setting Website
WiFiServer ESPWebserver(80);  //default port 80 for HTTP
#endif
#endif
//--------------------------------------------------------------
//Z21 Protokoll Typ Spezifikationen
#if defined(LAN) || defined (WIFI) || defined(ESP_WIFI)
#include "Z21type.h"    //Z21 Data Information
#endif

//--------------------------------------------------------------
//S88 Singel Bus:
#if defined(S88N)
#include "S88.h"
#endif

//--------------------------------------------------------------
//Dallas Temperatur Sensor:
#if defined(DALLASTEMPSENSE) //&& defined(MEGA_MCU)
#include <OneWire.h>
#include <DallasTemperature.h>
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
// arrays to hold device address
DeviceAddress insideThermometer;
#define DALLAS_RESOLUTION 9  //Temperatur resolution
#endif

//--------------------------------------------------------------
//INA219 current sensor
#if defined (INA219)
  #include <Wire.h>
  #include <INA219_WE.h>
  INA219_WE ina219 = INA219_WE(INA219);   //default Address 0x40
#endif

//--------------------------------------------------------------
//DCC Interface Master Short Detection:
//EXTERNAL BOOSTER:
#define DetectShortCircuit 0x1FF    //to detect short circuit  (0xFF)
unsigned int ShortTime = 0;            //Time Count for Short Detect
unsigned long LEDcount = 0;    //Timer for Status LED
//INTERNAL BOOSTER:
#if defined(BOOSTER_INT_MAINCURRENT)
  #define BOOSTER_INT_CURRENT_SHORT_DETECT //wenn nicht aktiviert dann hier einschalten!
  unsigned long ShortTimeINT = 0;      //Time Count for internal short detect
  byte ShortTimeINTcounter = 0;       //Count the low detection
  #if defined(ESP8266_MCU)
    //slow down readings on ESP8266 to let WIFI work! Slow down to min. ATTENTION WIFI may stop!    
    #define VAmpReadTimeNormal 10  //interval of reading normal (ms)
  #else //other MCU:
    #define VAmpINTMesureCount 10   //Anzahl des Messungen
    byte CountVAmpINT = 0;      //Zahl des Messung
  #endif
  uint16_t CalcVAmpINT  = 0;  //zwischen Messung
  uint16_t VAmpINT = 0;       //current power on the track
  #ifndef DETECT_SHORT_INT_WAIT
     #define DETECT_SHORT_INT_WAIT 20 //time in ms to wait before power off
  #endif  
  #ifndef DETECT_SHORT_INT_VALUE  //normal operation
    #if defined(INA219)
        #define DETECT_SHORT_INT_VALUE 3200   //mA for Short Power
    #elif defined(ESP8266_MCU)
        #define DETECT_SHORT_INT_VALUE  205   //because of WeMos internal voltage devider!                 á (2A * 0.33 Ohm) / (3,3/1024) = 204.8
    #else
      #if defined(AREF_1V1)
        #define DETECT_SHORT_INT_VALUE  1000  //analogRead value for "mA" that is too much (AREF = 1.1V)  á (3,3A * 0.33 Ohm) / (1,1/1024) = 1014
      #else
        #define DETECT_SHORT_INT_VALUE  250  //analogRead value for "mA" that is too much (AREF = 5.0V)
      #endif
    #endif  
  #endif
#endif

//setting for DCC ACK sense Value
#if defined(INA219)
#define DCC_ACK_SENSE_VALUE 60       //WeMos D1 Mini with INA219 connected!
#elif defined(ESP8266) //ESP8266 or WeMos D1 mini
#define DCC_ACK_SENSE_VALUE  4   //WeMos has a voltage divider for 3.1 Volt -> we not want to modify the board!
#else
#define DCC_ACK_SENSE_VALUE 30     //value difference
#endif

//--------------------------------------------------------------
DCCPacketScheduler dcc;
#define DCC     //activate DCC Interface (always)

//--------------------------------------------------------------
#if defined(DCCGLOBALDETECTOR) && defined(DCC)
#include "Z21_RailCom.h"
#endif

//--------------------------------------------------------------
#if defined(XPRESSNET)
XpressNetMasterClass XpressNet;
#endif

//--------------------------------------------------------------
//LocoNet-Bus:
#if defined (LOCONET)
#include "LNInterface.h"
#endif

//--------------------------------------------------------------
//XpressNet-Bus:
#if defined(XPRESSNET)
#include "XBusInterface.h"
#endif

//--------------------------------------------------------------
//Z21 Ethernet communication:
#if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
#include "Z21_LAN.h"
#endif

//--------------------------------------------------------------
//OLED Display
#if defined(Z21DISPLAY)
#include "OLEDdisplay.h"
#endif


//--------------------------------------------------------------
//Power function:
#include "POWER.h"      //load Z21 Power functions

//--------------------------------------------------------------
//Webpage
#if defined(HTTPCONF) || defined(ESP_HTTPCONF)
#include "Webpage.h"
#endif

//--------------------------------------------------------------------------------------------
//Z21 Software Reset:
#if defined(SOFT_RESET)
#include <avr/wdt.h>
#define soft_restart()        \
do                          \
{                           \
    wdt_enable(WDTO_15MS);  \
    for(;;)                 \
    {                       \
    }                       \
} while(0)
#endif

//--------------------------------------------------------------------------------------------
#if defined(DEBUG) && !defined(DUE_MCU) && !defined(ESP8266) && !defined(ESP32)
//ONLY for Atmega, not for Arduino DUE or ESP chip (ARM)!
int freeRam () 
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
#endif

//--------------------------------------------------------------------------------------------
//ONLY for ESP
#if defined(DEBUG) && (defined(ESP8266) || defined(ESP32))
int freeRam () 
{
  return ESP.getFreeHeap();
}
#endif
//--------------------------------------------------------------
