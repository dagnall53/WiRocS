
  #include "OLED.h"
  #include <Arduino.h> //needed 
  #include "Directives.h"
  //#include "Globals.h"
   #include <SSD1306.h> // alias for `#include "SSD1306Wire.h"` //https://github.com/ThingPulse/esp8266-oled-ssd1306
   // options are 0x3c 0x3d on bus 1 (OLED1 and OLED2)  or - IF 0x3c is seen on bus 2 it assumes this is 32 high so any 0x3c on bus 1 must also be 32 hi (becoming OLED3 and OLED4)  
  SSD1306  OLED1(0x3c, OLED_SDA, OLED_SCL);//== RocDisplay 1,2,3&4
  SSD1306  OLED2(0x3d, OLED_SDA, OLED_SCL);// RocDisplays 5,6,7,8  
  // small displays
  SSD1306 OLED3(0x3c, OLED_SDA2, OLED_SCL2, GEOMETRY_128_32 ); // RocDisplays 1&2 //Display 3 is the only one to use the secondary ports
  SSD1306 OLED4(0x3c, OLED_SDA, OLED_SCL, GEOMETRY_128_32 );  //RocDisplays 3&4
   //
  
#define DisplayWidth 255  //>100 to allow passing of {} formatting but Rocnet protocol limits the length sent to about 110    
#define TerminalDisplayFont 0
 
char L1[DisplayWidth],L2[DisplayWidth],L3[DisplayWidth],L4[DisplayWidth],L5[DisplayWidth],L6[DisplayWidth],L7[DisplayWidth],L8[DisplayWidth],L9[DisplayWidth],L10[DisplayWidth];
bool OLED1Present,OLED2Present,OLED3Present,OLED4Present;
uint8_t offset;

uint8_t OLed_Clock_Settings[5]; ///bit settings
#define _ClockON 0
#define _ClockLeft 1
#define _ClockAna 2
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
void SetFont(uint8_t OLed_x,uint8_t Font){
FontSelected=Font;
 
if (OLed_x==1){
  switch (Font) { 
    case 0: 
          OLED1.setFont(Font_7x5);
          
    break;
    case 1: 
          OLED1.setFont(Font_6x5w);
          
    break;
    case 2: 
          OLED1.setFont(Font_6x5n);
          
    break;
    case 3: 
          OLED1.setFont(Font_5x5inv);
          
    break;
    case 4: 
          OLED1.setFont(Font_High);
          
    break;
    
    default:
          OLED1.setFont(Font_7x5);
          
    break;
         }
   }
 if (OLed_x==2){
  switch (Font) {
    case 0: 
          OLED2.setFont(Font_7x5);
          
    break;
    case 1: 
          OLED2.setFont(Font_6x5w);
          
    break;
    case 2: 
          OLED2.setFont(Font_6x5n);
          
    break;
    case 3: 
          OLED2.setFont(Font_5x5inv);
          
    break;
    case 4: 
          OLED2.setFont(Font_High);
          
    break;
    
    default:
          OLED2.setFont(Font_7x5);
          
    break;
         }
   }
if (OLed_x==3){
  switch (Font) {
    case 0: 
          OLED3.setFont(Font_7x5);
          
    break;
    case 1: 
          OLED3.setFont(Font_6x5w);
          
    break;
    case 2: 
          OLED3.setFont(Font_6x5n);
          
    break;
    case 3: 
          OLED3.setFont(Font_5x5inv);
          
    break;
    case 4: 
          OLED3.setFont(Font_High);
          
    break;
    
    default:
          OLED3.setFont(Font_7x5);
          
    break;
         }
   }

if (OLed_x==4){
  switch (Font) {
    case 0: 
          OLED4.setFont(Font_7x5);
          
    break;
    case 1: 
          OLED4.setFont(Font_6x5n);
          
    break;
    case 2: 
          OLED4.setFont(Font_6x5w);
          
    break;
    case 3: 
          OLED4.setFont(Font_5x5inv);
          
    break;
    case 4: 
          OLED4.setFont(Font_High);
          
    break;
    
    default:
          OLED4.setFont(Font_7x5);
          
    break;
         }
   }
   
}

