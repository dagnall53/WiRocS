
  #include "OLED.h"
  #include <Arduino.h> //needed 
  #include "Directives.h"
  // these outside #define to save putting more #defines in other places..
  bool OLED1Present,OLED2Present,OLED3Present,OLED4Present;
  uint8_t OLed_Clock_Settings[5]; ///bit settings
  #define _ClockON 0
  #define _ClockLeft 1
  #define _ClockAna 2
 
  #ifdef _OLED
  //#include "Globals.h"
   #include <SSD1306.h> // alias for `#include "SSD1306Wire.h"` //https://github.com/ThingPulse/esp8266-oled-ssd1306
   // options are 0x3c 0x3d on bus 1 (OLED1 and OLED2)  or - IF 0x3c is seen on bus 2 it assumes this is 32 high so any 0x3c on bus 1 must also be 32 hi (becoming OLED3 and OLED4)  
  
  // small displays
  #ifdef  _all64High
  SSD1306  OLED1(0x3c, OLED_SDA, OLED_SCL);//== RocDisplay 1,2,3&4
  SSD1306  OLED3(0x3d, OLED_SDA, OLED_SCL);// RocDisplays 5,6,7,8  
  SSD1306 OLED2(0x3c, OLED_SDA2, OLED_SCL2); // in all 64 hi mode, all oleds are 64 high
  SSD1306 OLED4(0x3d, OLED_SDA2, OLED_SCL2);  //
  #else
  SSD1306  OLED1(0x3c, OLED_SDA, OLED_SCL);//== RocDisplay 1,2,3&4
  SSD1306  OLED2(0x3d, OLED_SDA, OLED_SCL);// RocDisplays 5,6,7,8  
  SSD1306 OLED3(0x3c, OLED_SDA2, OLED_SCL2, GEOMETRY_128_32 ); // RocDisplays 1&2 //Display 3 is the only one to use the secondary ports
  SSD1306 OLED4(0x3c, OLED_SDA, OLED_SCL, GEOMETRY_128_32 );  //RocDisplays 3&4
  #endif 
   //
  
#define TextObjectLength 150  //>100 to allow passing of {} formatting but Rocnet protocol limits the length sent to about 110    
#define TerminalDisplayFont 0
#define NumberofTextStores 17

char TS[NumberofTextStores][TextObjectLength]; 

uint8_t offset;




bool RocFormatUsed[5];

//https://github.com/ThingPulse/esp8266-oled-ssd1306
// you will also need to add RodDisplay fonts (see https://github.com/dagnall53/Fonts-for-WiRocs)  to C:\Arduino\libraries\esp8266-oled-ssd1306-master\src\OLEDDisplayFonts.h

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
int MaxWidth;

// new set of OLED stuff to allow indexing
void OLEDdrawString(int OLED,int16_t x, int16_t y, String text){
 switch (OLED) { 
  case 1:   OLED1.drawString(x,y,text);     break;
  case 2:   OLED2.drawString(x,y,text);     break;
  case 3:   OLED3.drawString(x,y,text);     break;
  case 4:   OLED4.drawString(x,y,text);     break;
  default:     break;  
}
}
void OLEDclear(int OLED){
 switch (OLED) { 
  case 1:   OLED1.clear();     break;
  case 2:   OLED2.clear();     break;
  case 3:   OLED3.clear();     break;
  case 4:   OLED4.clear();     break;
  default:     break;  
}
}
uint16_t OLEDgetStringWidth(int OLED,String text){
 uint16_t result;
 switch (OLED) { 
  case 1:   result= OLED1.getStringWidth(text);     break;
  case 2:   result= OLED2.getStringWidth(text);     break;
  case 3:   result= OLED3.getStringWidth(text);     break;
  case 4:   result= OLED4.getStringWidth(text);     break;
  default:   result= -1;  break;  
}
return result;
}
void OLEDdrawStringMaxWidth(int OLED,int16_t x, int16_t y,int16_t maxLineWidth, String text){
 switch (OLED) { 
  case 1:   OLED1.drawStringMaxWidth(x,y,maxLineWidth, text);     break;
  case 2:   OLED2.drawStringMaxWidth(x,y,maxLineWidth, text);     break;
  case 3:   OLED3.drawStringMaxWidth(x,y,maxLineWidth, text);     break;
  case 4:   OLED4.drawStringMaxWidth(x,y,maxLineWidth, text);     break;
  default:     break;  
}
}

