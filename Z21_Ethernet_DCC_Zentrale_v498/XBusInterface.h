//--------------------------------------------------------------
/*

  XpressNetMaster Interface for Arduino
  
Funktionsumfang:  
- Fahren per LokMaus2 und MultiMaus
- Schalten von DCC Weichen mit der MultiMaus
- CV Programmieren CV Direct only

  Copyright (c) by Philipp Gahtow, year 2022
*/
#if defined(XPRESSNET)

//**************************************************************
//byte XNetUserOps = 0x00;
//byte XNetReturnLoco = 0x00;

//--------------------------------------------------------------
//Change Power Status
void notifyXNetPower(uint8_t State) {
  if (Railpower != State) {
    #if defined(XnDEB)
    Debug.print("XNet ");
      #if !defined(DEBUG)
      Debug.print("Power: ");
      Debug.println(State, HEX);
      #endif
    #endif
    globalPower(State);
  }
}

//--------------------------------------------------------------
void notifyXNetgiveLocoInfo(uint8_t UserOps, uint16_t Address) {
  //XNetReturnLoco |= 0x01;
  //XNetUserOps = UserOps;
  #if defined(DCC) 
  //dcc.getLocoStateFull(Address, false); //request for XpressNet only!
  uint8_t ldata[6];
  AllLocoData(Address, ldata);  //uint8_t Steps[0], uint8_t Speed[1], uint8_t F0[2], uint8_t F1[3], uint8_t F2[4], uint8_t F3[5]
  if (ldata[0] == 0x03)  //128 Steps?
      ldata[0]++;  //set Steps to 0x04
  XpressNet.SetLocoInfo(UserOps, ldata[0], ldata[1], ldata[2] & 0x1F, ldata[3]); //UserOps,Steps,Speed,F0,F1
  #endif
}

//--------------------------------------------------------------
void notifyXNetgiveLocoFunc(uint8_t UserOps, uint16_t Address) {
  //XNetReturnLoco |= 0x02;
  //XNetUserOps = UserOps;
  #if defined(DCC) 
  //dcc.getLocoStateFull(Address, false); //request for XpressNet only!
  XpressNet.SetFktStatus(UserOps, dcc.getFunktion13to20(Address), dcc.getFunktion21to28(Address)); //Fkt4, Fkt5
  #endif
}

//--------------------------------------------------------------
void notifyXNetgiveLocoMM(uint8_t UserOps, uint16_t Address) {
  uint8_t ldata[6];
  AllLocoData(Address, ldata);  //uint8_t Steps[0], uint8_t Speed[1], uint8_t F0[2], uint8_t F1[3], uint8_t F2[4], uint8_t F3[5]
  if (ldata[0] == 0x03)  //128 Steps?
      ldata[0]++;  //set Steps to 0x04
  XpressNet.SetLocoInfoMM(UserOps, ldata[0], ldata[1], ldata[2] & 0x1F, ldata[3], ldata[4], ldata[5]); //Steps,Speed,F0,F1,F2,F3
}

//--------------------------------------------------------------
void notifyXNetLocoDrive14(uint16_t Address, uint8_t Speed) {
  #if defined(LOCONET)
  sendLNSPD(Address, map(Speed, -14, 14, -128, 128)); 
  #endif
  
  #if defined(XnDEB)
  Debug.print("XNet A:");
  Debug.print(Address);
  Debug.print(", S14:");
  Debug.println(Speed, BIN);
  #endif

  #if defined(DCC) 
  if (Speed == 0) 
    dcc.setSpeed14(Address, (dcc.getLocoDir(Address) << 7) | (Speed & B01111111));
  else dcc.setSpeed14(Address, Speed);
  //dcc.getLocoStateFull(Address);      //request for other devices
  #endif
  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
  z21.setLocoStateExt (Address);
  #endif
}

