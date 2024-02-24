//--------------------------------------------------------------------------------------------
//POWER set configuration:
void globalPower (byte state) {
  if (Railpower != state) {

    if (Railpower == csServiceMode && state == csShortCircuit) {
      #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
        z21.setCVNackSC();  //response SHORT while Service Mode!
      #endif  
      #if defined(XPRESSNET) 
        XpressNet.setCVNackSC();
      #endif 
    }
    
    #if defined(Z21DISPLAY)
      DisplayCounter = 0xFF;
      if ((Railpower == csShortCircuit) || (state == csShortCircuit)) {
        Railpower = state;
        DisplayUpdateRailPower(true);
      }
      else {
        Railpower = state;
        DisplayUpdateRailPower(false);  
      }
    #else
      Railpower = state;
    #endif
    
    #if defined(DEBUG)
    Debug.print(F("Power: "));
    Debug.println(state);
    #endif
    switch (state) {
      case csNormal: 
        #if defined(DCC)
          dcc.setpower(ON);
          #if defined(ProgRelaisPin) 
            #if defined(PROG_OUT_INVERT)
              digitalWrite(ProgRelaisPin, HIGH);     //ProgTrack 
            #else
              digitalWrite(ProgRelaisPin, LOW);     //ProgTrack 
            #endif
          #endif  
          #if defined(INA219)
            ina219.setPGain(PG_320);     //Default setting range (320mV full range, ~16 A max. current, ~4mA resolution with 20 mOhm shunt)
          #endif 
        #endif
        #if defined(BOOSTER_EXT)
        if (digitalRead(ShortExtPin) == LOW)
          digitalWrite(GoExtPin, BOOSTER_EXT_ON);
        #endif
       
        #if (defined(BOOSTER_INT) && !defined(BOOSTER_INT_NDCC))
        digitalWrite(GoIntPin, BOOSTER_INT_ON);
        #endif
     
      break;
      case csTrackVoltageOff: 
        #if defined(DCC)
          dcc.setpower(OFF);
          #if defined(ProgRelaisPin) 
            #if defined(PROG_OUT_INVERT)
              digitalWrite(ProgRelaisPin, HIGH);     //ProgTrack 
            #else
              digitalWrite(ProgRelaisPin, LOW);     //ProgTrack 
            #endif
          #endif  
        #endif
        #if defined(BOOSTER_EXT)
        digitalWrite(GoExtPin, BOOSTER_EXT_OFF);
        #endif
        
        #if (defined(BOOSTER_INT) && !defined(BOOSTER_INT_NDCC))
        digitalWrite(GoIntPin, BOOSTER_INT_OFF);
        #endif
        
      break;
      case csServiceMode:
        #if defined(DCC) 
          dcc.setpower(SERVICE); //already on!
          #if defined(ProgRelaisPin) 
            #if defined(PROG_OUT_INVERT)
             digitalWrite(ProgRelaisPin, LOW);     //ProgTrack 
            #else
              digitalWrite(ProgRelaisPin, HIGH);     //ProgTrack 
            #endif
          #endif 
          #if defined(INA219)
            ina219.setPGain(PG_80);     //setting range (160mV full range
          #endif 
        #endif
        #if defined(BOOSTER_EXT)
          #if defined(BOOSTER_INT)
          digitalWrite(GoExtPin, BOOSTER_EXT_OFF);
          #else
          if (digitalRead(ShortExtPin) == LOW)
            digitalWrite(GoExtPin, BOOSTER_EXT_ON);
          #endif
        #endif

        #if (defined(BOOSTER_INT) && !defined(BOOSTER_INT_NDCC))
        digitalWrite(GoIntPin, BOOSTER_INT_ON);
        #endif
        
      break;
      case csShortCircuit: 
        #if defined(DCC)
          dcc.setpower(SHORT);  //shut down via GO/STOP just for the Roco Booster
          #if defined(ProgRelaisPin) 
            #if defined(PROG_OUT_INVERT)
              digitalWrite(ProgRelaisPin, HIGH);     //ProgTrack 
            #else
              digitalWrite(ProgRelaisPin, LOW);     //ProgTrack 
            #endif
          #endif  
        #endif
        #if defined(BOOSTER_EXT)
        digitalWrite(GoExtPin, BOOSTER_EXT_OFF);
        #endif
        
        #if (defined(BOOSTER_INT) && !defined(BOOSTER_INT_NDCC))
        digitalWrite(GoIntPin, BOOSTER_INT_OFF);
        #endif
        
      break;
      case csEmergencyStop:
        #if defined(DCC)
        dcc.eStop();  
        #endif
      break;
    }
    if (Railpower == csShortCircuit)
      digitalWrite(ShortLed, HIGH);   //Short LED show State "short"
    if (Railpower == csNormal)  
      digitalWrite(ShortLed, LOW);   //Short LED show State "normal" 
    #if defined(LAN) || defined(WIFI) || defined(ESP_WIFI)
    z21.setPower(Railpower);
    #endif
    #if defined(XPRESSNET)
    XpressNet.setPower(Railpower);  //send to XpressNet
    #endif
    #if defined(LOCONET)
    LNsetpower(); //send to LocoNet
    #endif
  }
}

