#ifndef RocSubs_h
 #define RocSubs_h

uint8_t LastSetSpeed;
uint32_t PingSendTime;
boolean  PingReflected;




uint8_t Value_for_PortD[PortsRange]; //ignore [0] so we can have 1...8
uint8_t Pi02_Port_Settings_D[PortsRange];
uint8_t DelaySetting_for_PortD[PortsRange];
uint8_t Configuration_for_PortD[PortsRange];
uint8_t EventMarker_for_PortD[PortsRange];
uint8_t ID_High_for_PortD[PortsRange]; //idh
uint8_t ID_Low_for_PortD[PortsRange]; //idl
uint8_t XRef_Port[PortsRange];
uint8_t Pi03_Setting_offposH[PortsRange];
uint8_t Pi03_Setting_offposL[PortsRange];
uint8_t Pi03_Setting_onposH[PortsRange];
uint8_t Pi03_Setting_onposL[PortsRange];
uint8_t Pi03_Setting_offsteps[PortsRange];
uint8_t Pi03_Setting_onsteps[PortsRange];
uint8_t Pi03_Setting_options[PortsRange];
uint32_t Pi03_Setting_LastUpdated[PortsRange];


extern void SERVOS(void);
extern uint16_t servoLR(int state, int port);
extern int FlashHL(int state, int port);
extern bool IsServo(uint8_t i);
extern bool IsPWM(uint8_t i);
extern bool IsInput(uint8_t i);
extern bool PortInvert(uint8_t i);
extern int DCC_Speed_Demand;
extern int Last_DCC_Speed_Demand;
uint8_t DIRF = 0 ;
extern IPAddress mosquitto;
extern bool Display1Present,Display2Present;

#define Recipient_Addr  1   //use with SetWordIn_msg_loc_value(sendMessage,Recipient_Addr,data  , or get sender or recipient addr  
#define Sender_Addr 3       //use with SetWordIn_msg_loc_value(sendMessage,Sender_Addr,data   
#define Code_Request 0<<5
#define Code_Event   1<<5
#define Code_Reply   2<<5


uint8_t ROC_netid;
uint16_t ROC_recipient;
uint16_t ROC_sender;
uint8_t ROC_group;
uint8_t ROC_code;
uint8_t ROC_len;
uint8_t ROC_Data[200];
uint8_t ROC_OUT_DATA[200];
uint16_t RocNodeID;
uint16_t EEPROMRocNodeID;


extern uint32_t StartedAt;

#include "EEPROMDEFAULTS.h"
 
extern void SetDefaultSVs(); // moved to EEPROMDEFAULTS.h



//extern uint8_t Interpolate(uint8_t Input,uint8_t lowerout,uint8_t upperout,uint8_t lowerin,uint8_t upperin);
uint8_t Interpolate(uint8_t Input,uint8_t lowerout,uint8_t upperout,uint8_t lowerin,uint8_t upperin){
  int Result;  // use int for internal function maths preciscion
  uint8_t ResultInt; 
   if ((upperin-lowerin)!=0)// Check for div zero issue? //input is in range for interplolate checks are done after this 
              { 
              Result=1024*(upperout-lowerout);
              Result=(Input-lowerin)*Result;
              Result=Result/(upperin-lowerin);
              Result=Result/1024;
              Result=Result+lowerout;
              }else
              {
                // no actual range, so just use average of output demands
              Result=(lowerout+upperout)/2;
              }
  if (Input<=lowerin){Result=lowerout;}
  if (Input>=upperin){Result=upperout;}
 if (Result>=255){
    Result=255;
  }
  ResultInt=abs(Result);            
 
   #ifdef _PWM_DEBUG
       DebugSprintfMsgSend( sprintf ( DebugMsg, "Interpolate<%d> INRange(<%d>to<%d>) OUTrange(<%d> to<%d>) Result<%d>",Input,lowerin,upperin,lowerout,upperout,Result));
     #endif
 
  
return ResultInt;
}

uint8_t UseSpeedTablesorCV6(uint8_t Input){
  int MidThrottle,MaxThrottle,ReverseTrim,ForwardTrim;
  uint16_t Result;
  uint8_t Bin;
  uint8_t NumBins;
  NumBins=28;
  bool Dir,LightsOn,DirInvert,UseSpeedTable;
          Dir=bitRead(DIRF,5);
          LightsOn= bitRead(DIRF,4);
          DirInvert=bitRead(CV[29],0);
          UseSpeedTable=bitRead(CV[29],4);
      ReverseTrim=CV[95];
      ForwardTrim=CV[66];
           if (CV[95]>=127){ReverseTrim=128-CV[95];}// can use negatives
           if (CV[66]>=127){ForwardTrim=128-CV[66];}// use msb ss -ve sign
           if (CV[95]<=1){ReverseTrim=0;}
           if (CV[65]<=1){ForwardTrim=0;}
           
   
 // assume simple CV5/6 table to start 
  MaxThrottle=128;// assume 0- 128 range
  MidThrottle=MaxThrottle/2;  
  if (!UseSpeedTable){
  if (Input>=MidThrottle){Result= Interpolate(Input,CV[6],CV[5],MidThrottle,MaxThrottle);}
                             else{Result=Interpolate(Input,0,CV[6],0,MidThrottle);}
  }else{ // speed table from cvs
    Bin=abs(NumBins*Input/MaxThrottle);
    Result=Interpolate(Input,CV[Bin+67],CV[Bin+68],(Bin*MaxThrottle/NumBins),((Bin+1)*MaxThrottle/NumBins));
       }
  
  if (DirInvert^Dir) {Result = Result + ForwardTrim; }  //trims.. 
               else {Result =  Result + ReverseTrim; }
   if (abs(Input)>=1){
   Result=Result+CV[2];}            
  if (Result>=255) {Result=255;} 
  #ifdef _PWM_DEBUG
       DebugSprintfMsgSend( sprintf ( DebugMsg, "Speed TableInput<%d>  Result<%d>",Input,Result));
  #endif
 
               
  return Result;
}




 uint16_t Get_ROCNodeID(){
  return  RocNodeID;
 }
 
String NICKname(){
return Nickname;
}

void PrintEEPROMSettings(){
  //code to serial print eprom settings in a form that can be used in SetDefaultSVs() 
    
  Serial.println(F(" ------Current EEPROM Settings----------"));
    
  for (int i = 1; i <= NumberOfPorts; i++) {
  Serial.print(" Value_for_PortD[");
  Serial.print(i);
  Serial.print("]=");
  Serial.print(Value_for_PortD[i]);Serial.println(";");
  Serial.print(" Pi02_Port_Settings_D[");
  Serial.print(i);
  Serial.print("]=");
  Serial.print(Pi02_Port_Settings_D[i]);Serial.println(";");
  Serial.print(" DelaySetting_for_PortD[");
  Serial.print(i);
  Serial.print("]=");
  Serial.print(DelaySetting_for_PortD[i]);  Serial.println(";");  
  Serial.print(" Configuration_for_PortD[");
  Serial.print(i);
  Serial.print("]=");
  Serial.print(Configuration_for_PortD[i]);Serial.println(";");
  Serial.print(" EventMarker_for_PortD[");
  Serial.print(i);
  Serial.print("]=");
  Serial.print(EventMarker_for_PortD[i]);Serial.println(";");
  Serial.print(" ID_High_for_PortD[");
  Serial.print(i);
  Serial.print("]=");
  Serial.print(ID_High_for_PortD[i]);Serial.println(";");
  Serial.print(" ID_Low_for_PortD[");
  Serial.print(i);
  Serial.print("]=");
  Serial.print(ID_Low_for_PortD[i]);Serial.println(";");
  Serial.print(" XRef_Port[");
  Serial.print(i);
  Serial.print("]=");
  Serial.print(XRef_Port[i]);Serial.println(";");
  Serial.print(" Pi03_Setting_offposH[");
  Serial.print(i);
  Serial.print("]=");
  Serial.print(Pi03_Setting_offposH[i]);Serial.println(";");
  Serial.print(" Pi03_Setting_offposL[");
  Serial.print(i);
  Serial.print("]=");
  Serial.print(Pi03_Setting_offposL[i]);Serial.println(";");
  Serial.print(" Pi03_Setting_onposH[");
  Serial.print(i);
  Serial.print("]=");
  Serial.print(Pi03_Setting_onposH[i]);Serial.println(";");
  Serial.print(" Pi03_Setting_onposL[");
  Serial.print(i);
  Serial.print("]=");
  Serial.print(Pi03_Setting_onposL[i]);Serial.println(";");
  Serial.print(" Pi03_Setting_offsteps[");
  Serial.print(i);
  Serial.print("]=");
  Serial.print(Pi03_Setting_offsteps[i]);Serial.println(";");
  Serial.print(" Pi03_Setting_onsteps[");
  Serial.print(i);
  Serial.print("]=");
  Serial.print(Pi03_Setting_onsteps[i]);Serial.println(";");
  Serial.print(" Pi03_Setting_options[");
  Serial.print(i);
  Serial.print("]=");
  Serial.print(Pi03_Setting_options[i]);Serial.println(";");
  Serial.print(" Pi03_Setting_LastUpdated[");
  Serial.print(i);
  Serial.print("]=");
  Serial.print(Pi03_Setting_LastUpdated[i]);Serial.println(";");
    }
  Serial.println(" //RN and CV settings");
  for (int i = 0; i <= 35 ; i++) {
      Serial.print(" RN[");
      Serial.print(i);
      Serial.print("]=");
      Serial.print(RN[i]);Serial.println(";");
     
      }
      for (int i = 0; i <= 30 ; i++) {
      Serial.print(" RNm[");
      Serial.print(i);
      Serial.print("]=");
      Serial.print(RNm[i]);Serial.println(";");
     
      }
  for (int i = 0; i <= 160 ; i++) {
      Serial.print(" CV[");
      Serial.print(i);
      Serial.print("]=");
      Serial.print(CV[i]);Serial.println(";");
      }
    
  Serial.println("//end of defaults ");

}


