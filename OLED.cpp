
  #include "OLED.h"
  #include <Arduino.h> //needed 
  #include "Directives.h"
  //#include "Globals.h"
   #include <SSD1306.h> // alias for `#include "SSD1306Wire.h"` //https://github.com/ThingPulse/esp8266-oled-ssd1306
  SSD1306  display1(0x3c, OLED_SDA, OLED_SCL);
  SSD1306  display2(0x3d, OLED_SDA, OLED_SCL);  
#define DisplayWidth 100  //100 to allow passing of {} formatting    33to allow up to 32 chars width for monspaced display
#define TerminalDisplayFont 6
 
char L1[DisplayWidth],L2[DisplayWidth],L3[DisplayWidth],L4[DisplayWidth],L5[DisplayWidth],L6[DisplayWidth],L7[DisplayWidth],L8[DisplayWidth],L9[DisplayWidth],L10[DisplayWidth];
bool Display1Present,Display2Present;
uint8_t offset;
//https://github.com/ThingPulse/esp8266-oled-ssd1306
// you will also need to add Monospaced Plain 8 size font (http://oleddisplay.squix.ch/#/home) to C:\Arduino\libraries\esp8266-oled-ssd1306-master\src\OLEDDisplayFonts.h


extern char DebugMsg[127];
extern uint8_t hrs;
extern uint8_t mins;
extern uint8_t secs;  
extern uint8_t divider;
extern char Nickname[35];
extern bool POWERON;
extern void DebugSprintfMsgSend(int CX);

void SetFont(uint8_t Disp,uint8_t Font){
if (Disp==1){
  switch (Font) {
    // Fonts need some work before being published. 
    /*
    case 0: 
          display1.setFont(ArialMT_Plain_10); // its a standard font and very readable 
         
          //Serial.print("Font 0 height ?");
          //Serial.println(ArialMT_Plain_10[1]);
    break;
    case 1: 
          display1.setFont(Roboto_8);
          
    break;
    case 2: 
          display1.setFont(Roboto_7);
    break;
    case 3: 
          display1.setFont(Roboto_6_inv);
    break;
    case 4: 
          display1.setFont(Roboto_Condensed_Light_20);
    break;
    case 5: 
          display1.setFont(Roboto_Condensed_Light_16);
    break;
    case 6: 
          display1.setFont(Roboto_Condensed_20);
    break;
    case 7: 
          display1.setFont(Monospaced_plain_8);
    break;
    case 8: 
          display1.setFont(DejaVu_Sans_Mono_14);
    break;
    case 9: 
          display1.setFont(ArialMT_Plain_10);
    break;
    */
    default:
          display1.setFont(ArialMT_Plain_10);
    break;
}
}
 if (Disp==2){
  switch (Font) {
    
    /*
    case 0: 
          //display2.setFont(ArialMT_Plain_10);
    break;
    case 1: 
          //display2.setFont(Dialog_plain_6);
    break;
    case 2: 
          //display2.setFont(Crushed_Regular_6);
    break;
    case 3: 
          //display2.setFont(Crushed_Regular_5);
    break;
    case 4: 
          //display2.setFont(DejaVu_Sans_Mono_14);
    break;
    case 5: 
          //display2.setFont(Monospaced_plain_6);
    break;
    case 6: 
          //display2.setFont(Monospaced_plain_8);
    break;
    */
    default:
          display2.setFont(ArialMT_Plain_10);
    break;
}}
}

void StringToL5(char *line, String input){
  for (int i=0;i<=DisplayWidth-1; i++){
    if (i <= input.length()){
      line[i]=input[i];
    }
  }
}

