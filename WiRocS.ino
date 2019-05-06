
//The main Compiler Directives and code options are in Directives.h
#include "Directives.h"

//----DO NOT FORGET TO UPLOAD THE SKETCH DATA ---
//  To check the code is working, in command prompt, set up a MQTT "debug" monitor: (e.g. For MQTT broker at 192.18.0.18) "CD C:\mosquitto  mosquitto_sub -h 192.168.0.18 -i "CMD_Prompt" -t debug -q 0" 
//  From V15, the main ESP8266 program is too big to OTA, so change to 4M(2M SPIFFS), but remember to re-upload the sketch data !!

//-----ESP32 compatibility--------
// use latest esp32fs https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/latest
// Filezilla can also upload over wifi, which may be more convenient. 
// 


#include <ArduinoOTA.h>

uint8_t SW_REV = 26;
String SW_Type= " Master";

#ifdef _Use_Wifi_Manager
       #include <WiFiManager.h>
#else
       #include "Secrets.h"
       String wifiSSID = SSID_RR;
       String wifiPassword = PASS_RR; 
       int BrokerAddr; 
#endif
#ifdef ESP32  // see also https://github.com/earlephilhower/ESP8266Audio#esp-32-spiffs-errors
  #include <WiFi.h>
  #include "SPIFFS.h"
#else
  #include <ESP8266WiFi.h>
#endif

IPAddress ipBroad;
IPAddress mosquitto;

//adding FTP support! should support esp32//https://github.com/nailbuster/esp8266FTPServer
#include <ESP8266FtpServer.h> 

FtpServer ftpSrv;   //set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial
//FTP support
uint8_t wifiaddr;
uint8_t ip0;
uint8_t ip1;
uint8_t subIPH;
uint8_t subIPL;
bool _HaveSent_Connected_Debug_Msg;
extern int SCP[5][7];  // Scroll position counter for [Rocdisplay] [Oleds] on each, oled displays (LIMIT  ONE SCROLLING per display)
extern int ScrollMsgLength[5][7];

#ifdef ESP32  // https://github.com/madhephaestus/ESP32Servo/tree/master/src
              // which is forked from https://github.com/jkb-git/ESP32Servo
  // quote "All pin numbers are allowed, but only pins 2,4,12-19,21-23,25-27,32-33 are recommended.
  #include <ESP32Servo.h>
#else
  #include <Servo.h>
#endif


#include <EEPROM.h>
#include "NVSettinginterface.h"
#include "Globals.h"
#include "Subroutines.h"
#include "MQTT.h" 
#include "RocSUBS.h"
#include "EEPROMDEFAULTS.h"
#include "Ports.h"
// #include <PubSubClient.h> is included in MQTT.cpp.. Very important to read the notes there...


 //MOVED to OLED.cpp  
 //#include <SSD1306.h> // alias for `#include "SSD1306Wire.h"` //https://github.com/ThingPulse/esp8266-oled-ssd1306
 // SSD1306  OLED1(0x3c, OLED_SDA, OLED_SCL);
 // SSD1306  OLED2(0x3d, OLED_SDA, OLED_SCL);
 #include <Wire.h>

 #include "OLED.h"

int SDelay[PortsRange]; //Number of ports +2
uint32_t LoopTimer;
uint32_t ScrollSpeedCounter;
uint32_t LocoCycle;
uint32_t TimeToClearDebugMessage;
bool DebugMsgCleared;
uint32_t StartedAt;



#ifdef _Audio
#include "Chuff.h"  
#endif
extern bool ChuffsOn();
bool Audio_Setup_Problem;
#ifdef _RFID
 #include "RFID_Subs.h"
#endif

extern  uint32_t Motor_Setting_Update_Time;
extern void ResetDebounce();
extern bool PortInvert( uint8_t i);
extern void OLED_4_RN_displays(int OLed_x,String L1,String L2,String L3,String L4);
extern void SetFont(uint8_t Disp,uint8_t Font);

extern uint8_t OLED_Settings[7];
extern void DebugSprintfMsgSend(int CX);
extern  bool OLED1Present,OLED2Present,OLED3Present,OLED4Present,OLED5Present,OLED6Present;


