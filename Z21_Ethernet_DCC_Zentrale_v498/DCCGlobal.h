//--------------------------------------------------------------
//handle Lok Data to request central for all Interfaces to request Status on Master
void AllLocoData(uint16_t adr, uint8_t *data) {
//uint8_t Steps[0], uint8_t Speed[1], uint8_t F0[2], uint8_t F1[3], uint8_t F2[4], uint8_t F3[5]

  //We are LocoNet Slave user?
  #if defined(LOCONET) && !defined(ESP32_MCU) //handle ESP32 with call back function!
    #if !defined(LnSLOTSRV)
    LNGetSetLocoSlot(adr,false);
    LNupdate();      //LocoNet update
    #if defined(REQEST)
    Debug.print(F("Ln Slave "));
    #endif
    #endif
  #endif

  //We are XpressNet Slave user?
  #if defined(XPRESSNET) 
    if (XpressNet.getOperationModeMaster() == false) {
      XpressNet.getLocoInfo(adr);
      #if defined(REQEST)
      Debug.print(F("Xn Slave "));
      #endif
    }
  #endif

  #if defined(DCC) 
  dcc.getLocoData(adr, data); 
  #endif
  
  #if defined(REQEST)
  Debug.print(F("LOK Data "));
  Debug.print(adr);
  Debug.print("-");
  Debug.print(data[1]); //speed
  Debug.print("-F0-4:");
  Debug.print(data[2] & 0x1F, BIN); //F0, F4, F3, F2, F1
  Debug.print("-F5-12:");
  Debug.print(data[3], BIN); //F5 - F12; Funktion F5 ist bit0 (LSB)
  Debug.print("-F13-20:");
  Debug.print(data[4], BIN); //F13-F20
  Debug.print("-F21-28:");
  Debug.print(data[5], BIN); //F21-F28
  Debug.print("-F29-31:");
  Debug.println(data[2] >> 5, BIN); //F31-F29
  #endif
}

//--------------------------------------------------------------
//DCC handle back the request switch state
void notifyTrnt(uint16_t Adr, bool State, bool active) 
{
  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
  z21.setTrntInfo(Adr, State);
  #endif

  #if defined(LOCONET)
  LNsetTrnt(Adr, State, active);
  #endif
  #if defined(XPRESSNET)
  XpressNet.SetTrntPos(Adr, State, active);
  #endif
  
  #if defined(REQEST)
  Debug.print(F("DCC Trnt "));
  Debug.print(Adr);
  Debug.print("-");
  Debug.print(State);
  Debug.print("-");
  Debug.println(active);
  #endif
}

//-------------------------------------------------------------- 
//DCC return a CV value:
void notifyCVVerify(uint16_t CV, uint8_t value) {
  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
  z21.setCVReturn (CV, value);
  #endif
  
  #if defined(XPRESSNET) 
  XpressNet.setCVReadValue(CV, value);
  #endif

  #if defined(LOCONET) 
  LNsetCVReturn(CV, value, 0);  //CV Read Okay
  #endif
  
  #if defined(REQEST)
  Debug.print(F("Verify CV#"));
  Debug.print(CV+1);
  Debug.print(" - ");
  Debug.print(value);
  Debug.print(" b");
  Debug.println(value, BIN);
  #endif
}

//-------------------------------------------------------------- 
//DCC return no ACK:
void notifyCVNack(uint16_t CV) {
  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
  z21.setCVNack();  //send back to device and stop programming!
  #endif
  
  #if defined(XPRESSNET) 
  XpressNet.setCVNack();
  #endif

  #if defined(LOCONET) 
  LNsetCVReturn(CV, 0, 1);  //CV Read Fails
  #endif
  
  #if defined(REQEST)
  Debug.print("CV#");
  Debug.print(CV+1);
  Debug.println(F(" no ACK"));
  #endif
}

//-------------------------------------------------------------- 
//Feedback the Current Sense value
#if defined(BOOSTER_INT_CURRENT_SHORT_DETECT)
uint16_t notifyCurrentSense() {
  #if defined(INA219) 
    uint16_t mA = ina219.getCurrent_mA();
    if (mA > 65000) //ignore this value!
      return VAmpINT;
    return mA;  
    
  #else
    return analogRead(VAmpIntPin);
    
  #endif  
}
#endif
//-------------------------------------------------------------- 