bool RocDisplayFormatted(int disp, int line, String Message){
  bool found;
  int rowPixel, NewLineOffset;int MsgLength;
  int ClockSpaceOffset,ClockPos,ClockRad;
  bool ClockAnalog,ClockLeft;
  String FormattedMsg;
  int j;
  bool in_format;
  // get clock analog/left from eeprom??
  ClockRad =8;
  ClockLeft=false;
  ClockAnalog=false;
  NewLineOffset=0; // or 16 if line 2
  if ((ClockAnalog)&&(ClockLeft)){ClockSpaceOffset=(ClockRad*2)+1;ClockPos=ClockRad;}
                 else{ClockSpaceOffset=0;ClockPos=127-ClockRad;}
  
  
  j=0;
  rowPixel=0;
  found=false;
   for (int i=0;i<=DisplayWidth-1; i++){
    if (i <= Message.length()){
      if (Message[i]=='{'){found=true;}
    }}
  if (found){//do RocDisplay formatting and send to display
     
  
    // do formatting stuff to format for display 1 or 2 if we have formatting { and } 
    // if { print any FormattedMsg we have and clear FormtattedMsg to prepare for next text
    // after { do any formatting, set tabs for next print etc
    // if } end format stuff, start collecting the next message to print in FormattedMsg[]
  j=0;in_format=false;
  //clean out Formatted message.
  FormattedMsg="";
  MsgLength=Message.length();
  //DebugSprintfMsgSend(sprintf ( DebugMsg, "Disp:%d line%d Before format<%s>",disp,line,Message.c_str()));
  if (Message.length()>=1){  
    for (int i=0;i<=(Message.length()-1);i++){
         if (Message[i]=='{'){ if (disp==1){display1.drawString(rowPixel+ClockSpaceOffset, line+NewLineOffset,FormattedMsg);rowPixel=rowPixel+display1.getStringWidth(FormattedMsg);}
                               if (disp==2){display2.drawString(rowPixel+ClockSpaceOffset, line+NewLineOffset,FormattedMsg);rowPixel=rowPixel+display2.getStringWidth(FormattedMsg);}
                               // use .getStringWidth(FormattedMsg)to move cursor across in case we have more text to print on the same line before next (T) Tab
                               in_format=true;                                
                               }
 // this code to make formatting for the 128*32 displays. Needs Some work to get the fonts better, so commented out for now.       
 /*        
         if ((in_format && Message[i]=='S')){  // this code looks at the Show Clock(S) coding. 
                             if (Message[i+1]=='R'){ClockSpaceOffset=0;ClockPos=127-ClockRad;ClockAnalog=true;} // 
                             if (Message[i+1]=='L'){ClockSpaceOffset=(ClockRad*2)+1;ClockPos=ClockRad;ClockAnalog=true;} // 
                             //if (Message[i+1]=='0'){ClockSpaceOffset=0;ClockPos=108;ClockAnalog==true;} // 
                             //if (Message[i+1]=='1'){ClockSpaceOffset=16;ClockPos=0;ClockAnalog=true;} // 
                                         i=i+1;    }
         if ((in_format && Message[i]=='L')){  // this code looks at the Lines (L) coding. 
                             if (Message[i+1]=='0'){NewLineOffset=0;    } // 
                             if (Message[i+1]=='1'){NewLineOffset=10;    } // 
                                         i=i+1;    }
                               
         if ((in_format && Message[i]=='F')){  // this code looks at the Fonts (F) coding. 
                             // DebugSprintfMsgSend(sprintf ( DebugMsg, "Disp:%d Changing font to %d",disp,Message[i+1]));
                             SetFont(disp,(Message[i+1]-48));    // selects Font 0-9 Message[] is font in ascii.. 48=0
                             //i=i+1;
                                             }  
                                             // 
     */
        if ((in_format && Message[i]=='T')){  // this code looks at the Columns (T) coding. 
                             if (Message[i+1]=='0'){rowPixel=20;    } // 20 is pixel start for column for departures
                             if (Message[i+1]=='1'){rowPixel=80;    } // 90 is pixel start for time column 
                                         i=i+1;    }                              
          
        if (!(in_format)&&(j<=(DisplayWidth-1))){
                                        if (!( ((Message[i-1]=='}')&&(Message[i]==' ')) ||((Message[i-1]==' ')&&(Message[i]==' ')) ))  {  // do not copy first space after '}' helps with alignment + saves space,
                                            FormattedMsg+=Message[i];j=j+1; } }
                                            
          if (Message[i]=='}'){ in_format=false;j=0;FormattedMsg="";}
          
          
     }}
    
    if (ClockAnalog){if (disp==1){ //display1.fillCircle(ClockPos,ClockRad+line,ClockRad);}
                                display1.drawCircle(ClockPos,ClockRad+line,ClockRad);
                                showTimeAnalog(1,ClockRad,ClockPos,ClockRad+line, 0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
                                showTimeAnalog(1,ClockRad,ClockPos,ClockRad+line, 0.1, 0.9, mins);
                                }
                     if (disp==2){ //display2.fillCircle(ClockPos,ClockRad+line,ClockRad);}
                                display2.drawCircle(ClockPos,ClockRad+line,ClockRad);
                                showTimeAnalog(2,ClockRad,ClockPos,ClockRad+line, 0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
                                showTimeAnalog(2,ClockRad,ClockPos,ClockRad+line, 0.1, 0.9, mins);
                                }
                     }
     
    }
  return found;
}

void OLED_5_line_display(int addr,String L1,String L2,String L3,String L4,String L5){
if (addr==1&&Display1Present){
   display1.clear();
 // TopLine();
  //display1.setFont(ArialMT_Plain_10);
  // display unformatted lines as before
  if (!RocDisplayFormatted(1,13,L1)){display1.drawString(offset, 13, L1);}
  if (!RocDisplayFormatted(1,23,L2)){display1.drawString(offset, 23, L2);}
  if (!RocDisplayFormatted(1,33,L3)){display1.drawString(offset, 33, L3);}
  if (!RocDisplayFormatted(1,43,L4)){display1.drawString(offset, 43, L4);}
  if (!RocDisplayFormatted(1,53,L5)){display1.drawString(offset, 53, L5);}
  display1.display();
}
if (addr==2&&Display2Present){
  display2.clear();
 // TopLine();
  //display2.setFont(ArialMT_Plain_10);
  if (!RocDisplayFormatted(2,13,L1)){display2.drawString(offset, 13, L1);}
  if (!RocDisplayFormatted(2,23,L2)){display2.drawString(offset, 23, L2);}
  if (!RocDisplayFormatted(2,33,L3)){display2.drawString(offset, 33, L3);}
  if (!RocDisplayFormatted(2,43,L4)){display2.drawString(offset, 43, L4);}
  if (!RocDisplayFormatted(2,53,L5)){display2.drawString(offset, 53, L5);}
  display2.display();
  }
}




void Oled_Station_Display_2(uint8_t Address,int Display,String Message){
   if (Address==60){
      if (Display==1){for (byte i = 0; i <= (DisplayWidth-1); i++) {L2[i]=(Message[i]);}}
      if (Display==2){for (byte i = 0; i <= (DisplayWidth-1); i++) {L3[i]=(Message[i]);}}
      if (Display==3){for (byte i = 0; i <= (DisplayWidth-1); i++) {L4[i]=(Message[i]);}}
      if (Display==4){for (byte i = 0; i <= (DisplayWidth-1); i++) {L5[i]=(Message[i]);}}
   }
   if (Address==61){
      if (Display==1){for (byte i = 0; i <= (DisplayWidth-1); i++) {L7[i]=(Message[i]);}}
      if (Display==2){for (byte i = 0; i <= (DisplayWidth-1); i++) {L8[i]=(Message[i]);}}
      if (Display==3){for (byte i = 0; i <= (DisplayWidth-1); i++) {L9[i]=(Message[i]);}}
      if (Display==4){for (byte i = 0; i <= (DisplayWidth-1); i++) {L10[i]=(Message[i]);}}
   }
}





extern uint16_t SW_REV;
extern String wifiSSID;
void OLED_initiate(byte address){
  //sets initial font as Font 0

  String MSGText1;String MSGText2;
  if (address=60){  
   display1.init();  
   //display1.resetDisplay();
   //display.invertDisplay();
   display1.flipScreenVertically();
   display1.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   SetFont(1,0);
   MSGText1="Looking for <";MSGText1+=wifiSSID;MSGText1+="> ";//too early for this as ssid is not read!
   MSGText2="Code <Ver:";MSGText2+=SW_REV;MSGText2+="> ";
   OLED_5_line_display(1,""," ","",MSGText2,"");
}
if (address=61){  
  display2.init();  
  //display1.resetDisplay();
  //display.invertDisplay();
  display2.flipScreenVertically();
  display2.setTextAlignment(TEXT_ALIGN_CENTER); offset=64;
  SetFont(2,0);
  MSGText1="Second display <";MSGText1+=wifiSSID;MSGText1+="> ";
  MSGText2="code <Ver:";MSGText2+=SW_REV;MSGText2+="> ";
  OLED_5_line_display(2,"","Second Display",MSGText2,"","");
}


//#endif
}

    
void LookForOLEDs(void){
 Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;
  Wire.begin(OLED_SDA, OLED_SCL);  // ESP32 default uses 21 and 22!!!
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1);  // maybe unneeded?
      if (i==60){Display1Present=true;OLED_initiate(60);}
      if (i==61){Display2Present=true;OLED_initiate(61);}
      
      
      } // end of good response
  } // end of for loop
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
  
}




