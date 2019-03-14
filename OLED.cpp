
  #include "OLED.h"
  #include <Arduino.h> //needed 
  #include "Directives.h"
  //#include "Globals.h"
   #include <SSD1306.h> // alias for `#include "SSD1306Wire.h"` //https://github.com/ThingPulse/esp8266-oled-ssd1306
   // options are 3c 3d on bus 1 (disp1 and 2)  or 3c on bus 2 is 32 high so 3c on bus 1 is also 32 hi (becoming disp 3 and disp4)  
   
  SSD1306  display1(0x3c, OLED_SDA, OLED_SCL);//== RocDisplay 1,2,3&4
  SSD1306  display2(0x3d, OLED_SDA, OLED_SCL);// RocDisplay 5,6,7,8  
  // small displays
  SSD1306 display3(0x3c, OLED_SDA2, OLED_SCL2, GEOMETRY_128_32 ); // RocDisplay 1&2 //Display 3 is the only one to use the secondary ports
  SSD1306 display4(0x3c, OLED_SDA, OLED_SCL, GEOMETRY_128_32 );  //RocDisplay 3&4
  //
  
#define DisplayWidth 100  //100 to allow passing of {} formatting    33to allow up to 32 chars width for monspaced display
#define TerminalDisplayFont 6
 
char L1[DisplayWidth],L2[DisplayWidth],L3[DisplayWidth],L4[DisplayWidth],L5[DisplayWidth],L6[DisplayWidth],L7[DisplayWidth],L8[DisplayWidth],L9[DisplayWidth],L10[DisplayWidth];
bool Display1Present,Display2Present,Display3Present,Display4Present;
uint8_t offset;
int FontVOffset;
uint8_t ClockSettings[5]; ///bit settings
#define _ClockON 0
#define _ClockLeft 1
#define _ClockAna 2

//https://github.com/ThingPulse/esp8266-oled-ssd1306
// you will also need to add Monospaced Plain 8 size font (http://oleddisplay.squix.ch/#/home) to C:\Arduino\libraries\esp8266-oled-ssd1306-master\src\OLEDDisplayFonts.h

uint32_t Disp2ReInit;
extern char DebugMsg[127];
extern uint8_t hrs;
extern uint8_t mins;
extern uint8_t secs;  
extern uint8_t divider;
extern char Nickname[35];
extern bool POWERON;
extern void DebugSprintfMsgSend(int CX);
extern bool Data_Updated;
extern uint32_t EPROM_Write_Delay;
extern void WriteEEPROM(void);
int FontSelected;

void SetFont(uint8_t Disp,uint8_t Font){
FontSelected=Font;
  // note, FontVOffset is to align the various fonts. It can only be +ve or it will fail on the top line (vertical =0)print. 
if (Disp==1){
  switch (Font) { 
    case 0: 
          display1.setFont(Font_7x5);
          FontVOffset=0;
    break;
    case 1: 
          display1.setFont(Font_6x5w);
          FontVOffset=0;
    break;
    case 2: 
          display1.setFont(Font_6x5n);
          FontVOffset=0;
    break;
    case 3: 
          display1.setFont(Font_5x5inv);
          FontVOffset=0;
    break;
    case 4: 
          display1.setFont(Font_High);
          FontVOffset=0;
    break;
    
    default:
          display1.setFont(ArialMT_Plain_10);
          FontVOffset=0;
    break;
         }
   }
 if (Disp==2){
  switch (Font) {
    case 0: 
          display2.setFont(Font_7x5);
          FontVOffset=0;
    break;
    case 1: 
          display2.setFont(Font_6x5w);
          FontVOffset=0;
    break;
    case 2: 
          display2.setFont(Font_6x5n);
          FontVOffset=0;
    break;
    case 3: 
          display2.setFont(Font_5x5inv);
          FontVOffset=0;
    break;
    case 4: 
          display2.setFont(Font_High);
          FontVOffset=0;
    break;
    
    default:
          display2.setFont(ArialMT_Plain_10);
          FontVOffset=0;
    break;
         }
   }
if (Disp==3){
  switch (Font) {
    case 0: 
          display3.setFont(Font_7x5);
          FontVOffset=0;
    break;
    case 1: 
          display3.setFont(Font_6x5w);
          FontVOffset=0;
    break;
    case 2: 
          display3.setFont(Font_6x5n);
          FontVOffset=0;
    break;
    case 3: 
          display3.setFont(Font_5x5inv);
          FontVOffset=0;
    break;
    case 4: 
          display3.setFont(Font_High);
          FontVOffset=0;
    break;
    
    default:
          display3.setFont(ArialMT_Plain_10);
          FontVOffset=0;
    break;
         }
   }

if (Disp==4){
  switch (Font) {
    case 0: 
          display4.setFont(Font_7x5);
          FontVOffset=0;
    break;
    case 1: 
          display4.setFont(Font_6x5n);
          FontVOffset=0;
    break;
    case 2: 
          display4.setFont(Font_6x5w);
          FontVOffset=0;
    break;
    case 3: 
          display4.setFont(Font_5x5inv);
          FontVOffset=0;
    break;
    case 4: 
          display4.setFont(Font_High);
          FontVOffset=0;
    break;
    
    default:
          display4.setFont(ArialMT_Plain_10);
          FontVOffset=0;
    break;
         }
   }
   
}

