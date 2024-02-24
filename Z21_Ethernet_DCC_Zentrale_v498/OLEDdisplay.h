//--------------------------------------------------------------
/*
  additional OLED Display 128x64 for Z21PG

  - show system info
  - show power status
  - display IP Address
  - show system information
  - default Display Driver SSD1306 0.96"
  - add second Display Driver SH1106 for 1.3"
  - add logos for different power mode
  - animate the CV progress
  
  Copyright (c) by Philipp Gahtow, year 2022
*/
#if defined(Z21DISPLAY)

//--------------------------------------------------------------
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#ifndef Z21DISPLAY_CONTRAST
#define Z21DISPLAY_CONTRAST 20   //reduce the contrast for a longer lifetime (0..255)
#endif

#ifndef AMP_DECIMALS
#define AMP_DECIMALS 2  // OLED decimal digits of precision for Amper on the Rail (x.xxA)
#endif

#define DISPLAY_POWER_LOGO 300  // x * 255 = ms time after config data will be removed in csNormal!

//--------------------------------------------------------------
static const unsigned char Z21Bitmap [] PROGMEM = {
  // Z21 Logo, 80x37px
  0x07, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x07, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x07, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x07, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x0f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xfe, 0x0f, 0xff, 0xff, 0x01, 0xff, 0xe0, 
  0x00, 0x00, 0x03, 0xfc, 0x0f, 0xff, 0xff, 0xc1, 0xff, 0xe0, 
  0x00, 0x00, 0x07, 0xfc, 0x1f, 0xff, 0xff, 0xe1, 0xff, 0xe0,  
  0x00, 0x00, 0x0f, 0xf8, 0x1f, 0xff, 0xff, 0xe1, 0xff, 0xe0, 
  0x00, 0x00, 0x1f, 0xf0, 0x1f, 0xff, 0xff, 0xf1, 0xff, 0xe0, 
  0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00, 0x1f, 0xf0, 0x0f, 0xe0,  
  0x00, 0x00, 0x3f, 0xc0, 0x00, 0x00, 0x07, 0xf0, 0x0f, 0xe0,  
  0x00, 0x00, 0x7f, 0xc0, 0x00, 0x00, 0x07, 0xf0, 0x0f, 0xe0,  
  0x00, 0x00, 0xff, 0x80, 0x00, 0x00, 0x07, 0xf0, 0x0f, 0xe0,  
  0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x0f, 0xe0,  
  0x00, 0x01, 0xfe, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x0f, 0xe0,  
  0x00, 0x03, 0xfc, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x0f, 0xe0, 
  0x00, 0x07, 0xfc, 0x00, 0x00, 0x7f, 0xff, 0xf0, 0x0f, 0xe0, 
  0x00, 0x0f, 0xf8, 0x00, 0x03, 0xff, 0xff, 0xf0, 0x0f, 0xe0, 
  0x00, 0x0f, 0xf0, 0x00, 0x0f, 0xff, 0xff, 0xe0, 0x0f, 0xe0, 
  0x00, 0x1f, 0xe0, 0x00, 0x1f, 0xff, 0xff, 0xe0, 0x0f, 0xe0, 
  0x00, 0x3f, 0xc0, 0x00, 0x1f, 0xff, 0xff, 0xc0, 0x0f, 0xe0, 
  0x00, 0x7f, 0xc0, 0x00, 0x3f, 0xff, 0xff, 0x00, 0x0f, 0xe0,  
  0x00, 0xff, 0x80, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x0f, 0xe0,  
  0x00, 0xff, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x00, 0x0f, 0xe0, 
  0x01, 0xfe, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x0f, 0xe0, 
  0x03, 0xfc, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x0f, 0xe0, 
  0x07, 0xfc, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x0f, 0xe0, 
  0x07, 0xf8, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x0f, 0xe0, 
  0x07, 0xf0, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x0f, 0xe0,  
  0x07, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0xcf, 0xe0, 
  0x07, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0xcf, 0xe0, 
  0x07, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0xcf, 0xe0, 
  0x07, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0xcf, 0xe0, 
  0x07, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0xcf, 0xe0
};
//--------------------------------------------------------------
static const unsigned char Power_PROG [] PROGMEM =
 //imageWidth 25, imageHeight 36
{   0x00, 0x22, 0x00, 0x00, 0x00, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf0, 0x00, 
    0x3f, 0x80, 0xfe, 0x00, 0x3f, 0x80, 0xff, 0x00, 0x3f, 0x80, 0xff, 0x00, 0x7f, 0xff, 0xff, 0x00, 
    0x70, 0x00, 0x07, 0x00, 0x60, 0xe6, 0x63, 0x00, 0x61, 0x96, 0x63, 0x00, 0x61, 0x86, 0x63, 0x00, 
    0x61, 0x82, 0x43, 0x00, 0x61, 0x83, 0xc3, 0x00, 0x61, 0x83, 0xc3, 0x00, 0x61, 0x91, 0x83, 0x00, 
    0x60, 0xe1, 0x83, 0x00, 0x70, 0x00, 0x07, 0x00, 0x7f, 0xff, 0xff, 0x00, 0x7f, 0xff, 0xff, 0x00, 
    0x7f, 0xff, 0xff, 0x00, 0x7f, 0xff, 0xff, 0x00, 0x7c, 0xff, 0xcf, 0x00, 0x78, 0x7f, 0x87, 0x00, 
    0x78, 0x7f, 0x87, 0x00, 0x3c, 0xff, 0xcf, 0x00, 0x3f, 0xff, 0xfe, 0x00, 0x1f, 0xff, 0xfc, 0x00, 
    0x0f, 0xff, 0xf8, 0x00, 0x07, 0xff, 0xf8, 0x00, 0x0f, 0xff, 0xf8, 0x00, 0x1c, 0x00, 0x3c, 0x00, 
    0x1c, 0x00, 0x0e, 0x00, 0x38, 0x00, 0x06, 0x00, 0x70, 0x00, 0x07, 0x00, 0xe0, 0x00, 0x03, 0x80
};
//--------------------------------------------------------------
static const unsigned char Power_ON[] PROGMEM = {
  // Power On Logo: 37x40px
    0x00, 0x01, 0xfe, 0x00, 0x07, 0x00, 0x0f, 0xff, 0xc0, 0x07, 0x00, 0x3e, 0x01, 0xf0, 0x07, 0x00, 0xf0, 0x00, 0x38, 0x07, 
    0x07, 0x80, 0x08, 0x07, 0x07, 0x0e, 0x00, 0x1c, 0x03, 0x87, 0x0c, 0x00, 0x1c, 0x01, 0x87, 0x1c, 0xe0, 0x3c, 0x39, 0xc7, 
    0x18, 0xe0, 0x78, 0x38, 0xc7, 0x31, 0xc0, 0xf8, 0x1c, 0x67, 0x33, 0x80, 0xd8, 0x0e, 0x67, 0x73, 0x01, 0xd8, 0x06, 0x77, 
    0x63, 0x03, 0xb8, 0x06, 0x37, 0x67, 0x07, 0x3f, 0xc7, 0x37, 0x66, 0x06, 0x3f, 0xc3, 0x37, 0x66, 0x0e, 0x01, 0xc3, 0x37, 
    0x66, 0x1f, 0xe3, 0x83, 0x37, 0x66, 0x1f, 0xe7, 0x03, 0x37, 0x66, 0x00, 0xee, 0x03, 0x37, 0x67, 0x00, 0xcc, 0x07, 0x37, 
    0x63, 0x00, 0xdc, 0x06, 0x37, 0x73, 0x01, 0xf8, 0x06, 0x77, 0x33, 0x81, 0xf0, 0x0e, 0x67, 0x31, 0xc1, 0xe0, 0x1c, 0x67, 
    0x38, 0xe1, 0xe0, 0x38, 0xc7, 0xfc, 0xe3, 0xc0, 0x39, 0xff, 0x2c, 0x03, 0x80, 0x01, 0xc7, 0x2e, 0x03, 0x00, 0x03, 0xc7, 
    0x27, 0x00, 0x00, 0x07, 0x47, 0x20, 0xf0, 0x00, 0x38, 0x47, 0x20, 0xbe, 0x01, 0xf0, 0x47, 0x20, 0x8f, 0xff, 0xd0, 0x47, 
    0x20, 0x83, 0xfe, 0x10, 0x47, 0x20, 0x82, 0x04, 0x10, 0x47, 0x20, 0x82, 0x04, 0x10, 0x47, 0x20, 0x82, 0x04, 0x10, 0x47, 
    0x20, 0x82, 0x04, 0x10, 0x47, 0xff, 0xff, 0xff, 0xff, 0xff, 0x20, 0x82, 0x04, 0x10, 0x47, 0x3f, 0x83, 0xfc, 0x1f, 0xc7
};
//--------------------------------------------------------------
static const unsigned char Power_OFF[] PROGMEM = {
    //Power OFF Logo: 37x40px
    0x00, 0x01, 0xfe, 0x00, 0x07, 0x00, 0x0f, 0xff, 0xc0, 0x07, 0x00, 0x3e, 0x01, 0xf0, 0x07, 0x00, 0x70, 0x00, 0x38, 0x07, 
    0x00, 0xc0, 0x00, 0x0c, 0x07, 0x01, 0x80, 0x00, 0x06, 0x07, 0x03, 0x00, 0x00, 0x03, 0x07, 0x06, 0x00, 0x30, 0x01, 0x87, 
    0x0c, 0x00, 0x30, 0x00, 0xc7, 0x0c, 0x00, 0x30, 0x00, 0xc7, 0x18, 0x00, 0x30, 0x00, 0x67, 0x18, 0x06, 0x31, 0x80, 0x67, 
    0x18, 0x06, 0x31, 0x80, 0x67, 0x30, 0x0c, 0x30, 0xc0, 0x37, 0x30, 0x18, 0x30, 0xc0, 0x37, 0x30, 0x18, 0x30, 0x60, 0x37, 
    0x30, 0x18, 0x30, 0x60, 0x37, 0x30, 0x18, 0x00, 0x60, 0x37, 0x30, 0x18, 0x00, 0x60, 0x37, 0x30, 0x18, 0x00, 0xc0, 0x37, 
    0x38, 0x0c, 0x00, 0xc0, 0x67, 0x18, 0x06, 0x01, 0x80, 0x67, 0x18, 0x07, 0x87, 0x80, 0x67, 0x3c, 0x03, 0xfe, 0x00, 0xc7, 
    0x2c, 0x00, 0x78, 0x00, 0xc7, 0xff, 0x00, 0x00, 0x03, 0xff, 0x23, 0x80, 0x00, 0x03, 0x47, 0x21, 0x80, 0x00, 0x06, 0x47, 
    0x20, 0xe0, 0x00, 0x1c, 0x47, 0x20, 0xf0, 0x00, 0x38, 0x47, 0x20, 0x3e, 0x01, 0xf0, 0x47, 0x20, 0x8f, 0xff, 0xd0, 0x47, 
    0x20, 0x83, 0xfe, 0x10, 0x47, 0x20, 0x82, 0x04, 0x10, 0x47, 0x24, 0x82, 0x04, 0x12, 0x47, 0x20, 0x82, 0x04, 0x10, 0x47, 
    0x20, 0x82, 0x04, 0x10, 0x47, 0xff, 0xff, 0xff, 0xff, 0xff, 0x20, 0x82, 0x04, 0x10, 0x47, 0x3f, 0x83, 0xfc, 0x1f, 0xc7
};
//--------------------------------------------------------------
static const unsigned char Power_EmSTOP[] PROGMEM = {
  //Power Em Stop Logo: 38x39px
    0x00, 0x7f, 0xff, 0xf8, 0x03, 0x00, 0xff, 0xff, 0xfc, 0x03, 0x01, 0xff, 0xff, 0xfe, 0x03, 0x01, 0xc0, 0x00, 0x0e, 0x03, 
    0x03, 0xc0, 0x00, 0x0f, 0x03, 0x07, 0x80, 0x00, 0x07, 0x83, 0x07, 0x00, 0x00, 0x03, 0x83, 0x0f, 0x00, 0x00, 0x03, 0xc3, 
    0x0e, 0x00, 0x00, 0x01, 0xe3, 0x1c, 0x00, 0x00, 0x00, 0xe3, 0x3c, 0x00, 0x00, 0x00, 0xf3, 0x38, 0x00, 0x00, 0x00, 0x73, 
    0x78, 0xef, 0x9f, 0x1f, 0x7b, 0xf1, 0x02, 0x31, 0x91, 0x3f, 0xe1, 0x02, 0x20, 0x91, 0x1f, 0xe1, 0xc2, 0x20, 0x91, 0x1f, 
    0xe0, 0x22, 0x20, 0x9f, 0x1f, 0xe0, 0x22, 0x20, 0x90, 0x1f, 0xf0, 0x22, 0x31, 0x90, 0x3f, 0x79, 0xc2, 0x1f, 0x10, 0x3b, 
    0x38, 0x00, 0x00, 0x00, 0x73, 0x3c, 0x00, 0x00, 0x00, 0xf3, 0x3e, 0x00, 0x00, 0x01, 0xe3, 0x2e, 0x00, 0x00, 0x01, 0xe3, 
    0xff, 0x00, 0x00, 0x03, 0xff, 0x27, 0x80, 0x00, 0x03, 0xc3, 0x27, 0x80, 0x00, 0x07, 0xc3, 0x23, 0xc0, 0x00, 0x0f, 0x43, 
    0x21, 0xc0, 0x00, 0x1e, 0x43, 0x21, 0xff, 0xff, 0xfe, 0x43, 0x20, 0xff, 0xff, 0xfc, 0x43, 0x20, 0xff, 0xff, 0xf8, 0x43, 
    0x20, 0x82, 0x04, 0x10, 0x43, 0x24, 0x82, 0x04, 0x12, 0x43, 0x20, 0x82, 0x04, 0x10, 0x43, 0x20, 0x82, 0x04, 0x10, 0x43, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0x20, 0x82, 0x04, 0x10, 0x43, 0x3f, 0x83, 0xfc, 0x1f, 0xc3
};
//--------------------------------------------------------------