void WriteEEPROM(void) {
   #ifdef _DefaultPrintOut 
     PrintEEPROMSettings();
  
    #endif

//write all variables across to RN[index's] for eeprom 
    for (int i = 0; i <= 160 ; i++) {
     EEPROM.write(i, CV[i]);
     }
 //   0 to 160 are cvs
for (int i = 1; i <= 35 ; i++) {
  EEPROM.write(i+180, RN[i]);
    }  // 180 to 215 are RN
    for (int i = 1; i <= 30 ; i++) {
  EEPROM.write(i+220, RNm[i]);
    }  // 220 to 225 are RNm
    // space from 226 to 320 is free  
  for (int i = 0; i <= NumberOfPorts; i++) { //rn 314 for 8 ports  594 for 16
    EEPROM.write(320 + (i*35), Value_for_PortD[i]);
    EEPROM.write(320 + (i*35),Value_for_PortD[i]);
    EEPROM.write(321 + (i*35),Pi02_Port_Settings_D[i]);
    EEPROM.write(322 + (i*35),DelaySetting_for_PortD[i]);
    EEPROM.write(323 + (i*35),Configuration_for_PortD[i]);
    EEPROM.write(324 + (i*35),EventMarker_for_PortD[i]);
    EEPROM.write(325 + (i*35),ID_High_for_PortD[i]); //idh
    EEPROM.write(326 + (i*35),ID_Low_for_PortD[i]); //idl
    EEPROM.write(327 + (i*35),XRef_Port[i]);

    EEPROM.write(328 + (i*35),Pi03_Setting_offposH[i]);
    EEPROM.write(329 + (i*35),Pi03_Setting_offposL[i]);
    EEPROM.write(330 + (i*35),Pi03_Setting_onposH[i]);
    EEPROM.write(331 + (i*35),Pi03_Setting_onposL[i]);
    EEPROM.write(332 + (i*35),Pi03_Setting_offsteps[i]);
    EEPROM.write(333 + (i*35),Pi03_Setting_onsteps[i]);
    EEPROM.write(334 + (i*35),Pi03_Setting_options[i]);
  }
   Serial.println(" ");Serial.println(RocNodeID);
    SetWordIn_msg_loc_value(RN, 1, RocNodeID);

    Serial.println("--");//
    Serial.println(F(" EEProm writing (but not committed yet--)"));//
    
  
     

    
    // add the serial writes 
    writeString(ssidEEPROMLocation,wifiSSID);
    writeString(passwordEEPROMLocation,wifiPassword);
    Serial.print(" Broker Addr");Serial.println(BrokerAddr);
    EEPROM.write(BrokerEEPROMLocation,BrokerAddr);
    EEPROM.write(RocNodeIDLocation,RN[1]);
    EEPROM.write(RocNodeIDLocation+1,RN[2]);
    Serial.print(" RocNodeID:");Serial.println( (EEPROM.read(RocNodeIDLocation+1)*256)+ EEPROM.read(RocNodeIDLocation) );
    delay(100); // or EEPROMRocNodeID
}

void ReadEEPROM() {
 // eprom has: 0-256 ==cv's
 for (int i = 0; i <= 160 ; i++) {
     CV[i] = EEPROM.read(i);
     }
 // for (int i = 1; i <= (35 + (NumberOfPorts*35)) ; i++) {
 //   RN[i] = EEPROM.read(i+300); // so, =EEPROM.read(300+i)
   // add the serial reads 
 // }
  
  for (int i = 1; i <= 35 ; i++) { //preparatory to reducing size of RN array. 
    RN[i] = EEPROM.read(i+180); // 
   // add the serial reaads 
  }
  for (int i = 1; i <= 30 ; i++) { //preparatory to reducing size of RN array. 
    RNm[i] = EEPROM.read(i+220); // 
   // add the serial reaads 
  }
   
   //RN 1(lo),2hi is node address 
    RocNodeID= getTwoBytesFromMessageHL(RN, 1);
      #ifdef _ForceRocnetNodeID_to_subIPL
       RocNodeID=subIPL;
      #endif
      if (RocNodeID==0){RocNodeID=3;}
   //Nickname, has length "RN[3]" followed by name ESP is default
   //Nickname[32] so max should be RN3+32 =35
       for (int i = 1 ;  i <= 30; i++) { Nickname[i] =char(0);}   
          //Serial.println("--------------------------------------");
          //Serial.print(" EEPROM Read: This Node Nickname is:");
          for (int i = 1 ;  i <= RN[3]; i++) {
                    Nickname[i-1] = char(RN[3 + i]);
                    }
          Nickname[RN[3]]=char(0);         //add null to end string nickname, length RN[3] ESP is default
          //Serial.println("'");
          //Serial.print("NICKNAME BUILT IS:");
          //Serial.println(Nickname);
    
  // eeprom 320 -> is saved port stuff
  
  
  for (int i = 1; i <= NumberOfPorts; i++) {
    Value_for_PortD[i] = EEPROM.read(320 + (i*35));
    Pi02_Port_Settings_D[i] = EEPROM.read(321 + (i*35));
    DelaySetting_for_PortD[i] = EEPROM.read(322 + (i*35));
    Configuration_for_PortD[i] = EEPROM.read(323 + (i*35));
    EventMarker_for_PortD[i] = EEPROM.read(324 + (i*35));
    ID_High_for_PortD[i] = EEPROM.read(325 + (i*35)); //idh
    ID_Low_for_PortD[i] = EEPROM.read(326 + (i*35)); //idl
    XRef_Port[i] = EEPROM.read(327 + (i*35));

    Pi03_Setting_offposH[i] = EEPROM.read(328 + (i*35));
    Pi03_Setting_offposL[i] = EEPROM.read(329 + (i*35));
    Pi03_Setting_onposH[i] = EEPROM.read(330 + (i*35));
    Pi03_Setting_onposL[i] = EEPROM.read(331 + (i*35));
    Pi03_Setting_offsteps[i] = EEPROM.read(332 + (i*35));
    Pi03_Setting_onsteps[i] = EEPROM.read(333 + (i*35));
    Pi03_Setting_options[i] = EEPROM.read(334 + (i*35));
  }
  
  wifiSSID=read_String(ssidEEPROMLocation);
  EEPROMRocNodeID=(EEPROM.read(RocNodeIDLocation+1)*256)+EEPROM.read(RocNodeIDLocation);  // low then hi Plan to move to this from RocNodeID, or use it as backup
  wifiPassword=read_String(passwordEEPROMLocation);
  BrokerAddr=EEPROM.read(BrokerEEPROMLocation);
   Serial.print(" Broker Addr:");Serial.println(BrokerAddr);
   Serial.print(" RocNodeID:");Serial.println(RocNodeID);
   Serial.print(" Copy of RocNodeID:");Serial.println(EEPROMRocNodeID);
}