void OLEDdrawLine(int OLED,int16_t x0, int16_t y0, int16_t x1, int16_t y1){
 switch (OLED) { 
  case 1:   OLED1.drawLine(x0,y0,x1,y1);     break;
  case 2:   OLED2.drawLine(x0,y0,x1,y1);     break;
  case 3:   OLED3.drawLine(x0,y0,x1,y1);     break;
  case 4:   OLED4.drawLine(x0,y0,x1,y1);     break;
  default:     break;  
}
}

void OLEDdrawVerticalLine(int OLED,int16_t x0, int16_t y0, int16_t l){
 switch (OLED) { 
  case 1:   OLED1.drawVerticalLine(x0,y0,l);     break;
  case 2:   OLED2.drawVerticalLine(x0,y0,l);     break;
  case 3:   OLED3.drawVerticalLine(x0,y0,l);     break;
  case 4:   OLED4.drawVerticalLine(x0,y0,l);     break;
  default:     break;  
}
}
void OLEDdrawRect(int OLED,int16_t x0, int16_t y0, int16_t x1, int16_t y1){
 switch (OLED) { 
  case 1:   OLED1.drawRect(x0,y0,x1,y1);     break;
  case 2:   OLED2.drawRect(x0,y0,x1,y1);     break;
  case 3:   OLED3.drawRect(x0,y0,x1,y1);     break;
  case 4:   OLED4.drawRect(x0,y0,x1,y1);     break;
  default:     break;  
}
}

void OLEDfillRect(int OLED,int16_t x0, int16_t y0, int16_t x1, int16_t y1){
 switch (OLED) { 
  case 1:   OLED1.fillRect(x0,y0,x1,y1);     break;
  case 2:   OLED2.fillRect(x0,y0,x1,y1);     break;
  case 3:   OLED3.fillRect(x0,y0,x1,y1);     break;
  case 4:   OLED4.fillRect(x0,y0,x1,y1);     break;
  default:     break;  
}
}
void OLEDsetFont(int OLED,const uint8_t* fontData){
 switch (OLED) { 
  case 1:   OLED1.setFont(fontData);     break;
  case 2:   OLED2.setFont(fontData);     break;
  case 3:   OLED3.setFont(fontData);     break;
  case 4:   OLED4.setFont(fontData);     break;
  default:     break;  
}
}

void OLEDsetColor(int OLED, OLEDDISPLAY_COLOR color){
switch (OLED) { 
  case 1:   OLED1.setColor(color);     break;
  case 2:   OLED2.setColor(color);     break;
  case 3:   OLED3.setColor(color);     break;
  case 4:   OLED4.setColor(color);     break;
  default:     break;  
}
}

void OLEDfillCircle(int OLED, int16_t x, int16_t y, int16_t radius){
switch (OLED) { 
  case 1:   OLED1.fillCircle(x,y,radius);     break;
  case 2:   OLED2.fillCircle(x,y,radius);     break;
  case 3:   OLED3.fillCircle(x,y,radius);     break;
  case 4:   OLED4.fillCircle(x,y,radius);     break;
  default:     break;  
}
}
void OLEDdrawCircle(int OLED, int16_t x, int16_t y, int16_t radius){
switch (OLED) { 
  case 1:   OLED1.drawCircle(x,y,radius);     break;
  case 2:   OLED2.drawCircle(x,y,radius);     break;
  case 3:   OLED3.drawCircle(x,y,radius);     break;
  case 4:   OLED4.drawCircle(x,y,radius);     break;
  default:     break;  
}
}
bool OLEDPresent(int OLED){
  bool result;
  result=false;  
switch (OLED) { 
  case 1:   result=OLED1Present;     break;
  case 2:   result=OLED2Present;     break;
  case 3:   result=OLED3Present;     break;
  case 4:   result=OLED4Present;     break;
  default:     break;  
}
return result;
}
void OLEDsetTextAlignment(int OLED,OLEDDISPLAY_TEXT_ALIGNMENT textAlignment){

  switch (OLED) { 
  case 1:   OLED1.setTextAlignment(textAlignment);    break;
  case 2:   OLED2.setTextAlignment(textAlignment);    break;
  case 3:   OLED3.setTextAlignment(textAlignment);    break;
  case 4:   OLED4.setTextAlignment(textAlignment);    break;
  default:     break;  
}
}

