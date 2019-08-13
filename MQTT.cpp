#include "MQTT.h"
#include <Arduino.h> //needed 
//#include "Directives.h"


extern uint16_t Message_Length;
extern boolean Message_Decoded;
extern uint8_t CV[200];
extern uint8_t RN[36];
extern uint8_t RNm[35]; 
extern char Nickname[35];
extern uint8_t hrs;
extern uint8_t mins;
extern uint8_t secs;  
extern uint8_t divider;
extern char DebugMsg[127];
extern uint8_t NodeMCUPinD[18]; // hard fixed //Number of ports +2
extern int connects;
extern boolean  Data_Updated;
extern void SignOfLifeFlash(bool state);

extern uint32_t EPROM_Write_Delay;
extern uint32_t Ten_Sec;

extern uint32_t MsgSendTime;
extern boolean MSGReflected;
extern uint8_t SentMessage[128];
extern uint8_t SentMessage_Length;
extern String SentTopic;
// globals used 

#include <PubSubClient.h>
#include <WiFiClient.h>
WiFiClient espClient;
PubSubClient client(espClient);
//if using Really Small Message Broker and not Mosquitto, you will need to use V 3_1 not 3_1_1
//so change PubSubClient. h line 19 to define '3_1' like this: 
//#define MQTT_VERSION MQTT_VERSION_3_1  ///needed for rsmb  


extern IPAddress mosquitto;


extern uint16_t RocNodeID;
extern void PrintTime(String MSG) ;
extern void WriteEEPROM(void);
extern uint8_t ip0;
extern uint8_t ip1;
extern uint8_t subIPH;
extern uint8_t subIPL;


extern void copyUid (byte *buffOut, byte *buffIn, byte bufferSize) ;
extern bool compareUid(byte *buffer1, byte *buffer2, byte bufferSize) ;
extern void dump_byte_array(byte* buffer, int bufferSize) ;

extern uint32_t PingSendTime;
extern boolean  PingReflected;



extern uint8_t ROC_netid;
extern uint16_t ROC_recipient;
extern uint16_t ROC_sender;
extern uint8_t ROC_group;
extern uint8_t ROC_code;
extern uint8_t ROC_len;
extern uint8_t ROC_Data[260];
#define Recipient_Addr  1   //use with SetWordIn_msg_loc_value(sendMessage,Recipient_Addr,data  , or get sender or recipient addr  
#define Sender_Addr 3       //use with SetWordIn_msg_loc_value(sendMessage,Sender_Addr,data   
extern int IntFromPacket_at_Addr(uint8_t* msg, uint8_t highbyte);
//uint8_t ROC_OUT_DATA[200];
//uint16_t RocNodeID;




void MQTTFetch (char* topic, byte* payload, unsigned int Length) { 
  //do a check on length matching payload[7] ??

  if ((strncmp("PiNg", topic, 4) == 0)) {
    Message_Length = 0; Message_Decoded = true;         //do not bother to do work on this, its a copy of what I sent
    if (RocNodeID == ((payload[0] << 8) + payload[1])) {
      PingReflected = true;
    }
    return;
  }
  if ((strncmp("rocnet/ht", topic, 9) == 0)) {
    Message_Length = 0;
    Message_Decoded = true;
    return;
  }

  Message_Length = Length;
  if (Message_Length >= 1) {

    SignOfLifeFlash( HIGH) ;  ///turn On
    ROC_netid = payload[0];
    ROC_recipient = IntFromPacket_at_Addr(payload, Recipient_Addr);
    ROC_sender = IntFromPacket_at_Addr(payload, Sender_Addr);
    ROC_group = payload[5];
    ROC_code = payload[6];
    ROC_len = payload[7];
    for (int i = 1; i <= ROC_len; i++) {
      ROC_Data[i] = payload[7 + i];
    }
    Message_Decoded = false;
    #ifdef _SERIAL_MQTT_DEBUG
    Serial.println();
      Serial.print("*MQTT Received [");
      Serial.print(topic);
      Serial.print("] Payload Length[");
      Serial.print(ROC_len);
      Serial.print("] ");
      dump_byte_array(payload, Message_Length);
    #endif

    if ((SentTopic == topic) && (compareUid(SentMessage, payload, Message_Length))) {
      MSGReflected = true;
      DebugSprintfMsgSend( sprintf ( DebugMsg, "*Sensor Confirmed Address:%d state:%d", ROC_Data[4], ROC_Data[3]));
      
    }
  }
}                                                 //*Sensor Seen Node:%d Address:%d state:%d

