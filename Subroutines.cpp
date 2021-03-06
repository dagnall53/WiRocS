#include "Subroutines.h"
#include <Arduino.h> //needed 

extern uint8_t hrs;
extern uint8_t mins;
extern uint8_t secs;  
extern uint8_t divider;
extern uint16_t Message_Length;
extern uint8_t recMessage[128];
extern uint8_t NodeMCUPinD[18]; // hard fixed //Number of ports +2
extern void SignOfLifeFlash(bool state);
// globals

#define RN_PACKET_NETID  0
#define RN_PACKET_RCPTH  1
#define RN_PACKET_RCPTL  2
#define RN_PACKET_SNDRH  3
#define RN_PACKET_SNDRL  4
#define RN_PACKET_GROUP  5
#define RN_PACKET_ACTION 6
#define RN_PACKET_LEN    7
#define UID_LEN         7



void PrintTime(String MSG) {
 delay(1);
 Serial.printf("@<%02d:%02d:%02ds>:%s",hrs,mins,secs,MSG.c_str());
}

int rnSenderAddrFromPacket(unsigned char* rn, int seven) {
  return rn[RN_PACKET_SNDRL] + rn[RN_PACKET_SNDRH] * (seven ? 128 : 256);
}

int rnReceipientAddrFromPacket(unsigned char* rn, int seven) {
  return rn[RN_PACKET_RCPTL] + rn[RN_PACKET_RCPTH] * (seven ? 128 : 256);
}


int getTwoBytesFromMessageHL( uint8_t* msg, uint8_t highloc) {
  return msg[highloc + 1] + msg[highloc] * (256);
}



/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void SetWordIn_msg_loc_value(uint8_t* msg, uint8_t firstbyte, int value) {
  msg[firstbyte + 1] = value % 256; //low byte
  msg[firstbyte] = value / 256; //order is high first then low
}
int IntFromPacket_at_Addr(uint8_t* msg, uint8_t highbyte) { //example IntFromPacket_at_Addr(recMessage,Recipient_Addr))
  return msg[highbyte + 1] + msg[highbyte] * 256;
}

void dump_byte_array(byte* buffer, int bufferSize) {
  for (int i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
  Serial.println("");
}
void dump_byte(uint8_t buffer) {

  Serial.print(buffer < 0x10 ? " 0" : " ");
  Serial.print(buffer, HEX);
  Serial.print("h ");

}


uint8_t lnCalcCheckSumm(uint8_t *cMessage, uint8_t cMesLen) {
  unsigned char ucIdx = 0;
  char cLnCheckSum = 0;

  for (ucIdx = 0; ucIdx < cMesLen - 1; ucIdx++) //check summ is the last byte of the message
  {
    cLnCheckSum ^= cMessage[ucIdx];
  }

  return (~cLnCheckSum);
}



uint16_t AddrFull(uint8_t HI, uint8_t LO) {
  uint16_t FullAddress;
  FullAddress = 128 * (HI & 0x0F) + LO;
  return (FullAddress);
}



void copyUid (byte *buffOut, byte *buffIn, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    buffOut[i] = buffIn[i];
  }
  if (bufferSize < UID_LEN) {
    for (byte i = bufferSize; i < UID_LEN; i++) {
      buffOut[i] = 0;
    }
  }
}


bool compareUid(byte *buffer1, byte *buffer2, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    if (buffer1[i] != buffer2[i]) {
      return (false);
    }
  }
  return (true);
}


void Show_MSG() {
  if (Message_Length >= 1) {
#ifdef _SERIAL_SUBS_DEBUG
    Serial.println();
    Serial.print(F("SHOW Message :"));
    Serial.print(F(" Len:"));
    Serial.print(Message_Length);

    dump_byte_array(recMessage, Message_Length);

#endif
  }
}
extern void DebugSprintfMsgSend(int CX);
extern char DebugMsg[127];
extern uint16_t RocNodeID;
extern bool OLED1Present,OLED2Present,OLED3Present,OLED4Present,OLED5Present,OLED6Present;
extern void OLEDS_Display(String L1,String L2,String L3,String L4);
extern void OLED_4_RN_displays(int OLed_x,String L1,String L2,String L3,String L4);

