#include "OTA.h"
#include "credentials.h"
#include <PS4Controller.h>
#include <Adafruit_VCNL4040.h>
//#include <Adafruit_BNO08x.h>

Adafruit_VCNL4040 vcnl4040 = Adafruit_VCNL4040();

#define sbot_BATSTAT 5U
#define sbot_LED3 15U
#define sbot_LED4 2U
#define sbot_LED1 4U
#define sbot_LED2 16U
#define Motor_FWD 1U
#define Motor_RVS 0U
#define Motor_CP1 12U
#define Motor_CP2 14U
#define Motor_CP3 26U
#define Motor_CP4 27U

void LED_set(void);
bool Robot_Dir = true;
bool Joystick_Flag = 0;
uint16_t Robot_Spd = 0U;
uint32_t OTA_timer = 0U;
uint8_t Joystick_X = 0U;
uint8_t Joystick_Y = 0U;
uint8_t Joystick_Yt = 0U;
uint8_t LED_Counter = 0U;

void notify(){
  Joystick_X = (uint8_t)(PS4.LStickX()+128U);
  Joystick_Y = (uint8_t)(PS4.LStickY()+128U);
  Joystick_Flag = 1;  
}
 
void setup() {
  Serial.begin(19200);
  Serial.println("Booting");
  setupOTA("Sbot", mySSID, myPASSWORD);
  
  pinMode(sbot_BATSTAT, INPUT_PULLUP);
  pinMode(sbot_LED1, OUTPUT);
  pinMode(sbot_LED2, OUTPUT);
  pinMode(sbot_LED3, OUTPUT);
  pinMode(sbot_LED4, OUTPUT);
  pinMode(Motor_CP1, OUTPUT);
  pinMode(Motor_CP2, OUTPUT);
  pinMode(Motor_CP3, OUTPUT);
  pinMode(Motor_CP4, OUTPUT);
  analogWrite(Motor_CP1, 128U);
  analogWrite(Motor_CP2, 128U);
  analogWrite(Motor_CP3, 128U);
  analogWrite(Motor_CP4, 128U);
  
  digitalWrite(sbot_LED1, HIGH); 
  PS4.attach(notify);
  PS4.begin();
  Serial.println("Ready");
  digitalWrite(sbot_LED2, HIGH); 

  if (!vcnl4040.begin()) {
    Serial.println("Couldn't find VCNL4040 chip");
    while (1);
  }  
  digitalWrite(sbot_LED3, HIGH); 
  Serial.println("Found VCNL4040 chip");
  vcnl4040.setProximityLEDCurrent(VCNL4040_LED_CURRENT_200MA);
  vcnl4040.setProximityLEDDutyCycle(VCNL4040_LED_DUTY_1_40);
  vcnl4040.setAmbientIntegrationTime(VCNL4040_AMBIENT_INTEGRATION_TIME_80MS);
  vcnl4040.setProximityIntegrationTime(VCNL4040_PROXIMITY_INTEGRATION_TIME_8T);
  vcnl4040.setProximityHighResolution(false);

  digitalWrite(sbot_LED4, HIGH); 
  while(OTA_timer < 150000U){
    OTA_timer++;  
    ArduinoOTA.handle(); 
  }
  digitalWrite(sbot_LED1, LOW); 
  digitalWrite(sbot_LED2, LOW); 
  digitalWrite(sbot_LED3, LOW); 
  digitalWrite(sbot_LED4, LOW); 
  
  delay(300);
}

void loop() {
  delay(300);
  LED_set();
  if(Joystick_Flag == 1){
    Joystick_Flag = 0;
    if(Joystick_Y < 128U){
      /*Reverse*/
      analogWrite(Motor_CP1, Joystick_Y);
      analogWrite(Motor_CP2, Joystick_Y);
      analogWrite(Motor_CP3, Joystick_Y);
      analogWrite(Motor_CP4, Joystick_Y);    
    }
    else{
      /*Forward*/
      if(Joystick_X < 108U){
        /*Turn left*/
        analogWrite(Motor_CP3, Joystick_Y);
        analogWrite(Motor_CP4, Joystick_Y); 
        Joystick_Yt = Joystick_Y / 2U; 
        analogWrite(Motor_CP1, Joystick_Yt);
        analogWrite(Motor_CP2, Joystick_Yt);        
      }
      else if(Joystick_X > 148U){
        /*Turn right*/
        analogWrite(Motor_CP1, Joystick_Y);
        analogWrite(Motor_CP2, Joystick_Y);
        Joystick_Yt = Joystick_Y - 128U; 
        Joystick_Yt = Joystick_Yt / 2U;         
        Joystick_Yt = Joystick_Yt + 128U;
        analogWrite(Motor_CP3, Joystick_Yt);
        analogWrite(Motor_CP4, Joystick_Yt);          
      }
      else{
        /*Forward*/
        analogWrite(Motor_CP1, Joystick_Y);
        analogWrite(Motor_CP2, Joystick_Y);
        analogWrite(Motor_CP3, Joystick_Y);
        analogWrite(Motor_CP4, Joystick_Y);         
      }
    }
  }
  else{
    /*wait for jystick to connect*/
    delay(600);
  }
}

void LED_set(void){
  uint16_t sbot_eye = vcnl4040.getProximity();
  digitalWrite(sbot_LED1, LOW); 
  digitalWrite(sbot_LED2, LOW); 
  digitalWrite(sbot_LED3, LOW); 
  digitalWrite(sbot_LED4, LOW); 

  if(digitalRead(sbot_BATSTAT) == 0U){
    /*USB detected - Turn off all motors for charging*/  
    analogWrite(Motor_CP1, 128U);
    analogWrite(Motor_CP2, 128U);
    analogWrite(Motor_CP3, 128U);
    analogWrite(Motor_CP4, 128U);
  }
  else if(sbot_eye < 50U){   
    /* No Obstacles detected*/
    LED_Counter++;     
    if(LED_Counter == 1U){
      digitalWrite(sbot_LED1, HIGH); 
      digitalWrite(sbot_LED4, LOW);  
    }
    else if(LED_Counter == 2U){
      digitalWrite(sbot_LED2, HIGH); 
      digitalWrite(sbot_LED1, LOW);  
    }
    else if(LED_Counter == 3U){
      digitalWrite(sbot_LED3, HIGH); 
      digitalWrite(sbot_LED2, LOW);  
    }
    else{
      digitalWrite(sbot_LED4, HIGH); 
      digitalWrite(sbot_LED3, LOW); 
      LED_Counter = 0U;
    } 
  }
  else if(sbot_eye < 300U){ 
    digitalWrite(sbot_LED1, HIGH);   
  }
  else if(sbot_eye < 600U){ 
    digitalWrite(sbot_LED1, HIGH);   
    digitalWrite(sbot_LED2, HIGH);   
  }
  else if(sbot_eye < 900U){ 
    digitalWrite(sbot_LED3, HIGH); 
    digitalWrite(sbot_LED1, HIGH);   
    digitalWrite(sbot_LED2, HIGH);     
  }
  else{ 
    digitalWrite(sbot_LED4, HIGH); 
    digitalWrite(sbot_LED3, HIGH); 
    digitalWrite(sbot_LED1, HIGH);   
    digitalWrite(sbot_LED2, HIGH);     
  }
}