void ROCSerialPrint(uint8_t *msg)   {
  Serial.print("NetId  RidH  RidL   SidH  SidL  Grp   Code  Len");
  for (byte i = 1; i <= (msg[7]); i++) {
    Serial.print("    D");
    Serial.print(i);
  }
  Serial.println();
  for (byte i = 0; i <= (7 + msg[7]); i++) {
    Serial.print(" ");
    dump_byte(msg[i]);
  }
  Serial.println();
}


char*  Show_ROC_MSG() {

  if (Message_Length >= 1) {
    DebugMessage[0] = 0;
    strcat(DebugMessage, " NetID:");
    snprintf(DebugMessage, sizeof(DebugMessage), "%s%d", DebugMessage, ROC_netid);
    strcat(DebugMessage, " Rec:"); snprintf(DebugMessage, sizeof(DebugMessage), "%s%d", DebugMessage, ROC_recipient);
    strcat(DebugMessage, " Sdr:"); snprintf(DebugMessage, sizeof(DebugMessage), "%s%d", DebugMessage, ROC_sender);
    strcat(DebugMessage, " Grp:"); snprintf(DebugMessage, sizeof(DebugMessage), "%s%d", DebugMessage, ROC_group);
    strcat(DebugMessage, " Code[");
    if ((ROC_code & 0x60) == 0) {
      strcat(DebugMessage, "Req]:");
    }
    if ((ROC_code & 0x60) == 0x20) {
      strcat(DebugMessage, "Evt]:");
    }
    if ((ROC_code & 0x60) == 0x40) {
      strcat(DebugMessage, "Rpy]:"); ////add request event reply then code.. (5 bits)
    }
    snprintf(DebugMessage, sizeof(DebugMessage), "%s%d", DebugMessage, (ROC_code & 0x1F));
    for (byte i = 1; i <= ROC_len; i++) {
      strcat(DebugMessage, " D"); snprintf(DebugMessage, sizeof(DebugMessage), "%s%d", DebugMessage, i);
      strcat(DebugMessage, "="); snprintf(DebugMessage, sizeof(DebugMessage), "%s%d", DebugMessage, (ROC_Data[i]));
    }

    //Serial.print(DebugMessage);
  } return DebugMessage;
}

void Show_ROC_MSGS(uint8_t *payload) {
  ROC_netid = sendMessage[0];
  ROC_recipient = IntFromPacket_at_Addr(sendMessage, Recipient_Addr);
  ROC_sender = IntFromPacket_at_Addr(sendMessage, Sender_Addr);
  ROC_group = sendMessage[5];
  ROC_code = sendMessage[6];
  ROC_len = sendMessage[7];
  for (byte i = 1; i <= ROC_len; i++) {
    ROC_Data[7 + i];
  }
  Message_Length = ROC_len + 7;
  Serial.print(Show_ROC_MSG());
}

//extern void DoLocoMotor(void);
extern void SetMotorSpeed(int SpeedDemand_local,uint8_t dirf);
extern void  ImmediateStop(void);
extern void WriteAnalogtoPort(uint8_t port,uint16_t demand);


void ROC_CS() { //group 1
  uint16_t CVNum;
  uint8_t OldValue;
  switch (ROC_code) {
    case 0:  {}    //NOP
      break;
    case 2:  {
        POWERON = ROC_Data[1];
        Serial.println();
        Serial.print(" Power set to:");
        Serial.println(POWERON);
        sendMessage[8] = 0x00; //clear before doing anything later .....
        sendMessage[0] = ROC_netid;
        SetWordIn_msg_loc_value(sendMessage, Recipient_Addr, 0x00 ); //response is to host, not cs
        SetWordIn_msg_loc_value(sendMessage, Sender_Addr, RocNodeID ); //sent from the rocnode ?
        sendMessage[5] = ROC_group;
        sendMessage[6] = ROC_code | Code_Reply; //action group, response
        sendMessage[7] = 1;             //len of data coming next
        bitWrite(sendMessage[8], 1, 1); //set 'Idle' ?
        bitWrite(sendMessage[8], 0, POWERON); //set bit 0 in status to power state
        //MQTTSend("rocnet/cs",sendMessage);        //sends status response something wrong with this
        //ROCSerialPrint(sendMessage);
        Message_Decoded = true;
        if (POWERON == false) {
                DebugSprintfMsgSend(sprintf(DebugMsg," NODE OFF "));
                 ImmediateStop();
                  }
        if (POWERON == true) {
                DebugSprintfMsgSend(sprintf(DebugMsg," NODE ON "));
                Last_DCC_Speed_Demand=0;
                SetMotorSpeed(Last_DCC_Speed_Demand,DIRF);
                //DoLocoMotor();
                  }
      }
      break;
    case 8:  {
        //Serial.print(" debug**Group 1, Code 8: ROC_Data[1]<");Serial.print(ROC_Data[1]);Serial.print(" >  ROC_Data[2]<");Serial.print(ROC_Data[2]); Serial.print(">  equates to address of:"); Serial.println ((ROC_Data[2] + (ROC_Data[1] * 256)));
  #ifdef _LOCO_SERVO_Driven_Port      // do not do these cv response unless this is a LOCO 
  
       // Is this address to me?
       if (((ROC_Data[2] + (ROC_Data[1] * 256)) == MyLocoAddr) || ((ROC_Data[2] == 0) && (ROC_Data[1] == 0))) {   
     
     #ifdef _SERIAL_DEBUG  
          if ((ROC_Data[2] + (ROC_Data[1] * 256)) == MyLocoAddr) { Serial.print(" Group 1, Code 8: CV change for this Loco ");
                                                                  }else { Serial.print(" Group 1, Code 8: CV change for 0 0  ");}
     #endif                                                           
          CVNum = ((ROC_Data[3] * 256) + ROC_Data[4]);
          OldValue = CV[CVNum]; //save the old value
          Message_Decoded = true; //we understand these even if they are not for us
          if (ROC_Data[6] == 1) { //this is a SET CV
             if (CVNum == 8) {    //set defaults or other things when CV8=13
                             if ((ROC_Data[5] == 8)) {// CV[8]==8 set to 8 - Triggers EEPROM defaults
                                  Serial.print("Setting Defaults");
                                  DebugSprintfMsgSend(sprintf(DebugMsg,"CV[8] set to 8 - Triggers EEPROM defaults"));
                                  SetDefaultSVs();
                                  if (millis()>=StartedAt+1000){// but do not do this if we have just started as it may have been just a re-send from mosquitto
                                      Data_Updated = true;
                                      WriteEEPROM();
                                      EPROM_Write_Delay = millis() + 500; }
                                                      }
                                                      
                                if ((ROC_Data[5] == 13)) { // // CV[8]==13 set to 13 triggers sending settings for recording or replacing those in the default code here
                                  PrintEEPROMSettings();
                                                      }
                                if ((ROC_Data[5] == 100)) { // CV[8] set to 100 Spare trigger for tests 
                                 
                                                      }
                                                      
                                                      
                            }
            else {   //Setting, but not special settings using CV[8] Y set if address is explicitly for me
              if ((ROC_Data[2] + (ROC_Data[1] * 256)) == MyLocoAddr) { //only set if address is explicitly for me
                   //mqtt debug message
                   
                   DebugSprintfMsgSend( sprintf ( DebugMsg, "Set CV[%d]=%d   ",CVNum,ROC_Data[5]) );
              CV[CVNum] = ROC_Data[5]; //set the new data
              if (OldValue != ROC_Data[5]) {
                     if (millis()>=StartedAt+1000) {  // Use started at Timer  TO STOP EEPROM UPDATE if the mqtt sends a (repeated) valid set command on node connecting
                                          Data_Updated = true;  
                                          WriteEEPROM();     
                                          EPROM_Write_Delay = millis() + Ten_Sec; //update the time so you can press the same set and get another ten seconds delay
                                                                        }
                                           }
                                                                      }
                 }
            

          } //set end

          if (ROC_Data[6] == 0) { //get
            CVNum = ((ROC_Data[3] * 256) + ROC_Data[4]);
            DebugSprintfMsgSend( sprintf ( DebugMsg, "Get CV[%d]=%d",CVNum,CV[CVNum]));
          } //get end

          //Serial.println("    Building and sending response"); Send response for both GET and SET...
          //----------------SEND CV Starts-----------------
          sendMessage[0] = ROC_netid;
          SetWordIn_msg_loc_value(sendMessage, Recipient_Addr, 0x00 ); //response is to host, not cs
          SetWordIn_msg_loc_value(sendMessage, Sender_Addr, MyLocoAddr ); //??sent from the loco not rocnodeid??
          sendMessage[5] = ROC_group;
          sendMessage[6] = ROC_code | Code_Reply; //action group, response
          sendMessage[7] = 6;   //len of data coming next
          SetWordIn_msg_loc_value(sendMessage, 8, MyLocoAddr); //set 8 and 9 with which loco I am
          SetWordIn_msg_loc_value(sendMessage, 10, CVNum); //set 10 and 11 with the CV number
          sendMessage[12] = CV[CVNum];     //the CV value
          sendMessage[13] = 1;  //set
          
         #ifdef _showRocMSGS
            Serial.print("Response:");
            Show_ROC_MSGS(sendMessage);
         #endif
          
          MQTTSend("rocnet/ht", sendMessage);  //HOST not cs??
          //------------SEND CV ENDS------------
          //Serial.println("------------ double check message content------------");
          //ROCSerialPrint(sendMessage);
          //Serial.println("-------------- end double check --------------");


        }  //end of cv stuff  for this loco or 00
        
     #endif //  _LOCO_SERVO_Driven_Port   // add #ifdef to match #ifdef around "if" at the beginning else {}do not match!
        Message_Decoded = true;
      } //end of this case
      break;


  }
} // end of roc cs 
extern bool ButtonState[PortsRange] ;
extern bool LastDebounceButtonState[PortsRange]; //Number of ports +2
extern bool LastSentButtonState[PortsRange]; //Number of ports +2
extern void SetSoundEffect(uint8_t Data1,uint8_t Data2,uint8_t Data3);
extern void BeginPlay(int Channel,const char *wavfilename, uint8_t Volume);
extern void SetMotorSpeed(int SpeedDemand,uint8_t dirf);