void StringToL5(char *line, String input){
  for (int i=0;i<=DisplayWidth-1; i++){
    if (i <= input.length()){
      line[i]=input[i];
    }
  }
}
int GetNumber(String Message, int i){
  int Number,j; bool FoundEnd;
  FoundEnd=false;Number=0;j=0;
  while ((!FoundEnd)&&(j<=3)){
    if ((Message[i+j]>= 48)&&(Message[i+j]<= 57)){
        Number=(Number*10)+(Message[i+j]-48);
        j=j+1;
        }else
        {FoundEnd=true;}
    }
  return Number;
}

bool RocDisplayFormatted(int disp, int PixelsDown, String Message){
  bool found;
  int rowPixel,rowPixel1, NewLineOffset;int MsgLength;
  int ClockSpaceOffset,ClockPos,ClockRad;
  //bool ClockAnalog,ClockLeft;
  String FormattedMsg,BitMsg; char BitChar;
  int j,DisplayLine;
  bool in_format;
  uint8_t ClockSettingBefore;
  ClockSettingBefore=ClockSettings[disp];
  // get clock analog/left from eeprom??
  ClockRad =8;
  NewLineOffset=8;
  DisplayLine=0;// 
  j=0;
  rowPixel=0;
  found=false;
   for (int i=0;i<=DisplayWidth-1; i++){
    if (i <= Message.length()){
      if (Message[i]=='{'){found=true;}
    }}
  if (found){//do RocDisplay formatting and send to display
    // do formatting stuff to format for display if we have formatting { and } 
    
   //DebugSprintfMsgSend(sprintf ( DebugMsg, "Disp:%d PixelsDown%d full <%s>",disp,PixelsDown,Message.c_str()));
  
  
                                   
  if ( (bitRead(ClockSettings[disp],_ClockLeft))&&(bitRead(ClockSettings[disp],_ClockAna))&&(bitRead(ClockSettings[disp],_ClockON)  ) )
             {  ClockSpaceOffset=(ClockRad*2)+1; ClockPos= ClockRad;}
           else{ClockSpaceOffset= 0;             ClockPos= 127-ClockRad;}                     
  
  j=0;in_format=false;
  //clean out Formatted message.
  FormattedMsg="";
  MsgLength=Message.length();
  //DebugSprintfMsgSend(sprintf ( DebugMsg, "Disp:%d PixelsDown%d Before format<%s>",disp,PixelsDown,Message.c_str()));
  if (Message.length()>=1){  
    for (int i=0;i<=(Message.length()-1);i++){
         if (Message[i]=='{'){ if (disp==1){display1.drawString(rowPixel+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),FormattedMsg);rowPixel=rowPixel+display1.getStringWidth(FormattedMsg);}
                               if (disp==2){display2.drawString(rowPixel+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),FormattedMsg);rowPixel=rowPixel+display2.getStringWidth(FormattedMsg);}
                               if (disp==3){display3.drawString(rowPixel+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),FormattedMsg);rowPixel=rowPixel+display3.getStringWidth(FormattedMsg);}
                               if (disp==4){display4.drawString(rowPixel+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),FormattedMsg);rowPixel=rowPixel+display4.getStringWidth(FormattedMsg);}
                               
                               // using .getStringWidth(FormattedMsg)to move cursor across in case we have more text to print on the same line before next (T) Tab
                               in_format=true;                                
                               }
     
   
         if ((in_format && Message[i]=='S')){  // this code looks at the Show Clock(S) coding. 
                             if (Message[i+1]=='R'){bitSet(ClockSettings[disp],_ClockON); bitClear(ClockSettings[disp],_ClockLeft);bitSet(ClockSettings[disp],_ClockAna);} // 
                             if (Message[i+1]=='L'){bitSet(ClockSettings[disp],_ClockON); bitSet(ClockSettings[disp],_ClockLeft);bitSet(ClockSettings[disp],_ClockAna);}
                             if (Message[i+1]=='0'){bitClear(ClockSettings[disp],_ClockON);bitClear(ClockSettings[disp],_ClockAna);} // 
                             if (Message[i+1]=='1'){bitSet(ClockSettings[disp],_ClockON); bitClear(ClockSettings[disp],_ClockAna);}
                             Serial.print("<");Serial.print(disp);Serial.print(">");Serial.print(Message[i+1]);Serial.print("_ ");
                                         i=i+1;  
                                        if (ClockSettings[disp]!=ClockSettingBefore){
                                            Data_Updated = true;
                                            WriteEEPROM();
                                            EPROM_Write_Delay = millis() + 1000;
                                                        }
                                         }
   
                                         
         if ((in_format && Message[i]=='L')){  // this code looks at the Lines (L) coding. 
                             if (Message[i+1]=='0'){ if (DisplayLine!=0){rowPixel=0;} DisplayLine=0;   } // reset rowpixel if changing lines
                             if (Message[i+1]=='1'){ if (DisplayLine!=1){rowPixel=0;} DisplayLine=1;   } // 
                                         i=i+1;    }
                               
         if ((in_format && Message[i]=='F')){  // this code looks at the Fonts (F) coding. 
                              //DebugSprintfMsgSend(sprintf ( DebugMsg, "Disp:%d Changing font to %d  getnumber<%d>",disp,(Message[i+1]-48),GetNumber(Message,i+1)));
                             SetFont(disp,GetNumber(Message,i+1));    // selects Font  Message[] (now not limited to just 9)
                             //i=i+1;
                                             } 
       
                                            
                                             
                                             // ?? add W .. Departure column width in pixels..
     
        if ((in_format && Message[i]=='T')){  // this code looks at the Columns (T) coding. 
                             if (Message[i+1]=='0'){rowPixel=rowPixel+1; if ((rowPixel<=10)||(DisplayLine==1)) {rowPixel=10;}    } // 10 is pixel start for column for departures, unless initial stuff is bigger.
                             if (Message[i+1]=='1'){rowPixel=102; if ( bitRead(ClockSettings[disp],_ClockAna)) {rowPixel=86;}   } // (80 with analog clock, 102 without) is pixel start for time column 
                                         i=i+1;    }  

        if ((in_format && Message[i]=='B')){  // this code looks at the Bitmaps (B) coding. 
                               BitChar=GetNumber(Message,i+1)+1; //Select char from bitmaps with offset to avoid asking for char 0 
                               BitMsg=BitChar;BitMsg+="";
                              FontVOffset=0;
                              //DebugSprintfMsgSend(sprintf ( DebugMsg, "Disp:%d Byte %d  %d",disp,(Message[i+1]-48),GetNumber(Message,i+1)));
                               if (disp==1){display1.setFont(RocBitMap);
                                            display1.drawString(rowPixel+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),BitMsg);
                                            rowPixel=rowPixel+display1.getStringWidth(BitMsg);
                                             }
                               if (disp==2){display2.setFont(RocBitMap);
                                            display2.drawString(rowPixel+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),BitMsg);
                                            rowPixel=rowPixel+display2.getStringWidth(BitMsg);
                                            }
                               if (disp==3){display3.setFont(RocBitMap);
                                            display3.drawString(rowPixel+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),BitMsg);
                                            rowPixel=rowPixel+display3.getStringWidth(BitMsg);
                                           }
                               if (disp==4){display4.setFont(RocBitMap);
                                            display4.drawString(rowPixel+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),BitMsg);
                                            rowPixel=rowPixel+display4.getStringWidth(BitMsg);
                                            }            
                               
                              SetFont(disp,FontSelected); // set back to last selected font; 
                             //DebugSprintfMsgSend(sprintf ( DebugMsg, "Disp:%d Byte %d  %d",disp,(Message[i+1]-48),GetNumber(Message,i+1)));// gets number from Message next byte(s)
                             i=i+1;
                                             } 

                                                                     
          
        if (!(in_format)&&(j<=(DisplayWidth-1))){
                                        if (!( ((Message[i-1]=='}')&&(Message[i]==' ')) ) )  {  // do not copy first space after '}' helps with alignment + saves display space,
                                            FormattedMsg+=Message[i];j=j+1; } 
                                            }
                                            
          if (Message[i]=='}'){ in_format=false;j=0;FormattedMsg="";}
          
          
     }
     }
    //draw small real time clock in the top line only 
    
    if (bitRead(ClockSettings[disp],_ClockLeft)){ClockSpaceOffset=(ClockRad*2)+1;ClockPos=ClockRad;}
                                            else{ClockSpaceOffset=0;             ClockPos=127-ClockRad;}


    if ( (bitRead(ClockSettings[disp],_ClockAna)) && (bitRead(ClockSettings[disp],_ClockON))&& (disp==1) ){  
                                display1.fillCircle(ClockPos,ClockRad,ClockRad);
                                display1.setColor(BLACK);
                                  showTimeAnalog(disp,ClockRad,ClockPos,ClockRad, 0, 0.6, hrs * 5 + (int)(mins * 5 / 60));
                                  showTimeAnalog(disp,ClockRad,ClockPos,ClockRad, 0, 0.9, mins);
                                display1.setColor(WHITE);
                               }
    
    if ( (bitRead(ClockSettings[disp],_ClockAna)) && (bitRead(ClockSettings[disp],_ClockON))&& (disp==2) ){  
                                display2.fillCircle(ClockPos,ClockRad,ClockRad);
                                display2.setColor(BLACK);
                                  showTimeAnalog(disp,ClockRad,ClockPos,ClockRad, 0, 0.6, hrs * 5 + (int)(mins * 5 / 60));
                                  showTimeAnalog(disp,ClockRad,ClockPos,ClockRad, 0, 0.9, mins);
                                display2.setColor(WHITE);
                               }
    
      if ( (bitRead(ClockSettings[disp],_ClockAna)) && (bitRead(ClockSettings[disp],_ClockON))&& (disp==3) ){  
                                display3.fillCircle(ClockPos,ClockRad,ClockRad);
                                display3.setColor(BLACK);
                                  showTimeAnalog(disp,ClockRad,ClockPos,ClockRad, 0, 0.6, hrs * 5 + (int)(mins * 5 / 60));
                                  showTimeAnalog(disp,ClockRad,ClockPos,ClockRad, 0, 0.9, mins);
                                display3.setColor(WHITE);
                               }
     if ( (bitRead(ClockSettings[disp],_ClockAna)) && (bitRead(ClockSettings[disp],_ClockON))&& (disp==4) ){  
                                display4.fillCircle(ClockPos,ClockRad,ClockRad);
                                display4.setColor(BLACK);
                                  showTimeAnalog(disp,ClockRad,ClockPos,ClockRad, 0, 0.6, hrs * 5 + (int)(mins * 5 / 60));
                                  showTimeAnalog(disp,ClockRad,ClockPos,ClockRad, 0, 0.9, mins);
                                display4.setColor(WHITE);
                               }
     
    }
  return found;
}

