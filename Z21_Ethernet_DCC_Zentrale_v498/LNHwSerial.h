//--------------------------------------------------------------
/*

  LocoNet Hardware Serial connector
  only for ESP8266 and ESP32

  Use a share
  
  Copyright (c) by Philipp Gahtow, year 2022
  
*/

#if defined(LOCONET) && (defined(ESP8266_MCU) || defined(ESP32_MCU)) 

#include <LocoNet.h>   //include LocoNet OP Codes

#include <SoftwareSerial.h>
SoftwareSerial LocoNetTX;     //Software Serial for TX Signal
#define LocoNetRX Serial      //Hardware Serial for RX Signal

#define LnBufferMaxData 21  //length of LocoNet data packet
#define LnBufferSize 5      //size of the RX and TX buffer
#define LnMsgTimeout 1000   //timeout to revert a msg in µs

typedef struct  //Msg Buffer
{
  uint8_t read = 0;     //zähler lesen
  uint8_t write = 0;    //zähler schreiben
  uint8_t data[LnBufferSize][LnBufferMaxData];  //zu sendende Daten
} LnBuffer;

uint8_t writeRXpos = 0;
LnBuffer LnRX;
LnBuffer LnTX;

unsigned long previousData = 0;        // will store last time data war received

//--------------------------------------------------------------
void LocoNetHw_init() {
  //ESP8266 and ESP32:
  LocoNetRX.end();    //stop running configuration
  LocoNetRX.begin(16660);  //set speed to Hw Serial
  
  //UCRXI = Invert RX
  //UCTXI = Invert TX
  //UCBN  = DataBits Count (2bin) 0:5bit, 1:6bit, 2:7bit, 3:8bit
  //UCSBN = StopBits Count (2bit) 0:disable, 1:1bit, 2:1.5bit, 3:2bit
  //U0C0 = BIT(UCRXI) | BIT(UCBN) | BIT(UCBN+1) | BIT(UCSBN); 
  // Inverse RX, 11 = 8bit Data, 01 = 1bit Stop

  LocoNetTX.begin(16660, SWSERIAL_8N1, LNTxPin, LNTxPin, true); //Set Up SoftwareSerial asyncron
  LocoNetTX.enableTx(true);   //enable TX Mode!
  
  //clear the RX and TX Buffer:
  for (uint8_t i; i < LnBufferSize; i++) {
    for (uint8_t m; m < LnBufferMaxData; m++) {
      LnRX.data[i][m] = 0xFF;    
      LnTX.data[i][m] = 0xFF;
    }
  }
  LnRX.read = 0;
  LnRX.write = 0;
  LnTX.read = 0;
  LnRX.write = 0;
}


//--------------------------------------------------------------
bool LocoNet_available() {
  return LnRX.read != LnRX.write;
}

//--------------------------------------------------------------
uint8_t LocoNet_readLength() {
  if (LocoNet_available()) {
    uint8_t len = LnRX.data[LnRX.read][0] >> 5; //1. Byte = length
    if (len == B100)
      return 2;
    else if (len == B101)
      return 4;
    else if (len == B110)
      return 6;
    else return LnRX.data[LnRX.read][1] & 0x7F;  //Länge in 2. Byte codiert.
  }
  return 0;
}

//--------------------------------------------------------------
void LocoNet_readData(uint8_t *data) {
  if (LocoNet_available()) {
    uint8_t len = LocoNet_readLength();
    for (uint8_t s = 0; s < len; s++) {
      data[s] = LnRX.data[LnRX.read][s];
    }
    LnRX.data[LnRX.read][0] = 0xFF; //clear
    LnRX.read++;
    if (LnRX.read == LnBufferSize)
      LnRX.read = 0;
  }
}

//--------------------------------------------------------------
void LocoNet_sendData(uint8_t *data) {
  uint8_t len = data[0] >> 5;  //read 1. Byte
  //calculate the length:
  if (len == B100)
    len = 2;
  else if (len == B101)
    len = 4;
  else if (len == B110)
    len = 6;
  else len = (data[1] & 0x7F); //2. Byte in der Nachricht ein 7-Bit-Zählwert ist (BYTE COUNT) 
  uint8_t XOR = 0xFF;  
  //copy Data to TX Buffer:
  for(uint8_t s = 0; s < (len-1); s++) {  //without XOR
    LnTX.data[LnTX.write][s] = data[s];
    XOR ^= data[s];  
  }
  LnTX.data[LnTX.write][len-1] = XOR; //last byte
  data[len-1] = XOR;  //add XOR to requested data
  LnTX.write++;
  if (LnTX.write == LnBufferSize)
     LnTX.write = 0;
}

//--------------------------------------------------------------
void LocoNet_updateRX() {
  if (LocoNetRX.available()) {
    previousData = micros();  //save in time
    uint8_t d = LocoNetRX.read();
    LnRX.data[LnRX.write][writeRXpos] = d;   //save data
    uint8_t len = LnRX.data[LnRX.write][0] >> 5;  //read 1. Byte
    //calculate the length:
    if (len == B100)
      len = 2;
    else if (len == B101)
      len = 4;
    else if (len == B110)
      len = 6;
    else if (writeRXpos >= 1) {
      len = (LnRX.data[LnRX.write][1] & 0x7F); //2. Byte in der Nachricht ein 7-Bit-Zählwert ist (BYTE COUNT) 
      if (len >= LnBufferMaxData) {  //ERROR LENGTH
        LnRX.data[LnRX.write][0] = 0xFF;  //reset
        writeRXpos = 0;
      }
    }
    
    writeRXpos++;
    if ((writeRXpos == len) && (len > 1)) { //accept only data with min. length of 2
      writeRXpos = 0;
      uint8_t cksum = 0;  //Prüfsumme
      for (uint8_t i = 0; i < len; i++) {
        cksum ^= LnRX.data[LnRX.write][i];
      }
      if (cksum == 0xFF) {
        LnRX.write++;
        if (LnRX.write == LnBufferSize)
            LnRX.write = 0;
      }
      #if defined(LnDEB)
      else Debug.println(F("LN XOR Fail!"));
      #endif
    }
  }
  else {
    if ((writeRXpos != 0) && (micros() - previousData >= LnMsgTimeout)) {   //Timeout!
      LnRX.data[LnRX.write][0] = 0xFF;  //reset
      writeRXpos = 0;
    }
  }
}

//--------------------------------------------------------------
void LocoNetRXTXupdate() {
  //check if we have received data?
  LocoNet_updateRX();
  
  //check if we have to send data?
  if (LnTX.read != LnTX.write) {
    uint8_t writepos = 0;
    uint8_t len = LnTX.data[LnTX.read][writepos] >> 5;
    if (len == B100)
      len = 2;
    else if (len == B101)
      len = 4;
    else if (len == B110)
      len = 6;
    else len = (LnTX.data[LnTX.read][writepos + 1] & 0x7F);  // 2. Byte = length!
    do {
      LocoNetTX.write(LnTX.data[LnTX.read][writepos]);
      LocoNet_updateRX();
      if (writeRXpos == writepos && LnRX.data[LnRX.write][writeRXpos] != LnTX.data[LnTX.read][writepos]) {
        return; //stop here!!!!!!!!!!!!!!!!!!
        //we will not go further!!!!!!!!!
      }
      writepos++;
    } while (writepos < len);
    //next data:
    LnTX.read++;
    if (LnTX.read == LnBufferSize)
      LnTX.read = 0;
  }
}


#endif
