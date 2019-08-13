
  #include "OLED.h"
  #include <Arduino.h> //needed 
  #include "Directives.h"
  // these outside #define to save putting more #defines in other places..
  bool OLED1Present,OLED2Present,OLED3Present,OLED4Present,OLED5Present,OLED6Present;
  uint8_t OLED_Settings[7]; ///bit settings for oleds 1-6
  int SCP[5][7];  // Scroll position counter for [display 1-4] on  [Oleds 1-6] on each, oled (LIMIT  ONE SCROLLING per display)
  int ScrollMsgLength[5][7]; //Scroll msg length counter
  #define _ClockON 0
  #define _ClockLeft 1
  #define _ClockAna 2
  #define _ClockFlip 3
  #define _ClockInvert 4
  #define _32 5
  #ifdef _OLED
  //#include "Globals.h"
  #include "OLEDDisplayFonts.h"
  //#include "OLED_Font_High.h"  // perhapsseparate out fonts to make easier to modify?
   #include <SSD1306.h> // alias for `#include "SSD1306Wire.h"` //https://github.com/ThingPulse/esp8266-oled-ssd1306
   // options are 0x3c 0x3d on bus 1 (OLED1 and OLED2)  or - IF 0x3c is seen on bus 2 it assumes this is 32 high so any 0x3c on bus 1 must also be 32 hi (becoming OLED3 and OLED4)  
  
  // small displays
  
  SSD1306 OLED1(0x3c, OLED_SDA, OLED_SCL);  // RN 60 RocDisplays 1,2,3 & 4
  SSD1306 OLED3(0x3d, OLED_SDA, OLED_SCL);  // RN 60 RocDisplays 9,10,11,12
  SSD1306 OLED2(0x3c, OLED_SDA2, OLED_SCL2);// RN 61  RocDisplays 5,6,7,8 
  SSD1306 OLED4(0x3d, OLED_SDA2, OLED_SCL2);// RN 61 displays 13,14,15,16
  //new stuff for software controlled X32 mode 
  SSD1306 OLED5(0x3c, OLED_SDA, OLED_SCL, GEOMETRY_128_32 );   //RN 60   //RocDisplays 1&2
  SSD1306 OLED6(0x3c, OLED_SDA2, OLED_SCL2, GEOMETRY_128_32 ); //RN 60  RocDisplays 3&4?  
 
 
   //
  
#define TextObjectLength 260  //>100 to allow passing of {} formatting but Rocnet protocol limits the length sent to about 110    
#define TerminalDisplayFont 0
#define NumberofTextStores 17

char TS[NumberofTextStores][TextObjectLength]; 

uint8_t offset;


bool Observed_Change;

bool RocFormatUsed[7];

//https://github.com/ThingPulse/esp8266-oled-ssd1306
// RocDisplay fonts are now in the oleddislayfonts.h arduino file, but see also (see https://github.com/dagnall53/Fonts-for-WiRocs) 

//uint32_t Disp2ReInit;
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
extern uint32_t StartedAt;
int CharCountinGetnumber;

// new set of OLED stuff to allow indexing
void OLEDScreen(int OLED,bool flip, bool invert){

//  Serial.print("Switching ");Serial.print (OLED); Serial.print ("to flip<");Serial.print(flip);Serial.print(">  invert<");Serial.print(invert);Serial.println(">");
if (invert){
 switch (OLED) { 
  case 1:   OLED1.invertDisplay();     break;
  case 2:   OLED2.invertDisplay();     break;
  case 3:   OLED3.invertDisplay();     break;
  case 4:   OLED4.invertDisplay();     break;
  case 5:   OLED5.invertDisplay();     break;
  case 6:   OLED6.invertDisplay();     break;
  default:     break;  
          }} 
          else{
    switch (OLED) { // Inversion or set normal first
      case 1:   OLED1.normalDisplay();     break;
      case 2:   OLED2.normalDisplay();     break;
      case 3:   OLED3.normalDisplay();     break;
      case 4:   OLED4.normalDisplay();     break;
      case 5:   OLED5.normalDisplay();     break;
      case 6:   OLED6.normalDisplay();     break;
      default:     break;  
        }}

  
if (flip)  {

switch (OLED) { // reset orient or mirroring first
  case 1:   OLED1.resetOrientation();     break;
  case 2:   OLED2.resetOrientation();     break;
  case 3:   OLED3.resetOrientation();     break;
  case 4:   OLED4.resetOrientation();     break;
  case 5:   OLED5.resetOrientation();     break;
  case 6:   OLED6.resetOrientation();     break;
  default:     break;  
       }}
       else {
 switch (OLED) { 
  case 1:   OLED1.flipScreenVertically();     break;
  case 2:   OLED2.flipScreenVertically();     break;
  case 3:   OLED3.flipScreenVertically();     break;
  case 4:   OLED4.flipScreenVertically();     break;
  case 5:   OLED5.flipScreenVertically();     break;
  case 6:   OLED6.flipScreenVertically();     break;
  default:     break;  
              }}        
 
}







void SetOLEDDefaults(){//Set Clock off, normal(flipped), not inverted and NOT 32 display

  #ifdef  _ROCDISP_EEPROM_DEBUG
  Serial.print("SETTING ALL OLED DEFAULTS");
  #endif
  for (int OLed_x=0; OLed_x<=6; OLed_x++)
      {
    bitClear(OLED_Settings[OLed_x],_ClockON); 
    bitClear(OLED_Settings[OLed_x],_ClockLeft);
    bitClear(OLED_Settings[OLed_x],_ClockAna);
    bitSet(OLED_Settings[OLed_x],_ClockFlip);
    bitClear(OLED_Settings[OLed_x],_ClockInvert);
    bitClear(OLED_Settings[OLed_x],_32);
  } }