void OLEDS_Display(String L1,String L2,String L3,String L4,String L5){
  OLED_5_line_display(1,L1,L2,L3,L4,L5);
  OLED_5_line_display(2,L1,L2,L3,L4,L5);
  OLED_5_line_display(3,L1,L2,"","","");
  OLED_5_line_display(4,L1,L2,"","","");
}
void OLED_5_line_display(int addr,String L1,String L2,String L3,String L4,String L5){
  bool just_text;
  just_text=false;
if ((addr==3)&&(Display3Present)){
    display3.clear();SetFont(addr,10);
    if (!RocDisplayFormatted(3,0,L1)){display3.drawString(offset, 0, L1);}
    if (!RocDisplayFormatted(3,16,L2)){display3.drawString(offset, 16, L2);}
    display3.display();
    }
if ((addr==4)&&(Display4Present)){
    display4.clear();SetFont(addr,10);
    if (!RocDisplayFormatted(4,0,L1)){display4.drawString(offset, 0, L1);}
    if (!RocDisplayFormatted(4,16,L2)){display4.drawString(offset, 16, L2);}
    display4.display();
    }
    
if ((addr==1)&&(Display1Present)){
   display1.clear();SetFont(addr,10);
   // display 4 lines 
  
  if (!RocDisplayFormatted(1,0,L1)){display1.drawString(offset, 0, L1);}
  if (!RocDisplayFormatted(1,16,L2)){display1.drawString(offset, 16, L2);}
  if (!RocDisplayFormatted(1,32,L3)){display1.drawString(offset, 32, L3);}
  if (!RocDisplayFormatted(1,48,L4)){display1.drawString(offset, 48, L4);}
  
  display1.display();
}
if ((addr==2)&&(Display2Present)){
   display2.clear();SetFont(addr,10);// set default font
  if (!RocDisplayFormatted(2,0,L1)){display2.drawString(offset, 0, L1);}
  if (!RocDisplayFormatted(2,16,L2)){display2.drawString(offset,16, L2);}
  if (!RocDisplayFormatted(2,32,L3)){display2.drawString(offset, 32, L3);}
  if (!RocDisplayFormatted(2,48,L4)){display2.drawString(offset, 48, L4);}

  display2.display();
  }
}




