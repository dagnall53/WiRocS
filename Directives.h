#ifndef _Directives_h
 #define _Directives_h
 #include <Arduino.h> //needed

                                  // LED_BUILTIN;?
                                  // 2 is default on //GPIO 02 SignalLed definition onboard led on most esp8266 and esp32
                                  //led is 22 for lolin lite? clashes with audio! 
                                 //5 on d32 board (screws up I2C display!) = 
                                 // LED_BUILTIN = 2??; 
 #define SignalLed 2 // Revised definition in V5 this is now the PIN /GPIO number not D[0]. Is used (0nly) in SignOfLifeFlash 
// #define SignalLed 16 // for red led on esp8266  ???
 #define SignalON LOW  //defined so I can change the "phase of the SignalLED" easily.
 #define SignalOFF HIGH

 #define _Scan_for_Mosquitto  // New define to switch on scanning for mosquitto. Useful if your router does not have the IP address of the mosquitto server reserved!.
 
// -----------------------------------------------------------------------------------------------------------
// NB -- Loco use is incompatble with OLED - so the OLED define is later, and switched off if PWM is set. ----
//------------------------------------------------------------------------------------------------------------
//#define _LOCO_SERVO_Driven_Port 1  // D1 if using as mobile (LOCO) decoder node.. node becomes a loco with servo on port D "1"  for motor control
//#define _LocoPWMDirPort  3         // D3 add this second Port if using PWM loco motor control Using "3" assumes a L293 inputs driven from port D(_LOCO_SERVO_Driven_Port) and D(_LocoPWMDirPort)
                                     //DO Notuse other ports unless you change the settings in detachservos
 

//IF PWM, which sort of PWM driver??

//#define _NodeMCUMotorShield      //NodeMCU Motor Shield has inverters to the main H drives and so pin D1 is the PWM  and D3 is the PWM direction
                                   //HOWEVER MY boards all exhibit strange behaviour after eeprom programming, when motor will only move one way.


//#define _6612Driver              // much better hardware,  but more complex to drive, I will use it driving the two H drives with Enable being connected "on" via a simple Rc network. 
  
 //assume if a loco then you need front and back lights...
 #ifdef _LOCO_SERVO_Driven_Port
  #define BACKLight 2  //NodeMCU D2
  #define FRONTLight 5 //NodeMCU D5
//#define SteamOutputPin 6
 #endif



//---------------AUDIO--------------------


#define _AudioNoDAC  //to use Earle F Philhowers's audio libraries and his clever single transistor 1 bit oversampling dac for audio (connect D9 (rx)to base of NPN via a 1K, Emitter to ground and Collector is drive to speaker connected to V+) 

// -- only used with the Audio DAc version.. --
//#define _AudioDAC  //to use Earle F Philhowers's audio libraries and I2C dac for audio 
//--- audio dac interface control ports---These are set depending on the two defines above..

 #if defined (_AudioDAC) || defined(_AudioNoDAC)
   #define _Audio          //RX/D9, D8, and D4 pins.defined below    
   
                           //but needs to be defined in order for NoDac to work
   #ifdef ESP32                   // tryalso18/19?
        #define I2SDAC_LRC 21    //Pin GPio 21 is SDA on esp 32
        #define I2SDAC_DIN 22 //try rx pin ?? // 22==pin GPio 22(SCL ) on ESP32
    #else
        #define I2SDAC_LRC 0    //4 ==is the  "D" number ie 4 is D4 is used in no dac because of default ESP8266 i2s settings (is i2s clock?) but can be used as input if you need the pin ..
        #define I2SDAC_DIN 9   //D9/rx used for both Audio types, is main transistor base drive for NoDac USE a Resistor to the Base. I found 10K fine. (makes noises whilst uploading!!)
   #endif
 #endif
 #ifdef _AudioDAC
     #define I2SDAC_CLK 8 //D8 used by DAC version only
 #endif 
//-- end of audio defines



// Setup Defines for completely blank hardware
//from ver 15 equivalents to these defines should be automatically set if the eeprom is empty.. So they may not be needed, but if you have to reset anything, they may be useful.
//If used, After running ONCE with them set, comment them out and re-program so that the rocrail i/o,  loco addr and pin functions etc can be set via rocrail

//#define _ForceRocnetNodeID_to_subIPL //stops the ability of rocnet to change the node number, but prevents the possibility of two nodes having same node number
//#define _ForceDefaultEEPROM //
//#define _Force_Loco_Addr 3 

//#define _DefaultPrintOut 1 //Set this so you can capture your own "defaults" and put them in the RocSubs:SetDefaultSVs() function 

 //---------------------------------------------------------------end of main compiler #defines--------------
 //old define

 //#define _RFID 1  //if using rfid reader THIS IS A VERY OLD part of the ESPWIFI code and not tested recently. 
 //               It adds a RFID reader and sends sensor messages depending on the RFID tag "read.
 //               it's interaction with the new WiRocS code is UNTESTED.