#if defined(Z21DISPLAY_SH1106)
//-------------CONFIG SH110X DISPLAY DRIVER-----------------------------------------
#include <Adafruit_SH110X.h>
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
#define DISPLAY_BG SH110X_BLACK   //color for background
#define DISPLAY_TEXT SH110X_WHITE //Text color
#else
//-------------CONFIG SSD1306 DISPLAY DRIVER-----------------------------------------
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
#define DISPLAY_BG BLACK            //color for background
#define DISPLAY_TEXT SSD1306_WHITE  //Text color
#endif

//--------------------------------------------------------------
#define DisplayLineHight 9      //Pixel to get the next line
boolean DisplayReady = false;   //Boot-Up progress has finish
uint8_t DisplayCounter = 0xFF;  //for animation

//--------------------------------------------------------------
//remove old writing with a block box
void DisplayClearPart(uint8_t x, uint8_t y, uint8_t width) {
  //x,y,width,hight
  display.fillRect(x, y, width-x, DisplayLineHight, DISPLAY_BG);
  display.setCursor(x,y);
}

//--------------------------------------------------------------
/*
#if defined(DISPLAY_TRANSMISSION)
//display LAN/WLAN transmission:
void DisplayDataTransmission(bool activ = true) {
  if (Railpower == csShortCircuit) //no space for System Info!
    return;
  uint8_t color = DISPLAY_BG;
  if (activ == true)
    color = DISPLAY_TEXT;
  display.fillTriangle(100, 1, 98, 3, 102, 3, color); // Send Data
  display.drawLine(100,3,100,10,color);
  display.fillTriangle(105, 10, 103, 8, 107, 8, color); // Receive Data
  display.drawLine(105,1,105,8, color);
  display.display();  //show...
}
#endif
*/