// functions start 
void SignOfLifeFlash(bool state){  //on ESP8266 SignalLed port is also I2CSDL, so will flash regularly if any OLED is present on I2C bus!
  if (!(OLED1Present||OLED3Present||OLED5Present)){ // do not drive the pin if OLEDS that connect to the main I2C are present!!
    digitalWrite(SignalLed, state) ; ///turn On/off  // Note will not flash if port (eg on esp8266 is D4) is set to output either! 
    // WAS   digitalWrite(NodeMCUPinD[SignalLed], state) ; ///turn On
}}

int32_t SigStrength(void){
  return WiFi.RSSI();
 }

void ConnectionPrint() {
  Serial.println("");
  Serial.println(F("---------------------------Connected-----------------------"));
  Serial.print (F(" Connected to SSID:"));
  Serial.print(WiFi.SSID());
  Serial.print(F("  IP:"));
  Serial.println(WiFi.localIP());
  Serial.print(F(" WiFi strength:"));
  Serial.println(SigStrength());
  Serial.println(F("-----------------------------------------------------------"));      
 
}
extern void Port_Setup_and_Report();

void Report_address_and_do_any_forced_settings(){
  #ifdef _ROCDISP_EEPROM_DEBUG
  Serial.println("Report_address_and_do_any_forced_settings");
  #endif
   #ifdef _LOCO_SERVO_Driven_Port
      //MyLocoAddr=CV[1];
      //MyLocoAddr = CV[18] + ((192-(CV[17]) * 256));
      MyLocoAddr=CV[1];
      Serial.println(F("---------------------- LOCO Setup   -----------------------"));
      Serial.print(F(  "          'Locomotive Address' is"));
      if bitRead( CV[29],5){ MyLocoAddr = CV[18] + (((CV[17]&63) * 256));
                           Serial.print("{Long}"); Serial.println (MyLocoAddr);}
                     else{ Serial.print("{Short}"); Serial.println (MyLocoAddr);}
       #ifdef  _Force_Loco_Addr
          MyLocoAddr= _Force_Loco_Addr;
          CV[1]=_Force_Loco_Addr;
          CV[17]=0;
          CV[18]=_Force_Loco_Addr;
          Serial.print("FORCED by #defines to long as short set at:"); Serial.println (MyLocoAddr);
       #endif 
   #endif
 

 Port_Setup_and_Report();
 ImmediateStop(); //stop motors as soon as ports set up
      
}