extern void SetFont(uint8_t Disp,uint8_t Font);
extern bool CheckWiFiConnected(void);
extern boolean MQTT_Connected(void);
extern void ViewAllOLEDSettings(int OLed_x);
extern uint32_t StartedAt; 
void FlashMessage (String msg, int Repeats, int ON, int Off) {
  String IAM,IAM2,IAM3,IAM4,IAM5,IAM6;
  IAM="(";IAM+=RocNodeID;
   IAM2=IAM+",60,Display 5-8)";
   IAM3=IAM+",61,Display 1-4)";//9-12 internal 1-4 from the outside world
   IAM4=IAM+",61,Display 5-8)"; //13-16 internal but 5-8 for the outside world
   IAM5=IAM+",60,Display 1-2)";
   IAM6=IAM+",60,Display 3-4)";
   IAM+=",60, Display 1-4)";
  Serial.println(msg);
  if (CheckWiFiConnected()&& MQTT_Connected()){DebugSprintfMsgSend( sprintf ( DebugMsg, "OLED Flash<%s>",msg.c_str()));}
  for (int i = 0; i <= Repeats; i++) {
    //Serial.print("+");
   //OLEDS_Display(msg,IAM,"",""); 
   if(OLED1Present){SetFont(1,99);OLED_4_RN_displays(1,IAM,msg,"",""); } 
   if(OLED2Present){SetFont(2,99);OLED_4_RN_displays(2,IAM2,msg,"","");   }
   if(OLED3Present){SetFont(3,99);OLED_4_RN_displays(3,IAM3,msg,"","");   }
   if(OLED4Present){SetFont(4,99);OLED_4_RN_displays(4,IAM4,msg,"","");   }
   if(OLED5Present){SetFont(5,99);OLED_4_RN_displays(5,IAM5,msg,"","");   }
   if(OLED6Present){SetFont(6,99);OLED_4_RN_displays(6,IAM6,msg,"","");   }
      SignOfLifeFlash( SignalON) ; ///turn on
    delay(ON);
    //OLEDS_Display("",IAM,"","");   
    if(OLED1Present){SetFont(1,99);OLED_4_RN_displays(1,IAM,"","",""); } 
    if(OLED2Present){SetFont(2,99);OLED_4_RN_displays(2,IAM2,"","","");   }
    if(OLED3Present){SetFont(3,99);OLED_4_RN_displays(3,IAM3,"","","");   }
    if(OLED4Present){SetFont(4,99);OLED_4_RN_displays(4,IAM4,"","","");   }
    if(OLED5Present){SetFont(5,99);OLED_4_RN_displays(5,IAM5,"","","");   }
    if(OLED6Present){SetFont(6,99);OLED_4_RN_displays(6,IAM6,"","","");   }
    SignOfLifeFlash( SignalOFF) ; ///turn OFF
    delay(Off);
    
  }

}
void SetPortPinIndex() {
  //set my indexed port range for indexed use later  other code translates D0 to the pin number ESP uses..
#ifdef ESP32
   // on oled board, 4 is oled scl 5 isoled sda are oled 
   //See stuff in Directives.h that sets "Dn" to GPIO numbers
  NodeMCUPinD[0] = D0;
  NodeMCUPinD[1] = D1;
  NodeMCUPinD[2] = D2;
  NodeMCUPinD[3] = D3;
  NodeMCUPinD[4] = D4;
  NodeMCUPinD[5] = D5;
  NodeMCUPinD[6] = D6;
  NodeMCUPinD[7] = D7;
  NodeMCUPinD[8] = D8;
  NodeMCUPinD[9] = D9;
  NodeMCUPinD[10] = D10;
  NodeMCUPinD[11] = D11;
  NodeMCUPinD[12] = D12;
  NodeMCUPinD[13] = D13;
  NodeMCUPinD[14] = D14;
  NodeMCUPinD[15] = D15;
  NodeMCUPinD[16] = D16;

  
#else
//index for ESP8266, which has defines for D0 etc..
  NodeMCUPinD[0] = D0;  //the pins that relate to my D0-10 are set in ESP nodemcu etc directives 
  NodeMCUPinD[1] = D1;
  NodeMCUPinD[2] = D2;
  NodeMCUPinD[3] = D3;
  NodeMCUPinD[4] = D4;
  NodeMCUPinD[5] = D5;
  NodeMCUPinD[6] = D6;
  NodeMCUPinD[7] = D7;
  NodeMCUPinD[8] = D8; // 
  NodeMCUPinD[9] = D9; //RX
  NodeMCUPinD[10] = D10;// TX 
  

#endif
}