void StringToChar(char *line, String input){
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
   for (int i=0;i<=DisplayWidth-1; i++){
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
         if (Message[i]=='{'){ if (OLed_x==1){OLED1.drawString(RowPixel[DisplayLine]+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),FormattedMsg);RowPixel[DisplayLine]=RowPixel[DisplayLine]+OLED1.getStringWidth(FormattedMsg);}
                               if (OLed_x==2){OLED2.drawString(RowPixel[DisplayLine]+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),FormattedMsg);RowPixel[DisplayLine]=RowPixel[DisplayLine]+OLED2.getStringWidth(FormattedMsg);}
                               if (OLed_x==3){OLED3.drawString(RowPixel[DisplayLine]+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),FormattedMsg);RowPixel[DisplayLine]=RowPixel[DisplayLine]+OLED3.getStringWidth(FormattedMsg);}
                               if (OLed_x==4){OLED4.drawString(RowPixel[DisplayLine]+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),FormattedMsg);RowPixel[DisplayLine]=RowPixel[DisplayLine]+OLED4.getStringWidth(FormattedMsg);}
                               
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
   
                                         
         if ((in_format && Message[i]=='L')){  // this code looks at the Lines (L) coding.
                                            DisplayLine=GetNumber(Message,i+1);
                                            //DebugSprintfMsgSend(sprintf ( DebugMsg, "OLed_x:%d Changing Line number to %d GN<%d>",OLed_x,DisplayLine,GetNumber(Message,i+1)));
                                            i=i+1;
                                            } 
                             //if (Message[i+1]=='0'){ if (DisplayLine!=0){RowPixel[DisplayLine]=0;} DisplayLine=0;   } // reset rowpixel if changing lines
                             //if (Message[i+1]=='1'){ if (DisplayLine!=1){RowPixel[DisplayLine]=0;} DisplayLine=1;   } // 
                             //                }
                               
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
                               TabOne=GetNumber(Message,i+1); //S               
                                            } 
                                             // ?? W .. "Departure column width" IF set sets the second tab position in pixels..Is volatile, and lost for next message!
     
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
                               if (OLed_x==1){OLED1.setFont(RocBitMap);
                                            OLED1.drawString(RowPixel[DisplayLine]+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),BitMsg);
                                            RowPixel[DisplayLine]=RowPixel[DisplayLine]+OLED1.getStringWidth(BitMsg);
                                             }
                               if (OLed_x==2){OLED2.setFont(RocBitMap);
                                            OLED2.drawString(RowPixel[DisplayLine]+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),BitMsg);
                                            RowPixel[DisplayLine]=RowPixel[DisplayLine]+OLED2.getStringWidth(BitMsg);
                                            }
                               if (OLed_x==3){OLED3.setFont(RocBitMap);
                                            OLED3.drawString(RowPixel[DisplayLine]+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),BitMsg);
                                            RowPixel[DisplayLine]=RowPixel[DisplayLine]+OLED3.getStringWidth(BitMsg);
                                           }
                               if (OLed_x==4){OLED4.setFont(RocBitMap);
                                            OLED4.drawString(RowPixel[DisplayLine]+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),BitMsg);
                                            RowPixel[DisplayLine]=RowPixel[DisplayLine]+OLED4.getStringWidth(BitMsg);
                                            }            
                                          
                              SetFont(OLed_x,FontSelected); // set back to last selected font; 
                             //DebugSprintfMsgSend(sprintf ( DebugMsg, "OLed_x:%d Byte %d  %d",OLed_x,(Message[i+1]-48),GetNumber(Message,i+1)));// gets number from Message next byte(s)
                             i=i+1;
                                             } 

                                                                     
          
        if (!(in_format)&&(j<=(DisplayWidth-1))){
                                       // if (!( ((Message[i-1]=='}')&&(Message[i]==' ')) ) )  {  // do not copy first space after '}' helps with alignment + saves display space,
                                     if (!((ignoreJ2&&inJ2)||(ignoreJ1&&inJ1)))   {    FormattedMsg+=Message[i];j=j+1;} } 
                                           // }
                                            
          if (Message[i]=='}'){ in_format=false;j=0;FormattedMsg="";}
          
          
     }
     }
    //draw small real time clock in the top line only 
    
    if (bitRead(OLed_Clock_Settings[OLed_x],_ClockLeft)){ClockSpaceOffset=(ClockRad*2)+1;ClockPos=ClockRad;}
                                            else{ClockSpaceOffset=0;             ClockPos=127-ClockRad;}


    if ( (bitRead(OLed_Clock_Settings[OLed_x],_ClockAna)) && (bitRead(OLed_Clock_Settings[OLed_x],_ClockON))&& (OLed_x==1) ){  
                                OLED1.fillCircle(ClockPos,ClockRad,ClockRad);
                                OLED1.setColor(BLACK);
                                  showTimeAnalog(OLed_x,ClockRad,ClockPos,ClockRad, 0, 0.5, hrs * 5 + (int)(mins * 5 / 60));
                                  showTimeAnalog(OLed_x,ClockRad,ClockPos,ClockRad, 0, 1, mins);
                                OLED1.setColor(WHITE);
                               }
    
    if ( (bitRead(OLed_Clock_Settings[OLed_x],_ClockAna)) && (bitRead(OLed_Clock_Settings[OLed_x],_ClockON))&& (OLed_x==2) ){  
                                OLED2.fillCircle(ClockPos,ClockRad,ClockRad);
                                OLED2.setColor(BLACK);
                                  showTimeAnalog(OLed_x,ClockRad,ClockPos,ClockRad, 0, 0.5, hrs * 5 + (int)(mins * 5 / 60));
                                  showTimeAnalog(OLed_x,ClockRad,ClockPos,ClockRad, 0, 1, mins);
                                OLED2.setColor(WHITE);
                               }
    
      if ( (bitRead(OLed_Clock_Settings[OLed_x],_ClockAna)) && (bitRead(OLed_Clock_Settings[OLed_x],_ClockON))&& (OLed_x==3) ){  
                                OLED3.fillCircle(ClockPos,ClockRad,ClockRad);
                                OLED3.setColor(BLACK);
                                  showTimeAnalog(OLed_x,ClockRad,ClockPos,ClockRad, 0, 0.5, hrs * 5 + (int)(mins * 5 / 60));
                                  showTimeAnalog(OLed_x,ClockRad,ClockPos,ClockRad, 0, 1, mins);
                                OLED3.setColor(WHITE);
                               }
     if ( (bitRead(OLed_Clock_Settings[OLed_x],_ClockAna)) && (bitRead(OLed_Clock_Settings[OLed_x],_ClockON))&& (OLed_x==4) ){  
                                OLED4.fillCircle(ClockPos,ClockRad,ClockRad);
                                OLED4.setColor(BLACK);
                                  showTimeAnalog(OLed_x,ClockRad,ClockPos,ClockRad, 0, 0.5, hrs * 5 + (int)(mins * 5 / 60));
                                  showTimeAnalog(OLed_x,ClockRad,ClockPos,ClockRad, 0, 1, mins);
                                OLED4.setColor(WHITE);
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
void OLED_4_RN_displays(int OLed_x,String L1,String L2,String L3,String L4){
  if ((OLed_x==1)&&(OLED1Present)){
   OLED1.clear();
   // display 4 lines 
   RocFormatUsed[1]=false;
     
      if (!RocDisplayFormatted(1,0,L1)) {OLED1.drawStringMaxWidth(offset,  0,MaxWidth, L1);}
      if (!RocDisplayFormatted(1,16,L2)){OLED1.drawStringMaxWidth(offset, 16,MaxWidth, L2);}
      if (!RocDisplayFormatted(1,32,L3)){OLED1.drawStringMaxWidth(offset, 32,MaxWidth, L3);}
      if (!RocDisplayFormatted(1,48,L4)){OLED1.drawStringMaxWidth(offset, 48,MaxWidth, L4);}
   OLED1.display();
}
if ((OLed_x==3)&&(OLED3Present)){
    OLED3.clear();// display 2 lines 
       RocFormatUsed[3]=false;
     if (!RocDisplayFormatted(3,0,L1)) {OLED3.drawStringMaxWidth(offset,  0,MaxWidth, L1);}
     if (!RocDisplayFormatted(3,16,L2)){OLED3.drawStringMaxWidth(offset, 16,MaxWidth, L2);}
    OLED3.display();
    }
if ((OLed_x==4)&&(OLED4Present)){
    OLED4.clear();// display 2 lines
       RocFormatUsed[4]=false; 
     if (!RocDisplayFormatted(4,0,L1)) {OLED4.drawStringMaxWidth(offset,  0,MaxWidth, L1);}
     if (!RocDisplayFormatted(4,16,L2)){OLED4.drawStringMaxWidth(offset, 16,MaxWidth, L2);}
    OLED4.display();
    }
    

if ((OLed_x==2)&&(OLED2Present)){
   OLED2.clear();// display 4 lines
      RocFormatUsed[2]=false; 
    if (!RocDisplayFormatted(2,0,L1)) {OLED2.drawStringMaxWidth(offset,  0,MaxWidth, L1);}
    if (!RocDisplayFormatted(2,16,L2)){OLED2.drawStringMaxWidth(offset, 16,MaxWidth, L2);}
    if (!RocDisplayFormatted(2,32,L3)){OLED2.drawStringMaxWidth(offset, 32,MaxWidth, L3);}
    if (!RocDisplayFormatted(2,48,L4)){OLED2.drawStringMaxWidth(offset, 48,MaxWidth, L4);}
   OLED2.display();
  }
}




void OLED_Displays_Setup(uint8_t Address,int Display,String Message){
     // use both for 4 line termius display and also for 4 option of 2 x32 displays 
      if (Display==1){for (uint16_t i = 0; i <= (DisplayWidth-1); i++) {L1[i]=(Message[i]);}}
      if (Display==2){for (uint16_t i = 0; i <= (DisplayWidth-1); i++) {L2[i]=(Message[i]);}}
      if (Display==3){for (uint16_t i = 0; i <= (DisplayWidth-1); i++) {L3[i]=(Message[i]);}}
      if (Display==4){for (uint16_t i = 0; i <= (DisplayWidth-1); i++) {L4[i]=(Message[i]);}}
   
      if (Display==5){for (uint16_t i = 0; i <= (DisplayWidth-1); i++) {L5[i]=(Message[i]);}}
      if (Display==6){for (uint16_t i = 0; i <= (DisplayWidth-1); i++) {L6[i]=(Message[i]);}}
      if (Display==7){for (uint16_t i = 0; i <= (DisplayWidth-1); i++) {L7[i]=(Message[i]);}}
      if (Display==8){for (uint16_t i = 0; i <= (DisplayWidth-1); i++) {L8[i]=(Message[i]);}}
   
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
      if (i==60){Serial.println (" OLED 3 ");OLED3Present=true;OLED_initiate(3,2);count++;}
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
      // if OLED3 is present assume any address 60 on Bus 1here will be 32 high therefor== OLED4
      if ((OLED_SDA!=OLED_SDA2)&& (OLED_SCL!=OLED_SCL2)){
       if (i==60){if (OLED3Present){Serial.println (" OLED 4 "); OLED4Present=true;OLED_initiate(4,1);count++;}
                                       else{Serial.println (" OLED 1 ");OLED1Present=true;OLED_initiate(1,1);count++;}
                                       }
                                       }
           
        if (i==61){Serial.println (" OLED 2 ");OLED2Present=true;OLED_initiate(2,1);count++;}
 
      } // end of good response
  } // end of for loop
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" OLED (s).");
  
  }