void drawRect(void) {
// #ifdef ESP32 
  if(Display1Present){  display1.drawRect(0, 0, 8, 8);}
// #endif  
}


void fillRect(void) {
// #ifdef ESP32
  if(Display1Present){   display1.fillRect(0,0,8,8);}
// #endif   
  }


void SignalStrengthBar( int32_t rssi) { //https://stackoverflow.com/questions/15797920/how-to-convert-wifi-signal-strength-from-quality-percent-to-rssi-dbm
  int PosX,PosY;
 if(Display1Present){
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
  if (disp==1&&Display1Present){display1.drawLine(x1,y1,x2,y2);}
  if (disp==2&&Display2Present){display2.drawLine(x1,y1,x2,y2);}
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
  if (disp==1&&Display1Present){display1.drawCircle(x2, y2,circsize);}
  if (disp==2&&Display2Present){display2.drawCircle(x2,y2,circsize);}
}

void BigClock(int disp,int clocksize){
  int center_x,center_y;
  center_x=64;
  center_y=clocksize;
  if (disp==1&&Display1Present){
      if (clocksize>=20){
          display1.drawCircle(center_x, center_y, clocksize);
          for (int i=1;i<=12;i++){showTimeAnalog(1,clocksize,center_x,center_y, 0.8 ,0.95 , (i * 5) );}
          showTimeAnalog(1,clocksize,center_x,center_y, -0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
          showTimeAnalog(1,clocksize,center_x,center_y, -0.1, 0.9, mins);
         if (divider==1){ showTimeAnalog(1,clocksize,center_x,center_y, -0.2, 0.5, secs);
          showTimeAnalogCircle(1,clocksize,4, center_x,center_y, -0.2, 0.65, secs);
          showTimeAnalog(1,clocksize,center_x,center_y, 0.8, 0.9, secs);}
          display1.display();
       if (divider==1){ showTimeAnalog(1,clocksize,center_x,center_y, 0.2, 0.5, secs);
        showTimeAnalogCircle(1,clocksize,4, center_x,center_y, 0.2, 0.65, secs);
        showTimeAnalog(1,clocksize,center_x,center_y, 0.8, 0.9, secs);}
        display1.display();
        }else{ 
            display1.drawCircle(center_x, center_y, clocksize);
            showTimeAnalog(1,clocksize,center_x,center_y, 0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
            showTimeAnalog(1,clocksize,center_x,center_y, 0.1, 0.9, mins);
            display1.display();
        }}
  
  if (disp==2&&Display2Present){
      if (clocksize>=20){
          display2.drawCircle(center_x, center_y, clocksize);
          for (int i=1;i<=12;i++){showTimeAnalog(2,clocksize,center_x,center_y, 0.8 ,0.95 , (i * 5) );}
          showTimeAnalog(2,clocksize,center_x,center_y, -0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
          showTimeAnalog(2,clocksize,center_x,center_y, -0.1, 0.9, mins);
         if (divider==1){ showTimeAnalog(2,clocksize,center_x,center_y, -0.2, 0.5, secs);
          showTimeAnalogCircle(2,clocksize,4, center_x,center_y, -0.2, 0.65, secs);
          showTimeAnalog(2,clocksize,center_x,center_y, 0.8, 0.9, secs);}
          display2.display();
      }else{
            display2.drawCircle(center_x, center_y, clocksize);
            showTimeAnalog(2,clocksize,center_x,center_y, 0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
            showTimeAnalog(2,clocksize,center_x,center_y, 0.1, 0.9, mins);
            display2.display();
          }
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
   SignalStrengthBar(SigStrength()); 
   if (POWERON) {fillRect();}else{drawRect();}
   if (Analog){if ((L1[0]==0)&&(L2[0]==0)&&(L3[0]==0)&&(L4[0]==0)&&(L5[0]==0)){BigClock(1,30);  }else{ BigClock(1,10);  }
               }else{SetFont(1,0);display1.drawString(64,0,MSGTextC);display1.display(); }
   }// adds time display 1
if (Display2Present){ 
   display2.setTextAlignment(TEXT_ALIGN_LEFT); offset=0;
   SetFont(2,TerminalDisplayFont);  
   OLED_5_line_display(2,L6,L7,L8,L9,L10);
   display2.setTextAlignment(TEXT_ALIGN_CENTER);offset=64;
   if (Analog){if ((L6[0]==0)&&(L7[0]==0)&&(L8[0]==0)&&(L9[0]==0)&&(L10[0]==0)){BigClock(2,30);  }else{ BigClock(2,10);  }
    }else{SetFont(2,0);display2.drawString(64,0,MSGTextC); display2.display();}
   } // display 2
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




