//--------------------------------------------------------------
//update Z21 System Info
#if defined(BOOSTER_INT_MAINCURRENT)
void DisplayUpdateRailData(uint16_t inAm, float volt, float temp) {
  if (Railpower == csShortCircuit) //no space for System Info!
    return;
  //display WLAN signal on ESP:  
  #if defined(ESP_WIFI)
    int WLAN_Signal = WiFi.RSSI();
  #endif  
  #if defined(ESP_WIFI) || defined(WIFI) 
    display.fillRect(114, 0, 14, 13,DISPLAY_BG);  //clear old WLAN Signal 
    if (WLAN_Signal > 30) { //not connected!
      display.drawLine(121,4,127,12,DISPLAY_TEXT);  // \ //
      display.drawLine(127,4,121,12,DISPLAY_TEXT);  // / //
    }
    else {
      if (WLAN_Signal > -30)  
        display.drawLine(127,2,127,12,DISPLAY_TEXT);  //Ausgezeichnet
      if (WLAN_Signal > -65) 
        display.drawLine(125,4,125,12,DISPLAY_TEXT);  //Sehr gut
      if (WLAN_Signal > -71) 
        display.drawLine(123,6,123,12,DISPLAY_TEXT);  //Akzeptabel
      if (WLAN_Signal > -78) 
        display.drawLine(121,8,121,12,DISPLAY_TEXT);  //Schlecht
      if (WLAN_Signal > -85) 
        display.drawLine(119,10,119,12,DISPLAY_TEXT); //Sehr schlecht   
      //mark position of empty bar:
      for (uint8_t i = 0; i < 6; i++) {
        display.drawPixel(119 + (i * 2),12, DISPLAY_TEXT);
      }
    }
    //WLAN Antenne:
    display.drawLine(117,0,117,12,DISPLAY_TEXT); // | //
    display.drawLine(114,0,120,0,DISPLAY_TEXT);  // - //
    display.drawLine(114,1,117,5,DISPLAY_TEXT);  // \ //
    display.drawLine(117,5,120,1,DISPLAY_TEXT);  // / //
    display.drawLine(117,5,120,1,DISPLAY_TEXT);  // / //
    
    DisplayClearPart(88, 0, 110);
    #if defined(WIFI)
      display.print(ESPsoftAPStationNum);  //AP connected clients
    #else
      display.print(WiFi.softAPgetStationNum());  //AP connected clients
    #endif  
    display.drawLine(95,0,95,7,DISPLAY_TEXT); // | //
    display.setCursor(98,0);
    display.print("8"); //max 8 number of clients
  #endif  
  if (Railpower == csServiceMode) //stop here for full screen logo:
    return;  
  //display System Info:  
  DisplayClearPart(40, DisplayLineHight, 80);
  display.print(String(float(inAm)/1000, AMP_DECIMALS));   // A.aa Amper
  display.print(F("A"));
  //ESP8266 has only one ADC, so there is only the sense Value!
  if (temp != 0.0) {
    DisplayClearPart(90, 18, SCREEN_WIDTH);   // 18 = DisplayLineHight * 2
    display.print(temp,1);  //one decimal place only!
    display.print((char)247);
    display.print(F("C"));
  }
  if (volt != 0) {
    DisplayClearPart(0, DisplayLineHight, 40);
    display.print(volt / 1000,1); //one decimal place only!
    display.print(F("V"));
  }
  // drawing commands to make them visible on screen!
  display.display();
}
#endif