void ROC_MOBILE() { //group 2
  uint8_t AdjustedSpeedDemand; 
  switch (ROC_code) {
    case 0:  {}    //NOP
      break;
    case 1:  {}    //setup
      break;
    case 2:  {   //set Velocity, direction , lights
        // Serial.print("Local:");  Serial.print(CV[1]); Serial.print(" MSG for:");  Serial.print(ROC_recipient);
        //set Velocity, direction , lights
        Message_Decoded = true; //we understand these even if they are not for us
#ifdef _LOCO_SERVO_Driven_Port
        if (ROC_recipient == MyLocoAddr) { //data for me, do it!
                                            //ROC_Data[1] is speed demand
          bitWrite(DIRF, 5, ROC_Data[2]);   // set direction and lights
          bitWrite(DIRF, 4, ROC_Data[3]);
          // Modifying output Moved all speed tables stuff to here 
          DebugSprintfMsgSend( sprintf ( DebugMsg, " Set Speed<%d> Dir<%d> Lights<%d>",ROC_Data[1], bitRead(DIRF,5),bitRead(DIRF,4)));  
          AdjustedSpeedDemand=UseSpeedTablesorCV6(ROC_Data[1]);
          //DebugSprintfMsgSend( sprintf ( DebugMsg, "Roc Set Speed<%d> Table_adjusted_speed<%d> ", ROC_Data[1],AdjustedSpeedDemand)); 
         
          SetMotorSpeed(AdjustedSpeedDemand,DIRF);
          //SetMotorSpeed(ROC_Data[1],DIRF);old                          
          }
#endif
             }    //set Velocity, direction , lights
      break;
    case 3:  {
        Message_Decoded = true; //we understand these 
        if (ROC_recipient == MyLocoAddr) {     //for me, do it!
          //Serial.print(" Function change for :");  
          //Serial.print(ROC_recipient); Serial.print(" data :"); 
    #ifdef _Audio          
    #ifdef _LOCO_SERVO_Driven_Port              
         SetSoundEffect(ROC_Data[1],ROC_Data[2],ROC_Data[3]); //Move settings to SetSoundEffect 
    #endif     
/*               //ROC_Data[1];    //F1-F8   
                 //ROC_Data[2];    //F9-F16
                 //ROC_Data[3];    //F17-F24
*/        
#endif
        
        }
            }    //end case 3 functions
      break;
    case 4:  {}    //query
      break;
    case 5:  {}    //fieldcmd
      break;
  }
}
void ROC_CLOCK() {
  hrs = ROC_Data[5];
  mins = ROC_Data[6];
  secs = 1;
  divider=ROC_Data[8];// Roc_data[7] is temperature
  //PrintTime("Time synch \n");
  //bad idea, to have lots of things transmitting immediately after synch.. 
  //test with delay based on subIPL  
  delay(subIPL);
  if (POWERON) {
  DebugSprintfMsgSend( sprintf ( DebugMsg, " IPaddr .%d  Time Synchronised. Power is ON",subIPL));
  }
  else{  DebugSprintfMsgSend( sprintf ( DebugMsg, " IPaddr .%d  Time Synchronised. Power is OFF",subIPL));
  }
    Message_Decoded = true;
  //set / synch clock
}



void ROC_NODE() { //stationary decoders GROUP 3
  uint8_t TEMP;

  switch (ROC_code) {
  uint8_t NodeClass;  
  
    case 8:  {  //Identify         class manuID  versionH  versionL  nr I/O  subipH  subipL //vendor @ class I/O revison 
        Message_Decoded = true; //we understand these even if they are not for us
        if ( (ROC_recipient ==   RocNodeID) || (    ROC_recipient ==   0)) {   //Serial.println();
          Serial.print("Responding to IDENTIFY. This node is:");
          Serial.print(RocNodeID) ;
          sendMessage[0] = ROC_netid;
          SetWordIn_msg_loc_value(sendMessage, Recipient_Addr, ROC_sender);
          SetWordIn_msg_loc_value(sendMessage, Sender_Addr, RocNodeID);
          sendMessage[5] = 3;
          sendMessage[6] = 8 + 32; //action group and code bit 6 (32) = set for REPLY
          sendMessage[7] = 7 + RN[3]; //len of data coming next + nicname length
          if (RN[3] >= 1) {
            Serial.print(" Nickname :");
            Serial.print(RN[3]);
            Serial.print(" chars :'");
          }
          //Identify... data is:  class manuID  versionH  versionL  nr I/O  subipH  subipL
          NodeClass= 0x01; //class? = io?"bit 0= accessory" bit 1= dcc Bit 3=RFID FF= Accessory DCC RFID
          #ifdef _LOCO_SERVO_Driven_Port
                  NodeClass= 0x02;
          #endif
          
          sendMessage[8] = NodeClass; 
          sendMessage[9] = 13; //manuid
          SetWordIn_msg_loc_value(sendMessage, 10, SW_REV);
          sendMessage[12] = NumberOfPorts; //8 io seems fixed in rocrail?
          sendMessage[13] = subIPH; //sub IPh
          sendMessage[14] = subIPL; //sub IPl
          for (int i = 1 ;  i <= RN[3]; i++) {
            Serial.print(char(RN[3 + i]));
            sendMessage[14 + i] = RN[3 + i];
          } //nickname, length RN[3] ESP is default
          Serial.println("'");
          //delay(subIPL*2); //prevent simultaneous responses to identify query
          MQTTSend("rocnet/dc", sendMessage);
          delay(100); //leave plenty of time before sending next mqtt 
          DebugSprintfMsgSend( sprintf ( DebugMsg, "Responding to Identify "));
          //ROCSerialPrint(sendMessage);
        }
        Message_Decoded = true;
      }    //Identify... data is:  class manuID  versionH  versionL  nr I/O  subipH  subipL
      break;
    case 9:  {
        Message_Decoded = true; //we understand these even if they are not for us
        if ( (ROC_recipient ==   RocNodeID) || (    ROC_recipient ==   0)) {
          if ( (ROC_recipient ==   RocNodeID) ){   //ONLY IF ITS actually me.. Do not switch off for generic 0 message.
                DebugSprintfMsgSend( sprintf ( DebugMsg, "Grp 3 Code 9 Shutting Node <%d> power off ",ROC_recipient));
                POWERON = false;}
          else {  DebugSprintfMsgSend( sprintf ( DebugMsg, "Grp 3 Code 9 for <%d> power off IGNORED. ",ROC_recipient)); }
        }
        Message_Decoded = true;
      }    //(Stationary )  Node Shutdown
      break;
    case 10:  {
        Message_Decoded = true; //we understand these even if they are not for us//Acknowledge
        if ( (ROC_recipient ==   RocNodeID) || (    ROC_recipient ==   0)) {
          Serial.print("ACKnowledging action:");       //action and port in D1, D2
          Serial.print(ROC_Data[1]);
          if (ROC_len >= 2) {
            Serial.print(" port:");
            Serial.print(ROC_Data[2]);
          }
          Serial.println(  );
          //ROCSerialPrint(recMessage);

        }
        Message_Decoded = true;
      }
      break;
    case 11:  {  //SHOW
        Message_Decoded = true; //we understand these even if they are not for us
        if ( (ROC_recipient ==   RocNodeID) || (    ROC_recipient ==   0)) {
          FlashMessage("RocView Requests SHOW", 10, 500, 100);  //hold the LED on for 800ms, off for 800ms, 6 times!
          }

        Message_Decoded = true;
      }
      break;


  }//end of case

}//end rocnode
extern int SDemand[PortsRange]; //Number of ports +2