void OLEDdrawString(int OLED,int16_t x, int16_t y, String text){
 switch (OLED) { 
  case 1:   OLED1.drawString(x,y,text);     break;
  case 2:   OLED2.drawString(x,y,text);     break;
  case 3:   OLED3.drawString(x,y,text);     break;
  case 4:   OLED4.drawString(x,y,text);     break;
  case 5:   OLED5.drawString(x,y,text);     break;
  case 6:   OLED6.drawString(x,y,text);     break;
  default:     break;  
}
}
void OLEDclear(int OLED){
 switch (OLED) { 
  case 1:   OLED1.clear();     break;
  case 2:   OLED2.clear();     break;
  case 3:   OLED3.clear();     break;
  case 4:   OLED4.clear();     break;
  case 5:   OLED5.clear();     break;
  case 6:   OLED6.clear();     break;
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
  case 5:   result= OLED5.getStringWidth(text);     break;
  case 6:   result= OLED6.getStringWidth(text);     break;
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
  case 5:   OLED5.drawStringMaxWidth(x,y,maxLineWidth, text);     break;
  case 6:   OLED6.drawStringMaxWidth(x,y,maxLineWidth, text);     break;
  default:     break;  
}
}

void OLEDdrawLine(int OLED,int16_t x0, int16_t y0, int16_t x1, int16_t y1){
 switch (OLED) { 
  case 1:   OLED1.drawLine(x0,y0,x1,y1);     break;
  case 2:   OLED2.drawLine(x0,y0,x1,y1);     break;
  case 3:   OLED3.drawLine(x0,y0,x1,y1);     break;
  case 4:   OLED4.drawLine(x0,y0,x1,y1);     break;
  case 5:   OLED5.drawLine(x0,y0,x1,y1);     break;
  case 6:   OLED6.drawLine(x0,y0,x1,y1);     break;
  default:     break;  
}
}

void OLEDdrawVerticalLine(int OLED,int16_t x0, int16_t y0, int16_t l){
 switch (OLED) { 
  case 1:   OLED1.drawVerticalLine(x0,y0,l);     break;
  case 2:   OLED2.drawVerticalLine(x0,y0,l);     break;
  case 3:   OLED3.drawVerticalLine(x0,y0,l);     break;
  case 4:   OLED4.drawVerticalLine(x0,y0,l);     break;
  case 5:   OLED5.drawVerticalLine(x0,y0,l);     break;
  case 6:   OLED6.drawVerticalLine(x0,y0,l);     break;
  default:     break;  
}
}
void OLEDdrawRect(int OLED,int16_t x0, int16_t y0, int16_t x1, int16_t y1){
 switch (OLED) { 
  case 1:   OLED1.drawRect(x0,y0,x1,y1);     break;
  case 2:   OLED2.drawRect(x0,y0,x1,y1);     break;
  case 3:   OLED3.drawRect(x0,y0,x1,y1);     break;
  case 4:   OLED4.drawRect(x0,y0,x1,y1);     break;
  case 5:   OLED5.drawRect(x0,y0,x1,y1);     break;
  case 6:   OLED6.drawRect(x0,y0,x1,y1);     break;
  default:     break;  
}
}

void OLEDfillRect(int OLED,int16_t x0, int16_t y0, int16_t x1, int16_t y1){
 switch (OLED) { 
  case 1:   OLED1.fillRect(x0,y0,x1,y1);     break;
  case 2:   OLED2.fillRect(x0,y0,x1,y1);     break;
  case 3:   OLED3.fillRect(x0,y0,x1,y1);     break;
  case 4:   OLED4.fillRect(x0,y0,x1,y1);     break;
  case 5:   OLED5.fillRect(x0,y0,x1,y1);     break;
  case 6:   OLED6.fillRect(x0,y0,x1,y1);     break;
  default:     break;  
}
}
void OLEDsetFont(int OLED,const uint8_t* fontData){
 switch (OLED) { 
  case 1:   OLED1.setFont(fontData);     break;
  case 2:   OLED2.setFont(fontData);     break;
  case 3:   OLED3.setFont(fontData);     break;
  case 4:   OLED4.setFont(fontData);     break;
  case 5:   OLED5.setFont(fontData);     break;
  case 6:   OLED6.setFont(fontData);     break;
  default:     break;  
}
}

void OLEDsetColor(int OLED, OLEDDISPLAY_COLOR color){
switch (OLED) { 
  case 1:   OLED1.setColor(color);     break;
  case 2:   OLED2.setColor(color);     break;
  case 3:   OLED3.setColor(color);     break;
  case 4:   OLED4.setColor(color);     break;
  case 5:   OLED5.setColor(color);     break;
  case 6:   OLED6.setColor(color);     break;
  default:     break;  
}
}

void OLEDfillCircle(int OLED, int16_t x, int16_t y, int16_t radius){
switch (OLED) { 
  case 1:   OLED1.fillCircle(x,y,radius);     break;
  case 2:   OLED2.fillCircle(x,y,radius);     break;
  case 3:   OLED3.fillCircle(x,y,radius);     break;
  case 4:   OLED4.fillCircle(x,y,radius);     break;
  case 5:   OLED5.fillCircle(x,y,radius);     break;
  case 6:   OLED6.fillCircle(x,y,radius);     break;
  default:     break;  
}
}
void OLEDdrawCircle(int OLED, int16_t x, int16_t y, int16_t radius){
switch (OLED) { 
  case 1:   OLED1.drawCircle(x,y,radius);     break;
  case 2:   OLED2.drawCircle(x,y,radius);     break;
  case 3:   OLED3.drawCircle(x,y,radius);     break;
  case 4:   OLED4.drawCircle(x,y,radius);     break;
  case 5:   OLED5.drawCircle(x,y,radius);     break;
  case 6:   OLED6.drawCircle(x,y,radius);     break;
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
  case 5:   result=OLED5Present;     break;
  case 6:   result=OLED6Present;     break;
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
  case 5:   OLED5.setTextAlignment(textAlignment);    break;
  case 6:   OLED6.setTextAlignment(textAlignment);    break;
  default:     break;  
}
}