//--------------------------------------------------------------
//draw a long rail on display
void drawLongRail(void) {
  display.drawLine(0, 47, 44, 47,DISPLAY_TEXT);  //Schiene links
  display.drawLine(79, 47, SCREEN_WIDTH, 47,DISPLAY_TEXT);  //Schiene rechts
  display.drawLine(0, 59, SCREEN_WIDTH, 59,DISPLAY_TEXT);  //Schiene unten
  display.drawRect(-4, 45, 7, 17, DISPLAY_TEXT);  //Schwelle
  display.drawRect(8, 45, 7, 17, DISPLAY_TEXT);  //Schwelle
  display.drawRect(20, 45, 7, 17, DISPLAY_TEXT);  //Schwelle
  display.drawRect(32, 45, 7, 17, DISPLAY_TEXT);  //Schwelle
  //Logo
  display.drawRect(81, 45, 7, 17, DISPLAY_TEXT);  //Schwelle
  display.drawRect(93, 45, 7, 17, DISPLAY_TEXT);  //Schwelle
  display.drawRect(105, 45, 7, 17, DISPLAY_TEXT);  //Schwelle
  display.drawRect(117, 45, 7, 17, DISPLAY_TEXT);  //Schwelle
}

//--------------------------------------------------------------
//Update the config data on display
void DisplayConfigData() {

    //remove all old data:
    display.fillRect(0,18, SCREEN_WIDTH, 46, DISPLAY_BG);  //remove the config data
    
    byte y = DisplayLineHight;  //Start after S88 Module output!

    #if defined(S88N)
    y += DisplayLineHight;
    DisplayUpdateS88N();
    #endif
  
    #if defined(LAN)
    y += DisplayLineHight;
    DisplayClearPart(0, y, SCREEN_WIDTH);           
    display.print(F("Eth:  "));
    display.print(LAN_ip);
    #endif
  
    #if defined(WIFI)
    if (ESPSwVer != 0.0) {    //we already receive data?
      y += DisplayLineHight;
      DisplayClearPart(0, y, SCREEN_WIDTH);             
      display.print(F("WLAN: "));
      if (WLANlocalIP[0] != 0)
        display.print(WLANlocalIP);             //0xE4 = Client IP
      else display.print(F("[none]"));  
      y += DisplayLineHight;
      DisplayClearPart(0, y, SCREEN_WIDTH);           
      display.print(WLANssid);                //0xE5 = Client SSID Name
      y += DisplayLineHight;
      DisplayClearPart(0, y, SCREEN_WIDTH);            
      display.print(F("ESP Sw v"));
      display.print(ESPSwVer);                //0xE8 = ESP Sw Version
    }
    #endif
  
    #if defined(ESP_WIFI)
    y += DisplayLineHight;
    DisplayClearPart(0, y, SCREEN_WIDTH);
    display.print(F("ID:"));  //AP SSID             
    display.print(ssidAP);  //AP Name
    y += DisplayLineHight;
    DisplayClearPart(0, y, SCREEN_WIDTH);             
    display.print(F("Pw:"));
    display.print(passAP);
    y += DisplayLineHight;
    DisplayClearPart(0, y, SCREEN_WIDTH);             
    display.print(F("IP:"));
    display.print(WiFi.softAPIP());
    y += DisplayLineHight;
    DisplayClearPart(0, y, SCREEN_WIDTH);
    display.print(F("WLAN: "));
    display.print(WiFi.localIP().toString());
    y += DisplayLineHight;
    DisplayClearPart(0, y, SCREEN_WIDTH);
    display.print(ssid.c_str());
    #endif
 
  // drawing commands to make them visible on screen!
  display.display();
}