//--------------------------------------------------------------------------------------------
//from DCCPacketScheduler -> notify power state when change into Programming Mode
void notifyRailpower(uint8_t state) {
  if (Railpower != state) {
    #if defined(Z21DEBUG)  
    Debug.print(F("dcc "));
    #endif          

    globalPower(state);
  }
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#if defined(DCC)

//--------------------------------------------------------------------------------------------
uint16_t getRailmA() {
 #if defined (INA219)          // current via INA219 
    uint16_t mA = ina219.getCurrent_mA();
    if (mA > 65000) //ignore this value!
      return VAmpINT;
    return mA;  
 #elif defined(ESP32_MCU)
    return (analogRead(VAmpIntPin)) / senseResist * 1000 * (3.3/4096);  //adjusted to Uref more accurate
 #elif defined(ESP8266_MCU)    //Wemos D1 mini has a voltage divider!
    uint16_t mA = analogRead(VAmpIntPin);
    if (mA > 3)  //remove to low values!
      return mA / senseResist * 1000 * (3.3/1024);  //adjusted to Uref more accurate
    return 0;  
 #elif defined(AREF_1V1)
    return analogRead(VAmpIntPin) / senseResist * 1000 * (Uref/1024);  //adjusted to Uref more accurate
 #else
    return analogRead(VAmpIntPin) * 10; //old!
 #endif  
}

//--------------------------------------------------------------------------------------------
uint16_t getRailVolt() {
  #if defined (INA219)
    return (float) ((ina219.getBusVoltage_V() + (ina219.getShuntVoltage_mV()/1000)) * 1000);     //rail voltage, correctded (shunt voltage)
  #elif defined(ESP32_MCU)
    return ((float) analogRead(VoltIntPin) * 1000 * (100/4.7) * (3.3/496)); //adjusted to Uref more accurate
  #elif defined(MEGA_MCU)
    #if defined(AREF_1V1)
    return ((float) analogRead(VoltIntPin) * 1000 * (100/4.7) * (Uref/1024)); //adjusted to Uref more accurate
    #else
    return ((float)(analogRead(VoltIntPin)-121) / 0.008); //old!
    #endif
  #else  //other MCU:
    return 0;
  #endif  
}

//--------------------------------------------------------------------------------------------
void ShortDetection() { 
  //Short Circuit?
  //Check BOOSTER extern
  #if defined(BOOSTER_EXT)
  if ((digitalRead(ShortExtPin) == HIGH) && (digitalRead(GoExtPin) == BOOSTER_EXT_ON) && (Railpower != csShortCircuit)) {  
    ShortTime++;
    if(ShortTime == DetectShortCircuit) {
        globalPower(csShortCircuit);
        #if defined(DEBUG)
        Debug.println(F("TRACK_SHORT_CIRCUIT EXT"));
        #endif
    }
  }
  else ShortTime = 0;
  #endif
  //Check BOOSTER2 (z.B. TLE5206)
  #if defined(BOOSTER_INT)
  //---------------Short2 for CDE external Booster----------------------------------
  #if defined(BOOSTER_EXT_CDE)
  if ((digitalRead(ShortIntPin) == LOW) && (Railpower != csShortCircuit)) {
    globalPower(csShortCircuit);
    #if defined(DEBUG)
    Debug.println(F("TRACK_SHORT_CIRCUIT CDE"));
    #endif
  }
  //---------------Short2 TLE detection----------------------------------
  #elif defined(BOOSTER_INT_TLE5206)
    #if defined(BOOSTER_INT_NDCC)
      if ((digitalRead(ShortIntPin) == LOW) && (Railpower != csShortCircuit)) {
    #else
    //---------------Old: without RailCom support----------------------------------
      if ((digitalRead(ShortIntPin) == LOW) && (digitalRead(GoIntPin) == BOOSTER_INT_ON) && (Railpower != csShortCircuit)) {
    #endif
        globalPower(csShortCircuit);
        #if defined(DEBUG)
        Debug.println(F("TRACK_SHORT_CIRCUIT_INT"));
        #endif
      }
  #endif
  
  #if defined(BOOSTER_INT_CURRENT_SHORT_DETECT)
    //Check if RailCom is on the rail?
    if (dcc.getRailComStatus() == false) {
/*
      #if defined (INA219)          // current via INA219 
        float mA = ina219.getCurrent_mA() - 5;    //reduce 5mA current power of H-Bridge
        if (mA >= 0)
          VAmpINT = mA;
        if (ina219.getOverflow()) {
          #if defined(DEBUG)
            Debug.println(F("INA Overflow!"));
          #endif  
        }
  */
      #if defined(INA219)   
        VAmpINT = getRailmA();
                    
      #elif defined(ESP8266_MCU)
        //slow down the readings to not stop WIFI! & change reading time to detect ACK
        if (Railpower != csServiceMode) {
          if( millis() % VAmpReadTimeNormal == 0 ) { //slow down!
            VAmpINT = analogRead(VAmpIntPin);   //don't middle the sense! Do not read so often!!!
          }
        }
      #else //other MCU
        //Middle the current sense pin
        CalcVAmpINT += analogRead(VAmpIntPin);
        CountVAmpINT += 1;
        if (CountVAmpINT > VAmpINTMesureCount) {
          VAmpINT = CalcVAmpINT / CountVAmpINT;
          CalcVAmpINT = 0;
          CountVAmpINT = 0;
        }
      #endif

      if ((VAmpINT >= DETECT_SHORT_INT_VALUE) && (Railpower != csShortCircuit)) {
        /*
        #if defined(DEBUG)
          Debug.print(digitalRead(DCCPin));
          Debug.print(digitalRead(GoIntPin));
          Debug.print("-");
          Debug.print(millis() - ShortTimeINT);
          Debug.print(" mA: ");
          Debug.print(VAmpINT);
        #endif
        */
        ShortTimeINTcounter = 6;    //ignore low values!
        if (((millis() - ShortTimeINT) > DETECT_SHORT_INT_WAIT) || (VAmpINT >= DETECT_SHORT_INT_VALUE + (DETECT_SHORT_INT_VALUE / 2) ) ) {
            globalPower(csShortCircuit);
            #if defined(DEBUG)
              Debug.print(VAmpINT);
              Debug.print("-t");
              Debug.print(millis() - ShortTimeINT);
              Debug.println(F(" TRACK_SHORT_INT"));
            #endif
        }
      }
      else {
        if (ShortTimeINTcounter > 0) {
          ShortTimeINTcounter--;
          /*
          #if defined(DEBUG)
            Debug.print(dcc.getRailComStatus());
            Debug.print(digitalRead(DCCPin));
            Debug.print(digitalRead(GoIntPin));
            Debug.print("-");
            Debug.print(VAmpINT);
            Debug.print(" mA ");
            Debug.println(ShortTimeINTcounter);
          #endif
          */
        }
        else ShortTimeINT = millis();
      }
    }
    #endif
  #endif
}
#endif

//--------------------------------------------------------------------------------------------
void updateLedButton() {
  //read out last LED status:
  bool lastLedState = false;  //OFF
  #if defined(POWER_LED_INVERT)
  if (digitalRead(DCCLed) == LOW)
  #else
  if (digitalRead(DCCLed) == HIGH)
  #endif
    lastLedState = true;  //ON
    
  //Button to control Railpower state
  pinMode(Z21ButtonPin, INPUT_PULLUP); 
  if ((digitalRead(Z21ButtonPin) == LOW) && (Z21ButtonLastState == false)) {  //Button DOWN
    #if !(defined(ADD_ACK_COMP) && defined(ESP8266_MCU))
      Z21ButtonLastState = true;
      LEDcount = millis();
    #endif
  }
  else {
    if ((digitalRead(Z21ButtonPin) == HIGH) && (Z21ButtonLastState == true)) {  //Button UP
       Z21ButtonLastState = false;
       #if defined(DEBUG)
         Debug.print(F("Button "));
      #endif
      if (Railpower == csNormal) {
        if(millis() - LEDcount > 750) { //push long?
          if (FIXSTORAGE.read(52) == 0x00)   //Power-Button (short): 0=Gleisspannung aus, 1=Nothalt  
            globalPower(csEmergencyStop);  
          else globalPower(csTrackVoltageOff);
        }
        else {
          if (FIXSTORAGE.read(52) == 0x00)  //Power-Button (short): 0=Gleisspannung aus, 1=Nothalt  
            globalPower(csTrackVoltageOff);
          else globalPower(csEmergencyStop);
        }
      }
      else globalPower(csNormal);
      LEDcount = millis();
    }
  }
  //reset to the last State:
  if (lastLedState) {
    pinMode(DCCLed, OUTPUT); 
    #if defined(POWER_LED_INVERT)
      digitalWrite(DCCLed, LOW);
    #else
      digitalWrite(DCCLed, HIGH);
    #endif
  }

  //Update LED  
  if (Railpower == csNormal) {
    pinMode(DCCLed, OUTPUT);  
    #if defined(POWER_LED_INVERT)
      digitalWrite(DCCLed, LOW);
    #else 
      digitalWrite(DCCLed, HIGH);
    #endif
    #if defined(Z21DISPLAY)
      if(millis() % DISPLAY_POWER_LOGO == 0 ) 
        DisplayUpdateRailPower(false); //Update OLED Display
    #endif  
  }
  else {  //Flash:
    unsigned long currentMillis = millis(); 
    if (currentMillis > LEDcount) {
      pinMode(DCCLed, OUTPUT); 
      if (Railpower == csTrackVoltageOff) {
        if (lastLedState)
          LEDcount = currentMillis + 1100;    //long OFF
        else LEDcount = currentMillis + 300;  //short ON
      }
      else if (Railpower == csEmergencyStop) {
        if (lastLedState)
          LEDcount = currentMillis + 80;    //short OFF
        else LEDcount = currentMillis + 700;  //long ON
      }
      else if (Railpower == csShortCircuit) {
        LEDcount = currentMillis + 200;  //short flash
        #if defined(Z21DISPLAY)
           DisplayUpdateRailPower(false); //Update OLED Display
        #endif
      }
      else {  //csServiceMode:
        LEDcount = currentMillis + 100;  //fast short flash 
        #if defined(Z21DISPLAY)
          if(currentMillis % 5 == 0 ) {
            DisplayUpdateRailPower(false); //Update OLED Display
          }
        #endif
      }
      if (lastLedState) 
        pinMode(DCCLed, INPUT);   //OFF
      else {
        #if defined(POWER_LED_INVERT)
          digitalWrite(DCCLed, LOW);  //ON
        #else
          digitalWrite(DCCLed, HIGH); //OFF
        #endif
      }
    }
  }
}