extern uint8_t OLED_EEPROM_Setting(int OLed_x);


void OLEDDisplay(uint8_t OLED){
switch (OLED) { 
  case 1:   OLED1.display();     break;
  case 2:   OLED2.display();     break;
  case 3:   OLED3.display();     break;
  case 4:   OLED4.display();     break;
  case 5:   OLED5.display();     break;
  case 6:   OLED6.display();     break;  
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
    CharCountinGetnumber=j-1;
    if (CharCountinGetnumber<=0){CharCountinGetnumber=0;}
   // Serial.printf(" MSG @%d <%c><%d> Char count %d \n",i,Message[i-1],Number,j);
  return Number;
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

void OLEDsettingView(int OLed_x){
  Serial.printf("Current <OLed:%d> Present:%d Flipped:%d Invert:%d ClockL:%d ClockAna:%d ClockOn:%d _32:%d \n",OLed_x,OLEDPresent(OLed_x),
                                                                            bitRead(OLED_Settings[OLed_x],_ClockFlip),
                                                                            bitRead(OLED_Settings[OLed_x],_ClockInvert),
                                                                            bitRead(OLED_Settings[OLed_x],_ClockLeft),
                                                                            bitRead(OLED_Settings[OLed_x],_ClockAna),
                                                                            bitRead(OLED_Settings[OLed_x],_ClockON),
                                                                            bitRead(OLED_Settings[OLed_x],_32)
                                                                             );
}

void OLEDEEPROMsettingView(int OLed_x){
Serial.printf("EEProm <OLed:%d> Present:%d Flipped:%d Invert:%d ClockL:%d ClockAna:%d ClockOn:%d _32:%d \n",OLed_x,OLEDPresent(OLed_x),
                                                                            bitRead(OLED_EEPROM_Setting(OLed_x),_ClockFlip),
                                                                            bitRead(OLED_EEPROM_Setting(OLed_x),_ClockInvert),
                                                                            bitRead(OLED_EEPROM_Setting(OLed_x),_ClockLeft),
                                                                            bitRead(OLED_EEPROM_Setting(OLed_x),_ClockAna),
                                                                            bitRead(OLED_EEPROM_Setting(OLed_x),_ClockON),
                                                                            bitRead(OLED_EEPROM_Setting(OLed_x),_32)
                                                                             );

}

void ViewAllOLEDSettings(int OLed_x){
  OLEDsettingView(OLed_x);
  OLEDEEPROMsettingView(OLed_x);
}

void SetAll_32Mode(int OLed_x){
  for (int i=1;i<=6;i++){ if (bitRead(OLED_Settings[OLed_x],_32)){bitSet(OLED_Settings[i],_32);}
                                                             else{bitClear(OLED_Settings[i],_32);}
     }         
}

void SaveOledSettings(int OLed_x){
// should print and save if changed
                                 
                                  if (OLed_x==2){OLED_Settings[6]=OLED_Settings[OLed_x];} // keep the "paired" OLED set at the same settings..
                                  if (OLed_x==6){OLED_Settings[2]=OLED_Settings[OLed_x];}
                                  if (OLed_x==1){OLED_Settings[5]=OLED_Settings[OLed_x];}
                                  if (OLed_x==5){OLED_Settings[1]=OLED_Settings[OLed_x];}  
                                                 
 //#ifdef  _ROCDISP_EEPROM_DEBUG    
                                 Serial.printf("---SAVING Changed OLED Setting ---/n");                        
                                 OLEDsettingView(OLed_x);
                                 OLEDEEPROMsettingView(OLed_x);
 //#endif                        
                                Data_Updated = true;
                                WriteEEPROM();
                                EPROM_Write_Delay = millis() + 10; 
                                //delay(100);        
                                if (bitRead(OLED_Settings[OLed_x],_32)!= bitRead(OLED_EEPROM_Setting(OLed_x),_32)){
                                                          // #ifdef  _ROCDISP_EEPROM_DEBUG  
                                                                Serial.println("Re-Scanning OLEDS after display mode change");
                                                          // #endif 
                                                                LookForOLEDs(false);// FAlse means less serial printing
                                                                } // Re-Set up displays
                                                      


  
}


int ClockRad;
bool RocDisplayFormatted(int OLed_x, int PixelsDown, String Message){
  bool found,Changed_32;  bool in_format;
  int j,DisplayLine,RowPixel[15],NewLineOffset,MsgLength,TabOne,TabZero,cx;
  int ClockSpaceOffset,ClockPos,FontSelected;
  //bool ClockAnalog,ClockLeft;
  String FormattedMsg,BitMsg,ClockTime; char BitChar;
  bool FlashON,ignoreJ1,ignoreJ2,inJ1,inJ2,ignoreNF,CA; //CA is command accepted
  //uint8_t ClockSettingBefore;
  char MSGTextC[20];
  int ScrolledCharCount;
  bool InScrolling,_ignore_Scrolling, _ignore_Width;
  Changed_32=false;
  InScrolling=false;_ignore_Scrolling=false;_ignore_Width=false;
  ScrolledCharCount=0;
  FontSelected=TerminalDisplayFont;
  FlashON=secs%2;
  ignoreJ1=false;inJ1=false;
  ignoreJ2=false;inJ2=false;
  int RocDisplayMsg;
  if (PixelsDown==0){RocDisplayMsg=1;}
  if (PixelsDown==16){RocDisplayMsg=2;}  
  if (PixelsDown==32){RocDisplayMsg=3;}
  if (PixelsDown==48){RocDisplayMsg=4;}
  cx=sprintf(MSGTextC,"%02d:%02d",hrs,mins);
    ClockTime=MSGTextC; // easy way to convert to string
    
  //ClockSettingBefore=OLED_Settings[OLed_x];
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
  CA=false;
  //Observed_Change=false; 
  j=0;in_format=false;
  
   for (int i=0;i<=TextObjectLength-1; i++){
    if (i <= Message.length()){
      if (Message[i]=='{'){found=true; RocFormatUsed[OLed_x]=true;}
    }}
  // #ifdef  _ROCDISP_EEPROM_DEBUG 
  //          if ( (!Observed_Change) && ( (OLED_Settings[OLed_x]!=OLED_EEPROM_Setting(OLed_x) ))) { // part of debug to find where the change I see happens
  //              Observed_Change=true;   
  //              Serial.printf("--- Time %dms --Before PARSING --\n -- Pixels down %d -- \n ",((millis()-StartedAt)),PixelsDown);
  //              Serial.printf("Message <%s> \n",Message.c_str());                     
  //              OLEDsettingView(OLed_x);
  //              OLEDEEPROMsettingView(OLed_x);
  //              }
 //  #endif
    
    if (found){//do RocDisplay formatting and send to display
        // do formatting stuff to format for display if we have formatting { and } 
        //DebugSprintfMsgSend(sprintf ( DebugMsg, "OLed_x:%d PixelsDown%d full <%s>",OLed_x,PixelsDown,Message.c_str()));
        // set default font to RocDisplay Font F0
        SetFont(OLed_x,FontSelected);
        // set up left and right tabs if clock analog used                        
        if ( (bitRead(OLED_Settings[OLed_x],_ClockLeft))&&(bitRead(OLED_Settings[OLed_x],_ClockAna))&&(bitRead(OLED_Settings[OLed_x],_ClockON)  ) )
             {  ClockSpaceOffset=(ClockRad*2)+1; ClockPos= ClockRad;}
             else{ClockSpaceOffset= 0;           ClockPos= 127-ClockRad;}                     
        //clean out Formatted message.
        FormattedMsg="";
        MsgLength=Message.length();
        if (Message.length()>=1){  
            for (int i=0;i<=(Message.length()-1);i++){  // MAIN PARSING CODE
            CA=false;
                 
            if (Message[i]=='{'){ 
                OLEDdrawString(OLed_x,RowPixel[DisplayLine]+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),FormattedMsg);
                RowPixel[DisplayLine]=RowPixel[DisplayLine]+OLEDgetStringWidth(OLed_x,FormattedMsg);
                //if font = high set row pixel for row below as well.
                if (FontSelected==4){RowPixel[DisplayLine+1]=RowPixel[DisplayLine];}
                             // code uses .getStringWidth(FormattedMsg)to move each line's cursor (RowPixel) across in case we have more text to print on the same line before next (T) Tab
                in_format=true;FormattedMsg=""; 
                }
     
   
             if ((in_format && Message[i]=='S')){ // this code looks at the Set OLed up (S) coding (sets clock etc. 
                        if (!CA){ CA=true;
                             #ifdef  _ROC_MSG_Parsing_DEBUG
                             Serial.printf("%c%c",Message[i],Message[i+1]);
                             #endif
                        switch (Message[i+1]){
                          case 'R':
                              {bitSet(OLED_Settings[OLed_x],_ClockON); bitClear(OLED_Settings[OLed_x],_ClockLeft);bitSet(OLED_Settings[OLed_x],_ClockAna);}
                          break;
                          case 'L':
                              {bitSet(OLED_Settings[OLed_x],_ClockON); bitSet(OLED_Settings[OLed_x],_ClockLeft);bitSet(OLED_Settings[OLed_x],_ClockAna);}
                          break;
                          case '0':
                              {bitClear(OLED_Settings[OLed_x],_ClockON);bitClear(OLED_Settings[OLed_x],_ClockAna);}
                          break;
                          case '1':
                              {bitSet(OLED_Settings[OLed_x],_ClockON); bitClear(OLED_Settings[OLed_x],_ClockAna);}
                          break;
                          case '2': //Two rocdisplays Displays
                              //{Changed_32=true;bitSet(OLED_Settings[OLed_x],_32);}
                          break;
                          case '3':  //or "32 pixel" (two display)
                              {Changed_32=true;bitSet(OLED_Settings[OLed_x],_32);}
                          break;
                          case '4': //4 rocdisplay Displays
                            //  {Changed_32=true;bitClear(OLED_Settings[OLed_x],_32);}
                          break;
                          case '6': //or "64 pixel" (four rocdisplays)
                              {Changed_32=true;bitClear(OLED_Settings[OLed_x],_32);}
                          break;
                          case 'C':
                              FormattedMsg+=ClockTime;
                          break;
                          case 'F': //Flip : not inverted
                              {bitSet(OLED_Settings[OLed_x],_ClockFlip);bitClear(OLED_Settings[OLed_x],_ClockInvert); OLEDScreen(OLed_x,true, false);}
                          break;
                          case 'I'://Not Flip : Inverted
                              {bitClear(OLED_Settings[OLed_x],_ClockFlip);bitSet(OLED_Settings[OLed_x],_ClockInvert);OLEDScreen(OLed_x,false, true);}
                          break;
                          case 'f'://Flip : Inverted
                              {bitSet(OLED_Settings[OLed_x],_ClockFlip);bitSet(OLED_Settings[OLed_x],_ClockInvert);OLEDScreen(OLed_x,true, true);}
                          break;
                          case 'N'://not Flip : not Inverted
                              {bitClear(OLED_Settings[OLed_x],_ClockFlip);bitClear(OLED_Settings[OLed_x],_ClockInvert);OLEDScreen(OLed_x,false, false);}
                          break;
                          case 'x':
                                SetOLEDDefaults();
                          break;
                          case 'E': // save to eeprom -- not strictly needed now since V25 saves automatically --- 
                              if ((OLED_Settings[OLed_x]!=OLED_EEPROM_Setting(OLed_x))){
                                                        Serial.println("Saving Settings on demand");
                                                        SaveOledSettings(OLed_x);
                                                        }
                           break;
                           default:
                           Serial.println("S message not understood");
                           break;
                           }
                           i=i+1;          
                           } }
   
                                         
             if ((in_format && Message[i]=='L')){  // this code does the Lines (L) coding, moving stuff down 8 pixels .
                             if (!CA){ CA=true;
                             DisplayLine=GetNumber(Message,i+1);
                             #ifdef  _ROC_MSG_Parsing_DEBUG
                             Serial.printf("<%c%d>",Message[i],DisplayLine);
                             #endif
                             //DebugSprintfMsgSend(sprintf ( DebugMsg, "OLed_x:%d Changing Line number to %d GN<%d>",OLed_x,DisplayLine,GetNumber(Message,i+1)));
                             i=i+CharCountinGetnumber; //CharCountinGetnumber
                             } }
                             
                               
             if ((in_format && Message[i]=='F')){  // this code looks at the Fonts (F) coding. 
                                 // selects Font  Message[] (now not limited to just 9)
                             if (!CA){ CA=true;
                             FontSelected=GetNumber(Message,i+1);
                             SetFont(OLed_x,FontSelected);
                             #ifdef  _ROC_MSG_Parsing_DEBUG
                             Serial.printf("<%c%d>",Message[i],FontSelected);
                             #endif
                             i=i+CharCountinGetnumber; //CharCountinGetnumber
                             }} 

             if ((in_format && Message[i]=='J')){  // this code looks at the Flash (J) coding. 
                             if (!CA){CA=true;
                             #ifdef  _ROC_MSG_Parsing_DEBUG
                             Serial.printf("%c%c",Message[i],Message[i+1]);
                             #endif  
                             if (Message[i+1]=='0'){inJ1=false;inJ2=false;ignoreJ1=false;ignoreJ2=false;  } // reset at J0
                             if (Message[i+1]=='1'){inJ1=true;ignoreJ1=true^FlashON;} // 
                             if (Message[i+1]=='2'){inJ2=true;inJ1=false;ignoreJ1=false;ignoreJ2=false^FlashON; } //NB false in ignorej2 here inverts when flashon acts !!
                             if (Message[i+1]=='S'){InScrolling=true;ScrolledCharCount=0;}
                             if (Message[i+1]=='C'){InScrolling=false;}
                             i=i+1;
                             } }
                             
            if ((in_format && Message[i]=='T')){  // this code looks at the Columns (T) coding.
                            if (!CA){ CA=true; 
                            #ifdef  _ROC_MSG_Parsing_DEBUG
                            Serial.printf("%c%c",Message[i],Message[i+1]);
                            #endif   
                            if ((in_format && Message[i+2]=='L')){// Check for 'TxLx' codes, as they are common! Saves changing to "correct" 'LxTx' structure
                              DisplayLine=GetNumber(Message,i+3);}
                            if (Message[i+1]=='0'){if (RowPixel[DisplayLine]<=TabZero) {RowPixel[DisplayLine]=TabZero;}}  
                            if (Message[i+1]=='1'){RowPixel[DisplayLine]=TabOne; 
                                        if ( bitRead(OLED_Settings[OLed_x],_ClockAna)) {RowPixel[DisplayLine]=RowPixel[DisplayLine]-(2*ClockRad);}   
                                                  } // offset if Analog clock is in operation
                            i=i+1;
                            } }
       
             if ((in_format && Message[i]=='W')){  // this code looks at the Departure column width (W) coding.
                             if (!CA){ CA=true;
                                // ?? W .. "Departure column width" IF set sets the second tab position in pixels..Is volatile, and lost for next message!
                             TabOne=GetNumber(Message,i+1);
                             #ifdef  _ROC_MSG_Parsing_DEBUG
                             Serial.printf("<%c%d>",Message[i],TabOne);
                             #endif
                             i=i+CharCountinGetnumber;
                             }}
                                             
             if ((in_format && Message[i]=='w')){  // this code looks at the Platform column width (w) coding.
                             if (!CA){ CA=true; 
                             #ifdef  _ROC_MSG_Parsing_DEBUG
                             Serial.print("w");
                             #endif
                             // ?? w .. "Platform column width" IF set sets the first tab position in pixels..Is volatile, and lost for next message!
                             TabZero=GetNumber(Message,i+1);
                             #ifdef  _ROC_MSG_Parsing_DEBUG
                             Serial.printf("<%c%d>",Message[i],TabZero);
                             #endif
                             i=i+CharCountinGetnumber; //CharCountinGetnumber
                             }} 
                                             
            if (((in_format && Message[i]=='B'))&& !((ignoreJ2&&inJ2)||(ignoreJ1&&inJ1))){  // this code looks at the Bitmaps (B) coding. 
                             if (!CA){ CA=true;
                                     // adding flash capability ignore bitmaps after J1 or J2
                             BitChar=GetNumber(Message,i+1)+1; //Select char from bitmaps with offset to avoid asking for char 0 
                             BitMsg=BitChar;BitMsg+="";
                                                      //DebugSprintfMsgSend(sprintf ( DebugMsg, "OLed_x:%d Byte %d  %d",OLed_x,(Message[i+1]-48),GetNumber(Message,i+1)));
                             OLEDsetFont(OLed_x,RocBitMap);
                             OLEDdrawString(OLed_x,RowPixel[DisplayLine]+ClockSpaceOffset, PixelsDown+(DisplayLine*NewLineOffset),BitMsg);
                             RowPixel[DisplayLine]=RowPixel[DisplayLine]+OLEDgetStringWidth(OLed_x,BitMsg);
                             SetFont(OLed_x,FontSelected); // set back to last selected font; 
                                                     //DebugSprintfMsgSend(sprintf ( DebugMsg, "OLed_x:%d Byte %d  %d",OLed_x,(Message[i+1]-48),GetNumber(Message,i+1)));// gets number from Message next byte(s)
                             #ifdef  _ROC_MSG_Parsing_DEBUG
                             Serial.printf("<%c%d>",Message[i],BitChar);
                             #endif
                             i=i+CharCountinGetnumber; //CharCountinGetnumber
                             } }

                              
                              // stuff for scroll control
            if (InScrolling){ScrolledCharCount=ScrolledCharCount+1; 
                          if (ScrolledCharCount>=ScrollMsgLength[RocDisplayMsg][OLed_x]){ScrollMsgLength[RocDisplayMsg][OLed_x]= ScrolledCharCount;} 
                          }  
                          
            _ignore_Scrolling= (InScrolling &&(ScrolledCharCount<=SCP[RocDisplayMsg][OLed_x]));   //_ignore_Scrolling is the ignore beginning of scrolling message stuff

                                  // _ignore_Width senses if the scrolling text is approx one character width beyond the T1 position 
            if (FontSelected==4){    _ignore_Width=(InScrolling &&((RowPixel[DisplayLine]+OLEDgetStringWidth(OLed_x,(FormattedMsg)) )>=TabOne-16));}// F4 is wider than other fonts, so allow at least 16 pixels
                             else{_ignore_Width=(InScrolling &&((RowPixel[DisplayLine]+OLEDgetStringWidth(OLed_x,(FormattedMsg)) )>=TabOne-8)); }
        
        
            if (!(in_format)&&(j<=(TextObjectLength-1))){ // Not In format, This is text add text to formatted_message to display
                              ignoreNF=( ((Message[i-1]=='}')&&(Message[i]==' '))||((Message[i+1]=='{')&&(Message[i]==' ')) ) ;// Ignore spaces near formatting ..do not copy first space after '}' or before {helps with alignment + saves display space,
                              if (!((ignoreJ2&&inJ2)||(ignoreJ1&&inJ1)||(_ignore_Scrolling)|| (_ignore_Width)||(ignoreNF)   ))   {   //J1 J2  are the flash function indicators  
                                     FormattedMsg+=Message[i];j=j+1;} 
                              } 
                              
                                            
            if (Message[i]=='}'){ in_format=false;j=0;}
            }// MAIN PARSING CODE
            }// end of loop parsing the message
     if ( (OLED_Settings[OLed_x]!=OLED_EEPROM_Setting(OLed_x) )) {   
                      SaveOledSettings(OLed_x);}
     
    // IF Analog clock set, draw small real time clock in the top line only 
    // first set up left or right positioning
    if (bitRead(OLED_Settings[OLed_x],_ClockLeft)){ClockSpaceOffset=(ClockRad*2)+1;ClockPos=ClockRad;}
                                              else{ClockSpaceOffset=0;             ClockPos=127-ClockRad;}
    // if analog and clock on, draw the clock                                        
    if ( (bitRead(OLED_Settings[OLed_x],_ClockAna)) && (bitRead(OLED_Settings[OLed_x],_ClockON)) ){  
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
  OLED_4_RN_displays(5,L1,L2,"","");
  OLED_4_RN_displays(6,L1,L2,"","");
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
      if ((L1[0]==0)&&(L2[0]==0)&&(L3[0]==0)&&(L4[0]==0)){
            RRPowerOnIndicator(OLed_x); 
            SignalStrengthBar(OLed_x);
         if ( (bitRead(OLED_Settings[OLed_x],_32))  && (OLed_x>=5) ){BigClock(OLed_x,15);}else{BigClock(OLed_x,30);}// only oleds 5 and 6 (x32 high)need small clock
            }   
            OLEDDisplay(OLed_x);
   }

}




void SetupTextArrays(uint8_t Address,int Display,String Message){
     // move the roc messsages to the TS stores where we will look at them every second!

      if (Address==60){for (uint16_t i = 0; i <= (TextObjectLength-1); i++) {TS[Display][i]=(Message[i]);}
             // Serial.print("setting up TS");Serial.println(Display);
              }
      if (Address==61){for (uint16_t i = 0; i <= (TextObjectLength-1); i++) {TS[Display+8][i]=(Message[i]);} 
             // Serial.print("setting up TS");Serial.println(Display+8);              
              }
  
}





extern uint16_t SW_REV;
extern String wifiSSID;

void OLED_initiate(uint8_t address,int I2CBus,bool disp){ // see commands in //https://github.com/ThingPulse/esp8266-oled-ssd1306
  String MSGText1;String MSGText2;  
    
  if (address==1){ 
   if (disp){Serial.println(F("Initiating Display 1"));} 
   OLED1.init();  
   OLED1.setI2cAutoInit(true);
   //OLED1.flipScreenVertically();
   OLEDScreen(address,bitRead(OLED_Settings[address],_ClockFlip), bitRead(OLED_Settings[address],_ClockInvert));
   OLED1.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   }
  if (address==2){  
   if (disp){Serial.println(F("Initiating Display 2")); }
  OLED2.init();    //Disp2ReInit=millis()+6000;
  OLED2.setI2cAutoInit(true);  
  //OLED2.flipScreenVertically();
  OLEDScreen(address,bitRead(OLED_Settings[address],_ClockFlip), bitRead(OLED_Settings[address],_ClockInvert));
 
  OLED2.setTextAlignment(TEXT_ALIGN_CENTER); offset=64;
    }
  if (address==3){ 
   if (disp){ Serial.println(F("Initiating Display 3"));} 
   OLED3.init(); 
   OLED3.setI2cAutoInit(true); 
   //OLED3.flipScreenVertically();
   OLEDScreen(address,bitRead(OLED_Settings[address],_ClockFlip), bitRead(OLED_Settings[address],_ClockInvert));
   OLED3.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   }
   if (address==4){ 
    if (disp){Serial.println(F("Initiating Display 4")); }
   OLED4.init(); 
   OLED4.setI2cAutoInit(true); 
   //OLED4.flipScreenVertically();
   OLEDScreen(address,bitRead(OLED_Settings[address],_ClockFlip), bitRead(OLED_Settings[address],_ClockInvert));
   OLED4.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   }
    if (address==5){ 
   if (disp){ Serial.println(F("Initiating Display 5")); }
   OLED5.init(); 
   OLED5.setI2cAutoInit(true); 
   //OLED5.flipScreenVertically();
   OLEDScreen(address,bitRead(OLED_Settings[address],_ClockFlip), bitRead(OLED_Settings[address],_ClockInvert));
   OLED5.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   }
    if (address==6){ 
   if (disp){ Serial.println(F("Initiating Display 6")); }
   OLED6.init(); 
   OLED6.setI2cAutoInit(true); 
   //OLED6.flipScreenVertically();
   OLEDScreen(address,bitRead(OLED_Settings[address],_ClockFlip), bitRead(OLED_Settings[address],_ClockInvert));
   OLED6.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   
   }
  OLEDclear(address);
  SetFont(address,4);
  MSGText2="INIT:";MSGText2+=address;
 if (disp){ Serial.println(F("show Init: on display"));}
  OLEDdrawString(address,64, 0, MSGText2);
  OLEDDisplay(address);
  delay(100);
  SetFont(address,0);



   
}
extern uint8_t OLED_EEPROM_Setting(int OLed_x);
extern int NumberOfOLEDS;   
 
void LookForOLEDs(bool Display){

#ifdef _ROCDISP_EEPROM_DEBUG
 
  //          if ( (!Observed_Change) && ( (OLED_Settings[OLed_x]!=OLED_EEPROM_Setting(OLed_x) ))) { // part of debug to find where the change I see happens
  //              Observed_Change=true;   
  //              Serial.printf("--- Time %dms --Before PARSING --\n -- Pixels down %d -- \n ",((millis()-StartedAt)),PixelsDown);
  //              Serial.printf("Message <%s> \n",Message.c_str());                     
  //              OLEDsettingView(OLed_x);
  //              OLEDEEPROMsettingView(OLed_x);
  //              }

#endif


if (Display){  
Serial.println ();
  Serial.print ("I2C scanner. using secondary SDA2:");Serial.print(OLED_SDA2);Serial.print("  SCL2:");Serial.print(OLED_SCL2);Serial.println("  Scanning");
            }
  byte count = 0;
  OLED6Present=false;OLED5Present=false;OLED4Present=false;OLED3Present=false;OLED2Present=false;OLED1Present=false;

  
  Wire.begin(OLED_SDA2, OLED_SCL2);  // this is bus2
  for (byte i = 8; i <= 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
        if (Display){
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.print (")");
       if (i==60){ if (bitRead(OLED_Settings[2],_32)||bitRead(OLED_Settings[6],_32)){Serial.println (" OLED 6 ");} // if _32 set, we have _32 displays
                                               else{Serial.println (" OLED 2 ");}}
                                               
       if (i==61){Serial.println (" OLED 4 ");}
      
                    }    
      if (i==60){ if (bitRead(OLED_Settings[2],_32)||bitRead(OLED_Settings[6],_32)){OLED2Present=false;OLED6Present=true;OLED_initiate(6,2,Display);count++;} // if _32 set, we have _32 displays
                                               else{OLED2Present=true;OLED6Present=false;OLED_initiate(2,2,Display);count++;}}
                
      if (i==61){OLED4Present=true;OLED_initiate(4,2,Display);count++;}
 
      
      } // end of good response
  } // end of for loop
  if (Display){
  Serial.println ();
  Serial.print ("I2C scanner. using SDA:");Serial.print(OLED_SDA);Serial.print("  SCL:");Serial.print(OLED_SCL);Serial.println("  Scanning");
              }
  Wire.begin(OLED_SDA, OLED_SCL);  // 
  for (byte i = 8; i <= 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
        if (Display){
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.print (")");
      

      if (i==60){ if (bitRead(OLED_Settings[1],_32)||bitRead(OLED_Settings[5],_32)){Serial.println (" OLED 5 ");} // if _32 set, we have _32 displays
                                               else{Serial.println (" OLED 1 ");}
                }
      if (i==61){Serial.println (" OLED 3 ");}
                    }
                    
      if (i==60){ if (bitRead(OLED_Settings[1],_32)||bitRead(OLED_Settings[5],_32)){OLED1Present=false;OLED5Present=true;OLED_initiate(5,1,Display);count++;} // if _32 set, we have _32 displays
                                               else{OLED1Present=true;OLED5Present=false;OLED_initiate(1,1,Display);count++;}
                }
      if (i==61){OLED3Present=true;OLED_initiate(3,1,Display);count++;}
                    
 
      } // end of good response
  } // end of for loop
  NumberOfOLEDS=count;
  #ifdef _ROCDISP_EEPROM_DEBUG
  for (byte i = 1; i <= 6; i++)
  {
   if (OLEDPresent(i)) { OLEDsettingView(i); }
   }
  #endif
  if (Display){                  
  Serial.println ("SCAN Done.");
  Serial.print ("Found ");
  Serial.print (NumberOfOLEDS, DEC);
  Serial.println (" OLED (s).");}
    else{if (DEC>=1){Serial.println ("");
  Serial.print ("Scan completed, found :");
  Serial.print (NumberOfOLEDS, DEC);
  Serial.println (" OLED(s)");}}
}