//--------------------------------------------------------------
//update power status
void DisplayUpdateRailPower(bool clear) {
  //Railpower: ON, OFF, PROG, SHORT
  if (DisplayCounter == 0xFF) 
    DisplayClearPart(40, 0, 70);
  if (clear == true)
    display.clearDisplay();
  switch (Railpower) {
      case csNormal: {
        if (DisplayCounter == 0)  //nothing more to do!
          return;
        else DisplayCounter--;
        if (DisplayCounter == 1) {  //last timer tick
          display.fillRect(0,18, SCREEN_WIDTH, 46, DISPLAY_BG); //remove the config data
          drawLongRail();
          display.drawBitmap(42,22, Power_ON, 37, 40, DISPLAY_TEXT); //EmSTOP Logo
          display.display();
          return;
        }
        if (DisplayCounter == 254) {
          display.print(F("-ON-")); //first time show power status
          DisplayConfigData();
        }
        else return;
        break;
      }
      case csTrackVoltageOff: {
        display.print(F("-OFF-")); 
        display.fillRect(0,18, SCREEN_WIDTH, 46, DISPLAY_BG);  //remove the config data
        drawLongRail();
        display.drawBitmap(42,22, Power_OFF, 37, 40, DISPLAY_TEXT); //Power OFF Logo
        break;
      }
      case csServiceMode: {
        display.fillRect(0,18, SCREEN_WIDTH, 46, DISPLAY_BG);  //remove the config data
        display.drawBitmap(51,22, Power_PROG, 25, 36, DISPLAY_TEXT); //Service Mode CV Logo
        break;
      }
      case csShortCircuit: {
          //draw blinking attention mark!
          uint8_t color = DISPLAY_BG; //flash color
          if (DisplayCounter == 0xFF) { //start-up
            DisplayCounter = 0xF0; //clear the counter
            display.clearDisplay();
            color = DISPLAY_TEXT; //change color
          }
          else {
            DisplayCounter = 0xFF; //reset
            display.fillRect(0 ,0, SCREEN_WIDTH, SCREEN_HEIGHT, DISPLAY_TEXT); //make screen white
          }  
          for(int8_t i=22; i < 26; i+=1) {
            display.drawTriangle(
            64  , 32-i,
            64-i, 32+i,
            64+i, 32+i, color);
          }
          display.fillTriangle(62 ,20, 64, 20, 58, 35, color);    // (/)
          display.drawLine(58, 35, 67, 31, color); // (-)
          display.drawLine(58, 36, 67, 32, color); // (-)
          display.fillTriangle(65 ,32, 67, 32, 62, 44, color);  // (/)
          display.fillTriangle(61 ,44, 65, 44, 63, 49, color);  // (\/)
          display.display();
          return;
        }
      case csEmergencyStop: {
        display.fillRect(0,18, SCREEN_WIDTH, 46, DISPLAY_BG);  //remove the config data
        drawLongRail();
        display.drawBitmap(42,23, Power_EmSTOP, 38, 39, DISPLAY_TEXT); //EmSTOP Logo
        break;
      }
      default: display.print(F("-!?!-")); break;  //unknown!
  }
  
  display.setCursor(0,0);             // Start at top-left corner
  display.print(F("Z21PG"));
  display.display();
  if (Railpower != csNormal) {  //only show config in normal mode
    DisplayCounter = 0xFF;  //reset
  }
}