void Status(){
  SetFont(1,99);OLED_4_RN_displays(1,"Setting up WiFi","","","");
  SetFont(2,99);OLED_4_RN_displays(2,"Setting up WiFi","","","");
  Serial.println();Serial.println();
  Serial.println(F("-----------------------------------------------------------"));
#ifdef _LOCO_SERVO_Driven_Port
#ifdef _Audio 
#ifdef _AudioNoDAC
                 Serial.println(F("        ESP8266 MQTT Rocnet Loco decoder with  1 pin Sound")); 
                 #else
                 Serial.println(F("        ESP8266 MQTT Rocnet Loco decoder with DAC based Sound"));
                 #endif
    #else 
  Serial.println(F("             ESP8266 MQTT Rocnet Loco decoder     ")); 
  #endif
#else
  #ifdef _Audio
         #ifdef _AudioNoDAC
                 Serial.println(F("        ESP8266 MQTT Rocnet Static Node with 1 pin (D9)Sound    ")); 
                 #else
                 Serial.println(F("        ESP8266 MQTT Rocnet Static Node with DAC based Sound    "));
                 #endif
  #else 
  Serial.println(F("             ESP8266 MQTT Rocnet Static Node     ")); 
  #endif
#endif  
  Serial.println(F("-----------------------------------------------------------"));
  Serial.print(F(  "                    revision:"));
  Serial.print(SW_REV); Serial.print(SW_Type); Serial.println();
  Serial.println(F("-----------------------------------------------------------"));
  

#ifdef _Use_Wifi_Manager
   WiFiManager wifiManager;  //this  stores ssid and password invisibly  !!
  //reset settings - for testing
  //wifiManager.resetSettings();
  wifiManager.autoConnect("ROCNODE ESP AP");  //ap name (with no password) to be used if last ssid password not found
#else    

  WiFi.mode(WIFI_STA);  //Alternate "normal" connection to wifi
#ifndef ESP32
  WiFi.setOutputPower(0.0); //0 sets transmit power to 0dbm to lower power consumption, but reduces usable range.. try 30 for extra range
  WiFi.setOutputPower(30);
#endif
  WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
  OLED_4_RN_displays(1,"Connecting to",wifiSSID.c_str(),"","");OLED_4_RN_displays(2,"Connecting to",wifiSSID.c_str(),"","");
  Serial.print(F("Trying to connect to {"));
              Serial.print(wifiSSID.c_str());
              Serial.println(F("} "));
  LoopTimer=millis()+Ten_Sec;            
  while ((WiFi.status() != WL_CONNECTED) && (millis()<= LoopTimer)) {
              SignOfLifeFlash( SignalON) ; 
              delay(150);
              SignOfLifeFlash( SignalOFF) ;
              delay(150);
              Serial.print(".");}
    if (WiFi.status() != WL_CONNECTED){
                Serial.println(F("SSID NOT FOUND  Trying for forced reset "));
                delay(10);
                ESP.restart();
                }           
 
#endif  // not using wifi manager

  //if you get here you should have connected to the WiFi
  ipBroad = WiFi.localIP();
  ip0=ipBroad[0];
  ip1=ipBroad[1];
  subIPH = ipBroad[2];
  subIPL = ipBroad[3];
  wifiaddr = ipBroad[3];
  ConnectionPrint();
  ipBroad[3] = 255; //Set broadcast to local broadcast ip e.g. 192.168.0.255 //used in udp version of this program
 
  
  
  //++++++++++ MQTT setup stuff   +++++++++++++++++++++
  mosquitto[0] = ipBroad[0]; mosquitto[1] = ipBroad[1]; mosquitto[2] = ipBroad[2];
  // old method of storage, now replaced replaced mosquitto[3]= RN[14];                //saved mosquitto address, where the (lowest address) broker is! saved as RN[14], will loop through 18-5 to find one
  mosquitto[3]= BrokerAddr;  //use saved broker address as your broker ip address.
  SetFont(1,99); OLED_4_RN_displays(1,"Connected to",wifiSSID.c_str(),"Connecting to MQQT","");
  SetFont(2,99); OLED_4_RN_displays(2,"Connected to",wifiSSID.c_str(),"Connecting to MQQT","");
  Serial.print(F(" Mosquitto will first try to connect to:"));
  Serial.println(mosquitto);
  //MQTT_Setup();
  reconnect(); // includes MQTT_Setup();
 
  //------------------ IF rfid -------------------------
    #ifdef  _RFID
      SetupRFID(); //note I have not tested this stuff recently.. 
    #endif

  RocNodeID= getTwoBytesFromMessageHL(RN, 1);   
   #ifdef _ForceRocnetNodeID_to_subIPL
    RocNodeID=subIPL;
   #endif 
   if (RocNodeID==0){RocNodeID=3;}
   Report_address_and_do_any_forced_settings();
     
 #ifdef _LOCO_SERVO_Driven_Port
  Loco_motor_servo_demand = 90;
  digitalWrite(NodeMCUPinD[FRONTLight], 1);  //Turn off direction lights
  digitalWrite(NodeMCUPinD[BACKLight], 1); //Turn off direction lights
  #endif


}

void _SetupOTA(String StuffToAdd){
  String Name;
  //ota stuff  Port defaults to 8266
  //ArduinoOTA.setPort(8266);
  //Hostname defaults to esp8266-[ChipID]

 #ifdef _LOCO_SERVO_Driven_Port 
  Name="RN LOCO(";
  #else
  Name="RN STAT(";
 #endif
 Name=Name+StuffToAdd;
 Name=Name+")";
 Serial.printf("--------------------------------------------------------------\n--- Setting OTA Hostname <%s> ---\n",Name.c_str());
  Serial.printf("------------------------------------------------------\n");
 ArduinoOTA.setHostname(Name.c_str());
  //No authentication by default
  //ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  //---------------------end ota stuff -------
   
}

