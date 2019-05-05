#ifndef NVSettingInterface_h
 #define NVSettingInterface_h

 //the #include statment and code go here...
 #include "Arduino.h"
 #include "Directives.h"

#ifdef ESP32
  #define NumberOfPorts 16// moved here to solve compile issue 
  #define PortsRange 18 // for setting index range in my indexed variables with some "headroom"
#else
  #define NumberOfPorts 8 // moved here to solve compile issue 
  #define PortsRange 10 // for setting index range in my indexed variables with some "headroom"
#endif
 #include <EEPROM.h>
  #define _EEPROMdebug //adds serial debug help 
 const byte numChars = 32; //max size I will allow the saved ssid, password and throttle name //4096 max on esp8266 i use EEPROM.begin(Serial_EEPROM_Starts+EEPROM_Serial_store_Size);
 const int Serial_EEPROM_Starts = (336 + (NumberOfPorts*35)); // one away from RN and Cv store which ends at 335 + (NumberOfPorts*35)

 const int BrokerEEPROMLocation =Serial_EEPROM_Starts; //(1 bytes 
 const int RocNodeIDLocation=BrokerEEPROMLocation+2; //2 bytes
 
 const int ssidEEPROMLocation = RocNodeIDLocation+10; // 10 leaves room for other stuff if needed?
 const int passwordEEPROMLocation = ssidEEPROMLocation+numChars; // 32
 const int DisplayClockBoolEEPROMLocation=1+passwordEEPROMLocation+numChars; // about 77 to here
                                                                           // +15 (Bools are 5*3) 
 const int EEPROM_Serial_store_Size=256; 
 


 
 int MSG_content_length();
 void CheckForSerialInput();
 void recvWithEndMarker(); //Gets data <receivedChars> from serial interface
 void showNewData() ;    //shows recieved (serial) <receivedChars> data on serial terminal, useful for debugging 
 void WriteWiFiSettings();
 void TestFillEEPROM(int d);
 void writeString(int add,String data);  //defned here to allow use later but before its properly defined..
 String read_String(int add);             //defned here to allow use later but before its defined..
 void recvWithEndMarker() ;
 void showNewData();



#endif