void SetFont(uint8_t OLed_x,uint8_t Font){
FontSelected=Font;
switch (Font) { 
    case 0: OLEDsetFont(OLed_x,Font_7x5); break;
    case 1: OLEDsetFont(OLed_x,Font_6x5w);  break;
    case 2: OLEDsetFont(OLed_x,Font_6x5n);   break;
    case 3: OLEDsetFont(OLed_x,Font_5x5inv);  break;
    case 4: OLEDsetFont(OLed_x,Font_High);  break;
    
    default:OLEDsetFont(OLed_x,Font_7x5);  break;
         }
}

void OLEDDisplay(uint8_t OLED){
switch (OLED) { 
  case 1:   OLED1.display();     break;
  case 2:   OLED2.display();     break;
  case 3:   OLED3.display();     break;
  case 4:   OLED4.display();     break;
  default:     break;  
  
}
}   
 
void StringToChar(char *line, String input){
  for (int i=0;i<=TextObjectLength-1; i++){
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
int ClockRad;
bool RocDisplayFormatted(int OLed_x, int PixelsDown, String Message){
  bool found;  bool in_format;
  int j,DisplayLine,RowPixel[15],NewLineOffset,MsgLength,TabOne,TabZero;
  int ClockSpaceOffset,ClockPos;
  //bool ClockAnalog,ClockLeft;
  String FormattedMsg,BitMsg; char BitChar;
  bool FlashON,ignoreJ1,ignoreJ2,inJ1,inJ2;
  FlashON=secs%2;
  ignoreJ1=false;inJ1=false;
  ignoreJ2=false;inJ2=false;
  
  uint8_t ClockSettingBefore;
  ClockSettingBefore=OLed_Clock_Settings[OLed_x];
  // get clock analog/left from eeprom??
  ClockRad =9;
  NewLineOffset=8;
  DisplayLine=0;// 
  TabZero=10;TabOne=102;
  j=0;
  for (int i=0;i<=10; i++){
      RowPixel[i]=0;
      }
  found=false;
   for (int i=0;i<=TextObjectLength-1; i++){
    if (i <= Message.length()){
      if (Message[i]=='{'){found=true; RocFormatUsed[OLed_x]=true;}
    }}
  if (found){//do RocDisplay formatting and send to display
    // do formatting stuff to format for display if we have formatting { and } 
    //DebugSprintfMsgSend(sprintf ( DebugMsg, "OLed_x:%d PixelsDown%d full <%s>",OLed_x,PixelsDown,Message.c_str()));
 
   // set default font to RocDisplay Font F0
  SetFont(OLed_x,0);
                                   
  if ( (bitRead(OLed_Clock_Settings[OLed_x],_ClockLeft))&&(bitRead(OLed_Clock_Settings[OLed_x],_ClockAna))&&(bitRead(OLed_Clock_Settings[OLed_x],_ClockON)  ) )
             {  ClockSpaceOffset=(ClockRad*2)+1; ClockPos= ClockRad;}
           else{ClockSpaceOffset= 0;             ClockPos= 127-ClockRad;}                     
  
  j=0;in_format=false;
  //clean out Formatted message.
  FormattedMsg="";
  MsgLength=Message.length();
//  DebugSprintfMsgSend(sprintf ( DebugMsg, "OLed_x:%d PixelsDown%d Before format<%s>",OLed_x,PixelsDown,Message.c_str()));
  //Serial.print("UF text<");Serial.print(Message);Serial.println(">");
  if (Message.length()>=1){  
    for (int i=0;i<=(Message.length()-1);i++){
         if (Message[i]=='{'){ OLEDdrawString(OLed_x,RowPixel[DisplayLine]+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),FormattedMsg);
                                RowPixel[DisplayLine]=RowPixel[DisplayLine]+OLEDgetStringWidth(OLed_x,FormattedMsg);
                               // code uses .getStringWidth(FormattedMsg)to move each line's cursor (RowPixel) across in case we have more text to print on the same line before next (T) Tab
                               in_format=true;                                
                               }
     
   
         if ((in_format && Message[i]=='S')){  // this code looks at the Show Clock(S) coding. 
                             if (Message[i+1]=='R'){bitSet(OLed_Clock_Settings[OLed_x],_ClockON); bitClear(OLed_Clock_Settings[OLed_x],_ClockLeft);bitSet(OLed_Clock_Settings[OLed_x],_ClockAna);} // 
                             if (Message[i+1]=='L'){bitSet(OLed_Clock_Settings[OLed_x],_ClockON); bitSet(OLed_Clock_Settings[OLed_x],_ClockLeft);bitSet(OLed_Clock_Settings[OLed_x],_ClockAna);}
                             if (Message[i+1]=='0'){bitClear(OLed_Clock_Settings[OLed_x],_ClockON);bitClear(OLed_Clock_Settings[OLed_x],_ClockAna);} // 
                             if (Message[i+1]=='1'){bitSet(OLed_Clock_Settings[OLed_x],_ClockON); bitClear(OLed_Clock_Settings[OLed_x],_ClockAna);}
                            // Serial.print("<");Serial.print(OLed_x);Serial.print(">");Serial.print(Message[i+1]);Serial.print("_ ");
                                         i=i+1;  
                                        if (OLed_Clock_Settings[OLed_x]!=ClockSettingBefore){  
                                            Data_Updated = true;
                                            WriteEEPROM();
                                            EPROM_Write_Delay = millis() + 1000;
                                                        }
                                         }
   
                                         
         if ((in_format && Message[i]=='L')){  // this code does the Lines (L) coding, moving stuff down 8 pixels .
                                            DisplayLine=GetNumber(Message,i+1);
                                            //DebugSprintfMsgSend(sprintf ( DebugMsg, "OLed_x:%d Changing Line number to %d GN<%d>",OLed_x,DisplayLine,GetNumber(Message,i+1)));
                                            i=i+1;
                                            } 
                             
                               
         if ((in_format && Message[i]=='F')){  // this code looks at the Fonts (F) coding. 
                             SetFont(OLed_x,GetNumber(Message,i+1));    // selects Font  Message[] (now not limited to just 9)
                             i=i+1;
                                             } 

         if ((in_format && Message[i]=='J')){  // this code looks at the Flash (J) coding. 
                             if (Message[i+1]=='0'){inJ1=false;inJ2=false;ignoreJ1=false;ignoreJ2=false;  } // reset at J0
                             if (Message[i+1]=='1'){inJ1=true;ignoreJ1=true^FlashON;} // 
                             if (Message[i+1]=='2'){inJ2=true;inJ1=false;ignoreJ1=false;ignoreJ2=false^FlashON; } //NB false in ignorej2 here inverts when flashon acts !!
                             i=i+1;
                                             } 
       
         if ((in_format && Message[i]=='W')){  // this code looks at the Departure column width (W) coding. 
                               // ?? W .. "Departure column width" IF set sets the second tab position in pixels..Is volatile, and lost for next message!
                             TabOne=GetNumber(Message,i+1); //S               
                                            } 
                                             
     
        if ((in_format && Message[i]=='T')){  // this code looks at the Columns (T) coding. 
              if ((in_format && Message[i+2]=='L')){DisplayLine=GetNumber(Message,i+3);}// Check for 'TxLx' codes, as they are common! Saves changing to "correct" 'LxTx' structure
                             if (Message[i+1]=='0'){if (RowPixel[DisplayLine]<=TabZero) {RowPixel[DisplayLine]=TabZero;}}  
                             if (Message[i+1]=='1'){RowPixel[DisplayLine]=TabOne; 
                                                    if ( bitRead(OLed_Clock_Settings[OLed_x],_ClockAna)) {RowPixel[DisplayLine]=RowPixel[DisplayLine]-(2*ClockRad);}   
                                                    } // offset if Analog clock is in operation
                                            i=i+1;
                                            }  

        if (((in_format && Message[i]=='B'))&& !((ignoreJ2&&inJ2)||(ignoreJ1&&inJ1))){  // this code looks at the Bitmaps (B) coding. 
                                                            // adding flash capability ignore bitmaps after J1 or J2
                               BitChar=GetNumber(Message,i+1)+1; //Select char from bitmaps with offset to avoid asking for char 0 
                               BitMsg=BitChar;BitMsg+="";
                              
                              //DebugSprintfMsgSend(sprintf ( DebugMsg, "OLed_x:%d Byte %d  %d",OLed_x,(Message[i+1]-48),GetNumber(Message,i+1)));
                               OLEDsetFont(OLed_x,RocBitMap);
                               OLEDdrawString(OLed_x,RowPixel[DisplayLine]+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),BitMsg);
                               RowPixel[DisplayLine]=RowPixel[DisplayLine]+OLEDgetStringWidth(OLed_x,BitMsg);
                               SetFont(OLed_x,FontSelected); // set back to last selected font; 
                               //DebugSprintfMsgSend(sprintf ( DebugMsg, "OLed_x:%d Byte %d  %d",OLed_x,(Message[i+1]-48),GetNumber(Message,i+1)));// gets number from Message next byte(s)
                               i=i+1;
                              } 
          
        if (!(in_format)&&(j<=(TextObjectLength-1))){ // add text to formatted_message to display
                              // if (!( ((Message[i-1]=='}')&&(Message[i]==' ')) ) )  {  // do not copy first space after '}' helps with alignment + saves display space,
                              if (!((ignoreJ2&&inJ2)||(ignoreJ1&&inJ1)))   {    
                                     FormattedMsg+=Message[i];j=j+1;} 
                              } 
                              
                                            
        if (Message[i]=='}'){ in_format=false;j=0;FormattedMsg="";}
     }
     }
    // IF set, draw small real time clock in the top line only 
    
    if (bitRead(OLed_Clock_Settings[OLed_x],_ClockLeft)){ClockSpaceOffset=(ClockRad*2)+1;ClockPos=ClockRad;}
                                            else{ClockSpaceOffset=0;             ClockPos=127-ClockRad;}
                                            
    if ( (bitRead(OLed_Clock_Settings[OLed_x],_ClockAna)) && (bitRead(OLed_Clock_Settings[OLed_x],_ClockON)) ){  
                                OLEDfillCircle(OLed_x,ClockPos,ClockRad,ClockRad);
                                OLEDsetColor(OLed_x,BLACK);
                                  showTimeAnalog(OLed_x,ClockRad,ClockPos,ClockRad, 0, 0.5, hrs * 5 + (int)(mins * 5 / 60));
                                  showTimeAnalog(OLed_x,ClockRad,ClockPos,ClockRad, 0, 1, mins);
                                OLEDsetColor(OLed_x,WHITE);
                               }

    }
  return found;
}