void Commit_EEprom(String reason){
  //+++++++++++++commit any changed writes to the  Eprom and change the ports if they have been altered..
    Data_Updated = false;
    DebugSprintfMsgSend(sprintf(DebugMsg,"Commiting EEPROM :%s",reason.c_str()));
    for (int i=1;i<=8;i++){
          DetachServo(i); }  //switches off ALL servos
           // need to detach PWM interrupts before using eeprom comitt()??  seems not
  #ifdef _LocoPWMDirPort
   //       WriteAnalogtoPort(_LOCO_SERVO_Driven_Port, 0);
  //        WriteAnalogtoPort(_LocoPWMDirPort, 0);
  #endif    
    delay(100);
    EEPROM.commit();
    ReadEEPROM();     //get the RN and CV registers
    LookForOLEDs(false); // false means no serial stuff
    //Port_Setup_and_Report();  //make any port direction changes.
    //ImmediateStop(); //stop motors as soon as ports set up
     #ifdef _LocoPWMDirPort  //is a restart of pwm needed ?? 
      //    WriteAnalogtoPort(_LOCO_SERVO_Driven_Port, 0);
     //     WriteAnalogtoPort(_LocoPWMDirPort, 0);
     //     digitalWrite( NodeMCUPinD[_LOCO_SERVO_Driven_Port], false);   
     //     digitalWrite(NodeMCUPinD[_LocoPWMDirPort] , false) ;
          
    #endif
    SetMotorSpeed(0,0);
    Last_DCC_Speed_Demand= 0; 
  
    Loco_motor_servo_demand=90;
//+++++++++  Set up other things that may have been changed...+++++++++
    Report_address_and_do_any_forced_settings(); // includes Port_Setup_and_Report();
   
} //+++++++++++END commit to EPROM
void SetDefaultEEPROM(){
              
    Serial.println(" ******* EPROM EMPTY....Setting Default EEPROM values *****");
    SetDefaultSVs();
    CV[8] = 0x0D; //DIY MFR code
    CV[7] = SW_REV; //ver
    CV[1]=3; //set initial loco address as 3 (regardless of whatever  the set defaults function says)
    #ifdef  _Force_Loco_Addr
      CV[1]= _Force_Loco_Addr;
    #endif
    WriteEEPROM();
    EPROM_Write_Delay = millis() + Ten_Sec;
    Commit_EEprom("Setting Defaults ");
    delay(100);
   
}

void SetupFTP(){
   /////FTP Setup, ensure SPIFFS is started before ftp;  /////////
  #ifdef ESP32       //esp32 we send true to format spiffs if cannot mount //NOT TESTED!
    if (SPIFFS.begin(true)) 
  #elif defined ESP8266
    if (SPIFFS.begin()) 
  #endif
  {
  Serial.println("SPIFFS opened!");
  ftpSrv.begin("esp8266","esp8266");    //username, password for ftp.  set ports in ESP8266FtpServer.h  (default 21, 50009 for PASV)
                         } 
}

extern bool Observed_Change;  // used trying to find why oledsetup is changing
extern void ViewAllOLEDSettings(int OLed_x);
extern void SetAll_32Mode(int OLed_x);