//--------------------------------------------------------------
void notifyXNetLocoDrive28(uint16_t Address, uint8_t Speed) {
  #if defined(LOCONET)
  sendLNSPD(Address, map(Speed, -28, 28, -128, 128)); 
  #endif
  
  #if defined(XnDEB)
  Debug.print("XNet A:");
  Debug.print(Address);
  Debug.print(", S28:");
  Debug.println(Speed, BIN);
  #endif

  #if defined(DCC) 
  if (Speed == 0)
    dcc.setSpeed28(Address, (dcc.getLocoDir(Address) << 7) | (Speed & B01111111));
  else dcc.setSpeed28(Address, Speed);
  //dcc.getLocoStateFull(Address);      //request for other devices
  #endif
  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
  z21.setLocoStateExt (Address);
  #endif
}

//--------------------------------------------------------------
void notifyXNetLocoDrive128(uint16_t Address, uint8_t Speed) {
  #if defined(LOCONET)
  sendLNSPD(Address, Speed);
  #endif
  
  #if defined(XnDEB)
  Debug.print("XNet A:");
  Debug.print(Address);
  Debug.print(", S128:");
  Debug.println(Speed, BIN);
  #endif

  #if defined(DCC) 
  //if ((Speed & 0x7F) == 0) 
//    dcc.setSpeed128(Address, (dcc.getLocoDir(Address) << 7) | (Speed & B01111111));
  //else 
  dcc.setSpeed128(Address, Speed);
  //dcc.getLocoStateFull(Address);      //request for other devices
  #endif
  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
  z21.setLocoStateExt (Address);
  #endif
}

//--------------------------------------------------------------
void notifyXNetLocoFunc1(uint16_t Address, uint8_t Func1) {
  #if defined(XnDEB)
  Debug.print("XNet A:");
  Debug.print(Address);
  Debug.print(", F1:");
  Debug.println(Func1, BIN);
  #endif

  #if defined(DCC) 
  dcc.setFunctions0to4(Address, Func1);	//- F0 F4 F3 F2 F1
  //dcc.getLocoStateFull(Address);      //request for other devices
  #endif
  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
  z21.setLocoStateExt (Address);
  #endif

  #if defined(LOCONET)
  byte DIRF = Func1 | (!dcc.getLocoDir(Address) << 5);  //invertierte Fahrtrichtung!
    //Beim einschalten einer Funktion wird die Fahrtrichtung geändert - deshalb hier invertiert!
  sendLNDIRF(Address, DIRF);
  #endif
}

//--------------------------------------------------------------
void notifyXNetLocoFunc2(uint16_t Address, uint8_t Func2) {
  #if defined(XnDEB)
  Debug.print("XNet A:");
  Debug.print(Address);
  Debug.print(", F2:");
  Debug.println(Func2, BIN);
  #endif

  #if defined(DCC) 
  dcc.setFunctions5to8(Address, Func2);	//- F8 F7 F6 F5
  //dcc.getLocoStateFull(Address);      //request for other devices
  #endif
  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
  z21.setLocoStateExt (Address);
  #endif

  #if defined(LOCONET)
  sendLNSND(Address, Func2);
  #endif
}

//--------------------------------------------------------------
void notifyXNetLocoFunc3(uint16_t Address, uint8_t Func3) {
  #if defined(XnDEB)
  Debug.print("XNet A:");
  Debug.print(Address);
  Debug.print(", F3:");
  Debug.println(Func3, BIN);
  #endif

  #if defined(DCC) 
  dcc.setFunctions9to12(Address, Func3);	//- F12 F11 F10 F9
  //dcc.getLocoStateFull(Address);      //request for other devices
  #endif
  
  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
  z21.setLocoStateExt (Address);
  #endif

  #if defined(LOCONET) 
  sendLNF3(Address, Func3);
  #endif
}

