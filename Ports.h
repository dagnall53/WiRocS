#ifndef Ports_h
 #define Ports_h
#include <Stepper.h>


bool ButtonState[PortsRange];
boolean STATE[PortsRange];
bool LastDebounceButtonState[PortsRange]; //Number of ports +2
bool LastSentButtonState[PortsRange]; //Number of ports +2
uint32_t PortDebounceDelay[PortsRange]; //Number of ports +2
uint32_t lastDebounceTime[PortsRange]; //Number of ports +2
uint32_t PortTimingDelay[PortsRange]; //Number of ports +2

uint32_t Motor_Setting_Update_Time;


uint16_t ServoLastPos[PortsRange];
uint32_t WaitUntill;




//servo settings   
int SDemand[PortsRange]; //Number of ports +2



//int SloopDelay = 2;  //100 is about 4 seconds with 50ms delay in loop
int ServoOff_Delay_Until[PortsRange]; //Number of ports +2


//Servo Loco_LOCO_SERVO_Driven_Port;  //create servo object to control a servo  //servo 0 in ver107 on to be loco, but connected on pin D1. 
Servo myservo1;  //create servo object to control a servo
Servo myservo2;  //create servo object to control a servo
Servo myservo3;  //create servo object to control a servo
Servo myservo4;  //create servo object to control a servo
Servo myservo5;  //create servo object to control a servo
Servo myservo6;  //create servo object to control a servo
Servo myservo7;  //create servo object to control a servo
Servo myservo8;  //create servo object to control ... used for loco motors in ver 106 back

//conditional externs 
#ifdef _Audio 
extern uint8_t SoundEffect_Request[3];
extern void SetChuffPeriod(uint8_t Speed, int WavsPerRevolution );
extern int Wavs_Per_Revolution;
#endif


//
//Stuff to work the ports on the ESP
void SetMotorSpeed(int SpeedDemand_local,uint8_t dirf); 
uint8_t SetLocoMotorRC(int LocoPort, uint8_t SpeedDemand,bool dir); //RC  Speed demand range is approx mph  and outputs the speedemenad in case it limits it
uint8_t SetLocoMotorPWM(int LocoPWMPort, int LocoDirPort, uint8_t SpeedDemand,bool dir);  //PWM Speed demand range is approx mph  and outputs the speedemenad in case it limits it
void ImmediateStop(void);
void DoLocoMotor(void);
void READ_PORT( int i);
void ReadInputPorts(void);
void Port_Mode_Set(int i);
void Port_Setup_and_Report(void);
boolean Debounce (int i);
int FlashHL(int state, int port);
void FLASHING(void);
void DetachServo(int i);
void DETACH(void);
void SetServo( int i, uint16_t value);
uint16_t servoLR(int state, int port);
void SERVOS(void);

//the actual code follows 
// BIT position use in PI02 and Pi03 settings 
#define _input 0// was ==1
#define _toggle 5// was ==32
#define _setelsewhere 1// was ==2
#define _invert 6// was ==64
#define _flash 7
//PI03 
#define _servo 5// was ==32
#define _pwm 7 // was ==128

bool IsInput(uint8_t i){
return bitRead(Pi02_Port_Settings_D[i],_input); //
}
bool SetElsewhere(uint8_t i){
return bitRead(Pi02_Port_Settings_D[i],_setelsewhere); //
}
bool PortToggle(uint8_t i){
return bitRead(Pi02_Port_Settings_D[i],_toggle); //
}
bool PortInvert( uint8_t i){
  return bitRead(Pi02_Port_Settings_D[i],_invert); //
}
bool PortFlashing(uint8_t i){
  return bitRead(Pi02_Port_Settings_D[i],_flash); //
}


bool IsServo(uint8_t i){
 return bitRead(Pi03_Setting_options[i],_servo);
}
bool IsPWM(uint8_t i){
return bitRead(Pi03_Setting_options[i],_pwm);
}


void WriteAnalogtoPort(uint8_t port,uint16_t demand){
  #ifdef ESP32
  //port is channel!
      #ifdef _ESP32_PWM_DEBUG
        DebugSprintfMsgSend( sprintf ( DebugMsg, " Setting PWM on Pin%d (Channel<%d>) to duty cycle %d", NodeMCUPinD[port],port,demand)); 
      #endif
    // placed in else below ledcWrite(port, demand); // write to Channel (port) also needed this in line 557 ledcSetup(ledChannel,1000, 10);ledcSetup(ledChannel, freq, resolution);to be setup for 1khz, 10 bit res(10240 like esp8266
  // plus  stuff to work DAC25 and DAC26 0-255///
  if ((port == DAC25is) || (port = DAC26is)){
        dacWrite(NodeMCUPinD[port], demand/4);
    }else{
        ledcWrite(port, demand);  }
  #else
        analogWrite(NodeMCUPinD[port],demand);  // 0--1024 analogwrite to Pin(NodemcupinD(port) for esp8266
  #endif
}