void setup() {  
   bool UsedDefaults;
    Observed_Change=false; 
   _HaveSent_Connected_Debug_Msg=false;
   Serial.begin(115200);
   NumberOfOLEDS=0;
   // oled if fitted
 EEPROM.begin(Serial_EEPROM_Starts+EEPROM_Serial_store_Size);

#ifdef _LOCO_SERVO_Driven_Port
pinMode(NodeMCUPinD[_LOCO_SERVO_Driven_Port], OUTPUT);
#ifdef _LocoPWMDirPort
pinMode(NodeMCUPinD[_LocoPWMDirPort], OUTPUT);
#endif
ImmediateStop(); //stop motors as soon as setup set up
#endif
//#ifdef _LocoPWMDirPort  //pinMode(NodeMCUPinD[_LocoPWMDirPort], OUTPUT);pinMode(NodeMCUPinD[_LOCO_SERVO_Driven_Port], OUTPUT);
//  digitalWrite(NodeMCUPinD[_LocoPWMDirPort],LOW);//stop motors as soon as setup set up
//  digitalWrite(NodeMCUPinD[_LOCO_SERVO_Driven_Port],LOW);
//#endif
  Ten_Sec = 10000;
 
 // SetPortPinIndex();  //sets up nodemcucross references 

  #ifdef _Scan_for_Mosquitto
  ScanForBroker=true;
  #endif

//----------------------Setup from eeprom

  Data_Updated = false;
  
  #ifndef _ForceDefaultEEPROM
      if ((EEPROM.read(ssidEEPROMLocation) == 0xFF) && (EEPROM.read(ssidEEPROMLocation+1) == 0xFF)){SetDefaultEEPROM();} //good chance the eeprom is empty, or we are forcing this probably this is a first run. Can also set via CV[8]=8
  #endif
  #ifdef _ForceDefaultEEPROM
      {SetDefaultEEPROM();} //force because of directive
  #endif
   
  ReadEEPROM();     //get the RN and CV registers and now the wifi settings etc. 
  CV[8] = 0x0D; //DIY MFR code  set regardless!
  CV[7] = SW_REV; //ver    set regardless
  SetAll_32Mode(1); // set the _32 mode to the oled 1 setting regardless.
  delay(100); // time to get settings before OLED check
#ifdef _OLED   
   LookForOLEDs(true);   // needs eeprom begin and eeprom settings read  see also  https://roboindia.com/tutorials/i2c-address-scanner-nodemcu
#endif
  UsedDefaults=false;
  
  #ifdef myBrokerSubip
         BrokerAddr=BrokerAddrDefault;
         Serial.print(" Using Default Broker Addr:");Serial.println(BrokerAddr);
  #endif

  if ((wifiSSID=="")||(wifiSSID.length()>=90)){wifiSSID=SSID_RR;UsedDefaults=true;Serial.println("Using Default SSID");}         //if empty, or if bigger than 90 use the secrets default
  if ((wifiPassword=="")||(wifiPassword.length()>=90)){wifiPassword=PASS_RR;UsedDefaults=true;Serial.println("Using Default Password");} //if empty, or if bigger than 90 use the secrets default
  if ((BrokerAddr==0)||(BrokerAddr==255)){BrokerAddr=BrokerAddrDefault;UsedDefaults=true;Serial.println("Using Default Broker address");}   //zero and 255 are not  valid Ip for the broker, use default instead
  if (UsedDefaults){WriteWiFiSettings();} 
  
  //pinMode(0,INPUT_PULLUP);  WHY?? was this here  (Part of unsucessful attempt to add switch to force serial input readings).. but makes PWM motors spin at startup
  
  delay(10);


  
  CheckForSerialInput(); // allow a few seconds to check for  serial traffic to change wifi settings etc
  StartedAt=millis(); // this is a timer that is used to stop any messages recieved on start up from writing to eeprom.
                      // this prevents issues caused by the mosquitto broker resending stuff as the node connects.
  SetPortPinIndex();  //sets up nodemcucross references 

//------------------

  
  

  #ifdef  _Force_Loco_Addr
       CV[1]= _Force_Loco_Addr;
  #endif 
     MyLocoAddr=CV[1]; //short address
    if bitRead( CV[29],5){MyLocoAddr = CV[18] + (((CV[17]&63) * 256));} //long address
  #ifdef  _Force_Loco_Addr   //can only force short addresses
       MyLocoAddr= _Force_Loco_Addr;
       CV[1]=_Force_Loco_Addr;
       CV[17]=0;  //need to set these to forced loco address??
       CV[18]=_Force_Loco_Addr; //??
  #endif 
  

  Status();  //includes Port_Setup_and_Report();  //make any port direction changes.
  ImmediateStop(); //stop motors as soon as ports set up
  _SetupOTA(Nickname); //now we should have the nickname etc
 

  //Motor_Speed = 0;
  SetMotorSpeed(0,0);
  Last_DCC_Speed_Demand= 0; 
  MotorStopped=false;
  Loco_motor_servo_demand=90;
  Motor_Setting_Update_Time=millis();
  connects = 0;
  oldconnects = 0;

  POWERON = true;
  WaitUntill = millis();

  SensorOutput_Inactive = true;

  RFIDCycle = millis();
  LoopTimer = millis();
  LocoCycle = millis();
  ScrollSpeedCounter=millis();
  EPROM_Write_Delay = millis();
  SignOfLifeFlash( SignalON) ;  ///turn On
 
  
  
  ResetWiFi = false;
  MSGReflected = true;
  MsgSendTime = millis();
  lastsec = millis();
  
  secs=0;
  mins=0;
  hrs=0;

  LenDebugMessage = 1;
  DebugMessage[0] = 0xFF;
  for (int i = 0 ; i <= 127; i++) { //clean out debug message 
    DebugMessage[i] = 0;
  }

 
    //PrintTime("Start\n"); // only if you need to test startup times
    for (int i = 0 ; i <= 8; i++) { //set servo stuff to a default.
    SDelay[i] = 1000;
    SDemand[i] = 90;
    Pi03_Setting_LastUpdated[i] = millis();
  }
  //////////////////////////
  #ifdef  _DefaultPrintOut  //give a printout of whats set in the eeprom..
    Serial.printf("-----------------CURRENT EEPROM SETTNGS --------------\n");
    PrintEEPROMSettings();
    Serial.printf(" ----//end of saved EEPROM set of defaults----\n");
    
  #endif

  SetupFTP();

  #ifdef _Audio
     SetUpAudio(millis());
  #endif

  ResetDebounce();
  
  delay(10);
 #ifdef _OLED  
  Serial.println("Re-Scanning OLEDS after Audio Set-up");
  LookForOLEDs(false); // test again, Sometimes needed to Re-Set up displays After Audio Setup has potentially overwritten things (mainly because No_dac audio sets up for LRC ?!!)
                  //When using the software delta-sigma DAC, even though our playback circuit is not using the LRCLK or BCLK pins, 
                  //the ESP8266 internal hardware will be driving them. So these pins cannot be used as outputs in your application. 
                  //However, you can use the LRCLK and BCLK pins as inputs. 
                  //Simply start playback, then use the standard pinMode(xxx, INPUT/INPUT_PULLUP) Arduino commands 
                  //and you can, for example, use those two pins to read a button or sensor.
  
  
  SetFont(1,99); SetFont(2,99); SetFont(3,99); SetFont(4,99);SetFont(5,99); SetFont(6,99);// set to default 
 
  OLEDS_Display("In Main Loop","","","");
  for (int RocDisplay_n=0;RocDisplay_n<=4;RocDisplay_n++){
      for (int OLED=0; OLED<=6;OLED++) {SCP[RocDisplay_n][OLED]=-5; ScrollMsgLength[RocDisplay_n][OLED]= 10; 
       }}; // initial set up of scrolling
  #ifdef  _ROCDISP_EEPROM_DEBUG    
                                 Serial.printf("\n  --- after SCP stuff message At@%d ---\n\n",(millis()-StartedAt));                        
                                 ViewAllOLEDSettings(1); 
  #endif 
 #endif 
  //Serial.println("------------------------ Starting main loop ---------------");
   FlashMessage("---Entering Main Loop---", 5, 150, 150);
   StartedAt=millis(); // this is a timer that is used to stop any messages received on start up 
 }  ///end of setup ///////