void OLED_Displays_Setup(uint8_t Address,int Display,String Message){
     // use both for 4 line termius display and also for 4 option of 2 x32 displays 
      if (Display==1){for (byte i = 0; i <= (DisplayWidth-1); i++) {L1[i]=(Message[i]);}}
      if (Display==2){for (byte i = 0; i <= (DisplayWidth-1); i++) {L2[i]=(Message[i]);}}
      if (Display==3){for (byte i = 0; i <= (DisplayWidth-1); i++) {L3[i]=(Message[i]);}}
      if (Display==4){for (byte i = 0; i <= (DisplayWidth-1); i++) {L4[i]=(Message[i]);}}
   
      if (Display==5){for (byte i = 0; i <= (DisplayWidth-1); i++) {L5[i]=(Message[i]);}}
      if (Display==6){for (byte i = 0; i <= (DisplayWidth-1); i++) {L6[i]=(Message[i]);}}
      if (Display==7){for (byte i = 0; i <= (DisplayWidth-1); i++) {L7[i]=(Message[i]);}}
      if (Display==8){for (byte i = 0; i <= (DisplayWidth-1); i++) {L8[i]=(Message[i]);}}
   
}





extern uint16_t SW_REV;
extern String wifiSSID;
void OLED_initiate(uint8_t address,int I2CBus){
  String MSGText1;String MSGText2;
  if (address==1){ 
   Serial.println(F("Initiating Display 1")); 
   display1.init();  
   display1.setI2cAutoInit(true);
   display1.flipScreenVertically();
   display1.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   SetFont(1,0);
   }
  if (address==2){  
   Serial.println(F("Initiating Display 2")); 
  display2.init();Disp2ReInit=millis()+6000;
  display2.setI2cAutoInit(true);  
  display2.flipScreenVertically();
  display2.setTextAlignment(TEXT_ALIGN_CENTER); offset=64;
  SetFont(2,0);
    }
  if (address==3){ 
    Serial.println(F("Initiating Display 3")); 
   display3.init(); 
   display3.setI2cAutoInit(true); 
   display3.flipScreenVertically();
   display3.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   SetFont(3,0);
   }
   if (address==4){ 
    Serial.println(F("Initiating Display 4")); 
   display4.init(); 
   display4.setI2cAutoInit(true); 
   display4.flipScreenVertically();
   display4.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   SetFont(4,0);
   }
}

    
void LookForOLEDs(void){
Serial.println ();
  Serial.print ("I2C scanner. using secondary SDA2:");Serial.print(OLED_SDA2);Serial.print("  SCL2:");Serial.print(OLED_SCL2);Serial.println("  Scanning");
  byte count = 0;
  Wire.begin(OLED_SDA2, OLED_SCL2);  // this is bus2
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.print (")");
      if (i==60){Serial.println (" Display 3 ");Display3Present=true;OLED_initiate(3,2);count++;}
      } // end of good response
  } // end of for loop
  Serial.println ();
  Serial.print ("I2C scanner. using SDA:");Serial.print(OLED_SDA);Serial.print("  SCL:");Serial.print(OLED_SCL);Serial.println("  Scanning");
  Wire.begin(OLED_SDA, OLED_SCL);  // 
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.print (")");
      // if display 3 is present assume any address 60 disp 1 here will be 32 high == disp 4
      if ((OLED_SDA!=OLED_SDA2)&& (OLED_SCL!=OLED_SCL2)){
       if (i==60){if (Display3Present){Serial.println (" Display 4 "); Display4Present=true;OLED_initiate(4,1);count++;}
                                       else{Serial.println (" Display 1 ");Display1Present=true;OLED_initiate(1,1);count++;}
                                       }
                                       }
           
        if (i==61){Serial.println (" Display 2 ");Display2Present=true;OLED_initiate(2,1);count++;}
 
      } // end of good response
  } // end of for loop
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" OLED (s).");
  
  }