void RRPowerOnIndicator(int OLED_x) {
      if (!POWERON){ OLEDdrawRect(OLED_x,0,0,8,8);}
            else{    OLEDfillRect(OLED_x,0,0,8,8);}    
            }
  



void SignalStrengthBar(int OLED_x) { //https://stackoverflow.com/questions/15797920/how-to-convert-wifi-signal-strength-from-quality-percent-to-rssi-dbm
  int PosX,PosY,AntH;
  int32_t rssi;
  rssi=SigStrength();
 if((OLEDPresent(OLED_x))){
  // rssi -90 is just about dropout..
  // rssi -50 is a great signal
  //Antenna appears from -98db, is full with top bits at -89
  AntH=100+rssi; if (AntH>=10){AntH=10;}
                 if (AntH<=2){AntH=2;}
  PosX=108;  // position left right  max = 128
  PosY=0; // top left position up / down max 64
                  OLEDdrawLine(OLED_x,PosX,PosY+(10-AntH),PosX,PosY+10);
  if (rssi >= -89){  
                  OLEDdrawLine( OLED_x,PosX,PosY+4,PosX-4,PosY);
                  OLEDdrawLine( OLED_x,PosX,PosY+4,PosX+4,PosY);}
  if (rssi >= -50){OLEDdrawLine( OLED_x,PosX+10,PosY,PosX+10,PosY+10);}
  if (rssi >= -60){OLEDdrawLine( OLED_x,PosX+8,PosY+2,PosX+8,PosY+10);}
  if (rssi >= -70){OLEDdrawLine( OLED_x,PosX+6,PosY+4,PosX+6,PosY+10);}
  if (rssi >= -80){OLEDdrawLine( OLED_x,PosX+4,PosY+6,PosX+4,PosY+10);}
  if (rssi >= -85){OLEDdrawLine( OLED_x,PosX+2,PosY+8,PosX+2,PosY+10);}
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

          }
 }