void OLEDS_Display(String L1,String L2,String L3,String L4){
  OLED_4_RN_displays(1,L1,L2,L3,L4);
  OLED_4_RN_displays(2,L1,L2,L3,L4);
  OLED_4_RN_displays(3,L1,L2,"","");
  OLED_4_RN_displays(4,L1,L2,"","");
}
extern int32_t SigStrength(void);
void OLED_4_RN_displays(int OLed_x,String L1,String L2,String L3,String L4){
  if (OLEDPresent(OLed_x)){
   OLEDclear(OLed_x);
   // display 4 lines 
   RocFormatUsed[OLed_x]=false;
      if (!RocDisplayFormatted(OLed_x,0,L1)) {OLEDdrawStringMaxWidth(OLed_x,offset,  0,MaxWidth, L1);}
      if (!RocDisplayFormatted(OLed_x,16,L2)){OLEDdrawStringMaxWidth(OLed_x,offset, 16,MaxWidth, L2);}
      if (!RocDisplayFormatted(OLed_x,32,L3)){OLEDdrawStringMaxWidth(OLed_x,offset, 32,MaxWidth, L3);}
      if (!RocDisplayFormatted(OLed_x,48,L4)){OLEDdrawStringMaxWidth(OLed_x,offset, 48,MaxWidth, L4);}
   OLEDDisplay(OLed_x);

   if ((L1[0]==0)&&(L2[0]==0)&&(L3[0]==0)&&(L4[0]==0)){
            RRPowerOnIndicator(OLed_x); 
            SignalStrengthBar(OLed_x,SigStrength());
     #ifdef _all64High
            BigClock(OLed_x,30); 
     #else
            if(OLed_x>=3){BigClock(OLed_x,15);}else{BigClock(OLed_x,30);}
     #endif
            
            }   
  }

}