//--------------------------------------------------------------
void notifyXNetLocoFuncX(uint16_t Address, uint8_t group, uint8_t Func) {
  #if defined(XnDEB)
  Debug.print("XNet A:");
  Debug.print(Address);
  Debug.print(", F");
  Debug.print(group);
  Debug.print(":");
  Debug.println(Func, BIN);
  #endif

  if (group == 0x04) {
    #if defined(DCC) 
    dcc.setFunctions13to20(Address, Func);	//F20 F19 F18 F17 F16 F15 F14 F13
    #endif
    #if defined(LOCONET)
    sendLNF4(Address, Func);
    #endif
  }
  else if (group == 0x05) {
    #if defined(DCC) 
    dcc.setFunctions21to28(Address, Func);  //F28 F27 F26 F25 F24 F23 F22 F21
    #endif
    #if defined(LOCONET)
    sendLNF5(Address, Func);
    #endif
  }
  else if (group == 0x06) {
    #if defined(DCC) 
    dcc.setFunctions29to36(Address, Func); 
    #endif
  }

  #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
  z21.setLocoStateExt (Address);
  #endif

  
}

//--------------------------------------------------------------
void notifyXNetTrntInfo(uint8_t UserOps, uint16_t Address, uint8_t data) {
  uint16_t adr = ((Address * 4) + ((data & 0x01) * 2));
  byte pos = data << 4;
  bitWrite(pos, 7, 1);  //command completed!
  if (dcc.getBasicAccessoryInfo(adr) == false)
    bitWrite(pos, 0, 1);
  else bitWrite(pos, 1, 1);  
  if (dcc.getBasicAccessoryInfo(adr+1) == false)
    bitWrite(pos, 2, 1);  
  else bitWrite(pos, 3, 1);    
  XpressNet.SetTrntStatus(UserOps, Address, pos);
  #if defined(XnDEB)
    Debug.print("XNet: ");
    Debug.print(adr);
    Debug.print(", P:");
    Debug.println(pos, BIN);
  #endif
}

//--------------------------------------------------------------
void notifyXNetTrnt(uint16_t Address, uint8_t data) {
    #if defined(XnDEB)
    Debug.print("XNet TA:");
    Debug.print(Address);
    Debug.print(", P:");
    Debug.println(data, BIN);
    #endif

    #if defined(DCC) 
    dcc.setBasicAccessoryPos(Address,data & 0x01, bitRead(data,3));    //Adr, left/right, activ
    #endif
}

//--------------------------------------------------------------
void notifyXNetDirectCV(uint16_t CV, uint8_t data) {
  #if defined(XnDEB)
  Debug.print(F("XNet CV#:"));
  Debug.print(CV+1);
  Debug.print(" - ");
  Debug.println(data);
  #endif

  #if defined(DCC) 
  dcc.opsProgDirectCV(CV,data);  //return value from DCC via 'notifyCVVerify'
  #endif
}

//--------------------------------------------------------------
void notifyXNetDirectReadCV(uint16_t cvAdr) {
  #if defined(DCC) 
  dcc.opsReadDirectCV(cvAdr);  //read cv
  #endif
  #if defined(XnDEB)
  Debug.print(F("XNet CV#:"));
  Debug.println(cvAdr+1);
  #endif
}

//--------------------------------------------------------------
void notifyXNetPOMwriteByte (uint16_t Adr, uint16_t CV, uint8_t data) {
  #if defined(XnDEB)
  Debug.print(F("XNet POM:"));
  Debug.print(Adr);
  Debug.print(" CV#");
  Debug.print(CV+1);
  Debug.print("-");
  Debug.println(data);
  #endif  
  #if defined(DCC)
  dcc.opsProgramCV(Adr, CV, data);  //set decoder byte
  #endif
}

//--------------------------------------------------------------
void notifyXNetPOMwriteBit (uint16_t Adr, uint16_t CV, uint8_t data) {
  #if defined(XnDEB)
  Debug.print(F("XNet POM Bit:"));
  Debug.print(Adr);
  Debug.print("- CV: ");
  Debug.print(CV+1);
  Debug.print(" - ");
  Debug.println(data, BIN);
  #endif  
  #if defined(DCC)
  dcc.opsPOMwriteBit(Adr, CV, data);  //set decoder bit
  #endif
}

#endif
