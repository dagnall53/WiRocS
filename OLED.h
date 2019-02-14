#ifndef _OLED_h
  #define _OLED_h
  #include "Arduino.h"
  #include "Directives.h"
  
#define DisplayWidth 33  // to give 32 chars
char L1[DisplayWidth],L2[DisplayWidth],L3[DisplayWidth],L4[DisplayWidth],L5[DisplayWidth],L6[DisplayWidth],L7[DisplayWidth],L8[DisplayWidth],L9[DisplayWidth],L10[DisplayWidth];
bool Display1Present,Display2Present;
uint8_t offset;
//https://github.com/ThingPulse/esp8266-oled-ssd1306
// you will also need to add Monospaced Plain 8 size font (http://oleddisplay.squix.ch/#/home) to C:\Arduino\libraries\esp8266-oled-ssd1306-master\src\OLEDDisplayFonts.h

void OLED_5_line_display(int addr,String L1,String L2,String L3,String L4,String L5){
//#ifdef ESP32
if (addr==1&&Display1Present){
   display1.clear();
 // TopLine();
  //display1.setFont(ArialMT_Plain_10);
  display1.drawString(offset, 13, L1);
  display1.drawString(offset, 23, L2);
  display1.drawString(offset, 33, L3);
  display1.drawString(offset, 43, L4);
  display1.drawString(offset, 53, L5);
  display1.display();
}
if (addr==2&&Display2Present){
  display2.clear();
 // TopLine();
  //display2.setFont(ArialMT_Plain_10);
  display2.drawString(offset, 13, L1);
  display2.drawString(offset, 23, L2);
  display2.drawString(offset, 33, L3);
  display2.drawString(offset, 43, L4);
  display2.drawString(offset, 53, L5);
  display2.display();
  }

  //#endif
}
void OLED_Align(bool left){
 /* if (left){
    offset=0;
    if (Display1Present){display1.setTextAlignment(TEXT_ALIGN_LEFT);display1.setFont(Monospaced_plain_6);}
    if (Display2Present){display2.setTextAlignment(TEXT_ALIGN_LEFT);display2.setFont(Monospaced_plain_6);}
  }else{
    
    offset=64;
    if (Display1Present){display1.setTextAlignment(TEXT_ALIGN_CENTER);display1.setFont(ArialMT_Plain_10);}
    if (Display2Present){display2.setTextAlignment(TEXT_ALIGN_CENTER);display2.setFont(ArialMT_Plain_10);}
  }
  */
}


void Oled_Station_Display(uint8_t Address,int Display,String Message){
  int MsgLength;
  char FormattedMsg[DisplayWidth+5];
  int j;bool in_format;
    // do formatting stuff to format for display Find { and } etc...do stuff in between
  OLED_Align(true);  
  j=0;in_format=false;
  for (int i=0;i<=(DisplayWidth+4);i++){FormattedMsg[i]=' ';}
  MsgLength=Message.length();
  DebugSprintfMsgSend(sprintf ( DebugMsg, "Address:%d line%d Before format<%s>",Address,Display,Message.c_str()));
  if (Message.length()>=1){  
    for (int i=0;i<=(Message.length()-1);i++){
          if (Message[i]=='{'){ in_format=true;}
         if ((in_format && Message[i]=='T')){
                             if (Message[i+1]=='0'){j=6;    } // 6 is start for column for text
                             if (Message[i+1]=='1'){j=20;    } // 21 is start for time column
                                             }
          if (!(in_format)&&(j<=(DisplayWidth-1))){
                                        if (!( ((Message[i-1]=='}')&&(Message[i]==' ')) ||((Message[i-1]==' ')&&(Message[i]==' ')) ))  {  // do not copy first space after '}' helps with alignment + saves space,
                                            FormattedMsg[j]=(Message[i]);j=j+1; }}
          if (Message[i]=='}'){ in_format=false;}
     }}
     FormattedMsg[j]=0; // add end marker to close string
  //DebugSprintfMsgSend(sprintf ( DebugMsg, " After format display<%d> line<%d> Text<%s>",Address,Display,*FormattedMsg));
  DebugSprintfMsgSend(sprintf ( DebugMsg, " Address:%d line%d display <%s>",Address,Display,FormattedMsg));
 
  if (Address==60){
     if (Display==1){for (byte i = 0; i <= (DisplayWidth-1); i++) {L3[i]=(FormattedMsg[i]);}}
     if (Display==2){for (byte i = 0; i <= (DisplayWidth-1); i++) {L4[i]=(FormattedMsg[i]);}}
     if (Display==3){for (byte i = 0; i <= (DisplayWidth-1); i++) {L5[i]=(FormattedMsg[i]);}}


  }
 if (Address==61){
     if (Display==1){for (byte i = 0; i <= (DisplayWidth-1); i++) {L8[i]=(FormattedMsg[i]);}}
     if (Display==2){for (byte i = 0; i <= (DisplayWidth-1); i++) {L9[i]=(FormattedMsg[i]);}}
     if (Display==3){for (byte i = 0; i <= (DisplayWidth-1); i++) {L10[i]=(FormattedMsg[i]);}}
    }
  
}