void ROC_Programming() { //GROUP  5
  bool Data_Changed;
  switch (ROC_code) {
    case 4:  { //read port config
        Message_Decoded = true; //we understand these even if they are not for us
        if ( (ROC_recipient ==   RocNodeID) || (    ROC_recipient ==   0)) {
          int TEMP;
          int port;
          
         
          DebugSprintfMsgSend(sprintf(DebugMsg,"Multi byte READ Port# value, type delay :"));
//debugmsg

          sendMessage[0] = ROC_netid;
          SetWordIn_msg_loc_value(sendMessage, Recipient_Addr, ROC_sender );
          SetWordIn_msg_loc_value(sendMessage, Sender_Addr, RocNodeID);
          sendMessage[5] = ROC_group;
          sendMessage[6] = ROC_code | Code_Reply; //action group and code bit 6 (64) = set for REPLY
          sendMessage[7] = (((ROC_Data[2] - ROC_Data[1]) + 1) * 4); //len of data coming next from port to port..
          TEMP = 0;
          for (int i = 1 ;  i <= ((((ROC_Data[2] - ROC_Data[1]) + 1)) * 4); i = i + 4) { //port# value type  delay
            port = ROC_Data[1] + TEMP;
            sendMessage[7 + i] = port; //port# value type  delay
            sendMessage[7 + i + 1] = Value_for_PortD[port]; //value=
            sendMessage[7 + i + 2] = Pi02_Port_Settings_D[port]; //type = switch
            sendMessage[7 + i + 3] = DelaySetting_for_PortD[port]; //delay
            //Serial.print(port);
            TEMP = TEMP + 1;
          }
          MQTTSend("rocnet/ps", sendMessage);
          
          //ROCSerialPrint(sendMessage);
        }      Message_Decoded = true;
      }
      break;

    case 5: {
        Message_Decoded = true; //we understand these even if they are not for us
        if ( (ROC_recipient ==   RocNodeID) || (    ROC_recipient ==   0)) {
        
          DebugSprintfMsgSend(sprintf(DebugMsg,"Multi byte WRITE port, value type delay"));
//debugmsg
          for (byte i = 1; i <= ROC_len; i = i + 4) {
            //Serial.print(" Port:");
            //Serial.print(ROC_Data[i]);
            //Serial.print("] Value= :");
            //Serial.print(ROC_Data[i+1]);
            //Serial.print(" type= :");
            //Serial.print(ROC_Data[i+2]);
            //Serial.print(" Delay= :");
            //Serial.print(ROC_Data[i+3]);
            Value_for_PortD[ROC_Data[i]] = ROC_Data[i + 1]; //value=
            Pi02_Port_Settings_D[ROC_Data[i]] = ROC_Data[i + 2]; //type =
            DelaySetting_for_PortD[ROC_Data[i]] = ROC_Data[i + 3]; //delay
          }
          //Serial.println();
          if (millis()>=StartedAt+1000){
                Data_Updated = true;
                WriteEEPROM();
                EPROM_Write_Delay = millis() + Ten_Sec;}
        }
        Message_Decoded = true;
      }
      break;

    case 6:  {
        Message_Decoded = true; //we understand these even if they are not for us
        if ( (ROC_recipient ==   RocNodeID) || (    ROC_recipient ==   0)) {
          if ((ROC_Data[3] == subIPH) && (ROC_Data[4] == subIPL)) {
        Serial.print(F("Programming node:  set RocNet ID from:"));
        RocNodeID= getTwoBytesFromMessageHL(RN, 1);
        // eepromrocnetnode set here soon!
        Serial.print (RocNodeID);
        Data_Changed=false;
            RocNodeID= ((ROC_Data[1] << 8) + ROC_Data[2]);
            #ifdef _ForceRocnetNodeID_to_subIPL
              RocNodeID=subIPL;
              Serial.print(" Node ID forced to subIPL");
            #endif
            
            if (getTwoBytesFromMessageHL(RN, 1)!= RocNodeID){
            Serial.print(F("Programming node:  set RocNet ID to:"));
            SetWordIn_msg_loc_value(RN, 1, RocNodeID); //set RN 1 and 2
            Serial.print (RocNodeID);
            Data_Changed=true;}
            else {Serial.print(F("Programming node: Unchanged ID"));}
            //ROCSerialPrint(recMessage);


            if ( ROC_len >= 6) { //set nickname
              Serial.print(F("Programming nickname ?"));
              Serial.print( ROC_len - 5); //new, uses rn3 as length
              Serial.print(" bytes ");
              if (RN[3]!= ROC_len - 5){Data_Changed=true;}
              RN[3] = ROC_len - 5;
              for (int p = 1; ((p <= RN[3]) && (p <= 25)); p++) { // p is max size of name 
                if (RN[3+p]!= ROC_Data[p + 5]){Data_Changed=true;}
                Serial.write(ROC_Data[p + 5]);
                RN[3 + p] = ROC_Data[p + 5];
                                    }

                }
            Serial.println(" ");
            if (Data_Changed){
           if (millis()>=StartedAt+1000){
                Data_Updated = true;
                EPROM_Write_Delay = millis() + 100; // write quickly as we have changed something important!!
                WriteEEPROM();} 
            
          }
          else {Serial.print("Unchanged ID and nickname");}
          }
        }
        Message_Decoded = true;
      }
      break;
    case 7:  {  //report addr and status
        Message_Decoded = true; //we understand these even if they are not for us
        if ( (ROC_recipient ==   RocNodeID) || (    ROC_recipient ==   0)) {

           DebugSprintfMsgSend(sprintf(DebugMsg,"Reporting Node Addr and Status"));
          //ROCSerialPrint(recMessage);

/* 
 NEW: 
 Reply Data 1   2     3       4        5               6              7                8             9               10             11             12              13 
 RN[15         16    17       18        19             20             21               22            23              24             15              26              27      
 iotype       flags cstype  csdevice  I2C scan 0×20 H I2C scan 0×20 L I2C scan 0×30 H I2C scan 0×30 I2C scan 0×40 H I2C scan 0×40 L adc threshold I2C scan 0×50 H I2C scan 0×50 L
*/
RNm[15]=33;  //iotype not a pi, i2c-0
RNm[16]=0; //0= no options set 
RNm[17]=0; //cstype: 0=none, 1=dcc232, 2=sprog
RNm[18]=0; //csdevice: 0=/dev/ttyUSB0, 1=/dev/ttyUSB1 2= /dev/ttyACMO 3 gives error in radiobox in rocview..
RNm[19]=0;  //020H  //Pi o2's
RNm[20]=1; //020 l
RNm[21]=0; //030 H //Pi04's? / here is 0x38
RNm[22]=0;
if (Display1Present){bitSet(RNm[22],0);} //030 L ?? 
if (Display2Present){bitSet(RNm[22],1);}//not showing
RNm[23]=0; //040 H  //Pi03's
RNm[24]=1; //040 l
RNm[25]=0; //adc thresh
RNm[26]=0; //050 H (ROC DISPLAYS)
RNm[27]=0; //050 L  set 1 for "0x50" 3 for 0x50,0x51 etc..NOT USED BY my code as standard displays are 0x3C 0x3D


          sendMessage[0] = ROC_netid;
          SetWordIn_msg_loc_value(sendMessage, Recipient_Addr, ROC_sender);
          SetWordIn_msg_loc_value(sendMessage, Sender_Addr, RocNodeID);
          sendMessage[5] = ROC_group;
          sendMessage[6] = ROC_code | Code_Reply; //action group and code bit 6 (64) = set for REPLY
          sendMessage[7] = 13; //6 sends to RN20
         for (int i = 1 ;  i <= (sendMessage[7]); i = i + 1) { 
          sendMessage[7+i] = RNm[14+i];
         }
      
          MQTTSend("rocnet/ps", sendMessage);
         //DebugSprintfMsgSend(sprintf(DebugMsg," sending (line 1159)"));
         //ROCSerialPrint(sendMessage);

        }
        Message_Decoded = true;
      }

      break;
    case 8: {//set rocnode options etc..
        Message_Decoded = true; //we understand these even if they are not for us
        if ( (ROC_recipient ==   RocNodeID) || (    ROC_recipient ==   0)) {

          //if ((recMessage[10]==subIPH) && (recMessage[11]==subIPL)){
         
          DebugSprintfMsgSend(sprintf(DebugMsg,"Programming node options"));
          RNm[15] = ROC_Data[1];
          RNm[16] = ROC_Data[2];
          RNm[17] = ROC_Data[3];
          RNm[18] = ROC_Data[4];
          RNm[19] = ROC_Data[5]; //ROC_Data[5] is minimal length of time a sensor will report occupied.

         if (millis()>=StartedAt+1000){ 
                EPROM_Write_Delay = millis() + 500; //not ten sec, as we have all the data now..
                    Data_Updated = true;
                    WriteEEPROM();}
         
        }
        Message_Decoded = true;

      }
    case 9:  { //??Shutdown
        Message_Decoded = true; //we understand these even if they are not for us
        if ( (ROC_recipient ==   RocNodeID) || (    ROC_recipient ==   0)) {
           //this is used by the MACRO settings... I do not  use this
        
          DebugSprintfMsgSend(sprintf(DebugMsg,"Group5 Case 9 : Node  on/off set to :"));
          
          Serial.print("Data[1]:");
          Serial.print(ROC_Data[1]);
          Serial.print("  Data[2]:");
          Serial.println(ROC_Data[2]);
        
          //ROCSerialPrint(recMessage);
        }
        Message_Decoded = true;
      }

      break;
    case 11: {
        Message_Decoded = true; //we understand these even if they are not for us
        if ( (ROC_recipient ==   RocNodeID) || (    ROC_recipient ==   0)) {
           DebugSprintfMsgSend (sprintf ( DebugMsg, "UPDATE FROM ROCRAIL Value <%d> <%d>",ROC_Data[1],ROC_Data[2]));
           if ( (ROC_Data[1] == 0) && (ROC_Data[2] == 1)) {
                DebugSprintfMsgSend(sprintf ( DebugMsg, "** RE-SETTING all to Defaults--"));
                SetDefaultSVs();
                CV[8] = 0x0D; //DIY MFR code
                CV[7] = SW_REV; //ver
                CV[1]=3; //set initial loco address as 3 (regardless of whatever  the set defaults function says)
                #ifdef  _Force_Loco_Addr
                 CV[1]= _Force_Loco_Addr;
                #endif
                SetWordIn_msg_loc_value(RN,1,subIPL); //sets initial RocNode address as the subip to prevent clashes
                WriteEEPROM();
                Data_Updated = true;
                EPROM_Write_Delay = millis() + 1000;
                delay(100);
          }  
          if ( (ROC_Data[1] == 0) && (ROC_Data[2] == 0)) {
            Serial.println(F(" .. will reset SSID settings"));
            ResetWiFi = true;
          } 
          if ( (ROC_Data[1] == 1) && (ROC_Data[2] == 0)) { //256
            Serial.println(F(" .. will Send all settings for records"));
            PrintEEPROMSettings();
          }        
        }
        Message_Decoded = true;
      }

    case 12:  {
        Message_Decoded = true; //we understand these even if they are not for us
        if ( (ROC_recipient ==   RocNodeID) || (    ROC_recipient ==   0)) {
          int TEMP;
          int port;

          DebugSprintfMsgSend(sprintf(DebugMsg," Responding to Pi 02 Get data"));

          sendMessage[0] = ROC_netid;
          SetWordIn_msg_loc_value(sendMessage, Recipient_Addr, ROC_sender );
          SetWordIn_msg_loc_value(sendMessage, Sender_Addr, RocNodeID);
          sendMessage[5] = ROC_group;
          sendMessage[6] = ROC_code | Code_Reply; //action group and code bit 6 (64) = set for REPLY
          sendMessage[7] = (((ROC_Data[2] - ROC_Data[1]) + 1) * 4); //len of data coming next
          TEMP = 0;
          for (int i = 1 ;  i <= (((ROC_Data[2] - ROC_Data[1]) + 1) * 4); i = i + 4) { //port# value type  delay
            port = ROC_Data[1] + TEMP;
            sendMessage[7 + i] = port; //port# value type  delay
            sendMessage[7 + i + 1] = ID_High_for_PortD[port]; //idh
            sendMessage[7 + i + 2] = ID_Low_for_PortD[port]; //idl
            sendMessage[7 + i + 3] = XRef_Port[port]; //port
            //Serial.print(port);
            TEMP = TEMP + 1;
          }
          MQTTSend("rocnet/ps", sendMessage);
          
          //ROCSerialPrint(sendMessage);
        } Message_Decoded = true;
      }
      break;
    case 13: {
        Message_Decoded = true; //we understand these even if they are not for us
        if ( (ROC_recipient ==   RocNodeID) || (    ROC_recipient ==   0)) {

      
         DebugSprintfMsgSend(sprintf(DebugMsg," Responding to Pi 02 Set data  ") );
          for (byte i = 1; i <= ROC_len; i = i + 4) {
            //Serial.print(" Port:");
            //Serial.print(ROC_Data[i]);
            //Serial.print("] Value= :");
            //Serial.print(ROC_Data[i+1]);
            //Serial.print(" type= :");
            //Serial.print(ROC_Data[i+2]);
            //Serial.print(" Delay= :");
            //Serial.print(ROC_Data[i+3]);
            ID_High_for_PortD[ROC_Data[i]] = ROC_Data[i + 1]; //value=
            ID_Low_for_PortD[ROC_Data[i]] = ROC_Data[i + 2]; //type =
            XRef_Port[ROC_Data[i]] = ROC_Data[i + 3]; //delay
          }
          //Serial.println();
         if (millis()>=StartedAt+1000){
                EPROM_Write_Delay = millis() + 500; //not ten sec, as we have all the data now..
                Data_Updated = true;
                WriteEEPROM();}
          
        }
        Message_Decoded = true;
      }

      break;
    case 15:  {
        Message_Decoded = true; //we understand these even if they are not for us
        if ( (ROC_recipient ==   RocNodeID) || (    ROC_recipient ==   0)) {

          int TEMP;
          int port;
          


          PrintTime (" Multi byte READ Channel data ");
  DebugSprintfMsgSend(sprintf(DebugMsg," Responding to Pi 03 Get data "));
          sendMessage[0] = ROC_netid;
          SetWordIn_msg_loc_value(sendMessage, Recipient_Addr, ROC_sender);
          SetWordIn_msg_loc_value(sendMessage, Sender_Addr, RocNodeID);
          sendMessage[5] = ROC_group;
          sendMessage[6] = ROC_code | Code_Reply; //action group and code bit 6 (64) = set for REPLY
          sendMessage[7] = (((ROC_Data[2] - ROC_Data[1]) + 1) * 8); //len of data coming next
          //Serial.print(sendMessage[7]);
          //Serial.print(" Bytes ");
          port = ROC_Data[1];
          for (int i = 1 ;  i <= ((((ROC_Data[2] - ROC_Data[1])) + 1) * 8); i = i + 8) { //port# value type  delay
            if ((ROC_Data[1] >= 1) && (ROC_Data[1] <= NumberOfPorts)){ // Added new check for port range  is compatible with node range of available ports
              sendMessage[7 + i] = port; //port# value type  delay
              sendMessage[7 + i + 1] = Pi03_Setting_offposH[port];
              sendMessage[7 + i + 2] = Pi03_Setting_offposL[port];
              sendMessage[7 + i + 3] = Pi03_Setting_onposH[port];
              sendMessage[7 + i + 4] = Pi03_Setting_onposL[port];
              sendMessage[7 + i + 5] = Pi03_Setting_offsteps[port];
              sendMessage[7 + i + 6] = Pi03_Setting_onsteps[port];
              sendMessage[7 + i + 7] = Pi03_Setting_options[port];
               Serial.print(" Ch:");
               Serial.print(port);
               //Serial.print(" starting at:");
               //Serial.print(7 + i);   
               }else{// this is outside our range
              sendMessage[7 + i] = port; //port# value type  delay
              sendMessage[7 + i + 1] = 0;
              sendMessage[7 + i + 2] = 0;
              sendMessage[7 + i + 3] = 0;
              sendMessage[7 + i + 4] = 0;
              sendMessage[7 + i + 5] = 0;
              sendMessage[7 + i + 6] = 0;
              sendMessage[7 + i + 7] = 0;
               Serial.print("Outside Node range ");
                    }
            
            port = port + 1;
          }
          MQTTSend("rocnet/ps", sendMessage);
          Serial.println(" end");
          //ROCSerialPrint(sendMessage);
        }   Message_Decoded = true;
      }

      break;
    case 16: {
        Message_Decoded = true; //we understand these even if they are not for us
        if ( (ROC_recipient ==   RocNodeID) || (    ROC_recipient ==   0)) {
          //channel# offposH offposL onposH  onposL  offsteps  onsteps options
          DebugSprintfMsgSend(sprintf(DebugMsg," Responding to Pi 03 Set data"  ));
          for (byte i = 1; i <= ROC_len; i = i + 8) {
            //Serial.print(" Port:");
            //Serial.print(ROC_Data[i]);
            //Serial.print("] Value= :");
            //Serial.print(ROC_Data[i+1]);
            //Serial.print(" type= :");
            //Serial.print(ROC_Data[i+2]);
            //Serial.print(" Delay= :");
            //Serial.print(ROC_Data[i+3]);
            Pi03_Setting_offposH[ROC_Data[i]] = ROC_Data[i + 1];
            Pi03_Setting_offposL[ROC_Data[i]] = ROC_Data[i + 2];
            Pi03_Setting_onposH[ROC_Data[i]] = ROC_Data[i + 3];
            Pi03_Setting_onposL[ROC_Data[i]] = ROC_Data[i + 4];
            Pi03_Setting_offsteps[ROC_Data[i]] = ROC_Data[i + 5];
            Pi03_Setting_onsteps[ROC_Data[i]] = ROC_Data[i + 6];
            Pi03_Setting_options[ROC_Data[i]] = ROC_Data[i + 7];


          }
          if (millis()>=StartedAt+1000){
                EPROM_Write_Delay = millis() + 500; //not ten sec, as we have all the data now..
                Data_Updated = true;
                WriteEEPROM();}
          
        }
        Message_Decoded = true;
      }

      break;
    case 18: { //set channel#  valueH  valueL (Pi03 settings)
        Message_Decoded = true; //we understand these even if they are not for us
        if ( (ROC_recipient ==   RocNodeID) || (    ROC_recipient ==   0)) {
          uint16_t DesiredPos;

         // PrintTime( " Single Channel WRITE Ch:"); //channel# valueH  valueL
         // Serial.print(ROC_Data[1]);
          
          if (ROC_Data[4] == 0) {
          //  Serial.print(" updating Left position");
            Pi03_Setting_offposH[ROC_Data[1]] = ROC_Data[1 + 1];
            Pi03_Setting_offposL[ROC_Data[1]] = ROC_Data[1 + 2];
          }    //uses 150-600 so need to change it before using with servo
          if (ROC_Data[4] == 1) {
          //  Serial.print(" updating Right position");
            Pi03_Setting_onposH[ROC_Data[1]] = ROC_Data[1 + 1];
            Pi03_Setting_onposL[ROC_Data[1]] = ROC_Data[1 + 2];
          }      

          
   //ROCRAIL DesiredPos uses 150-600 so need to change it before using with servo 
          if ((Pi03_Setting_options[ROC_Data[1]] & 32)==32) { ///SERVO....To address a channel instead of a port the port type servo must be set on the interface tab of switches and outputs
             DesiredPos = (ROC_Data[1 + 1] * 256) + ROC_Data[1 + 2];
             SDemand[ROC_Data[1]] = ((DesiredPos - 150) * 2) / 5;  //why??  because it sets "demand" (in degrees) to set the servo position for test
             if (ROC_Data[4] == 1) { DebugSprintfMsgSend( sprintf ( DebugMsg, "Setting Servo[%d] Right pos to:%d  (=%d degrees)",ROC_Data[1],DesiredPos,SDemand[ROC_Data[1]]));}
                               else{ DebugSprintfMsgSend( sprintf ( DebugMsg, "Setting Servo[%d] Left pos to:%d  (=%d degrees)",ROC_Data[1],DesiredPos,SDemand[ROC_Data[1]]));}
             SERVOS();                                        } //set the servo immediately range 150-600 (rocrail limmits) = 0 to 180 so we can see the movement                                                }          
   
   
   //if PWM output   //ROCRAIL DesiredPos sets 0-4095 range,,   Arduino PWM range is 0-1023, 
           if (IsPWM(ROC_Data[1])) { //set pwm immediately, arduino range is 0-1023, rocrail has 0-4095 range
            DebugSprintfMsgSend( sprintf ( DebugMsg, " Setting PWM to :[%d]",DesiredPos/4));
            WriteAnalogtoPort(ROC_Data[1], DesiredPos/4);   //change to the desired pwm.
                                                                 } 
         if (millis()>=StartedAt+1000){
              Data_Updated = true;
              EPROM_Write_Delay = millis() + 30000; //30 sec, not ten sec, as we may be wanting to tweak it when we see the movement..
          }
          
        }
        Message_Decoded = true;
      }
      break;

  }//end code switch
}//end group 5