void RRPowerOnIndicator(int Disp) {
if (!POWERON){
  if((Disp==1)&&(Display1Present)){display1.drawRect(0,0,8,8);}
  if((Disp==2)&&(Display2Present)){display2.drawRect(0,0,8,8);}
  if((Disp==3)&&(Display3Present)){display3.drawRect(0,0,8,8);}
  if((Disp==4)&&(Display4Present)){display4.drawRect(0,0,8,8);}

  }else
  {
  if((Disp==1)&&(Display1Present)){display1.fillRect(0,0,8,8);}
  if((Disp==2)&&(Display2Present)){display2.fillRect(0,0,8,8);}
  if((Disp==3)&&(Display3Present)){display3.fillRect(0,0,8,8);}
  if((Disp==4)&&(Display4Present)){display4.fillRect(0,0,8,8);}
  
  }

 
}




void SignalStrengthBar(int disp, int32_t rssi) { //https://stackoverflow.com/questions/15797920/how-to-convert-wifi-signal-strength-from-quality-percent-to-rssi-dbm
  int PosX,PosY;
 if((Display1Present)&&(disp==1)){
  // rssi -90 is just about dropout..
  // rssi -40 is a great signal
  PosX=108;  // position left right  max = 128
  PosY=0; // top left position up / down max 64
  display1.drawVerticalLine(PosX,PosY,10);
  display1.drawLine(PosX,PosY+4,PosX-4,PosY);
  display1.drawLine(PosX,PosY+4,PosX+4,PosY);
  if (rssi >= -50) { display1.drawLine(PosX+10,PosY,PosX+10,PosY+10);}
  if (rssi >= -55){display1.drawLine(PosX+8,PosY+2,PosX+8,PosY+10);}
  if (rssi >= -65){display1.drawLine(PosX+6,PosY+4,PosX+6,PosY+10);}
  if (rssi >= -70){display1.drawLine(PosX+4,PosY+6,PosX+4,PosY+10);}
  if (rssi >= -80){display1.drawLine(PosX+2,PosY+8,PosX+2,PosY+10);}
 }

if((Display2Present)&&(disp==2)){
  // rssi -90 is just about dropout..
  // rssi -40 is a great signal
  PosX=108;  // position left right  max = 128
  PosY=0; // top left position up / down max 64
  display2.drawVerticalLine(PosX,PosY,10);
  display2.drawLine(PosX,PosY+4,PosX-4,PosY);
  display2.drawLine(PosX,PosY+4,PosX+4,PosY);
  if (rssi >= -50) { display2.drawLine(PosX+10,PosY,PosX+10,PosY+10);}
  if (rssi >= -55){display2.drawLine(PosX+8,PosY+2,PosX+8,PosY+10);}
  if (rssi >= -65){display2.drawLine(PosX+6,PosY+4,PosX+6,PosY+10);}
  if (rssi >= -70){display2.drawLine(PosX+4,PosY+6,PosX+4,PosY+10);}
  if (rssi >= -80){display2.drawLine(PosX+2,PosY+8,PosX+2,PosY+10);}
 }

 
if((Display3Present)&&(disp==3)){
  // rssi -90 is just about dropout..
  // rssi -40 is a great signal
  PosX=108;  // position left right  max = 128
  PosY=0; // top left position up / down max 64
  display3.drawVerticalLine(PosX,PosY,10);
  display3.drawLine(PosX,PosY+4,PosX-4,PosY);
  display3.drawLine(PosX,PosY+4,PosX+4,PosY);
  if (rssi >= -50) { display3.drawLine(PosX+10,PosY,PosX+10,PosY+10);}
  if (rssi >= -55){display3.drawLine(PosX+8,PosY+2,PosX+8,PosY+10);}
  if (rssi >= -65){display3.drawLine(PosX+6,PosY+4,PosX+6,PosY+10);}
  if (rssi >= -70){display3.drawLine(PosX+4,PosY+6,PosX+4,PosY+10);}
  if (rssi >= -80){display3.drawLine(PosX+2,PosY+8,PosX+2,PosY+10);}
 }

if((Display3Present)&&(disp==4)){
  // rssi -90 is just about dropout..
  // rssi -40 is a great signal
  PosX=108;  // position left right  max = 128
  PosY=0; // top left position up / down max 64
  display4.drawVerticalLine(PosX,PosY,10);
  display4.drawLine(PosX,PosY+4,PosX-4,PosY);
  display4.drawLine(PosX,PosY+4,PosX+4,PosY);
  if (rssi >= -50) { display4.drawLine(PosX+10,PosY,PosX+10,PosY+10);}
  if (rssi >= -55){display4.drawLine(PosX+8,PosY+2,PosX+8,PosY+10);}
  if (rssi >= -65){display4.drawLine(PosX+6,PosY+4,PosX+6,PosY+10);}
  if (rssi >= -70){display4.drawLine(PosX+4,PosY+6,PosX+4,PosY+10);}
  if (rssi >= -80){display4.drawLine(PosX+2,PosY+8,PosX+2,PosY+10);}
 }


 
 }