extern int32_t SigStrength(void);
extern uint32_t Timer[8];
extern int LoopCount;
void OLED_Status(){
  String MSGText;
  char MSGTextC[20];
  bool Analog;
  int cx;
  Analog=true;
  
  //Digital Time display 
              cx=sprintf(MSGTextC,"%02d:%02d:%02d",hrs,mins,secs);  
  MaxWidth=128; // for word wrap
 // Timer[0]=millis();
  for (int i=1;i<=6;i++){
   //Serial.print(" Display:");Serial.print(i);Serial.print(" present");Serial.println(OLEDPresent(i));
    if (OLEDPresent(i)){ 
      delay(1); // see if this removes clicks in audio by giving some time for other tasks..
      OLEDsetTextAlignment(i,TEXT_ALIGN_LEFT); offset=0;  SetFont(i,TerminalDisplayFont);  
      if ( (RocFormatUsed[i])&&(bitRead(OLED_Settings[i],_ClockAna))&&(bitRead(OLED_Settings[i],_ClockON)  ) ){// any roc formatted text and analog clock on showing??
            if (!bitRead(OLED_Settings[i],_ClockLeft) ){
                            offset=0;MaxWidth=127-(ClockRad*2);}else{
                            offset=1+(ClockRad*2);MaxWidth=127-(ClockRad*2);} }
      if ((i==1) ){OLED_4_RN_displays(i,TS[1],TS[2],TS[3],TS[4]);}      
      if ((i==5) ){OLED_4_RN_displays(i,TS[1],TS[2],"","");}
      if ((i==6) ){OLED_4_RN_displays(i,TS[5],TS[6],"","");}
      if ((i==2) ){OLED_4_RN_displays(i,TS[5],TS[6],TS[7],TS[8]);}
      if ((i==3) ){OLED_4_RN_displays(i,TS[9],TS[10],TS[11],TS[12]);}
      if ((i==4) ){OLED_4_RN_displays(i,TS[13],TS[14],TS[15],TS[16]);}
      OLEDsetTextAlignment(i,TEXT_ALIGN_CENTER);offset=64; // reset to centered for anything else
       }
    //   Timer[i]=millis();
  }// adds time displays
 // DebugSprintfMsgSend(sprintf ( DebugMsg, "OLedTiming 1<%d> 2<%d>  3<%d> 4<%d> Loop count<%d>",Timer[1]-Timer[0],Timer[2]-Timer[1],Timer[3]-Timer[2],Timer[4]-Timer[3],LoopCount) );// gets number from Message next byte(s)
 // LoopCount=0;                          
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

    
void LookForOLEDs(bool Display){};
void LookForOLED_secondary(void){};
void RRPowerOnIndicator(int Disp) ;
void drawRect(void){} ;

void fillRect(void) {};


void SignalStrengthBar(int oled){};  //https://stackoverflow.com/questions/15797920/how-to-convert-wifi-signal-strength-from-quality-percent-to-rssi-dbm


void showTimeAnalog(int disp,int clocksize,int center_x, int center_y, double pl1, double pl2, double pl3){};
void showTimeAnalogCircle(int disp,int clocksize,int circsize,int center_x, int center_y, double pl1, double pl2, double pl3){};

void BigClock(int disp,int clocksize){};

void OLED_Status(){};
void TimeGears(){};


#endif //_OLED