//------------------------------------------------------------------

 //#define _Use_Wifi_Manager //uncomment this to use a "standard" fixed SSID and Password
 
 
 //----------------DEBUG Defines 
 ////Debug settings
 // uncomment these to add extra debug messages. - useful after modifyng the code and something unexpected happens.. 
 // a minimum number of Mqtt debug message will always be set to allow monitoring of node status 
 //(typically the time synch is very useful to tell a node is prenet and working.) 

//#define _LoopTiming
//#define _SERIAL_DEBUG 
//#define _Input_DEBUG
//#define _SERIAL_Audio_DEBUG 
//#define _SERIAL_SUBS_DEBUG 
//#define _ROCDISP_SUBS_DEBUG
#define _showRocMSGS  // time synch?
//#define _SERIAL_MQTT_DEBUG
//#define _SERVO_DEBUG //(enables debug messages in set motor speed rc and pwm
//#define _PWM_DEBUG  // to assist pwm debug 
//#define _SpeedTableDEBUG 
//#define _ESP32_PWM_DEBUG  // to assist pwm debug 
//#define _ESP32_HallTest
// #define FTP_DEBUG in \libraries\esp8266FTPserver\ESP8266FtpServer.h to see ftp verbose on serial

  //ESP32 Stuff

  #ifdef ESP32
    //used in subroutines.h see  https://desire.giesecke.tk/index.php/2018/07/06/reserved-gpios/ 
    // note for GPIO pin 12 Make sure it is not pulled high by a peripheral device during boot or the module might not be able to start!
    //21,22 for audio (Audio uses I2C defaults SPI 21,22) 
    // inputs only with external pullup? 34 35 36 37 38 39 note GPIO34 ... GPIO39 do not have pu/pd circuits.
    // NO USE SPIO flash 6 7 8 9 10 (11?)  
    // ?? not working as expected             18(VSPI CLK)
    //                                                          25,26 default to dac mode give square wave, 11 khz 50% DAC!
    // following work fine  12 13 14 15 16 17    19   (21!) 23       27     32 33 
    // not exposed on my board / untested           20        24       28 29     36 37 38 39

    // also note from ESP32 servo library // quote "All pin numbers are allowed (for PWM), but only pins 2,4,12-19,21-23,25-27,32-33 are recommended.

  static const  uint8_t D0   = 2;  // V5 'D0' is not used now, used to be special pin identifier for signal led. 
  static const  uint8_t D1   = 19;//
  static const  uint8_t D2   = 12; // boot will fail if pulled high
  static const  uint8_t D3   = 13; //
  static const  uint8_t D4   = 14;  
  static const  uint8_t D5   = 15;
  static const  uint8_t D6   = 16;
  static const  uint8_t D7   = 17;
  static const  uint8_t D8   = 21; 
  static const  uint8_t D9   = 23; 
  static const  uint8_t D10   = 25;  
  static const  uint8_t  D11  = 26; 
  #define DAC25is  10
  #define DAC26is  11 //to keep port setting in rocsubs happy should not affect anything else ports D10 and D11
  static const  uint8_t  D12 = 27; 
  static const  uint8_t  D13  = 32; 
  static const  uint8_t  D14  = 33; 
  // these are input only
  static const  uint8_t  D15  = 34; 
  static const  uint8_t  D16  = 35; 
   // on oled board, 4 is oled scl 5 is oled sda for oled 

  #endif

#ifndef _LocoPWMDirPort
//---------------OLED types-- not compatible with pwm lococ- uses the same pins
//
#define _OLED             // define oleds, not properly tested for stability if off , normally left defined..// Saves 42kB program on ESP8266 if NOT defined (for OTA)

//Default is: Display 1-4 write to OLED 1 (Address 60)on primary I2C bus which is 64 high
//            Display 5-8 write to OLED 2 (Address 61)on primary I2C bus
// IF address 60 seen on SECONDARY BUS Then:
//    Display 1-2  write to OLED 3 (Address 60)on secondary I2C bus which is 32 high
//    Display 3-4  write to OLED 4 (Address 60)on primary I2C bus



 //--------------------------------OLED I2C------------------- 
 // These OLED SCL/SDA pin definitions use GPIO numbers 
  #ifdef ESP32
 static const uint8_t OLED_SDA = 4;  //gpio 4 on ESP32 Devkit  !! NB gpio 0 is not exposed on my Esp32 Devkit V1 (but is on some other boards..)
 static const uint8_t OLED_SCL = 5;  //gpio 5
 static const uint8_t OLED_SDA2 = 32;  //my rocrail port 13
 static const uint8_t OLED_SCL2 = 33;  //my rocrail port 14 
 #else
 static const uint8_t OLED_SDA2 = 5;  //D1 is 5 //used only for display 3
 static const uint8_t OLED_SCL2 = 4;//d2 is 4
 static const uint8_t OLED_SDA = 0;  //  known as D3 on esp 8266 
 static const uint8_t OLED_SCL = 2;  // known as D4 on esp 8266 
 #endif
 
#endif //not pwm loco 



#endif