void showTimeAnalog(int disp,int clocksize,int center_x, int center_y, double pl1, double pl2, double pl3)
{ 
  float angle;
  int  x1, x2, y1, y2;
  angle = ( (6*pl3) / 57.29577951 ) ; //Convert degrees to radians
                      //https://github.com/ThingPulse/esp8266-oled-ssd1306/blob/master/examples/SSD1306ClockDemo/SSD1306ClockDemo.ino
  x1 = center_x + (clocksize * pl1) * sin(angle );
  y1 = center_y - (clocksize * pl1) * cos(angle );
  x2 = center_x + (clocksize * pl2) * sin(angle);
  y2 = center_y - (clocksize * pl2) * cos(angle);
  if ((disp==1)&&(Display1Present)){display1.drawLine(x1,y1,x2,y2);}
  if ((disp==2)&&(Display2Present)){display2.drawLine(x1,y1,x2,y2);}
  if ((disp==3)&&(Display3Present)){display3.drawLine(x1,y1,x2,y2);}
  if ((disp==4)&&(Display4Present)){display4.drawLine(x1,y1,x2,y2);}
}

void showTimeAnalogCircle(int disp,int clocksize,int circsize,int center_x, int center_y, double pl1, double pl2, double pl3)
{ 
 float angle;
  int  x1, x2, y1, y2;
  angle = ( (6*pl3) / 57.29577951 ) ; //Convert degrees to radians
                      //https://github.com/ThingPulse/esp8266-oled-ssd1306/blob/master/examples/SSD1306ClockDemo/SSD1306ClockDemo.ino
  x1 = center_x + (clocksize * pl1) * sin(angle );
  y1 = center_y - (clocksize * pl1) * cos(angle );
  x2 = center_x + (clocksize * pl2) * sin(angle);
  y2 = center_y - (clocksize * pl2) * cos(angle);
  if ((disp==1)&&(Display1Present)){display1.drawCircle(x2, y2,circsize);}
  if ((disp==2)&&(Display2Present)){display2.drawCircle(x2,y2,circsize);}
   if ((disp==3)&&(Display3Present)){display3.drawCircle(x2,y2,circsize);}
    if ((disp==4)&&(Display4Present)){display4.drawCircle(x2,y2,circsize);}
}

