#ifndef _Directives_h
 #define _Directives_h
 #include <Arduino.h> //needed

 


 #define SignalLed 0 // always used in this format.. NodeMCUPinD[SignalLed]" on Nodemcu, uses board use D0! expects LED +resistor to +ve! DO not change this to a usable port..
 #define SignalON LOW  //defined so I can change the "phase of the SignalLED" easily.
 #define SignalOFF HIGH

 #define _Scan_for_Mosquitto  // New define to switch on scanning for mosquitto. Useful if your router does not have the IP address of the mosquitto server reserved!.
 

//#define _LOCO_SERVO_Driven_Port 1  //if using as mobile (LOCO) decoder node.. node becomes a loco with servo on port D "1"  for motor control
// #define _LocoPWMDirPort  3         //add this second Port if using PWM loco motor control Using "3" assumes a L293 inputs driven from port D(_LOCO_SERVO_Driven_Port) and D(_LocoPWMDirPort)
                                     //DO Notuse other ports unless you change the settings in detachservos
 

//IF PWM, which sort of PWM driver??
//#define _NodeMCUMotorShield      //NodeMCU Motor Shield has inverters to the main H drives and so pin D1 is the PWM  and D3 is the PWM direction
                                   //HOWEVER MY boards all exhibit strange behaviour after eeprom programming, when motor will only move one way.


// #define _6616Driver              // much better hardware,  but more complex to drive, I will use it driving the two H drives with Enable being connected "on" via a simple Rc network. 
  
 //assume if a loco then you need front and back lights...
 #ifdef _LOCO_SERVO_Driven_Port
  #define BACKLight 2  //NodeMCU D2
  #define FRONTLight 5 //NodeMCU D5
//#define SteamOutputPin 6
 #endif



//---------------AUDIO--------------------

//#define _AudioDAC  //to use Earle F Philhowers's audio libraries and I2C dac for audio 
#define _AudioNoDAC  //to use Earle F Philhowers's audio libraries and his clever single transistor 1 bit oversampling dac for audio (connect D9 (rx)to base of NPN via a 1K, Emitter to ground and Collector is drive to speaker connected to V+) 

//--- audio dac interface control ports---These are set depending on the two defines above..

 #if defined (_AudioDAC) || defined(_AudioNoDAC)
   #define _Audio          //RX/D9, D8, and D4 pins.defined below    
   
                           //but needs to be defined in order for NoDac to work
   #ifdef ESP32
        #define I2SDAC_LRC 21    //Pin GPio 21 is SDA on esp 32
        #define I2SDAC_DIN 22 //try rx pin  // pin GPio 22(SCL ) on ESP32
    #else
        #define I2SDAC_LRC 4    //D4 is used in no dac because of default ESP8266 i2s settings (is i2s clock?) but can be used as input if you need the pin ..
        #define I2SDAC_DIN 9   //D9/rx used for both Audio types, is main transistor base drive for NoDac USE a Resistor to the Base. I found 10K fine. (makes noises whilst uploading!!)
   #endif
 #endif
 #ifdef _AudioDAC
     #define I2SDAC_CLK 8 //D8 used by DAC version only
 #endif 
//-- end of audio defines


//----SET THESE WHEN INITIALLY SETTING UP THE CODE -------------- to set the eeprom to reasonably sensible defaults.
//from ver 15 equivalents to these should be automatically set if the eeprom is empty.. So they may not be needed, but if you have to reset anything, they may be useful.
//If used, After running ONCE with them set, comment them out and re-program so that the rocrail i/o,  loco addr and pin functions etc can be set via rocrail

//#define _ForceRocnetNodeID_to_subIPL //stops the ability of rocnet to change the node number, but prevents the possibility of two nodes having same node number
//#define _ForceDefaultEEPROM //
//#define _Force_Loco_Addr 3 

//#define _DefaultPrintOut 1 //Set this so you can capture your own "defaults" and put them in the RocSubs:SetDefaultSVs() function 

 //---------------------------------------------------------------end of main compiler #defines--------------
 //old define

 //#define _RFID 1  //if using rfid reader THIS IS AN OLD part of the code and not tested recently. 
 //               It adds a RFID reader and sends sensor messages depending on the RFID tag "read.
 //               it's interaction with the new code is UNTESTED.

 //#define _Use_Wifi_Manager //uncomment this to use a "standard" fixed SSID and Password
 //----------------DEBUG Defines 
 ////Debug settings

 // uncomment these to add extra debug messages. - useful after modifyng the code and something unexpected happens.. 
 // a minimum number of Mqtt debug message will always be set to allow monitoring of node status 
 //(typically the time synch is very useful to tell a node is prenet and working.) 
//#define _SERIAL_DEBUG 
//#define _Input_DEBUG
//#define _SERIAL_Audio_DEBUG 
//#define _SERIAL_SUBS_DEBUG 
#define _showRocMSGS
//#define _SERIAL_MQTT_DEBUG
//#define _SERVO_DEBUG //(enables debug messages in set motor speed rc and pwm
//#define _PWM_DEBUG  // to assist pwm debug 
//#define _ESP32_PWM_DEBUG  // to assist pwm debug 
//#define _ESP32_HallTest
// #define FTP_DEBUG in \libraries\esp8266FTPserver\ESP8266FtpServer.h to see ftp verbose on serial

  //ESP32 Stuff
#define DAC25is  10
#define DAC26is  11 //to keep port setting in rocsubs happy should not affect anything as ports 10 and 11
  #ifdef ESP32
    //used in subroutines.h see  https://desire.giesecke.tk/index.php/2018/07/06/reserved-gpios/ note GPIO34 ... GPIO39 do not have pu/pd circuits.
    // note for GPIO pin 12 Make sure it is not pulled high by a peripheral device during boot or the module might not be able to start!
//21,22 for audio
//input only external pullup? 34 35 36 37 38 39 
// NO USE SPIO flash 6 7 8 9 10 (11?)  I2C SPI 21,22 
// ?? not working as expected             18(VSPI CLK)
//                                                          25,26 default to dac mode give square wave, 11 khz 50% DAC!
// following work fine  12 13 14 15 16 17    19   (21!) 23       27     32 33 
// not exposed on my board / untested           20        24       28 29     36 37 38 39

  static const  uint8_t D0   = 2; // LED_BUILTIN;?  //GPIO 02 SignalLed definition onboard led on esp32?
  static const  uint8_t D1   = 19;
  static const  uint8_t D2   = 12;
  static const  uint8_t D3   = 13; //
  static const  uint8_t D4   = 14;  
  static const  uint8_t D5   = 15;
  static const  uint8_t D6   = 16;
  static const  uint8_t D7   = 17;
  static const  uint8_t D8   = 21; 
  static const  uint8_t D9   = 23; 
  static const  uint8_t D10   = 25;  
  static const  uint8_t  D11  = 26; 
  static const  uint8_t  D12 = 27; 
  static const  uint8_t  D13  = 32; 
  static const  uint8_t  D14  = 33; 
  static const  uint8_t  D15  = 34; 
  static const  uint8_t  D16  = 35; 
   // on oled board, 4 is oled scl 5 is oled sda for oled 

  #endif
 // These OLED SCL/SDA pin definitions are same for both ESP8266 and ESP32.
  static const uint8_t OLED_SCL = 4;  //also known as D2 on esp 8266, but
  static const uint8_t OLED_SDA = 5;  // also known as D1 on esp 8266
// The following hardware pinouts differ for ESP32 and ESP8266 variants
#endif
