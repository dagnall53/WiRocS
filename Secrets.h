    #define SSID_RR "Router"         // set if you do not want to use the Serial setting system
       #define PASS_RR "Password"    // set if you do not want to use the Serial setting system
 //      #define myBrokerSubip          // if set will force broker to the default set below always.
                                        // this can be counterproductive if your broker address is not fixed in the Wifi DHCP settings      
       int BrokerAddrDefault = 55;   // set default broker address
                                     // once found, this is saved in BrokerAddr=EEPROM.read(BrokerEEPROMLocation)
       bool ScanForBroker = true;   //new setting to allow to scan for broker if loses link
                                     //CAN ALSO BE SET true by #ifdef _Scan_for_Mosquitto in Directives