void OLED_initiate(byte address){
  //#ifdef ESP32

  String MSGText1;String MSGText2;
  if (address=60){  
   display1.init();  
   //display1.resetDisplay();
   //display.invertDisplay();
   display1.flipScreenVertically();
   display1.setTextAlignment(TEXT_ALIGN_CENTER);
   offset=64;
   display1.setFont(ArialMT_Plain_10);
   MSGText1="Looking for <";MSGText1+=wifiSSID;MSGText1+="> ";//too early for this as ssid is not read!
   MSGText2="Code <Ver:";MSGText2+=SW_REV;MSGText2+="> ";
   OLED_5_line_display(1,""," ","",MSGText2,"");
}
if (address=61){  
  display2.init();  
  //display1.resetDisplay();
  //display.invertDisplay();
  display2.flipScreenVertically();
  display2.setTextAlignment(TEXT_ALIGN_CENTER);
  offset=64;
  display2.setFont(ArialMT_Plain_10);
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
//  #ifdef ESP32
  double RAD = 3.141592 / 180;
  double LR = 89.99;
  double x1, x2, y1, y2;
  x1 = center_x + (clocksize * pl1) * cos((6 * pl3 + LR) * RAD);
  y1 = center_y + (clocksize * pl1) * sin((6 * pl3 + LR) * RAD);
  x2 = center_x + (clocksize * pl2) * cos((6 * pl3 - LR) * RAD);
  y2 = center_y + (clocksize * pl2) * sin((6 * pl3 - LR) * RAD);
  if (disp==1&&Display1Present){display1.drawLine((int)x1, (int)y1, (int)x2, (int)y2);}
  if (disp==2&&Display2Present){display2.drawLine((int)x1, (int)y1, (int)x2, (int)y2);}
//  #endif
}
void showTimeAnalogCircle(int disp,int clocksize,int circsize,int center_x, int center_y, double pl1, double pl2, double pl3)
{ 
 // #ifdef ESP32
  double RAD = 3.141592 / 180;
  double LR = 89.99;
  double x1, x2, y1, y2;
  x1 = center_x + (clocksize * pl1) * cos((6 * pl3 + LR) * RAD);
  y1 = center_y + (clocksize * pl1) * sin((6 * pl3 + LR) * RAD);
  x2 = center_x + (clocksize * pl2) * cos((6 * pl3 - LR) * RAD);
  y2 = center_y + (clocksize * pl2) * sin((6 * pl3 - LR) * RAD);
  if (disp==1&&Display1Present){display1.drawCircle((int)x2, (int)y2,circsize);}
  if (disp==2&&Display2Present){display2.drawCircle((int)x2, (int)y2,circsize);}
 // #endif
}

void BigClock(int disp,int clocksize){
  int center_x,center_y;
  center_x=64;
  center_y=clocksize;
  if (disp==1&&Display1Present){
      if (clocksize>=20){
        display1.drawCircle(center_x, center_y, clocksize);
        for (int i=1;i<=12;i++){showTimeAnalog(1,clocksize,center_x,center_y, -0.8,1 , i * 5 );}
        showTimeAnalog(1,clocksize,center_x,center_y, 0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
        showTimeAnalog(1,clocksize,center_x,center_y, 0.1, 0.9, mins);
       if (divider==1){ showTimeAnalog(1,clocksize,center_x,center_y, 0.2, 0.5, secs);
        showTimeAnalogCircle(1,clocksize,4, center_x,center_y, 0.2, 0.65, secs);
        showTimeAnalog(1,clocksize,center_x,center_y, -0.8, 0.9, secs);}
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
          for (int i=1;i<=12;i++){showTimeAnalog(2,clocksize,center_x,center_y, -0.8,1 , i * 5 );}
          showTimeAnalog(2,clocksize,center_x,center_y, 0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
          showTimeAnalog(2,clocksize,center_x,center_y, 0.1, 0.9, mins);
         if (divider==1){ showTimeAnalog(2,clocksize,center_x,center_y, 0.2, 0.5, secs);
          showTimeAnalogCircle(2,clocksize,4, center_x,center_y, 0.2, 0.65, secs);
          showTimeAnalog(2,clocksize,center_x,center_y, -0.8, 0.9, secs);}
          display2.display();
      }else{
            display2.drawCircle(center_x, center_y, clocksize);
            showTimeAnalog(2,clocksize,center_x,center_y, 0.1, 0.6, hrs * 5 + (int)(mins * 5 / 60));
            showTimeAnalog(2,clocksize,center_x,center_y, 0.1, 0.9, mins);
            display2.display();
          }
  }
 
   //showTimeAnalog(1,clockSize,clockCenterX,clockCenterY, -0.8, 0.9, secs);
   
}

void OLED_Status(){
//#ifdef ESP32

  String MSGText;
  bool Analog;
  Analog=true;
  
  //Time display //There must be a better way to format this!!!
              if(secs>=10){
                 if(mins>=10){ MSGText="";MSGText+=hrs;MSGText+=":"; MSGText+=mins; MSGText+=":"; MSGText+=secs;}else{
                               MSGText="";MSGText+=hrs;MSGText+=":0"; MSGText+=mins; MSGText+=":"; MSGText+=secs;}
                          }else{
                 if(mins>=10){ MSGText="";MSGText+=hrs;MSGText+=":"; MSGText+=mins; MSGText+=":0"; MSGText+=secs;}else{
                               MSGText="";MSGText+=hrs;MSGText+=":0"; MSGText+=mins; MSGText+=":0"; MSGText+=secs;}
                               }

  
if (Display1Present){ 
   //display1.setFont(ArialMT_Plain_10);  
   display1.setTextAlignment(TEXT_ALIGN_LEFT); 
   offset=0;
   display1.setFont(Monospaced_plain_8);                 
   MSGText="RocNode:";MSGText+=Nickname;
   OLED_5_line_display(1,L1,L2,L3,L4,L5);
   display1.setTextAlignment(TEXT_ALIGN_CENTER);display1.setFont(ArialMT_Plain_10);offset=64;
   SignalStrengthBar(WiFi.RSSI()); 
   if (POWERON) {fillRect();}else{drawRect();}
   if (Analog){if ((L1[0]==0)&&(L2[0]==0)&&(L3[0]==0)&&(L4[0]==0)&&(L5[0]==0)){BigClock(1,30);  }else{ BigClock(1,10);  }
               }else{display1.drawString(64,0,MSGText);display1.display(); }
    }// adds time display 1
if (Display2Present){ 
   //display2.setFont(ArialMT_Plain_10);
   display2.setTextAlignment(TEXT_ALIGN_LEFT); 
   display2.setFont(Monospaced_plain_8);  
   offset=0;                       
   OLED_5_line_display(2,L6,L7,L8,L9,L10);
   display2.setTextAlignment(TEXT_ALIGN_CENTER);display2.setFont(ArialMT_Plain_10);offset=64;
   if (Analog){if ((L6[0]==0)&&(L7[0]==0)&&(L8[0]==0)&&(L9[0]==0)&&(L10[0]==0)){BigClock(2,30);  }else{ BigClock(2,10);  }
    }else{display2.drawString(64,0,MSGText); display2.display();}
   } // display 2
  
 
   

//#endif 
}

void TimeGears(){
if (secs>=60){
              secs=0;mins=mins+1;
                     if (mins>=60){
                         mins=0;hrs=hrs+1;
                                if (hrs>=25){
                                      hrs=1;
                                            }
                                   }
              }
}


















#endif