extern uint32_t PortTimingDelay[PortsRange]; //Number of ports +2
#ifdef _Audio
extern bool PlayingSoundEffect;
#endif
void ROC_Outputs() { //group 9
     bool OKtoWrite;
      OKtoWrite=true;  // added to prevent direct writing to loco motor ports if used
  switch (ROC_code) {
    case 1: {
        Message_Decoded = true; //we understand these even if they are not for us
        if ( (ROC_recipient ==   RocNodeID) ) {
          boolean STATE; 
          STATE = ROC_Data[1];
          // IF THIS is FOR THE LOCO MOTOR PORTS, DO NOT SET THEM!
              #ifdef _LOCO_SERVO_Driven_Port
                      if (ROC_Data[4]==_LOCO_SERVO_Driven_Port){OKtoWrite=false;}
              #ifdef _LocoPWMDirPort
                      if (ROC_Data[4]==_LocoPWMDirPort){OKtoWrite=false;}
              #endif
             #endif 
       
        if ((ROC_Data[4]>= 1) && (ROC_Data[4]<= NumberOfPorts)&& (OKtoWrite)){ //in our available port range? 
          if (!IsInput(ROC_Data[4])) {           //is this port an output, check  first make sure its an output!
                 ButtonState[ROC_Data[4]] = STATE;
                  if (PortInvert(ROC_Data[4]))   {  //invert ?
                      STATE = !STATE;  } //invert state
                                                       
            if (IsServo(ROC_Data[4])) {   //SERVO?....To address a channel instead of a port the port type servo must be set on the interface tab of switches and outputs
                  ButtonState[ROC_Data[4]] = STATE;
                  SDemand[ROC_Data[4]] = servoLR(STATE, ROC_Data[4]);//just setting sdemand allows "servos" to drive the servo to the desired position 
                  if (SDemand[ROC_Data[4]] >= 180) {  //set limits 
                                   SDemand[ROC_Data[4]] = 180;
                                              }
                  if (SDemand[ROC_Data[4]] <= 0) {    //set limit
                                   SDemand[ROC_Data[4]] = 0;
                                              }
                  Pi03_Setting_LastUpdated[ROC_Data[4]] = millis();  
                  
                  if (OKtoWrite)   {    DebugSprintfMsgSend( sprintf ( DebugMsg, "Setting Output %d (SERVO) to State(%d) = Position:%d ",ROC_Data[4],STATE,SDemand[ROC_Data[4]])); 
                                   }   
               }//   End of servo                                         
          else {   //not servo Could be PWM or digital 
               if (IsPWM(ROC_Data[4])) {//is PWM outputs as settings for on and off channel positions
                          PortTimingDelay[ROC_Data[4]] = millis() + (DelaySetting_for_PortD[ROC_Data[4]] * 10);                                  
                          WriteAnalogtoPort(ROC_Data[4], FlashHL(STATE, ROC_Data[4]));   //set pwm, arduino range is 0-1023, rocrail has 0-4095 range
                          SDemand[ROC_Data[4]] = FlashHL(STATE, ROC_Data[4]); //save  what we have set for flashing 
                          if((Pi02_Port_Settings_D[ROC_Data[4]] & 129) == 128){  //flashing
                              DebugSprintfMsgSend( sprintf ( DebugMsg, "Setting Output FLASHING %d  to %d", 
                                                      ROC_Data[4], SDemand[ROC_Data[4]]));}
                          else { // not flashing
                                if ((ROC_Data[4]==DAC25is)||(ROC_Data[4]==DAC26is)){
                                     DebugSprintfMsgSend( sprintf ( DebugMsg, "Setting Output %d (Analog) to %.1f V", ROC_Data[4], 
                                      ((3.3*SDemand[ROC_Data[4]])/1024)));}                          //volts 1023==3.3
                                  else{
                                      DebugSprintfMsgSend( sprintf ( DebugMsg, "Setting Output %d (PWM) to %d%%", ROC_Data[4], 
                                      100*SDemand[ROC_Data[4]]/1024));}                          //set pwm, arduino range is 0-1023, rocrail has 0-4095 range
                                } // sent PWM demands
                  }   //end PWM, must be digtal      
                       //this not servo not pwm, so is a pure digital output
                       if (!(IsPWM(ROC_Data[4]))){
                          if((Pi02_Port_Settings_D[ROC_Data[4]] & 129) == 128){
                                    DebugSprintfMsgSend( sprintf ( DebugMsg, "Setting Output FLASHING D%d (GPIO)%d (Digital) to %d",ROC_Data[4],NodeMCUPinD[ROC_Data[4]], STATE));}
                                    else{    DebugSprintfMsgSend( sprintf ( DebugMsg, "Setting Output D%d (GPIO)%d (Digital) to %d",ROC_Data[4],NodeMCUPinD[ROC_Data[4]], STATE));}
                          digitalWrite (NodeMCUPinD[ROC_Data[4]], STATE); 
                          PortTimingDelay[ROC_Data[4]] = millis() + (DelaySetting_for_PortD[ROC_Data[4]] * 10);
                          SDemand[ROC_Data[4]] = servoLR(STATE, ROC_Data[4]);  //use sdemand to save state so we can flash
                       }
                }        

        } // End of is output
        } //a node we understand
    else {  // New bit for Sounds has own check for nodes avilability
      #if defined (_Audio) && !defined (_LOCO_SERVO_Driven_Port)  // add ability to play 8 sound effects by triggering switches but only for stationary nodes, not locos
      if (STATE &&!PlayingSoundEffect){
        int SFXNum;
        char Filename[31]; Filename[0]='\0';
        if ((ROC_Data[4]>=100) &&(ROC_Data[4]<=110)) {  //ADDRESS RANGE 100 on  are the special addresses  FOR SOUNDS! F1.wav to F8.wav 
          SFXNum=ROC_Data[4]-100; //  101 is sfx 1 triggers F1 etc.. 
          if (SFXNum<=1){SFXNum=1;}//set range of SFX available
          if (SFXNum>=10){SFXNum=10;}//set range
          sprintf (Filename,"/F%d.wav",SFXNum);
          BeginPlay(1,Filename,120);//Play at a set volume until I work out a way to set volumes through rocrail 
          //DebugSprintfMsgSend( sprintf ( DebugMsg, "Rd1%d Rd2%d Rd3%d Rd4%d Rd5%d Playing %s ",ROC_Data[1],ROC_Data[2],ROC_Data[3],ROC_Data[4],ROC_Data[5],Filename));
          // Message_Decoded = false;
        }          }
      #endif
      }// sounds 
   } //recipient = node
   } //end case 1
      break;


  }//end of switch code

}//end of ROC_Outputs