void SetMotorSpeed(int SpeedDemand_local,uint8_t dirfl){ //lc dirf to avoid confusion with DIRF ?
#ifdef _LOCO_SERVO_Driven_Port 
int servodemand;
int Brake_trigger_speed;
bool Dir,SoundsOn,AlternateSounds,LightsOn,Use14Stepthrottle,UseSpeedTable,FInvert,DirInvert;
         
          Brake_trigger_speed= (CV[2]+((CV[3]+CV[4])/2));
          Dir=bitRead(dirfl,5);
          LightsOn= bitRead(dirfl,4);
          DirInvert=bitRead(CV[29],0);
          Use14Stepthrottle= false; !bitRead(CV[29],1);
          UseSpeedTable=bitRead(CV[29],4);
           
         // DebugSprintfMsgSend( sprintf ( DebugMsg, " Setting Speed<%d> Dir<%d> Lights<%d>",SpeedDemand_local, bitRead(dirfl,5),bitRead(dirfl,4)));  
#ifdef _Audio
//do brakes squeal here
        SoundsOn=bitRead(SoundEffect_Request[2],0); //F9?
        AlternateSounds=bitRead(SoundEffect_Request[2],1); //F10?
        if (((Last_DCC_Speed_Demand) >= Brake_trigger_speed ) && (SpeedDemand_local==0)&&(SoundsOn)){ //  play Break effect if speed was above Vtart + acc step and F9 is chuffs on
                     #if defined (_SERIAL_Audio_DEBUG) ||  defined (_PWM_DEBUG)
                       DebugSprintfMsgSend( sprintf ( DebugMsg, " Brakes Last speed %d   Trigger speed %d ", Last_DCC_Speed_Demand, Brake_trigger_speed)); 
                     #endif  
        BeginPlay(1,"/brakes.wav",CV[111]); //brakes.wav should be a brake squeal sample to be played as we stop. 
                       }                                
#endif 


         if ((SpeedDemand_local==0)&&(Last_DCC_Speed_Demand!=0)) {Dir=Last_Direction;}              //keep the lights in the right direction until stopped?     
              else {Last_Direction=Dir;}    // This catches rocrail issue where direction changes if 0 is pressed. keeps lights on in correct phase until stop, but changes if 0 repeatedly pressed                              
// do lights          
          digitalWrite (NodeMCUPinD[FRONTLight], (LightsOn & !(Dir^DirInvert)));
          digitalWrite (NodeMCUPinD[BACKLight], (LightsOn & (Dir^DirInvert)));
          DCC_Speed_Demand= (int)SpeedDemand_local;
          if ((Dir^DirInvert)){DCC_Speed_Demand=-(int)SpeedDemand_local;}
          DoLocoMotor();  // update we have saved DCC_Speed_Demand as int here ....may not be needed here specifically since loop does this.... 
#endif     
} 

  



uint8_t SetLocoMotorRC(int LocoPort, int Motor_Setting,bool dir){  //RC servo is set  0 to 180  
  int servodemand;
  bool STOPdemand,DirInvert;
  STOPdemand=false;
  if ((Motor_Setting)==0) { STOPdemand=true; }
  if (POWERON){  
    servodemand = abs(Motor_Setting) ;
    if (dir) {servodemand = 90- servodemand; }  //
        else {servodemand = 90+ servodemand; }
    if (STOPdemand) { servodemand=90; }
    if (servodemand >= 178) { servodemand = 178; }
    if (servodemand <= 2  ) {  servodemand = 2;} //servo range should be held to    2-178
     
  #ifdef _SERVO_DEBUG
    DebugSprintfMsgSend( sprintf ( DebugMsg, "RC motor speed:%d dir<%d>  servo setting:%d ",Motor_Setting,dir, servodemand));      
  #endif        
    SetServo(LocoPort, servodemand );
    Loco_servo_last_position=servodemand;
             }else {
                  SetServo(LocoPort, 90 );}
    if (STOPdemand) {Motor_Setting=0;SetServo(LocoPort, 90 ); }
  return Motor_Setting;    
}