void SetupTextArrays(uint8_t Address,int Display,String Message){
     // move the roc messsages to the TS stores where we will look at  them every second!

#ifdef _all64High
      if (Address==60){for (uint16_t i = 0; i <= (TextObjectLength-1); i++) {TS[Display][i]=(Message[i]);}
              Serial.print("setting up TS");Serial.println(Display);
              }
      if (Address==61){for (uint16_t i = 0; i <= (TextObjectLength-1); i++) {TS[Display+8][i]=(Message[i]);} 
              Serial.print("setting up TS");Serial.println(Display+8);              
              }
#else
      for (uint16_t i = 0; i <= (TextObjectLength-1); i++) {TS[Display][i]=(Message[i]);}
#endif      
}





extern uint16_t SW_REV;
extern String wifiSSID;
void OLED_initiate(uint8_t address,int I2CBus){
  String MSGText1;String MSGText2;
  
  if (address==1){ 
   Serial.println(F("Initiating Display 1")); 
   OLED1.init();  
   OLED1.setI2cAutoInit(true);
   OLED1.flipScreenVertically();
   OLED1.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   SetFont(1,0);
   }
  if (address==2){  
   Serial.println(F("Initiating Display 2")); 
  OLED2.init();Disp2ReInit=millis()+6000;
  OLED2.setI2cAutoInit(true);  
  OLED2.flipScreenVertically();
  OLED2.setTextAlignment(TEXT_ALIGN_CENTER); offset=64;
  SetFont(2,0);
    }
  if (address==3){ 
    Serial.println(F("Initiating Display 3")); 
   OLED3.init(); 
   OLED3.setI2cAutoInit(true); 
   OLED3.flipScreenVertically();
   OLED3.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   SetFont(3,0);
   }
   if (address==4){ 
    Serial.println(F("Initiating Display 4")); 
   OLED4.init(); 
   OLED4.setI2cAutoInit(true); 
   OLED4.flipScreenVertically();
   OLED4.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
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
 #ifdef _all64High
      if (i==60){Serial.println (" OLED 2 ");OLED2Present=true;OLED_initiate(2,2);count++;}
      if (i==61){Serial.println (" OLED 4 ");OLED4Present=true;OLED_initiate(4,2);count++;}
 #else
     if (i==60){Serial.println (" OLED 3 ");OLED3Present=true;OLED_initiate(3,2);count++;}
 #endif
      
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
      
#ifdef _all64High
      if (i==60){Serial.println (" OLED 1 ");OLED1Present=true;OLED_initiate(1,1);count++;}
      if (i==61){Serial.println (" OLED 3 ");OLED3Present=true;OLED_initiate(3,1);count++;}
 #else
      if ((OLED_SDA!=OLED_SDA2)&& (OLED_SCL!=OLED_SCL2)){// if not all 64 high if OLED3 is present assume any address 60 on Bus switches to *32 displays therefor== OLED4
       if (i==60){if (OLED3Present){Serial.println (" OLED 4 "); OLED4Present=true;OLED_initiate(4,1);count++;}
                                       else{Serial.println (" OLED 1 ");OLED1Present=true;OLED_initiate(1,1);count++;}
                                       }
                                       }
           
        if (i==61){Serial.println (" OLED 2 ");OLED2Present=true;OLED_initiate(2,1);count++;}
 #endif
      } // end of good response
  } // end of for loop
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" OLED (s).");
  
  }