void SendPortChange(int RNID, boolean ST, uint8_t i) {
  Serial.print(" I/O change, Node :"); Serial.print(RNID);  Serial.print(" Sends io:");  Serial.print (i);  Serial.print(" is now:"); Serial.print(ST); Serial.println(" ");
  //new format send
  sendMessage[0] = ROC_netid;
  sendMessage[1] = 0x00;
  sendMessage[2] = 0x01; //recipient is the rocrail server
  SetWordIn_msg_loc_value(sendMessage, Sender_Addr, RNID);
  sendMessage[5] = 8;
  sendMessage[6] = 1; //action group and code
  sendMessage[7] = 4;   //len of data coming next
  sendMessage[8] = 0x00;
  sendMessage[9] = MyLocoAddr; //reporting loco address?
  sendMessage[10] = ST;             sendMessage[11] = i; //port
  MQTTSend("rocnet/sr", sendMessage);
  DebugSprintfMsgSend(sprintf ( DebugMsg, "Sensor change Seen Address:%d State:%d", i, ST));
  

}



extern void OLED_Displays_Setup(uint8_t ADDR,int Display,String Message);
void ROC_DISPLAY() { //display Group 12 rocdisplay
  Message_Decoded = false;
  uint8_t Address;
  uint8_t Disp;
  char Message[127]; // big to accept long msg from rocrail
  if  ((ROC_recipient ==   RocNodeID)||(ROC_recipient ==  0)){ // node 0 is global message
      Address= ROC_Data[1];
      Disp= ROC_Data[2];
   //Serial.println("Display  message for this node (or global)");Serial.print(" Display Addr:");Serial.print(Address);
   // Serial.print("> Display No {1-8}:");Serial.print(ROC_Data[2]);Serial.print("> len:");Serial.print(ROC_len);
   //Serial.print(" Text<"); // we have a display at 0x3c (60d )as standard
    if (ROC_len-3>=126){ROC_len==128;}
    for (byte i = 0; i <= (ROC_len-3); i++) { 
                     Message[i]=char (ROC_Data[i+3]); 
  //                   Serial.print(char(ROC_Data[i+3]));
                     }
  //  #ifdef TerminusDisplay 
    OLED_Displays_Setup(Address,Disp,Message);
  //  #endif
    
   // Serial.println(">");
   } // is display message
   Message_Decoded = true;
 }
      