uint8_t SetLocoMotorPWM(int LocoPort, int LocoDirPort, uint8_t Motor_Setting,bool dir){  //PWM Speed demand range is approx mph or % either range is 0-255
  // this code uses interpolate to use CV2, 5 6 
  uint8_t value;
  uint16_t PWMdemand;
  uint16_t MinSpeed;
  uint16_t AdditionalMotor_Setting;
  bool STOPdemand;
  uint16_t Max_Speed;
#ifdef _LocoPWMDirPort
  //Max_Speed= 255; 
  //if (Motor_Setting>=Max_Speed){Motor_Setting=Max_Speed;}  

  if (POWERON){ STOPdemand=false;
    if (Motor_Setting<=1) { PWMdemand=0;STOPdemand=true; }
  
    PWMdemand= 4*Motor_Setting;
    if (PWMdemand>=1023) {PWMdemand=1023;}//limit the max speed here 
     
     
     #ifdef _PWM_DEBUG
       DebugSprintfMsgSend( sprintf ( DebugMsg, "set loco PWM: input%d  output %d ",Motor_Setting, PWMdemand));
     #endif
  
  
  if (PWMdemand!=0) {
   #ifdef _NodeMCUMotorShield
      digitalWrite (NodeMCUPinD[LocoDirPort], (dir )) ;WriteAnalogtoPort(LocoPort,PWMdemand);
  #endif
  // addother hardware options here...
    #ifdef _separate_PWMS
    if (dir)
      digitalWrite (NodeMCUPinD[LocoDirPort], (dir )) ;WriteAnalogtoPort(LocoPort,PWMdemand);
      analogWrite (NodeMCUPinD[LocoDirPort], PWMdemand ; digitalWrite (NodeMCUPinD[LocoPort], (dir )) ;
  #endif
  // other hardware options
 
    
                   }
  }           
       

    if (STOPdemand) {Motor_Setting=0;    
                     SetChuffPeriod(0,Wavs_Per_Revolution);  
                     digitalWrite( NodeMCUPinD[LocoPort], false);   
                     digitalWrite (NodeMCUPinD[LocoDirPort] , false) ;
                    }  
 #endif
  return Motor_Setting;
}


void ImmediateStop(void){
  uint16_t Speed;
    #ifdef _LOCO_SERVO_Driven_Port 
   #ifndef _LocoPWMDirPort
               Speed= SetLocoMotorRC(_LOCO_SERVO_Driven_Port,0,0);
   #endif
   #ifdef _LocoPWMDirPort
               Speed=SetLocoMotorPWM(_LOCO_SERVO_Driven_Port,_LocoPWMDirPort,0,0);//
   #endif
   #endif
  }

extern uint32_t LoopTimer;
void DoLocoMotor(void){  //uses Last_DCC_Speed_Demand and DCC_Speed_Demand global variables to set servo or pwm including slowing and acceleration  
  //calls SetChuffPeriodFromSpeed so it takes into account the current acceleration 
 #ifdef _LOCO_SERVO_Driven_Port
  int offset;
  int steps;
  int SPEEDSET;
  bool DirInvert;
  DirInvert=bitRead(CV[29],0);
  bool Dir;
  Dir=bitRead(DIRF,5);
  
   ServoOff_Delay_Until[_LOCO_SERVO_Driven_Port] = millis() + 10000;  //reset the servo off delay for the motor...
   if (POWERON == false) { //Track power off, stop the motor, zero the motor servo immediately
                        ImmediateStop();    }
       else {   //Power is ON 
                //  Last_DCC_Speed_Demand and  DCC_Speed_Demand are Integer variables set negative if in reverse. this allows the maths below to work when reversing without speed change  
            if ((LoopTimer >= Motor_Setting_Update_Time)&& ( (Last_DCC_Speed_Demand != DCC_Speed_Demand))) { //do updates only at the required delay update rate on a change
                Motor_Setting_Update_Time=millis() +100; //do update only at 100ms delay update rate
                offset = DCC_Speed_Demand-Last_DCC_Speed_Demand;  //how far from the demand are we
          
       #ifdef _PWM_DEBUG
                     DebugSprintfMsgSend( sprintf ( DebugMsg, "DLM lastsp%d CurSpeed%d  offset%d speedset%d",Last_DCC_Speed_Demand,DCC_Speed_Demand,offset,SPEEDSET));
          #endif      
          
          
               //speed up or decellerate?
               if (abs(DCC_Speed_Demand) >= abs(Last_DCC_Speed_Demand)) {  //we are accelerating if more positive no need to take account of sign...
                                  steps = CV[3];    }
                    else    {     steps = CV[4];    }           //must be slowing down
                    
               if (abs(offset) >= abs(steps)) {                  //quite a way to go ?? use steps cv3/4
                    if (offset>=0){                               //Speed demand is higher than last demand
                            SPEEDSET=Last_DCC_Speed_Demand+steps; }   //offset is now either the error or steps, whichever is less4
                            else {SPEEDSET=Last_DCC_Speed_Demand-steps;}
                                       }
                      else {SPEEDSET=DCC_Speed_Demand; 
                      }                                             //Offset smaller than difference so just set to demand
 
//-----------------KICK START MOTOR
    int Kick;  //changed to a CV[65] related timer value 
    int TI;

    if(((Last_DCC_Speed_Demand)==0)&&((DCC_Speed_Demand)!=0)){//give a quick kick pulse THEN do the setLocoMotor to ensure starting from stopped. This works well with pwm, but not as well with servo
         
        
        #ifndef _LocoPWMDirPort    // servo port
                Kick=40;TI=CV[65]; 
             
                SetLocoMotorRC(_LOCO_SERVO_Driven_Port,Kick,DirInvert^Dir);
                delay(TI*2); //wait for this long before going to "correct" speed, longer for servo than pwm
                         //DebugSprintfMsgSend( sprintf ( DebugMsg, "Kick start motor speed%d Time:%dms  Dir%d",SPEEDSET,TI*2,Dir));
         #endif
        #ifdef _LocoPWMDirPort
                Kick=500;TI=CV[65];
                #ifdef _PWM_DEBUG
                     DebugSprintfMsgSend( sprintf ( DebugMsg, "Initiating Kick start motor %d  for %d*10ms",Kick,TI));
               #endif
                SetLocoMotorPWM(_LOCO_SERVO_Driven_Port,_LocoPWMDirPort,Kick,DirInvert^Dir);//
               delay(TI); //wait for this long before going to "correct" speed
                       // DebugSprintfMsgSend( sprintf ( DebugMsg, "Kick start motor speed%d Time:%dms  Dir%d",SPEEDSET,TI,Dir));
        #endif
                                           }
//-end ----------------KICK START MOTOR
    //  Last_Set_Dir=Dir;  // do we use this still ??
                          //do the motor drivers....       Use abs,because they use the dir^inverdir commands and will get confused with negative velocity
                         // re build the direction from the demans as the current direction may not be Dir^DirInv
      if (abs(SPEEDSET)==SPEEDSET){Dir=true;} //positive
           else{Dir=false;}
    
      #ifndef _LocoPWMDirPort
               SetLocoMotorRC(_LOCO_SERVO_Driven_Port,abs(SPEEDSET),Dir);
      #endif
      #ifdef _LocoPWMDirPort
               SetLocoMotorPWM(_LOCO_SERVO_Driven_Port,_LocoPWMDirPort,abs(SPEEDSET),Dir);
      #endif
    
    if((Last_DCC_Speed_Demand != SPEEDSET)&&(SPEEDSET!=0)){
          #ifdef _Audio      //sets up chuff period if speed changed here so it works with acceleration etc
               SetChuffPeriod(abs(SPEEDSET),Wavs_Per_Revolution);   
          #endif //is audio
                                        }
                                             
     Last_DCC_Speed_Demand=(int)SPEEDSET;  
      } //update time
   } //else for the power on check
    
 #endif //is loco
}

void READ_PORT( int i) {
  //boolean STATE;
  uint8_t TEMP;
  uint16_t senderADDR;
  if ( IsInput(i) && (!IsServo(i)||!IsPWM(i)))  { //only do this if this port is an "INPUT" and not a "SERVO"  // ??  hard set also sets inputstate to OUTPUT
    if ((Debounce(i))&&(millis()>=PortTimingDelay[i])){              //debounce is true if switch has changed for longer than the debounce time (~10ms) 
                                                                      //&& has the message been sent for long enough (port timing delay)?
      if ((PortToggle(i))&&(PortInvert(i)^digitalRead(NodeMCUPinD[i]) == 1)) {
           ButtonState[i] = !ButtonState[i]; //TOGGLE change only changes on one state!..
           }
      if (PortToggle(i))   {
        STATE[i] = ButtonState[i]; // record toggle state
           }
      else {  // not toggle, just use it  
        STATE[i] = PortInvert(i)^digitalRead(NodeMCUPinD[i]);
           }
      
#if defined (_SERIAL_DEBUG) || defined (_Input_DEBUG)
      Serial.print ("Change on IO port : ");
      Serial.print(i);
      Serial.print(" State");
      Serial.println(STATE[i]);
#endif
      SendPortChange(RocNodeID, STATE[i], i);
      PortTimingDelay[i] = millis() + (DelaySetting_for_PortD[i] * 10);
      LastSentButtonState[i]=STATE[i];
    }//if debounce
    // if not sent because of delays ?
    if ((millis()>=PortTimingDelay[i])&& (LastSentButtonState[i]!=STATE[i])){
      SendPortChange(RocNodeID, STATE[i], i);
      PortTimingDelay[i] = millis() + (DelaySetting_for_PortD[i] * 10);
      LastSentButtonState[i]=STATE[i];
   }
    
  }// input and not servo
}//end read port



void ReadInputPorts() {
   for (int i = 1 ; i <= NumberOfPorts; i++) {
      READ_PORT(i); // includes check for isinput()
    }
 
  
}
extern bool OLED1Present,OLED2Present,OLED3Present,OLED4Present,OLED5Present,OLED6Present;

void Port_Mode_Set(int i) {
  boolean hardset,setElsewhere, output,pullup;
  hardset = false;
  setElsewhere = false;
  pullup=true;
  String description;
  description =" ";
 
  output= (!IsInput(i)); //read current status 

//
  #ifdef _LOCO_SERVO_Driven_Port 
     
    #ifdef _LocoPWMDirPort
    #endif  
  #endif
//
  
  switch (i) {
  #ifdef _LOCO_SERVO_Driven_Port
      case _LOCO_SERVO_Driven_Port:
                      description =" LOCO Motor_Servo ";
                      Pi02_Port_Settings_D[i]= 0;
                      Pi03_Setting_options[i] = 10; //KEEP this i/o as a "SERVO" output regardless, 
                      bitSet(Pi03_Setting_options[i],_servo);
                      hardset =true;output=true;
      #ifdef _LocoPWMDirPort
                      description =" LOCO PWM ";
                      Pi02_Port_Settings_D[i]= 0; // set  as output for direction
                      Pi03_Setting_options[i] = 10; //10= delay to use for servo changes = 100ms rate ;
                      bitSet(Pi03_Setting_options[i],_pwm);
      #endif
            break;
  #endif

  #ifdef _LocoPWMDirPort
      case _LocoPWMDirPort:
                      description =" LOCO PWM direction ";
                      bitClear(Pi02_Port_Settings_D[i],_input); 
                      Pi03_Setting_options[i] = 0; // set  as output for direction nodemcumotor shield thIS is only for NodeMCU moto shield!
                      hardset =true;output=true;
      break;
  #endif

    // case SignalLed:  
    //                  description =" Node SignalLED ";
    //                  bitClear(Pi02_Port_Settings_D[i],_input);
    //                  Pi03_Setting_options[i] = 0;  
    //                  hardset =true;setElsewhere = false;output=true;
    //  break;
                      
  #ifdef _LOCO_SERVO_Driven_Port  
      case FRONTLight:
                      description =" Loco FRONTLight ";
                      bitClear(Pi02_Port_Settings_D[i],_input);
                      Pi03_Setting_options[i] = 0; 
                      hardset =true;setElsewhere = false;output=true;
       break;
       case BACKLight:
                      description =" Loco BACKLight ";
                      bitClear(Pi02_Port_Settings_D[i],_input); 
                      Pi03_Setting_options[i] = 0;
                      hardset =true;setElsewhere = false;output=true;
        break;
  #endif                      
  #ifdef SteamOutputPin
        case SteamOutputPin:
                     description =" Loco SteamPulse ";
                     Pi03_Setting_options[i] = 0;
                     bitClear(Pi02_Port_Settings_D[i],_input); 
                     hardset =true;setElsewhere = false;output=true;
         break;

  #endif
        
                      
  #ifdef _Audio  
        
     #ifdef _AudioDAC   
        case I2SDAC_LRC:
                   description =" LRC (shared with Audio) ";
                   //Pi02_Port_Settings_D[i]= 0; Pi03_Setting_options[i] = 0;
                  hardset =false; setElsewhere = true ;output=false;
        break;            
        case I2SDAC_CLK:
                   description =" CLK (used by Audio DAC)";
                   Pi02_Port_Settings_D[i]= 0; Pi03_Setting_options[i] = 0;
                   hardset =true;setElsewhere = true;output=true;
        break;
      
                    
        case I2SDAC_DIN:
                   description =" (RX)and DIN (used by Audio DAC)";
                   Pi02_Port_Settings_D[i]= 0; Pi03_Setting_options[i] = 0;
                   hardset =true;setElsewhere = true;output=true;
        break;
     #else  // Audio, but not using DAC
        case I2SDAC_LRC:
                   description =" LRC (used by Audio) - can be used as "; 
                   //Pi02_Port_Settings_D[i]= 
                   bitSet (Pi02_Port_Settings_D[i],_input ); output=false; // force setting to input here
                   Pi03_Setting_options[i] = 0;                                                  // force not pwm and not servo to input here
                   hardset =false;setElsewhere = false;
        break;
        case I2SDAC_DIN:
                   description =" (RX) and Audio Speaker Drive ";
                   Pi02_Port_Settings_D[i]= 0; Pi03_Setting_options[i] = 0;
                   hardset =true;setElsewhere = true;output=true;
        break;
     #endif
        
  #endif 
                 
         default:  // any defaults stuff set according to Pi03_Setting_options[i] and Pi02_Port_Settings_D[i]
                   output= (!IsInput(i));
   
         break;
    }//end of switch
    // do "Special cases" based on pin numbers
     if (NodeMCUPinD[i]==SignalLed ) {
                      description =" SignalLED ";
                      bitClear(Pi02_Port_Settings_D[i],_input);
                      Pi03_Setting_options[i] = 0;  
                      hardset =true;setElsewhere = false;output=true;
                      }
    
    if((OLED1Present||OLED3Present||OLED5Present)&&((NodeMCUPinD[i]==OLED_SCL)||(NodeMCUPinD[i]==OLED_SDA))){
      description ="I2C bus";bitSet(Pi02_Port_Settings_D[i],_input ); 
                      Pi03_Setting_options[i] = 0; 
      hardset =true;output=false;pullup=false;setElsewhere = true;
      }
       if((OLED2Present||OLED4Present||OLED6Present)&&((NodeMCUPinD[i]==OLED_SCL2)||(NodeMCUPinD[i]==OLED_SDA2))){
      description ="I2C bus";bitSet(Pi02_Port_Settings_D[i],_input ); 
                      Pi03_Setting_options[i] = 0; 
      hardset =true;output=false;pullup=false;setElsewhere = true;
      }

    
    if((NodeMCUPinD[i]>=34)&& (NodeMCUPinD[i]<=39)){
                      description ="Input NO PULLUP ";
                      bitSet(Pi02_Port_Settings_D[i],_input ); 
                      Pi03_Setting_options[i] = 0; // set  as output for direction nodemcumotor shield thIS is only for NodeMCU moto shield!
                      
        }
    if((NodeMCUPinD[i]>=25)&& (NodeMCUPinD[i]<=26)){
                      description =" ESP32 Native DAC ";
                      bitClear(Pi02_Port_Settings_D[i],_input); 
                      Pi03_Setting_options[i] = 128;  
                      hardset =true;output=true;setElsewhere = false;
        }

        //setting hardset message
      if (hardset||setElsewhere){
        //Pi02_Port_Settings_D[i]= Pi02_Port_Settings_D[i] & 0x2;
        bitSet (Pi02_Port_Settings_D[i],_setelsewhere);
      }
    
    // now do the setting proper and send some useful messages out on the serial interface 
    #ifdef ESP32
        Serial.print(F("RR Addr:"));Serial.print (i);Serial.print (F(" <GPIO:"));Serial.print (NodeMCUPinD[i]);Serial.print(F(">" ));
        
    #else
        Serial.print (F("Pin D"));
        Serial.print (i); Serial.print(F(" is Rocrail Address:"));Serial.print (i);
    #endif
    
       if (hardset){Serial.print(" (Hard_Set)");}
    Serial.print(description);
    if (!setElsewhere){
        if (IsServo(i)||IsPWM(i)){  // do first to force pi-02 to output 
                                 bitClear (Pi02_Port_Settings_D[i],_input ); 
                                 output=true;
                                 }
    
        if (output){ 
                    pinMode(NodeMCUPinD[i], OUTPUT); // works for servo for esp32 and esp8266, but pwm needs additional stuff for esp32
                    Serial.print (F(" Output"));
                    if (IsServo(i)) { Serial.print (F(" Servo"));}
                    if (IsPWM(i)) 
                                  {
                                    #ifdef ESP32
                                      ledcSetup(i,1000, 10);// i is the channel (0-16??) 1khz and 10 bit resolution
                                      ledcAttachPin(NodeMCUPinD[i], i); // attach pin (nodemcuetc) to this channel  
                                    #endif 
                                   WriteAnalogtoPort(i, 0);  
                                   Serial.print (F(" PWM"));  }
                   }
             else   { // is an input          
                    pinMode(NodeMCUPinD[i], INPUT_PULLUP);
                    if (!pullup && PortInvert(i)) {Serial.print (F(" {invert} "));}
                    if (pullup){Serial.print(F( " Input with pullup"));}
                    
                     }
                     
             
         if (!hardset){
                if (PortInvert(i)) {Serial.print (F(" {invert} "));}
                if (PortToggle(i)) {Serial.print (F(" {toggle} "));}
                if (PortFlashing(i)) {Serial.print (F(" {flashing} "));} 
                if ( !IsServo(i) && !IsPWM(i)){ // input or output set purely by Pi02 show the pi02 delay setting
                                  Serial.print(" Delay :"); Serial.print( DelaySetting_for_PortD[i]);
                                  if (DelaySetting_for_PortD[i] ==0){     Serial.print("ms ");}
                                                                   else { Serial.print("0ms ");}
                                              }
                if (IsServo(i)||IsPWM(i)){
                  // we do nothing with report yet, so do not print this if ((Pi03_Setting_options[i]&64)==64) {Serial.print (F(" {report} "));}
                   sprintf ( DebugMsg, " Off<%d> On<%d> ", (Pi03_Setting_offposH[i] * 256) + Pi03_Setting_offposL[i],(Pi03_Setting_onposH[i] * 256) + Pi03_Setting_onposL[i]);
                   Serial.print(DebugMsg);
                                         }
                
          
          
          //Serial.print (F(" Pi02 PortType:"));
          //Serial.print (Pi02_Port_Settings_D[i]);
         // Serial.print (F(" Pi03_Setting_options:"));
         // Serial.print (Pi03_Setting_options[i]);
                      }      
                     
                     }else{ // SET node directions elsewhere so do not printout port settings 
                    Serial.print (" Set elsewhere");
                           }
         
    
       
       Serial.println("");
  }


void Port_Setup_and_Report() {
      Serial.println(F("---------------------- ROCNODE Setup   -----------------------"));
      Serial.print(F(  "          'RocNet Address' is :"));
       //Serial.print(EEPROMRocNodeID); Serial.print(" :"); 
       Serial.println (RocNodeID); 
      Serial.println(F("--------------------------------------------------------------"));
      pinMode(NodeMCUPinD[0], OUTPUT);
    for (int i = 1 ; i <= NumberOfPorts; i++) {   
                     Port_Mode_Set(i);  // shows all port definitions.. Improved reporting code in V23
                                      }
          
}

void ResetDebounce(){
  for (int i = 0 ; i <= 8; i++) {
    LastDebounceButtonState[i] = PortInvert(i)^digitalRead(NodeMCUPinD[i]);
    LastSentButtonState[i]=LastDebounceButtonState[i];
    PortDebounceDelay[i]=millis();
    ButtonState[i] = PortInvert(i)^digitalRead(NodeMCUPinD[i]);
    PortTimingDelay[i] = millis()+ ((DelaySetting_for_PortD[i] * 10) + 1);
    ServoOff_Delay_Until[i] = millis() + 10000;
  }
}

boolean Debounce(int i) {  //Tests for inputs having changed, 
 //new
 unsigned long debounceDelay = 10;  //ms
 bool Reading;
 boolean SwitchSensed;
      SwitchSensed = false ;
      Reading= (PortInvert(i)^digitalRead(NodeMCUPinD[i]));
     //filter out any noise by setting a time buffer
     if ( (millis() - lastDebounceTime[i]) > debounceDelay) {
          if (Reading != LastSentButtonState[i]){  // changed state ? Check against SENT state
              SwitchSensed = true;
              LastDebounceButtonState[i]=Reading;
               }
     }
     if (Reading != LastDebounceButtonState[i]) {
        LastDebounceButtonState[i] = Reading; // update until it stays the same
        lastDebounceTime[i]=millis();  
         }
    

         
  return (SwitchSensed);
}
//below here was x2 in subroutines

int FlashHL(int state, int port) {
  int value;
  if (state == 0) {
    value = (Pi03_Setting_offposH[port] * 256) + Pi03_Setting_offposL[port];
  }
  if (state == 1) {
    value = (Pi03_Setting_onposH[port] * 256) + Pi03_Setting_onposL[port];
  }
  value = ((value / 4)); //set the PWM  range 0-1023
  return (value) ;
}

void FLASHING() {
  boolean set;
  for (uint8_t port = 1; port <= NumberOfPorts; port++) {
    set = (!ButtonState[port]); //get phasing right so "no flash" is same high or low state as the non blink state
    if (PortFlashing(port)^PortInvert(port)) { set = !set; }//invert set action if "invert",  "port blink" set ?  
    
   if((PortFlashing(port))&& (!IsInput(port))&&(!IsServo(port))) { //make sure its an output '0' and has port blink '128' set! and is NOT a Servo
      if ((set) && (millis() >= PortTimingDelay[port]) && (DelaySetting_for_PortD[port] >= 1)) {
          if (IsPWM(port)) {//has 'channel' blink set  if so, use PWM outputs
                            if (SDemand[port] == FlashHL(1, port)) {SDemand[port] = FlashHL(0, port);
                                           }else {SDemand[port] = FlashHL(1, port); }
                            WriteAnalogtoPort(port,SDemand[port]);
                           } else {                                        //its a digital output so just invert current state
          digitalWrite (NodeMCUPinD[port], !digitalRead(NodeMCUPinD[port]));
        }
        PortTimingDelay[port] = millis() + (DelaySetting_for_PortD[port] * 10);
      }
      

    }
  }
}


void DetachServo(int i) {
   switch (i) {
    case -1: {
        Serial.println(F(" ---- Switching OFF ALL Servos---"));
      //Loco_LOCO_SERVO_Driven_Port.detach();
        myservo1.detach();
        myservo2.detach();
        myservo3.detach();
        myservo4.detach();
        myservo5.detach();
        myservo6.detach();
        myservo7.detach();
        myservo8.detach();
      }
      break;
    case 1: {
        myservo1.detach();
      }
      break;
    case 2: {
        myservo2.detach();
      }
      break;
    case 3: {
        myservo3.detach();
      }
      break;
    case 4: {
        myservo4.detach();
      }
      break;
    case 5: {
        myservo5.detach();
      }
      break;
    case 6: {
        myservo6.detach();
      }
      break;
    case 7: {
        myservo7.detach();
      }
      break;
    case 8: {
        myservo8.detach();
      }
      break;
  }
 
}

void DETACH() {
 int i;
 for (int i = 1 ; i <= 8; i++) { //up to 8 servos..  no check here for loco servo!. 
    if ( (millis() >= ServoOff_Delay_Until[i])  && ( IsServo(i) ) ) { //double check its actually a servo !!, as this is called from two places{
        ServoOff_Delay_Until[i]=millis()+OneDay;  //set one day ahead as a simple way to avoid setting it again next time around... Moving the servo will reset it to millis anyway
      
        Serial.printf("\n Switching OFF Servo:%i ",i);
        //Serial.print(" Switching OFF Servo:");
        //Serial.println(i);
        DetachServo(i);
                                                                                       }
                                  }
             }





void SetServo( int i, uint16_t value) { //uses 0-180
  long MotorSpeed;
  uint16_t SavedValue;
  
  if (IsServo(i)) { //double check, as this is called from two places
    ServoLastPos[i]=value;
    switch (i) {
    
      case 1: {
          if (!myservo1.attached()) { 
            myservo1.attach(D1);
          }
          myservo1.write(value);
          }
        break;
      case 2: {
          if (!myservo2.attached()) {
            myservo2.attach(D2);
          }
          myservo2.write(value);
        }
        break;
      case 3: {
          if (!myservo3.attached()) {
            myservo3.attach(D3);
          }
          myservo3.write(value);
        }
        break;
      case 4: {
          if (!myservo4.attached()) {
            myservo4.attach(D4);
          }
          myservo4.write(value);
        }
        break;
      case 5: {
          if (!myservo5.attached()) {
            myservo5.attach(D5);
          }
          myservo5.write(value);
        }
        break;
      case 6: {
          if (!myservo6.attached()) {
            myservo6.attach(D6);
          }
          myservo6.write(value);
        }
        break;
      case 7: {
          if (!myservo7.attached()) {
            myservo7.attach(D7);
          }
          myservo7.write(value);
        }
        break;
      case 8: {
          if (!myservo8.attached()) {
            myservo8.attach(D8);
          }
          myservo8.write(value);
        }
        break;
    }  //switch
   
  
                                   }//double check its a servo 
}//end 

uint16_t servoLR(int state, int port) {
  int value;
  value = 375; //== mid , 90 degrees
  if (state == 0) {
    value = (Pi03_Setting_offposH[port] * 256) + Pi03_Setting_offposL[port];
  }
  if (state == 1) {
    value = (Pi03_Setting_onposH[port] * 256) + Pi03_Setting_onposL[port];
  }
  value = ((value - 150) * 2) / 5; //set the servo immediately range 150-600 = 0 to 180
  return   (value);
}

void SERVOS() {              //attaches and detaches servos, accelerates to demanded positions etc..

  int ServoPositionNow;
  int offset;
  int steps;
  int SERVOSET;
  int set;
  uint32_t LocalTimer;
  int MotorSpeed;
  LocalTimer=millis();
  for (int i = 1 ; i <= 8; i++) { //up to 8 servos.. originally,  _LOCO_SERVO_Driven_Port was just another srvo used here but with different settings  
#ifdef _LOCO_SERVO_Driven_Port    
    if (i!=_LOCO_SERVO_Driven_Port){ //not for loco speed control "servo"
#endif
    if (IsServo(i)) { //only if this port is a "servo"... To address a channel instead of a port the port type servo must be set on the interface tab of switches and outputs
        if (millis() >= (Pi03_Setting_LastUpdated[i] + (Pi03_Setting_options[i] & 15) * 10)) { //do update only at the required delay update rate
                 offset = SDemand[i] - ServoLastPos[i];  //how far from the S_demand are we
                 ServoPositionNow=ServoLastPos[i]; 
      if ((abs(offset) <= 3) && (PortFlashing(i))  && (ButtonState[i] == 1) ) { //REVERSING (Flashing):demand if within 3 of demand and reversing...needs inv to operate on buttonstate, ???as it will not switch off if inv..
  
//#ifdef _SERVO_DEBUG
//     DebugSprintfMsgSend( sprintf ( DebugMsg, " ( 506) Using Reversing code"));
//#endif
           ServoOff_Delay_Until[i] = LocalTimer + 2000; //keep servo active for at least 2 sec from now, we are a long way from our final position. 
          if (SDemand[i] == servoLR(1, i)) {    SDemand[i] = servoLR(0, i);   }
               else                        {    SDemand[i] = servoLR(1, i);   }
                                   } //end of REVERSING CODE 
     
        if (abs(offset) != 0) {  //we are not at the right position yet....
          ServoOff_Delay_Until[i] = LocalTimer + 2000; //2s sec off timer start but only do if the servo is NOT in the final position, when its in final position, it does NOT update
          if (ButtonState[i]) {   steps = Pi03_Setting_onsteps[i];  }
               else           {   steps = Pi03_Setting_offsteps[i]; }
              
          if (steps == 0)     {   SERVOSET = SDemand[i];
//#ifdef _SERVO_DEBUG
//                           DebugSprintfMsgSend( sprintf ( DebugMsg, "Set Servo Immediately"));
//#endif
                              }
       else {    //steps not == zero
       //if (i!=_LOCO_SERVO_Driven_Port){ //not for loco 
        steps=1; //force steps to 1 to prevent a strange servo behaviour where the servo jitters unstably around the end position.. which does not affect the RC controller for some reason 
       //}
            if (abs(offset) >= abs(steps)) { offset = (offset * steps) / abs(offset);  //offset is now either the error or steps, whichever is less
                                           }                            
            SERVOSET = ServoPositionNow + offset;
            if ((SERVOSET) >= 180) { SERVOSET = 180; }
            if ((SERVOSET) <= 0  ) {  SERVOSET = 0; } //range for servo is 0-180
            }
#ifdef _SERVO_DEBUG
 //NOTE Sending serial messages adds about 200ms delay in the loop !.
 //if (i!=_LOCO_SERVO_Driven_Port){ //not for loco 
//DebugSprintfMsgSend( sprintf ( DebugMsg, "Moving Servo[%d] from current position(%d) to demand:{%d}degrees by moving:%d steps ",i,ServoPositionNow,SDemand[i],offset));
 //}
#endif
          Pi03_Setting_LastUpdated[i] = LocalTimer ;
          SetServo( i, SERVOSET);       

        } //offset != 0
      } //time to do an update
    } //if servo
#ifdef _LOCO_SERVO_Driven_Port    
    } //if not loco
#endif
  } //end i 1..8 loop
}//end servos

#endif
