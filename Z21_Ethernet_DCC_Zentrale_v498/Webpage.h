//--------------------------------------------------------------------------------------------
#if defined(HTTPCONF) || defined(ESP_HTTPCONF)
//--------------------------------------------------------------------------------------------
#if defined(LAN)
void Webconfig() {
  EthernetClient client = server.available();
  if (client) {
    String receivedText = String(50);
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (receivedText.length() < 50) {
          receivedText += c;
        }
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println(F("Connection: close"));  // the connection will be closed after completion of the response
          //client.println(F("Refresh: 5"));  // refresh the page automatically every 5 sec
          client.println();   //don't forget this!!!
          //Website:
          client.println(F("<!DOCTYPE html><html><head>"));
          client.println(F("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>"));
          client.println(F("<title>Z21</title></head><body><h1>Z21</h1>"));
          //----------------------------------------------------------------------------------------------------          
          int firstPos = receivedText.indexOf("?");
          if (firstPos > -1) {
            byte lastPos = receivedText.indexOf(" ", firstPos);
            String theText = receivedText.substring(firstPos+3, lastPos); // 10 is the length of "?A="
            byte S88Pos = receivedText.indexOf("S=");
            #if defined(S88N)
              S88Module = receivedText.substring(S88Pos+2, receivedText.indexOf("HTTP")-1).toInt();
            #endif  
            byte Sip = theText.indexOf("A="); //Start IP, nur wenn DHCP aktiv ist Ausdruck vorhanden!
            if (Sip == 0xFF) {  //DHCP off
              LAN_DHCP = false;
              Sip = 0;
            }
            else {
              LAN_DHCP = true;
              Sip += 2;
            }
            byte Aip = theText.indexOf("&B=", Sip);
            byte Bip = theText.indexOf("&C=", Aip);
            byte Cip = theText.indexOf("&D=", Bip);
            byte Dip = theText.substring(Cip+3, S88Pos).toInt();
            Cip = theText.substring(Bip+3, Cip).toInt();
            Bip = theText.substring(Aip+3, Bip).toInt();
            Aip = theText.substring(Sip, Aip).toInt();
            LAN_ip[0] = Aip;
            LAN_ip[1] = Bip;
            LAN_ip[2] = Cip;
            LAN_ip[3] = Dip;
            #if defined(DEBUG)
            #if defined(S88N)
              Debug.print("S88: ");
              Debug.println(S88Module);
            #endif
            if (LAN_DHCP)
              Debug.print("DHCP ");
            Debug.print("IP: ");
            Debug.println(LAN_ip);
            #if !defined(SOFT_RESET)
            Debug.println(F("-> Restart to accept!"));
            #endif
            #endif

            #if !defined(SOFT_RESET)
            client.print(F("<dialog open><p>Reset Z21 to accept!</p></dialog>"));
            #endif

            FIXSTORAGE.FIXMODE(EELANDHCP, LAN_DHCP);
            FIXSTORAGE.FIXMODE(EELANip, Aip);
            FIXSTORAGE.FIXMODE(EELANip+1, Bip);
            FIXSTORAGE.FIXMODE(EELANip+2, Cip);
            FIXSTORAGE.FIXMODE(EELANip+3, Dip);

            #if defined(ESP_WIFI)
            FIXSTORAGE.commit();
            #endif
            
            #if defined(S88N)
            if (FIXSTORAGE.read(EES88Moduls) != S88Module) {
              FIXSTORAGE.FIXMODE(EES88Moduls, S88Module);
              #if defined(ESP_WIFI)
              FIXSTORAGE.commit();
              #endif
              #if defined(DEBUG)
              Debug.print("neu S88: ");
              Debug.println(S88Module);
              #endif
              SetupS88();
              #if defined(WIFI)
              WLANSetup();
              #endif
            }
            else {
            #endif

            #if defined(SOFT_RESET)
            soft_restart();     //Reboot MCU!
            #endif
            
            #if defined(S88N)
            }
            #endif
          }
          //----------------------------------------------------------------------------------------------------   
          client.print(F("<form method=get>"));       
          client.print(F("DHCP <input type=checkbox name=P"));
          if (LAN_DHCP)
            client.print(" checked");
          client.print(F("><br>"));
          client.print(F("IP: <input type=number min=0 max=254 name=A value="));
          client.println(LAN_ip[0]);
          client.print(F("><input type=number min=0 max=254 name=B value="));
          client.println(LAN_ip[1]);
          client.print(F("><input type=number min=0 max=254 name=C value="));
          client.println(LAN_ip[2]);
          client.print(F("><input type=number min=0 max=254 name=D value="));
          client.println(LAN_ip[3]);
          client.print(F("><br><br>8x S88 Module: <input type=number min=0 max="));
          #if defined(S88N)
          client.print(S88MAXMODULE);
          #else
          client.print("0");
          #endif
          client.print(F(" name=S value="));
          #if defined(S88N)
            client.print(S88Module);
          #else
            client.print("-");
          #endif
          client.print(F("><br><br><input type=submit></form></body></html>"));
          break;
        }
        if (c == '\n') 
          currentLineIsBlank = true; // you're starting a new line
        else if (c != '\r') 
          currentLineIsBlank = false; // you've gotten a character on the current line
      }
    }
    client.stop();  // close the connection:
  }
}
#endif