void BigClock(int disp,int clocksize){
  int center_x,center_y;
  center_x=64;
  center_y=clocksize;
  if ((disp==1)&&(Display1Present)){
          display1.drawCircle(center_x, center_y, clocksize);
          for (int i=1;i<=12;i++){showTimeAnalog(1,clocksize,center_x,center_y, 0.8 ,0.95 , (i * 5) );}
          showTimeAnalog(1,clocksize,center_x,center_y, -0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
          showTimeAnalog(1,clocksize,center_x,center_y, -0.1, 0.9, mins);
         if (divider==1){ showTimeAnalog(1,clocksize,center_x,center_y, -0.2, 0.5, secs);
          showTimeAnalogCircle(1,clocksize,4, center_x,center_y, -0.2, 0.65, secs);
          showTimeAnalog(1,clocksize,center_x,center_y, 0.8, 0.9, secs);}
          display1.display();
          }
  
  if ((disp==2)&&(Display2Present)){
         display2.drawCircle(center_x, center_y, clocksize);
          for (int i=1;i<=12;i++){showTimeAnalog(2,clocksize,center_x,center_y, 0.8 ,0.95 , (i * 5) );}
          showTimeAnalog(2,clocksize,center_x,center_y, -0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
          showTimeAnalog(2,clocksize,center_x,center_y, -0.1, 0.9, mins);
         if (divider==1){ showTimeAnalog(2,clocksize,center_x,center_y, -0.2, 0.5, secs);
          showTimeAnalogCircle(2,clocksize,4, center_x,center_y, -0.2, 0.65, secs);
          showTimeAnalog(2,clocksize,center_x,center_y, 0.8, 0.9, secs);}
          display2.display();
          }
          

if ((disp==3)&&(Display3Present)){
          display3.drawCircle(center_x, center_y, clocksize);
            for (int i=1;i<=12;i++){showTimeAnalog(disp,clocksize,center_x,center_y, 0.8 ,0.9, (i * 5) );}
            showTimeAnalog(disp,clocksize,center_x,center_y, -0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
            showTimeAnalog(disp,clocksize,center_x,center_y, -0.1, 0.9, mins);
            if (divider==1){ showTimeAnalog(disp,clocksize,center_x,center_y, -0.2, 0.5, secs);
              showTimeAnalogCircle(disp,clocksize,2, center_x,center_y, -0.2, 0.65, secs);
              showTimeAnalog(disp,clocksize,center_x,center_y, 0.8, 0.9, secs);}
          display3.display();
          }

if ((disp==4)&&(Display3Present)){
          display4.drawCircle(center_x, center_y, clocksize);
            for (int i=1;i<=12;i++){showTimeAnalog(disp,clocksize,center_x,center_y, 0.8 ,0.9, (i * 5) );}
            showTimeAnalog(disp,clocksize,center_x,center_y, -0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
            showTimeAnalog(disp,clocksize,center_x,center_y, -0.1, 0.9, mins);
            if (divider==1){ showTimeAnalog(disp,clocksize,center_x,center_y, -0.2, 0.5, secs);
              showTimeAnalogCircle(disp,clocksize,2, center_x,center_y, -0.2, 0.65, secs);
              showTimeAnalog(disp,clocksize,center_x,center_y, 0.8, 0.9, secs);}
          display4.display();
          }
  
}

extern int32_t SigStrength(void);

void OLED_Status(){
  String MSGText;
  char MSGTextC[20];
  bool Analog;
  int cx;
  Analog=true;
  
  //Digital Time display 
              cx=sprintf(MSGTextC,"%02d:%02d:%02d",hrs,mins,secs);  
  
if (Display1Present){ 
   display1.setTextAlignment(TEXT_ALIGN_LEFT); offset=0;
   SetFont(1,TerminalDisplayFont);               
   OLED_5_line_display(1,L1,L2,L3,L4,L5);
   display1.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   RRPowerOnIndicator(1);
   if ((L1[0]==0)&&(L2[0]==0)&&(L3[0]==0)&&(L4[0]==0)&&(L5[0]==0)){ SignalStrengthBar(1,SigStrength());BigClock(1,30); }
   }// adds time display 1
if (Display2Present){ 
   display2.setTextAlignment(TEXT_ALIGN_LEFT); offset=0;
   SetFont(2,TerminalDisplayFont);  
   OLED_5_line_display(2,L5,L6,L7,L8,L9);
   display2.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   if ((L6[0]==0)&&(L7[0]==0)&&(L8[0]==0)&&(L9[0]==0)&&(L5[0]==0)){RRPowerOnIndicator(2);SignalStrengthBar(2,SigStrength());BigClock(2,30);}
   } // display 2
   
if (Display3Present){ 
   display3.setTextAlignment(TEXT_ALIGN_LEFT); offset=0;
   SetFont(3,TerminalDisplayFont);  
   OLED_5_line_display(3,L1,L2,"","","");
   display3.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   if ((L1[0]==0)&&(L2[0]==0)){RRPowerOnIndicator(3);SignalStrengthBar(3,SigStrength());BigClock(3,15);} 
   } // display 3 is x32

if (Display4Present){ 
   display4.setTextAlignment(TEXT_ALIGN_LEFT); offset=0;
   SetFont(4,TerminalDisplayFont);  
   OLED_5_line_display(4,L3,L4,"","","");
   display4.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   if ((L3[0]==0)&&(L4[0]==0)){RRPowerOnIndicator(4);SignalStrengthBar(4,SigStrength());BigClock(4,15);} 
   } // display 4 is x32
}

void TimeGears(){
  // to run as a stand alone clock, this code fully updates mins and hrs.
  // BUT if divider != 1 rocrail can stop the clock, so we hold secs at 60 and stop incrementing hrs and mins (which Rocrail will have to increment)

  if(divider==1){
     if (secs>=60){
              secs=0;mins=mins+1;
                     if (mins>=60){
                         mins=0;hrs=hrs+1;
                                if (hrs>=25){
                                      hrs=1;
                                            }
                                   }
              }
  }else{
    if (secs>=60){secs=60;}
  }
}




