void RRPowerOnIndicator(int OLED_x) {
      if (!POWERON){ OLEDdrawRect(OLED_x,0,0,8,8);}
            else{    OLEDfillRect(OLED_x,0,0,8,8);}    
            }
  



void SignalStrengthBar(int OLED_x, int32_t rssi) { //https://stackoverflow.com/questions/15797920/how-to-convert-wifi-signal-strength-from-quality-percent-to-rssi-dbm
  int PosX,PosY;
 if((OLEDPresent(OLED_x))){
  // rssi -90 is just about dropout..
  // rssi -40 is a great signal
  PosX=108;  // position left right  max = 128
  PosY=0; // top left position up / down max 64
  OLEDdrawVerticalLine(OLED_x,PosX,PosY,10);
  OLEDdrawLine( OLED_x,PosX,PosY+4,PosX-4,PosY);
  OLEDdrawLine( OLED_x,PosX,PosY+4,PosX+4,PosY);
  if (rssi >= -50){OLEDdrawLine( OLED_x,PosX+10,PosY,PosX+10,PosY+10);}
  if (rssi >= -55){OLEDdrawLine( OLED_x,PosX+8,PosY+2,PosX+8,PosY+10);}
  if (rssi >= -65){OLEDdrawLine( OLED_x,PosX+6,PosY+4,PosX+6,PosY+10);}
  if (rssi >= -70){OLEDdrawLine( OLED_x,PosX+4,PosY+6,PosX+4,PosY+10);}
  if (rssi >= -80){OLEDdrawLine( OLED_x,PosX+2,PosY+8,PosX+2,PosY+10);}
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
  if (OLEDPresent(disp)){OLEDdrawLine( disp,x1,y1,x2,y2);}
  
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
  if (OLEDPresent(disp)){OLEDdrawCircle(disp,x2, y2,circsize);}
  }

void BigClock(int disp,int clocksize){
  int center_x,center_y;
  center_x=64;
  center_y=clocksize;
  if (OLEDPresent(disp)){
          OLEDdrawCircle(disp,center_x, center_y, clocksize);
          for (int i=1;i<=12;i++){
                showTimeAnalog(disp,clocksize,center_x,center_y, 0.8 ,0.95 , (i * 5) );}
          showTimeAnalog(disp,clocksize,center_x,center_y, -0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
          showTimeAnalog(disp,clocksize,center_x,center_y, -0.1, 0.9, mins);
         if (divider==1){ showTimeAnalog(disp,clocksize,center_x,center_y, -0.2, 0.5, secs);
              showTimeAnalogCircle(disp,clocksize,4, center_x,center_y, -0.2, 0.65, secs);
              showTimeAnalog(disp,clocksize,center_x,center_y, 0.8, 0.9, secs);}
          OLEDDisplay(disp);
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
  MaxWidth=128; // for word wrap
  for (int i=1;i<=4;i++){
   // Serial.print(" Display:");Serial.print(i);Serial.print(" present");Serial.println(OLEDPresent(i));
    if (OLEDPresent(i)){ 
      OLEDsetTextAlignment(i,TEXT_ALIGN_LEFT); offset=0;  SetFont(i,TerminalDisplayFont);  
      if ( (RocFormatUsed[i])&&(bitRead(OLed_Clock_Settings[i],_ClockAna))&&(bitRead(OLed_Clock_Settings[i],_ClockON)  ) ){// any roc formatted text and analog clock on showing??
            if (!bitRead(OLed_Clock_Settings[i],_ClockLeft) ){
                            offset=0;MaxWidth=127-(ClockRad*2);}else{
                            offset=1+(ClockRad*2);MaxWidth=127-(ClockRad*2);} }
      if ((i==1) ){OLED_4_RN_displays(i,TS[1],TS[2],TS[3],TS[4]);}
      if ((i==2) ){OLED_4_RN_displays(i,TS[5],TS[6],TS[7],TS[8]);}
  #ifdef _all64High
      if ((i==3) ){OLED_4_RN_displays(i,TS[9],TS[10],TS[11],TS[12]);}
      if ((i==4) ){OLED_4_RN_displays(i,TS[13],TS[14],TS[15],TS[16]);}
  #else
      if ((i==3) ){OLED_4_RN_displays(i,TS[1],TS[2],"","");}
      if ((i==4) ){OLED_4_RN_displays(i,TS[3],TS[4],"","");}
  #endif
      OLEDsetTextAlignment(i,TEXT_ALIGN_CENTER);offset=64; // reset to centered for anything else
       }
  }// adds time displays
 
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

#else // set empty fns to see how big they are

void SetFont(uint8_t Disp,uint8_t Font){};
void StringToChar(char *line, String input){};
int GetNumber(String Message, int i){};

void OLED_4_RN_displays(int OLed_x,String L1,String L2,String L3,String L4){};
bool RocDisplayFormatted(int OLed_x, int line, String Message){};
void OLEDS_Display(String L1,String L2,String L3,String L4){};
void SetupTextArrays(uint8_t Address,int Display,String Message){};

    
void LookForOLEDs(void){};
void LookForOLED_secondary(void){};
void RRPowerOnIndicator(int Disp) ;
void drawRect(void){} ;

void fillRect(void) {};


void SignalStrengthBar(int oled, int32_t rssi){};  //https://stackoverflow.com/questions/15797920/how-to-convert-wifi-signal-strength-from-quality-percent-to-rssi-dbm


void showTimeAnalog(int disp,int clocksize,int center_x, int center_y, double pl1, double pl2, double pl3){};
void showTimeAnalogCircle(int disp,int clocksize,int circsize,int center_x, int center_y, double pl1, double pl2, double pl3){};

void BigClock(int disp,int clocksize){};

void OLED_Status(){};
void TimeGears(){};


#endif //_OLED

