void MQTTSendQ1(String topic, uint8_t * payload) { //QoS1 version
  uint8_t Length;
  SignOfLifeFlash( HIGH) ;  ///turn On
  Length = payload[7] + 8;
  MsgSendTime = millis();
  MSGReflected = false;
  copyUid(SentMessage, payload, 128); //nb  dest, source,
  SentMessage_Length = Length;
  SentTopic=topic;
  //strncpy(SentTopic, topic, 20);     //nb dest, source
  #ifdef _SERIAL_MQTT_DEBUG
    Serial.print("*QOS 0 POST with check for 'reflection'[");
    Serial.print(topic);
    Serial.print("] ");
    dump_byte_array(payload, Length);
    Serial.println();
  #endif
  client.publish(topic.c_str(), payload, Length); //send as qos 0..for now
}
void testConnection  (int Number) {
  SignOfLifeFlash( HIGH) ;  ///turn On
  PingSendTime = millis();
  PingReflected = false;
  byte id[2];
  id[0] = Number >> 8;
  id[1] = Number & 0xFF;
  client.publish("PiNg", id, 2); //,strlen(payload));//send as qos 0..
}

void MQTTSend (String topic, uint8_t * payload) { //replaces rocsend
  uint8_t Length;
  SignOfLifeFlash( HIGH) ;  ///turn On
  Length = payload[7] + 8;

#ifdef _SERIAL_MQTT_DEBUG

    Serial.println();
    Serial.print("*MQTT POST [");
    Serial.print(topic);
    Serial.print("] ");
    dump_byte_array(payload, Length);
    Serial.println();

#endif

  client.publish(topic.c_str(), payload, Length);

}


//++++++++++ MQTT setup stuff   +++++++++++++++++++++
void MQTT_Setup(void){
  
  client.setServer(mosquitto, 1883);   //Hard set port at 1833
  client.setCallback(MQTTFetch);
}


boolean MQTT_Connected(void){
  return client.connected();
}

void MQTT_Loop(void){
    client.loop(); //gets wifi messages etc..
}
extern uint16_t MyLocoAddr ;
extern int32_t SigStrength();
extern bool DebugMsgCleared;
extern uint32_t TimeToClearDebugMessage;
extern uint32_t StartedAt;
extern bool CheckWiFiConnected(void);
extern bool _HaveSent_Connected_Debug_Msg;

