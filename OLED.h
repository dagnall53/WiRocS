#ifndef _OLED_h
  #define _OLED_h
  #include "Arduino.h"
  #include "Directives.h"
  
//  https://github.com/ThingPulse/esp8266-oled-ssd1306
//  you will also need to add Monospaced Plain 8 size font (http://oleddisplay.squix.ch/#/home) to ..Arduino\libraries\esp8266-oled-ssd1306-master\src\OLEDDisplayFonts.h

// Moved to to directives.. #define DisplayWidth 33  // to give 32 chars

void SetFont(uint8_t Disp,uint8_t Font);
void StringToL5(char *line, String input);
void OLED_5_line_display(int addr,String L1,String L2,String L3,String L4,String L5);
bool RocDisplayFormatted(int disp, int line, String Message);
void OLEDS_Display(String L1,String L2,String L3,String L4,String L5);
void OLED_Displays_Setup(uint8_t Address,int Display,String Message);
void OLED_initiate(uint8_t address);
    
void LookForOLEDs(void);


void drawRect(void) ;

void fillRect(void) ;


void SignalStrengthBar( int32_t rssi);  //https://stackoverflow.com/questions/15797920/how-to-convert-wifi-signal-strength-from-quality-percent-to-rssi-dbm


void showTimeAnalog(int disp,int clocksize,int center_x, int center_y, double pl1, double pl2, double pl3);
void showTimeAnalogCircle(int disp,int clocksize,int circsize,int center_x, int center_y, double pl1, double pl2, double pl3);

void BigClock(int disp,int clocksize);

void OLED_Status();
void TimeGears();














#endif

