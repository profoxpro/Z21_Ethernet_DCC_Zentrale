//-------------------------------------------------------------- 
//Reset the EEPROM to default:
void EEPROM_Load_Defaults() {

  #if defined(ESP32_MCU)
  portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
  portENTER_CRITICAL(&myMutex);
  #endif  

  #if defined(DEBUG)
     Debug.println(F("Set all EEPROM to default!")); 
  #endif  
  
  //DCCInterfaceMaster:
  FIXSTORAGE.FIXMODE(EEPROMRailCom, 1); //RailCom ON
  FIXSTORAGE.FIXMODE(EEPROMRSTsRepeat, 25); //Reset Pakete (start)
  FIXSTORAGE.FIXMODE(EEPROMRSTcRepeat, 6); //Reset Pakete (fortsetzen)
  #if defined(ESP8266_MCU)
    FIXSTORAGE.FIXMODE(EEPROMProgRepeat, 18); //Programmier Pakete, more to better detect ACK!
  #else  
    FIXSTORAGE.FIXMODE(EEPROMProgRepeat, 7); //Programmier Pakete
  #endif  

  FIXSTORAGE.FIXMODE(EES88Moduls, 62);   //S88 max Module
  //IP Werkseinstellung:
  FIXSTORAGE.FIXMODE(EELANDHCP,   0);  //kein DHCP
  FIXSTORAGE.FIXMODE(EELANip,   192);
  FIXSTORAGE.FIXMODE(EELANip+1, 168);
  FIXSTORAGE.FIXMODE(EELANip+2,   0);
  FIXSTORAGE.FIXMODE(EELANip+3, 111);

  //Default VCC Rail and Prog to 20V:
  FIXSTORAGE.FIXMODE(72, 32);
  FIXSTORAGE.FIXMODE(73, 78);
  FIXSTORAGE.FIXMODE(74, 32);
  FIXSTORAGE.FIXMODE(75, 78);

  //Default Prog option:
  FIXSTORAGE.FIXMODE(52, 0); //Power Button behaviour
  FIXSTORAGE.FIXMODE(53, 3);

  //Z21 Library:
  FIXSTORAGE.FIXMODE(1, 0xE8); //Z21 Serial LSB
  FIXSTORAGE.FIXMODE(0, 0x01); //Z21 Serial MSB

  #if defined(ESP_WIFI)
  FIXSTORAGE.commit();
  #endif

  #if defined(ESP32_MCU)  
  portEXIT_CRITICAL(&myMutex);
  #endif 
}

//-------------------------------------------------------------- 
//Check the firmware status need update?:
void EEPROM_Setup() {
  #if defined(DEBUG)
  Debug.print(F("Check EEPROM..."));
  #endif
  
  
  byte SwVerMSB = FIXSTORAGE.read(EEPROMSwVerMSB);
  byte SwVerLSB = FIXSTORAGE.read(EEPROMSwVerLSB);
  //Check if we already run on this MCU?
  if ((SwVerMSB == 0xFF) && (SwVerLSB == 0xFF)) {
    //First Startup, set everything to default!
    EEPROM_Load_Defaults();
    
  }
  //Check if there need to handel an update?
  if ( (SwVerMSB != highByte(Z21mobileSwVer)) || (SwVerLSB != lowByte(Z21mobileSwVer)) ) {
    //Update to new Firmware
    #if defined(DEBUG)
    Debug.println(F("Firmware Update!"));
    #endif
/*
    #if defined(ESP32)
    portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&myMutex);
    #endif
*/
    FIXSTORAGE.FIXMODE(EEPROMSwVerMSB, highByte(Z21mobileSwVer));
    FIXSTORAGE.FIXMODE(EEPROMSwVerLSB, lowByte(Z21mobileSwVer));

    #if defined(ESP_WIFI)
    FIXSTORAGE.commit();
    #endif
/*
    #if defined(ESP32)  
    portEXIT_CRITICAL(&myMutex);
    #endif 
    */
  }
  else {
    #if defined(DEBUG)
      Debug.println(F("OK"));
    #endif
  }
  #if defined(ESP32)
  yield();
  #endif
}
//-------------------------------------------------------------- 