void DoRocNet() {
  if (RocNodeID == IntFromPacket_at_Addr(sendMessage, Sender_Addr)){
        Message_Decoded = true;} //this is a reflected a message we originally sent
  if (Message_Length >= 1) { //have we recieved data?
    switch (ROC_group) {
      case 0:  {}    //{Host
        break;
      case 1:  {
          ROC_CS(); //Command Station Command Station rocnet/cs
        }
        break;
      case 2:  {
          ROC_MOBILE();
        }    //Locomotives and functions rocnet/lc
        break;
      case 3:  {
          ROC_NODE(); //Stationary decoders  Multiport for inputs and outputs  rocnet/dc
        }
        break;
      case 4:  {}    //Programming mobile DCC CVs rocnet/pm
        break;
      case 5:  {
          ROC_Programming(); //Programming stationary Including command stations  rocnet/ps
        }
        break;
      case 6:  {}    //GP Data transfer  General purpose data transfer between modules
        break;
      case 7:  {
          ROC_CLOCK(); //Clock Fast clock  rocnet/ck
        }
        break;
      case 8:  {
          Message_Decoded = true; //Position determination  rocnet/sr  //do not want to see these just now
        }
        break;
      case 9:  {
          ROC_Outputs(); //Outputs  rocnet/ot
        }
        break;
      case 10:  {}    //Input
        break;
      case 11:  {}    //Sound
        break;
      case 12:  {ROC_DISPLAY();}    //Display
        break;
      default:  {}    //Default!!
        break;
    }
  }     //end message length decode
#if defined (_showRocMSGS) || defined (_SERIAL_MQTT_DEBUG)  
  if   ((!Message_Decoded) && (Message_Length >= 1) ) {
    DebugSprintfMsgSend(sprintf(DebugMsg," Message not understood.. see serial output"));
     }  //print / send things not decoded if debugs are on
   if   ((!Message_Decoded)&& (Message_Length >= 1 )) {
      Serial.print(" MSG Not Understood<"); Serial.print(Show_ROC_MSG()); Serial.println(">");
   }
#endif
  Message_Length = 0; //reset !
}
#endif