// try to clear serial issue with v22
extern  void recvWithEndMarker();
extern bool newData;
extern char receivedChars[32];
 

void readserialport(){
  String TestData;

   recvWithEndMarker();
       if (newData == true) {TestData=receivedChars;
       Serial.print("io <");Serial.print(TestData);Serial.println(">"); }
}


bool CheckWiFiConnected(){
  uint32_t  T;
  bool conn;
  conn=true;
  T=millis()+10;
   while ((WiFi.status() != WL_CONNECTED) && (millis()<= T)) {
              delay(1) ;             }
         if ((WiFi.status() != WL_CONNECTED)){     conn=false;}
              return conn;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++MAIN LOOP++++++++++++++++++++++++++++++++++++++++++++

extern long Chuff_wav_period;
extern int ChuffDebugState;
extern int Wavs_Per_Revolution;
extern int Last_Wavs_Per_Revolution;
extern bool AlternateSounds();
int Hall;

int LastRocNodeID;
void DoFTP(){
  ftpSrv.handleFTP(); 
}

// timing test parameters
uint32_t Timer[8];
int LoopCount;
extern bool OLEDPresent(int OLED);

//========================MAIN LOOP==============================
void loop() {
 //  LoopCount++;
 //  Clear the retained debug Msg after a debug and delay (but this does not stop the repeats from mosquitto
// if (!(DebugMsgCleared) && (millis()>=TimeToClearDebugMessage)){ DebugMsgClear;DebugMsgCleared=true;Serial.print("C");}

// separated out oleds from sign of life flash to allow faster than 1 sec updates..

if ( LoopTimer>= ScrollSpeedCounter)  {//oled update system here 
    ScrollSpeedCounter=LoopTimer+500;
    #ifdef _OLED 
    //DebugSprintfMsgSend(sprintf ( DebugMsg, "Clk :%d :%d :%d :%d ",OLED_Settings[1],OLED_Settings[2],OLED_Settings[3],OLED_Settings[4]));
    TimeGears();
    // Scrolling message pointers  : Scroll position counter for displays (LIMIT  ONE scrolling count per Rocdisplay)
    for (int oled=1; oled<=6;oled++){  // six oleds entities (1-6) are possible, 
      if((OLEDPresent(oled))){
        for (int RocDMsg=1;RocDMsg<=4;RocDMsg++){//there are a max of 4 roc display messages (1-4) per OLED 
           SCP[RocDMsg][oled]=(SCP[RocDMsg][oled])+1; if (SCP[RocDMsg][oled]>=ScrollMsgLength[RocDMsg][oled]){SCP[RocDMsg][oled]= -5;}
           }}};  //Scroll position counter for displays (LIMIT  ONE scrolling count per Rocdisplay)
    OLED_Status();   
    #endif
   }

  //Sign of life flash 
  if (LoopTimer >= lastsec ) {//sign of life flash 
    SignOfLifeFlash(SignalON) ; ///turn On
    lastsec = lastsec +1000; secs = secs + divider;  
   
#ifdef _LoopTiming
    Serial.print("Loops achieved");Serial.println(LoopCount);
    LoopCount=0;
#else     
    if(Audio_Setup_Problem){Serial.print("A");}else{Serial.print(".");}
#endif    
   //  Serial.print("M<");Serial.print(hallRead());Serial.print("> ");
   }
           //make sure you call handleFTP() in loop  !! regardless of MQTT connection..
    DoFTP();
    ArduinoOTA.handle();
    
    if (!MQTT_Connected()) {    //if NOT connected, try to connect, but if we are connected, the you can do the main loop stuff.
        ImmediateStop();
      #ifdef _OLED  
        OLED_Status();Serial.print("!");
      #endif  
        reconnect();
        }else
     { 
    MQTT_Loop(); //gets wifi messages etc..
#ifdef _LoopTiming
    //SingleLoop=millis()-LoopTimer;
    LoopCount=LoopCount+1;
#endif    
    LoopTimer = millis();      //idea is to use LoopTimer in functions instead of millis to ensure synchronous behaviour in loop

    #ifdef _Audio
    if (!Audio_Setup_Problem){
      AudioLoop(LoopTimer);
      SoundEffects();
      if (TimeToChuff(LoopTimer)){ 
         if (!AlternateSounds()){
            Chuff("/Fenchurch","/Fench",35);} //~35 mph rough trigger speed .."chuff(" has formulae with number of wavcyclesperrpm  chuff selects sound samples (slow and Fast) and per wav switch period interval t sect slow or fast"/BBCH" is my best sounding set.. or try "/ivor_" or "/Fenchurch"  
         else{
            Chuff("/BBCH","/Fast",35);} //  chuff selects sound samples (slow and Fast) and per wav switch period interval t sect slow or fast"/BBCH" is my best sounding set.. or try "/ivor_" or "/Fenchurch"  
                             }
        }
    #endif
     
  //+++++++++++++++can reset wifi on command via "update node to sw 0"
  if ( ResetWiFi == true) { //reset settings - for testing purposes
      #ifdef _SERIAL_DEBUG
          Serial.println("  +++++++++++  RESETTING WiFi  +++++++++++++++  " );
          Serial.println("  +++++++++++  You will need to access the AP and browse 192.168.4.1  +++++++++++++++  " );
      #endif
        #ifdef _Use_Wifi_Manager
          WiFiManager wifiManager; wifiManager.resetSettings(); wifiManager.startConfigPortal("ROCNODE ESP AP"); //ap name (with no password) to be used if ssid password not stored/found
        #else
      //ESP.reset(); //needed ??
        #endif
      ResetWiFi = false; FlashMessage(" ++++++++ RESET AP Completed ++++++++ ", 10, 300, 300);
      ConnectionPrint();
      }
  //+++++++++++++++
  //other periodic updates and checks
  
  //+++++++++++++commit any changed writes to the  Eprom and change the ports if they have been altered..
  if ((LoopTimer >= EPROM_Write_Delay) && (Data_Updated)) {//commit EEPROM after a delay if it has not been explicitly called ..
      Commit_EEprom("Timed write after Data_Updated");
      }
  //+++++++++++++++                                                       

    #ifdef _RFID
      checkRFID();
    #endif
  //++++++++++Motor, ports etc  
  DoLocoMotor(); //needs checks internally for loco port existance
  SERVOS();
  FLASHING();
  ReadInputPorts();
  DETACH();     //check if servos need detaching...
  DoRocNet();   //do any messages ! includes... if (Message_Length >=1)

  //delay(5);   //slow this down for tests
  SignOfLifeFlash( SignalOFF) ; ///turn OFF signal lamp
 }// do if connected loop 
} //void loop