void DebugMsgSend (String topic, char* payload, bool Print) { //use with mosquitto_sub -h 127.0.0.1 -i "CMD_Prompt" -t debug -q 0
  char DebugMsgLocal[200];
  char DebugMsgTemp[200];
  int cx;
  int32_t Signal;
  bool retained;
  retained=true;
  Signal=SigStrength();
  
//  if (payload==""){client.publish(topic.c_str(),"",false);Serial.println("Clearing DebugMsg"); return; }
  // add node type specific data and subIPL addr whilst hrs and secs are not synchronised.
  #ifdef _LOCO_SERVO_Driven_Port
                                   cx= sprintf ( DebugMsgTemp, " RN:%d Sig(%ddB) Loco:%d(%s) <%s>",RocNodeID,Signal,  MyLocoAddr,Nickname, payload);
  if ( (hrs==0)&&(mins==0) ) {cx= sprintf ( DebugMsgTemp, "ip:%d RN:%d Sig(%ddB) Loco:%d(%s) <%s>",subIPL,RocNodeID,Signal,  MyLocoAddr,Nickname, payload);
  #else
  
       if (Signal >= MinWiFi) {cx= sprintf ( DebugMsgTemp, "RN:%d Sig(%ddB)(%s) %s",RocNodeID,Signal,  Nickname, payload);}
          else { cx= sprintf ( DebugMsgTemp, "RN:%d LOW Sig(%ddB)(%s) %s",RocNodeID,Signal,  Nickname, payload);}
       if ( (hrs==0)&&(mins==0) ) {if (Signal >= MinWiFi){cx= sprintf ( DebugMsgTemp, "ip:%d RN:%d Sig(%ddB)(%s) %s",subIPL,RocNodeID,Signal,  Nickname, payload);}
         else {cx= sprintf ( DebugMsgTemp, "ip:%d RN:%d LOW Sig(%ddB)(%s) %s",subIPL,RocNodeID,Signal,  Nickname, payload);}}
  #endif
  

   //add timestamp to outgoing message 
   if ((hrs==0)&&(mins==0)){//not Synchronised yet..
                                  cx=sprintf(DebugMsgLocal,"(%dms from start) %s",(millis()-StartedAt),DebugMsgTemp);}
                            else {cx=sprintf(DebugMsgLocal,"<%02d:%02d:%02ds> %s",hrs,mins,secs,DebugMsgTemp);}
                                
        
    //Serial.printf("\n *Debug Message:%s Msg Length:%d \n",DebugMsgLocal,cx);
   if (Print){ Serial.printf("\n*Debug Msg %s  \n",DebugMsgTemp);}
             else{cx= sprintf ( DebugMsgLocal, "RN:%d %s",RocNodeID,payload);  }  // shortened intro for debug msg on no print version                                                                     }

    if ((cx >= 120) && (strlen(payload) <= 100)) {
                   cx= sprintf ( DebugMsgLocal, "MSG-%s-", payload);
                       }//print just msg  line
    if (strlen(payload) >= 101) {
                   cx= sprintf ( DebugMsgLocal, "Node:%d Loco:%d Time %d:%d:%ds Msg TOO Big to print", RocNodeID, MyLocoAddr, hrs, mins, secs);
                       }
    // all ends with actually sending the message!
    if (MQTT_Connected()&& CheckWiFiConnected()) { // && _HaveSent_Connected_Debug_Msg ) { //_HaveSent_Connected_Debug_Msg was a test... 
        client.publish(topic.c_str(), DebugMsgLocal,retained); }//v26 seems not needed, but sensible to add check WiFi connected and so is MQTT??..// retained or not?// added at V17bb
        if (!Print){delay(1000);}// slow down for the Port Setup (the only place Print=false is used) to allow time for message propagation 
        TimeToClearDebugMessage=millis()+5000; // not used at present, but can be switched on in main loop
        DebugMsgCleared=false;
  }

void DebugMsgClear(void){  // attempt to prevent repeated transmissions of last sent message by Broker
  String topic;
  topic="debug";
  client.publish(topic.c_str(),"",true);
  delay(5);
  DebugMsgCleared=true;  
}

void DebugSprintfMsgSend(int CX){ //allows use of Sprintf function in the "cx" location
  DebugMsgSend ("debug", DebugMsg, true);
  delay(5);
}

void DebugSprintfMsgNoprint(int CX){ //allows use of Sprintf function in the "cx" location
  DebugMsgSend ("debug", DebugMsg, false);
  }

extern bool ScanForBroker;
extern int BrokerAddr;
extern void WriteWiFiSettings();
extern uint8_t SW_REV;
extern void OLED_4_RN_displays(int OLed_x,String L1,String L2,String L3,String L4);
extern void SetFont(uint8_t Disp,uint8_t Font);

extern void StringToChar(char *line, String input);
extern void FlashMessage (String msg, int Repeats, int ON, int Off);
extern void OLEDS_Display(String L1,String L2,String L3,String L4);

uint32_t TimeLimit;
void reconnect(void) {
  bool ConnectedNow; int cx;
  char ClientName[80];
  char myName[15] = "RocNetESPNode:";
  String MSGText1, MSGText2;
  sprintf(ClientName, "%s%i", myName, RocNodeID);// use sprint to build ClientName
  
  if (CheckWiFiConnected()){Serial.printf("--%s Connected at Address:%d.%d.%d.%d --\n  --Looking for MQTT at %d --\n",ClientName,ip0,ip1,subIPH,subIPL,mosquitto[3]);}
  
  if (!MQTT_Connected()) { // not connected to MQTT so need to do stuff
      SignOfLifeFlash( SignalON) ; ///turn on
      MQTT_Setup();   //Set any changes in MQTT mosquitto address ?
      MSGText1=" <";MSGText1+=mosquitto[0];MSGText1+=".";MSGText1+=mosquitto[1];MSGText1+=".";MSGText1+=mosquitto[2];MSGText1+=".";MSGText1+=mosquitto[3];MSGText1+=">";
      MSGText2="code <Ver:";MSGText2+=SW_REV;MSGText2+="> "; 
      Serial.print("MQTT trying ");Serial.println(MSGText1);
      
      if (mosquitto[3] != BrokerAddr ){ Serial.printf("%s updating Broker Addr was %d. to %d\n",ClientName,BrokerAddr,mosquitto[3]);}
      
      cx=sprintf( DebugMsg, "Trying :%d.%d.%d.%d",mosquitto[0],mosquitto[1],mosquitto[2],mosquitto[3]);
       
      ConnectedNow=client.connect(ClientName);//Attempt to connect   takes about 15 secs per unsucessful check (set in MQTT_SOCKET_TIMEOUT PubSubClient.h

      if (ConnectedNow) {// I want this Connected message before the flash message
            Serial.print("WiRocS ");Serial.print(MSGText2);Serial.print("MQTT Connected");Serial.println(MSGText1);}
            else{FlashMessage(DebugMsg, 2, 100, 100);} // flash "trying" mmessage in OLED (also sends to Serial port and mqtt debug)
      
      if (ConnectedNow) {
            // Serial.print("WiRocS ");Serial.print(MSGText2);Serial.print("MQTT Connected");Serial.println(MSGText1);
            connects=0;
            if (mosquitto[3] != BrokerAddr ){   //BrokerAddr is the MQQT broker address, save if changed
                   BrokerAddr=mosquitto[3];
                   WriteWiFiSettings();
                   }
            //can advise this node is connected now:
            Serial.printf ( DebugMsg, "%s Connected at:%d.%d.%d.%d Found MQTT at %d",ClientName,ip0,ip1,subIPH,subIPL,mosquitto[3]);
            cx=sprintf( DebugMsg, "IP:%d Broker OK :%d.%d.%d.%d",subIPL,mosquitto[0],mosquitto[1],mosquitto[2],mosquitto[3]);
            FlashMessage(DebugMsg, 4, 500, 100);  //Flash message sends to oled displays (and sends a MQTT debugmsg !!) 
                 
            //... and now subscribe to topics  http://wiki.rocrail.net/doku.php?id=rocnet:rocnet-prot-en#groups
            client.subscribe("rocnet/lc", 1 ); //loco
            client.subscribe("rocnet/#", 0);   //everything else
            //client.subscribe("PiNg", 0);  //my ping...for my qos 1 attempt
            /*   or do it individually.......
            client.subscribe("rocnet/dc",0);
            client.subscribe("rocnet/cs",0);
            client.subscribe("rocnet/ps",0);
            client.subscribe("rocnet/ot",1);
            client.subscribe("rocnet/sr",0); //to allow reflection check of my sensor events
            */
            delay(10); // time to stabilise everything?
            } 
               else { // not connected? try another address
                     connects=connects+1;
                     if ((connects>=5) && ScanForBroker && CheckWiFiConnected()) {
                      if (SigStrength()>=MinWiFi){  // added tests for connected to wifi / strength. Only increment broker addr if connected with good (better than (set in Directives) -85db)  signal
                          mosquitto[3]=mosquitto[3]+1; 
                          Serial.println(" Incrementing MQTT addresses ");
                          #ifdef myBrokerSubip 
                            mosquitto[3]= BrokerAddr  //change to force set  BrokerAddrDefault as your broker last ip address (defined in secrets)..
                          #endif
                          if (mosquitto[3]>=127){mosquitto[3]=3;} 
                          delay(10);  
                         }else {Serial.println(" Signal too low. Not incrementing MQTT address ");}   //limit mqtt brokerrange  to 3-127 to save scan time
            SignOfLifeFlash( SignalOFF) ; ///turn OFF
             }
          }
       }
 }