//--------------------------------------------------------------
//update S88N Module
#if defined(S88N)
void DisplayUpdateS88N() {
  if (DisplayReady) {
    display.setTextSize(1);
    DisplayClearPart(0, 18, 89);  //DisplayLineHight * 2
    display.print(F("S88:  "));
    display.print(S88Module);
    // drawing commands to make them visible on screen!
    display.display();
  }
}
#endif  //S88 Module

//--------------------------------------------------------------
void DisplayBoot(byte percent) {
  //draw a bar:
  display.drawRoundRect(14, 47, 100, 6, 2, DISPLAY_TEXT);     //outside (x0, y0, w, h, radius, color)
  display.fillRect(14, 49, percent, 2, DISPLAY_TEXT); //inside
  // drawing commands to make them visible on screen!
  display.display();
}

//--------------------------------------------------------------
//report OTA Update on Display!
#if (defined(ESP8266_MCU)  || defined(ESP32_MCU)) && defined(ESP_OTA)
void DisplayOTAStart() {
  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);             // Start at top-left corner
  display.print(F("Update..."));
  // drawing commands to make them visible on screen!
  display.display();
}
//--------------------------------------------------------------
void DisplayOTAProgress(uint8_t state) {
  display.setTextSize(1);
  DisplayClearPart(16, 36, SCREEN_WIDTH);
  display.printf("Progress: %u%%", state);
  // drawing commands to make them visible on screen!
  DisplayBoot(state);
}
//--------------------------------------------------------------
void DisplayOTAFinish() {
  display.clearDisplay();
  display.drawBitmap(22, 8, Z21Bitmap, 80, 37, DISPLAY_TEXT); //Z21 Logo
  display.display();
}
#endif //OTA Messages

