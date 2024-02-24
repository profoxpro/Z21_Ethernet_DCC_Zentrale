/************************************************************************************
 Z21 Ethernet DCC Command Station Interface config file 
 Copyright (c) 2015-2023 by Philipp Gahtow 
***********************************************************************************/

/*-------------**********************************-----------------------------------
/ ************ CHANGE TO SET UP YOUR INDIVIDUAL CONFIGURATION ***************
/  => uncomment ("//" or #undef) the following lines, if you not USE the Interface! 
/-------------**********************************-----------------------------------*/

////#define S88N    //S88N Interface (max 62 * 8 Module)

#define WIFI  //WiFi Interface über Serial-Port für Arduino UNO/MEGA/DUE zum ESP 8266
#define WLAN Serial3  // (DEFAULT = Serial2!) use not the standard WLAN Serial Interface. Select another serial:
//#define Z21VIRTUAL  // WiFi over SoftSerial for UNO only! - LAN and LocoNet will be inaktiv! 

//#define LAN       //LAN Ethernet Z21 LAN Kommunikation mit W5100 oder ENC28 Ethernet Shield. Bitte diese IP nur über die Webseite (http://192.168.0.111) ändern! (not for ESP8266 and ESP32)
//#define ENC28     //USE a ENC28J60 module - instead of w5100 Shield (MEGA only!)
//#define LANmacB2 0xEF   //Byte2 (DEFAULT = 84:2B:BC:EF:FE:ED!) change optional LAN MAC Address. MAC starts with: „84:2B:BC:..Byte*2..:..Byte*1..:..Byte*0..“
//#define LANmacB1 0xFE   //Byte1
//#define LANmacB0 0xED   //Byte0
//#define LANTimeoutDHCP 10000 //(DEFAULT!) Timeout to wait for a DHCP respose (Fix default Time: 5 sec)

//#define DCCGLOBALDETECTOR  //DCC Railcom Global Detector for MEGA on Serial Port 3 (RX only) - (not for ESP8266 and ESP32)

//#define XPRESSNET   //XpressNet Auto Master/Salve Interface (not for ESP8266 and ESP32)

//#define LOCONET     //LocoNet Interface (Timer1, Timer5 on MEGA, with LocoNet2 Library on ESP32)
//#define LnSLOTSRV   //LocoNet Master-Mode: provide a Slot Server for Loco to use FRED & DaisyII
//#define LnBufferUSB  //LocoNet LocoBuffer-USB at 57600 bps (Achtung: kein Debug über Serial Monitor möglich!)

//#define BOOSTER_EXT //External Booster Interface (zB. ROCO, CD[E])

#define BOOSTER_INT //internal Booster Interface (zB. TLE5206)
#define BOOSTER_INT_MAINCURRENT     //Standard Short Circuit Detection over current Sense resistor (VAmpIntPin) activate the current sensor for prog track and SHORT CIRCUIT Sense over MAINCURRENT
//#define DETECT_SHORT_INT_WAIT  20   //(OPTIONAL) Time in ms after internal short circuit is detected and power will switch off
#define DETECT_SHORT_INT_VALUE 700 //(OPTIONAL) value = (Amper * senseResist) / (Uref / 1024)

/* (DEFAULT OFF - OPTIONAL!) activate only one - Short2 detection PIN reading use: (only one can be active!)*/
//#define BOOSTER_INT_TLE5206    //internal Booster with TLE5206 or
//#define BOOSTER_EXT_CDE        //external CDE Booster

//#define PROG_OUT_INVERT     //(DEFAULT OFF!) invert the Output Signal for Service-Mode (Prog-Relay) - for L298n H-Bridge usage without Relay!
//#define ADD_ACK_COMP        //(DEFAULT OFF!) Comperator to detect exact the ACK pulse over interrupt

//#define DALLASTEMPSENSE   //Dallas 18B20 Temperatur Sensor for Arduino MEGA only!

//#define Z21DISPLAY FIND   //SSD1306 OLED I2C Display and Address for config Data for Arduino MEGA and ESP only! -->MEGA: 20(SDA), 21(SCL) || -->ESP8266: D2(SDA), D1(SCL)
//If you not know the Display I2C Address (normal: 0x3C) leave the value "FIND" so the central will search for it when starting!
//#define Z21DISPLAY_SH1106        //(DEFAULT OFF!) use OLEDs based on SH110X drivers
//#define Z21DISPLAY_CONTRAST 150   //(OPTIONAL) higher contrast (0..255; default = 20)
//#define AMP_DECIMALS 1     //(OPTIONAL) OLED decimal digits of precision for Amper on the Rail (default: x.xxA = 2)

//#define FS128 //default speed steps (Fahrstufen) => possible values are: FS14, FS28, FS128

#define Uref 1.10                     // measured reference voltage ARef-pin, individual value for every Arduino
//#define EXTERNAL_UREF_1V1   //optional: AREF with external 1.1 Volt, to get better CV# read with Arduino UNO

//#define INA219 0x40       //INA219 I2C current Sensor and Address for Arduino MEGA and ESP only!
#define senseResist 0.33   //or 0.1   // actual resistor for measuring current on track

/***************************************************************/
/*------------------DEBUG-ONLY---------------------------------*/
/***************************************************************/
#define Debug Serial //(DEFAULT = Serial!) use not the Default Standard Debug Serial Interface. Select another serial:
#define DebugBaud 115200 //Activate Debug on "Serial" with speed setting (on ESP8266 with LocoNet only 16660 baud)

/*---------- select what to show (options): --------------*/
#define DEBUG    //To see System-DATA on Serial
#define DEBUG_WLAN_CONFIG  //to see config data of Wifi ESP8266 (IP, Pw, ..)
//#define REPORT   //To see Sensor Messages (LocoNet & S88)
//#define REQEST   //To see answer Request Messages (Lok , Switch, CV)
//#define LnDEB    //To see RAW DATA of LocoNet Protokoll
//#define XnDEB    //To see XpressNet Data
#define Z21DEBUG //to see Z21 LAN control data
#define Z21DATADEBUG //to see RAW DATA of Z21 LAN Protokoll
#define Z21SYSTEMDATADEBUG  //to see the system data mainpower and temp