void RRPowerOnIndicator(int Disp) {
if (!POWERON){
  if((Disp==1)&&(OLED1Present)){OLED1.drawRect(0,0,8,8);}
  if((Disp==2)&&(OLED2Present)){OLED2.drawRect(0,0,8,8);}
  if((Disp==3)&&(OLED3Present)){OLED3.drawRect(0,0,8,8);}
  if((Disp==4)&&(OLED4Present)){OLED4.drawRect(0,0,8,8);}

  }else
  {
  if((Disp==1)&&(OLED1Present)){OLED1.fillRect(0,0,8,8);}
  if((Disp==2)&&(OLED2Present)){OLED2.fillRect(0,0,8,8);}
  if((Disp==3)&&(OLED3Present)){OLED3.fillRect(0,0,8,8);}
  if((Disp==4)&&(OLED4Present)){OLED4.fillRect(0,0,8,8);}
  
  }

 
}




void SignalStrengthBar(int disp, int32_t rssi) { //https://stackoverflow.com/questions/15797920/how-to-convert-wifi-signal-strength-from-quality-percent-to-rssi-dbm
  int PosX,PosY;
 if((OLED1Present)&&(disp==1)){
  // rssi -90 is just about dropout..
  // rssi -40 is a great signal
  PosX=108;  // position left right  max = 128
  PosY=0; // top left position up / down max 64
  OLED1.drawVerticalLine(PosX,PosY,10);
  OLED1.drawLine(PosX,PosY+4,PosX-4,PosY);
  OLED1.drawLine(PosX,PosY+4,PosX+4,PosY);
  if (rssi >= -50) { OLED1.drawLine(PosX+10,PosY,PosX+10,PosY+10);}
  if (rssi >= -55){OLED1.drawLine(PosX+8,PosY+2,PosX+8,PosY+10);}
  if (rssi >= -65){OLED1.drawLine(PosX+6,PosY+4,PosX+6,PosY+10);}
  if (rssi >= -70){OLED1.drawLine(PosX+4,PosY+6,PosX+4,PosY+10);}
  if (rssi >= -80){OLED1.drawLine(PosX+2,PosY+8,PosX+2,PosY+10);}
 }

if((OLED2Present)&&(disp==2)){
  // rssi -90 is just about dropout..
  // rssi -40 is a great signal
  PosX=108;  // position left right  max = 128
  PosY=0; // top left position up / down max 64
  OLED2.drawVerticalLine(PosX,PosY,10);
  OLED2.drawLine(PosX,PosY+4,PosX-4,PosY);
  OLED2.drawLine(PosX,PosY+4,PosX+4,PosY);
  if (rssi >= -50) { OLED2.drawLine(PosX+10,PosY,PosX+10,PosY+10);}
  if (rssi >= -55){OLED2.drawLine(PosX+8,PosY+2,PosX+8,PosY+10);}
  if (rssi >= -65){OLED2.drawLine(PosX+6,PosY+4,PosX+6,PosY+10);}
  if (rssi >= -70){OLED2.drawLine(PosX+4,PosY+6,PosX+4,PosY+10);}
  if (rssi >= -80){OLED2.drawLine(PosX+2,PosY+8,PosX+2,PosY+10);}
 }

 
if((OLED3Present)&&(disp==3)){
  // rssi -90 is just about dropout..
  // rssi -40 is a great signal
  PosX=108;  // position left right  max = 128
  PosY=0; // top left position up / down max 64
  OLED3.drawVerticalLine(PosX,PosY,10);
  OLED3.drawLine(PosX,PosY+4,PosX-4,PosY);
  OLED3.drawLine(PosX,PosY+4,PosX+4,PosY);
  if (rssi >= -50) { OLED3.drawLine(PosX+10,PosY,PosX+10,PosY+10);}
  if (rssi >= -55){OLED3.drawLine(PosX+8,PosY+2,PosX+8,PosY+10);}
  if (rssi >= -65){OLED3.drawLine(PosX+6,PosY+4,PosX+6,PosY+10);}
  if (rssi >= -70){OLED3.drawLine(PosX+4,PosY+6,PosX+4,PosY+10);}
  if (rssi >= -80){OLED3.drawLine(PosX+2,PosY+8,PosX+2,PosY+10);}
 }

if((OLED3Present)&&(disp==4)){
  // rssi -90 is just about dropout..
  // rssi -40 is a great signal
  PosX=108;  // position left right  max = 128
  PosY=0; // top left position up / down max 64
  OLED4.drawVerticalLine(PosX,PosY,10);
  OLED4.drawLine(PosX,PosY+4,PosX-4,PosY);
  OLED4.drawLine(PosX,PosY+4,PosX+4,PosY);
  if (rssi >= -50) { OLED4.drawLine(PosX+10,PosY,PosX+10,PosY+10);}
  if (rssi >= -55){OLED4.drawLine(PosX+8,PosY+2,PosX+8,PosY+10);}
  if (rssi >= -65){OLED4.drawLine(PosX+6,PosY+4,PosX+6,PosY+10);}
  if (rssi >= -70){OLED4.drawLine(PosX+4,PosY+6,PosX+4,PosY+10);}
  if (rssi >= -80){OLED4.drawLine(PosX+2,PosY+8,PosX+2,PosY+10);}
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
  if ((disp==1)&&(OLED1Present)){OLED1.drawLine(x1,y1,x2,y2);}
  if ((disp==2)&&(OLED2Present)){OLED2.drawLine(x1,y1,x2,y2);}
  if ((disp==3)&&(OLED3Present)){OLED3.drawLine(x1,y1,x2,y2);}
  if ((disp==4)&&(OLED4Present)){OLED4.drawLine(x1,y1,x2,y2);}
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
  if ((disp==1)&&(OLED1Present)){OLED1.drawCircle(x2, y2,circsize);}
  if ((disp==2)&&(OLED2Present)){OLED2.drawCircle(x2,y2,circsize);}
   if ((disp==3)&&(OLED3Present)){OLED3.drawCircle(x2,y2,circsize);}
    if ((disp==4)&&(OLED4Present)){OLED4.drawCircle(x2,y2,circsize);}
}

void BigClock(int disp,int clocksize){
  int center_x,center_y;
  center_x=64;
  center_y=clocksize;
  if ((disp==1)&&(OLED1Present)){
          OLED1.drawCircle(center_x, center_y, clocksize);
          for (int i=1;i<=12;i++){showTimeAnalog(1,clocksize,center_x,center_y, 0.8 ,0.95 , (i * 5) );}
          showTimeAnalog(1,clocksize,center_x,center_y, -0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
          showTimeAnalog(1,clocksize,center_x,center_y, -0.1, 0.9, mins);
         if (divider==1){ showTimeAnalog(1,clocksize,center_x,center_y, -0.2, 0.5, secs);
          showTimeAnalogCircle(1,clocksize,4, center_x,center_y, -0.2, 0.65, secs);
          showTimeAnalog(1,clocksize,center_x,center_y, 0.8, 0.9, secs);}
          OLED1.display();
          }
  
  if ((disp==2)&&(OLED2Present)){
         OLED2.drawCircle(center_x, center_y, clocksize);
          for (int i=1;i<=12;i++){showTimeAnalog(2,clocksize,center_x,center_y, 0.8 ,0.95 , (i * 5) );}
          showTimeAnalog(2,clocksize,center_x,center_y, -0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
          showTimeAnalog(2,clocksize,center_x,center_y, -0.1, 0.9, mins);
         if (divider==1){ showTimeAnalog(2,clocksize,center_x,center_y, -0.2, 0.5, secs);
          showTimeAnalogCircle(2,clocksize,4, center_x,center_y, -0.2, 0.65, secs);
          showTimeAnalog(2,clocksize,center_x,center_y, 0.8, 0.9, secs);}
          OLED2.display();
          }
          

if ((disp==3)&&(OLED3Present)){
          OLED3.drawCircle(center_x, center_y, clocksize);
            for (int i=1;i<=12;i++){showTimeAnalog(disp,clocksize,center_x,center_y, 0.8 ,0.9, (i * 5) );}
            showTimeAnalog(disp,clocksize,center_x,center_y, -0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
            showTimeAnalog(disp,clocksize,center_x,center_y, -0.1, 0.9, mins);
            if (divider==1){ showTimeAnalog(disp,clocksize,center_x,center_y, -0.2, 0.5, secs);
              showTimeAnalogCircle(disp,clocksize,2, center_x,center_y, -0.2, 0.65, secs);
              showTimeAnalog(disp,clocksize,center_x,center_y, 0.8, 0.9, secs);}
          OLED3.display();
          }

if ((disp==4)&&(OLED3Present)){
          OLED4.drawCircle(center_x, center_y, clocksize);
            for (int i=1;i<=12;i++){showTimeAnalog(disp,clocksize,center_x,center_y, 0.8 ,0.9, (i * 5) );}
            showTimeAnalog(disp,clocksize,center_x,center_y, -0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
            showTimeAnalog(disp,clocksize,center_x,center_y, -0.1, 0.9, mins);
            if (divider==1){ showTimeAnalog(disp,clocksize,center_x,center_y, -0.2, 0.5, secs);
              showTimeAnalogCircle(disp,clocksize,2, center_x,center_y, -0.2, 0.65, secs);
              showTimeAnalog(disp,clocksize,center_x,center_y, 0.8, 0.9, secs);}
          OLED4.display();
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
if (OLED1Present){ 
  OLED1.setTextAlignment(TEXT_ALIGN_LEFT); offset=0;  SetFont(1,TerminalDisplayFont);  
  if ( (RocFormatUsed[1])&&(bitRead(OLed_Clock_Settings[1],_ClockAna))&&(bitRead(OLed_Clock_Settings[1],_ClockON)  ) ){// any roc formatted text and analog clock on showing??
      if (!bitRead(OLed_Clock_Settings[1],_ClockLeft) ){
                            offset=0;MaxWidth=127-(ClockRad*2);}else{
                            offset=1+(ClockRad*2);MaxWidth=127-(ClockRad*2);} }
                      
   OLED_4_RN_displays(1,L1,L2,L3,L4);
   
   OLED1.setTextAlignment(TEXT_ALIGN_CENTER);offset=64; // reset to centered for anything else
   RRPowerOnIndicator(1);
   if ((L1[0]==0)&&(L2[0]==0)&&(L3[0]==0)&&(L4[0]==0)&&(L5[0]==0)){ SignalStrengthBar(1,SigStrength());BigClock(1,30); }
   }// adds time display 1
if (OLED2Present){ 
   OLED2.setTextAlignment(TEXT_ALIGN_LEFT); offset=0;
   SetFont(2,TerminalDisplayFont); 
   if ( (RocFormatUsed[2])&&(bitRead(OLed_Clock_Settings[2],_ClockAna))&&(bitRead(OLed_Clock_Settings[2],_ClockON)  ) ){// any roc formatted text and analog clock on showing??
      if (!bitRead(OLed_Clock_Settings[2],_ClockLeft) ){
                            offset=0;MaxWidth=127-(ClockRad*2);}else{
                            offset=1+(ClockRad*2);MaxWidth=127-(ClockRad*2);} } 
   OLED_4_RN_displays(2,L5,L6,L7,L8);
   OLED2.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   if ((L6[0]==0)&&(L7[0]==0)&&(L8[0]==0)&&(L9[0]==0)&&(L5[0]==0)){RRPowerOnIndicator(2);SignalStrengthBar(2,SigStrength());BigClock(2,30);}
   } // display 2
   
if (OLED3Present){ 
   OLED3.setTextAlignment(TEXT_ALIGN_LEFT); offset=0;
   SetFont(3,TerminalDisplayFont);  
   if ( (RocFormatUsed[3])&&(bitRead(OLed_Clock_Settings[3],_ClockAna))&&(bitRead(OLed_Clock_Settings[3],_ClockON)  ) ){// any roc formatted text and analog clock on showing??
      if (!bitRead(OLed_Clock_Settings[3],_ClockLeft) ){
                            offset=0;MaxWidth=127-(ClockRad*2);}else{
                            offset=1+(ClockRad*2);MaxWidth=127-(ClockRad*2);} }
   OLED_4_RN_displays(3,L1,L2,"","");
   OLED3.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   if ((L1[0]==0)&&(L2[0]==0)){RRPowerOnIndicator(3);SignalStrengthBar(3,SigStrength());BigClock(3,15);} 
   } // display 3 is x32

if (OLED4Present){ 
   OLED4.setTextAlignment(TEXT_ALIGN_LEFT); offset=0;
   SetFont(4,TerminalDisplayFont);  
   if ( (RocFormatUsed[4])&&(bitRead(OLed_Clock_Settings[4],_ClockAna))&&(bitRead(OLed_Clock_Settings[4],_ClockON)  ) ){// any roc formatted text and analog clock on showing??
      if (!bitRead(OLed_Clock_Settings[4],_ClockLeft) ){
                            offset=0;MaxWidth=127-(ClockRad*2);}else{
                            offset=1+(ClockRad*2);MaxWidth=127-(ClockRad*2);} }
   OLED_4_RN_displays(4,L3,L4,"","");
   OLED4.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
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




