//--------------------------------------------------------------------------------------------
#if defined(ESP8266_MCU) || defined(ESP32_MCU)
void Webconfig() {
  WiFiClient client = ESPWebserver.available();
  if (!client)
    return;
    
  String HTTP_req;            // stores the HTTP request 

  if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                HTTP_req += c;  // save the HTTP request 1 char at a time
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: keep-alive");
                    client.println();      //don't forget this!!!
                    // AJAX request for switch state
                    if (HTTP_req.indexOf("/ajax_switch") > -1) {
                        #if defined(DEBUG)
                        Debug.println("Reset WLAN conf!");
                        #endif
                        // read switch state and send appropriate paragraph text
                        ssid = HTTP_req.substring(HTTP_req.indexOf("&s=")+3,HTTP_req.indexOf("&p="));
                        pass = HTTP_req.substring(HTTP_req.indexOf("&p=")+3,HTTP_req.indexOf("&As="));
                        ssidAP = HTTP_req.substring(HTTP_req.indexOf("&As=")+4,HTTP_req.indexOf("&Ap="));
                        passAP = HTTP_req.substring(HTTP_req.indexOf("&Ap=")+4,HTTP_req.indexOf("&Ak="));
                        kanalAP = HTTP_req.substring(HTTP_req.indexOf("&Ak=")+4,HTTP_req.indexOf("&S8=")).toInt();
                        #if defined(S88N)                        
                          S88Module = HTTP_req.substring(HTTP_req.indexOf("&S8=")+4,HTTP_req.indexOf("&nocache")).toInt();
                        #endif
                        
                        if ((kanalAP < 1) || (kanalAP > 13)) {
                          kanalAP = SkanalAP;
                          client.print("Ka. error! ");
                        }
                        if (passAP.length() < 8) {
                          passAP = SpassAP;
                          client.print("Code length error (min. 8)! ");
                        }
                        
                        // write eeprom
                        EEPROMwrite (ssid, EEssidLength, EEssidBegin);
                        EEPROMwrite (pass, EEpassLength, EEpassBegin);
                        
                        EEPROMwrite (ssidAP, EEssidAPLength, EEssidAPBegin);
                        EEPROMwrite (passAP, EEpassAPLength, EEpassAPBegin);
                        FIXSTORAGE.FIXMODE(EEkanalAP, kanalAP);

                        FIXSTORAGE.commit(); 

                        ESPSetup();
                        
                        Udp.begin(z21Port);

                        client.println("saved");   //OK!
                    }
                    else {  // HTTP request for web page
                        // send web page - contains JavaScript with AJAX calls
                        client.println("<!DOCTYPE html>");
                        client.println("<html><head><title>Z21</title>");
                        client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>");
                        client.println("<script>");
                        client.println("function SetState() {");
                        client.println("document.getElementById(\"state\").innerHTML = \"wait\";");
                        client.println("nocache = \"&s=\" + document.getElementById(\"ssid\").value;");
                        client.println("nocache += \"&p=\" + document.getElementById(\"pass\").value;");
                        client.println("nocache += \"&As=\" + document.getElementById(\"ssidAP\").value;");
                        client.println("nocache += \"&Ap=\" + document.getElementById(\"passAP\").value;");
                        client.println("nocache += \"&Ak=\" + document.getElementById(\"kanalAP\").value;");
                        client.println("nocache += \"&S8=\" + document.getElementById(\"S88\").value;");
                        client.println("nocache += \"&nocache=\" + Math.random() * 1000000;");
                        client.println("var request = new XMLHttpRequest();");
                        client.println("request.onreadystatechange = function() {");
                        client.println("if (this.readyState == 4){");
//                        client.println("if (this.status == 200){");
//                        client.println("if (this.responseText != null) {");
                        client.println("document.getElementById(\"state\").innerHTML = this.responseText;");
                        client.println("top.window.location.reload(true);");
                        client.println("}}");
                        client.println("request.open(\"GET\", \"ajax_switch\" + nocache, true);");
                        client.println("request.send(null);");
                        //client.println("setTimeout('SetState()', 1000);");
                        client.println("}");
                        client.println("</script>");
                        client.println("</head>");
                        client.println("<body><h1>Z21 Einstellungen</h1><hr>");
                        client.print("<h2>WiFi Access-Point</h2>");
                        client.print("<dl><dd>IP: ");
                        client.print(WiFi.softAPIP());
                        client.print("</dd><dd>Connected Clients: ");
                        client.print(WiFi.softAPgetStationNum());
                        client.print(" of 8</dd><dd>SSID: <input type=\"text\" id=\"ssidAP\" value=\"");
                        client.print(ssidAP);
                        client.print("\"></dd><dd>code: <input type=\"text\" id=\"passAP\" value=\"");
                        client.print(passAP);
                        client.print("\"></dd><dd>Ka.: <input type=\"number\" min=\"1\" max=\"13\" id=\"kanalAP\" value=\"");
                        client.print(kanalAP);
                        client.println("\"></dd></dl>");
                        
                        client.print("<h2>WiFi Client</h2>");
                        client.print("<dl><dd>IP: ");
                        if (WiFi.status() == WL_CONNECTED) {
                          client.print(WiFi.localIP());
                          client.print(" (");
                          client.print(WiFi.RSSI());
                          client.print("dBm)");
                        }
                        else client.print("none");
                        client.print("</dd><dd>SSID: <input type=text id=\"ssid\" value=\"");  
                        client.print(ssid);
                        client.print("\"></dd><dd>code: <input type=text id=\"pass\" value=\"");
                        client.print(pass);
                        client.println("\"></dd></dl>");

                        client.println("<h2>S88 Module</h2>");
                        client.print("<dl><dd>8x Anzahl: <input type=number min=\"0\" max=\"62\" id=\"S88\" value=\"");
                        #if defined(S88N)                        
                          client.print(S88Module);
                          client.print("\"");
                        #else
                          client.print("0\" disabled");
                        #endif  
                        client.println("></dd></dl><br>");
                        
                        client.println("<input type=submit onclick=\"SetState()\">"); 
                        client.println("<p id=\"state\"></p>");
                        client.print("<hr><p>Z21_ESP_Central_Station_v");
                        client.print(Z21mobileSwVer);
                        client.print(bitRead(FIXSTORAGE.read(EEPROMRCN213), 2));  //RCN-213
                        #if defined (BOOSTER_INT_NDCC)
                        if (FIXSTORAGE.read(EEPROMRailCom) == 0x01)
                          client.print(".RAILCOM");
                        #endif
                        client.println("<br>Build: ");
                        client.println(comp_date);
                        client.println("<br>Copyright (c) 2022 Philipp Gahtow<br>digitalmoba@arcor.de");
                        client.println("</p></body>");
                        client.print("</html>");
                    }
                    // display received HTTP request on serial port
                    //Serial.print(HTTP_req);
                    HTTP_req = "";            // finished with request, empty string
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}
#endif
//--------------------------------------------------------------------------------------------
#endif //HTTPconf
