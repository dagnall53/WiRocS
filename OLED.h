#ifndef _OLED_h
  #define _OLED_h
  #include "Arduino.h"
  #include "Directives.h"
 #ifdef _OLED 
//  https://github.com/ThingPulse/esp8266-oled-ssd1306
//  you will also need to add fonts (http://oleddisplay.squix.ch/#/home) to ..Arduino\libraries\esp8266-oled-ssd1306-master\src\OLEDDisplayFonts.h

// Moved to to directives.. #define TextObjectLength 33  // to give 32 chars

void OLEDScreen(int OLED,bool flip, bool invert);
void SetFont(uint8_t Disp,uint8_t Font);
void StringToChar(char *line, String input);
int GetNumber(String Message, int i);
void OLED_4_RN_displays(int OLed_x,String L1,String L2,String L3,String L4);
bool RocDisplayFormatted(int OLed_x, int line, String Message);
void OLEDS_Display(String L1,String L2,String L3,String L4);
void SetupTextArrays(uint8_t Address,int Display,String Message);
void OLED_initiate(uint8_t address,int I2CBus,bool Display);
    
void LookForOLEDs(bool Display);
void LookForOLED_secondary(void);
void RRPowerOnIndicator(int Disp);
void drawRect(void) ;

void fillRect(void) ;


void SignalStrengthBar(int OLED);  //https://stackoverflow.com/questions/15797920/how-to-convert-wifi-signal-strength-from-quality-percent-to-rssi-dbm


void showTimeAnalog(int disp,int clocksize,int center_x, int center_y, double pl1, double pl2, double pl3);
void showTimeAnalogCircle(int disp,int clocksize,int circsize,int center_x, int center_y, double pl1, double pl2, double pl3);

void BigClock(int disp,int clocksize);

void OLED_Status();
void TimeGears();









#endif //_OLED


#endif