//--------------------------------------------------------------
//Init the Display
void DisplaySetup(void) {
  byte error, address = Z21DISPLAY;
  //suchen der I2C Address des Display am Bus:
  if (address == 0 || address > 127) {
    #if defined(DEBUG)
    Debug.print(F("Display..."));
    #endif
    Wire.begin();
      
    do {
      address++;
      // The i2c_scanner uses the return value of
      // the Write.endTransmisstion to see if
      // a device did acknowledge to the address.
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
    }
    while (error != 0 && address <= 127);
    
    if (error != 0)
      address = Z21DISPLAY; //kein I2C device!
    #if defined(DEBUG)
      if (error == 0) {
        Debug.print(F("0x"));
        if (address < 16)
          Debug.print("0");
        Debug.println(address,HEX);
      }
      else Debug.println(F("nicht gefunden!"));
    #endif
  }

  #if defined(Z21DISPLAY_SH1106)
  display.begin(address, true); // Address 0x3C default
  #if defined(Z21DISPLAY_CONTRAST)
    display.setContrast(Z21DISPLAY_CONTRAST);   //gewünschter Kontrast-Wert 0-255
  #endif    
  #else
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, address);  //<-- See datasheet for Address; 0x3D for 128x32, 0x3C for 128x64
  #if defined(Z21DISPLAY_CONTRAST)
    display.ssd1306_command(SSD1306_SETCONTRAST);
    display.ssd1306_command(Z21DISPLAY_CONTRAST);     //gewünschter Kontrast-Wert 0-255
  #endif  
  #endif

  // Clear the buffer
  display.clearDisplay();
  display.setTextColor(DISPLAY_TEXT);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner with Sw Version
  display.print(F("v"));
  display.print(String(Z21mobileSwVer).substring(0,1));
  display.print(".");
  display.print(String(Z21mobileSwVer).substring(1));

  display.setCursor(0,56);  
  display.print(comp_date); //Build Data
    
  display.drawBitmap(22, 8, Z21Bitmap, 80, 37, DISPLAY_TEXT); //Z21 Logo
  DisplayBoot(0); //Boot up bar...
}
#endif
