#include <Timers.h>

#include <Raptor.h>
#include <SPI.h>

/*************************************************************
  File:      RaptorBasics.ino
  Contents:  This program is a warmup for ME210 Lab 0, and
             serves as an introduction to event-driven programming 
  Notes:     Target: Arduino Leonardo
             Arduino IDE version: 1.6.7

  History:
  when       who  what/why
  ----       ---  ---------------------------------------------
  2016-01-09 MTP  program created
  2016-01-10 KN   Updated Raptorlib and RaptorProof to match
  2016-01-11 KLG  minor tweaks to make signed/unsigned consistent
 ************************************************************/
 
/*---------------Includes-----------------------------------*/

#include <Raptor.h>
#include <Timers.h>

/*---------------Module Defines-----------------------------*/

#define LIGHT_THRESHOLD    30 // *Choose your own thresholds* smaller at night
#define LINE_THRESHOLD    0 // *Choose your own thresholds*

#define ONE_SEC            1000
#define TIME_INTERVAL     ONE_SEC

#define TIMER_0            0
#define TIMER_1            1

#define LEFT_TRIGGER      0x01
#define CENTER_TRIGGER      0x04
#define RIGHT_TRIGGER      0x10
/*---------------Module Function Prototypes-----------------*/
void checkGlobalEvents(void);
void handleMoveForward(void);
void handleMoveBackward(void);
unsigned char TestTimer0Expired(void);
void RespTimer0Expired(void);
unsigned char TestForKey(void);
void RespToKey(void);
unsigned char TestForLightOn(void);
void RespToLightOn(void);
unsigned char TestForLightOff(void);
void RespToLightOff(void);
unsigned char TestForFence(void);
void RespToFence(void);
unsigned char TestTimer0Expired(void);
void RespTimer0Expired(void);
void printLightLevel(void);
void printLineLevel(void);
unsigned char TestTimer1Expired(void);
void RespTimer1Expired(void);


/*---------------State Definitions--------------------------*/
typedef enum {
   STATE_MOVE_FORWARD, STATE_MOVE_BACKWARD
} States_t;

/*---------------Module Variables---------------------------*/
States_t state;
unsigned char isLEDOn;

/*---------------Raptor Main Functions----------------*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Hello, world!");
  state = STATE_MOVE_FORWARD;
  isLEDOn = false;
  TMRArd_InitTimer(0, TIME_INTERVAL);
  TMRArd_InitTimer(1, TIME_INTERVAL);
}

void loop() {
  // put your main code here, to run repeatedly:
  checkGlobalEvents();
  //printLightLevel();
  //printLineLevel();
  
  /*
  switch(state) {
    case STATE_MOVE_FORWARD:
      handleMoveForward();
      break;
    case STATE_MOVE_BACKWARD:
      handleMoveBackward();
      break;
    default: //Should never get into an unhandled state
      Serial.println("What is this I do not even...");
  }
  */
}

/*----------------Module Functions--------------------------*/
void checkGlobalEvents(void) {
  if (TestTimer0Expired()) RespTimer0Expired();
  if (TestTimer1Expired()) RespTimer1Expired();
  if (TestForKey()) RespToKey();
  if(TestForLightOn()){
    }else{
    RespToLightOn();
    }
}

void handleMoveForward(void) {
  raptor.LeftMtrSpeed(25);
  raptor.RightMtrSpeed(25);
}

void handleMoveBackward(void) {
  raptor.LeftMtrSpeed(-25);
  raptor.RightMtrSpeed(-25);
}

unsigned char TestTimer0Expired(void) {
  return (unsigned char)(TMRArd_IsTimerExpired(TIMER_0));
}

void RespTimer0Expired(void) {
  static int Time = 0;
  
  TMRArd_InitTimer(TIMER_0, TIME_INTERVAL);
  if(isLEDOn) {
    isLEDOn = false;
    raptor.RGB(RGB_OFF);
  } else {
    isLEDOn = true;
    raptor.RGB(RGB_WHITE);
  }
}

unsigned char TestTimer1Expired(void) {
  return (unsigned char)(TMRArd_IsTimerExpired(TIMER_1));
}
void RespTimer1Expired(void) {
  static int Time = 0;
  TMRArd_InitTimer(TIMER_1, TIME_INTERVAL);
  if(state==STATE_MOVE_FORWARD) {
    handleMoveForward();
    state=STATE_MOVE_BACKWARD;
  } else {
    handleMoveBackward();
    state=STATE_MOVE_FORWARD;
  }
}


unsigned char TestForKey(void) {
  unsigned char KeyEventOccurred;
  KeyEventOccurred = Serial.available();
  return KeyEventOccurred;
}

void RespToKey(void) {
  unsigned char theKey;
  
  theKey = Serial.read();
  Serial.print(theKey);
  Serial.print(", ASCII=");
  Serial.println(theKey,HEX);
}

void printLightLevel(void){
  Serial.print("Light Level=");
  Serial.println(raptor.LightLevel());
}

void printLineLevel(void){
  Serial.print("\n Right_Line=");
  Serial.println(raptor.LineRight());
  Serial.print("Center_Line=");
  Serial.println(raptor.LineCenter());
  Serial.print("Left_Line=");
  Serial.println(raptor.LineLeft());
 
}

unsigned char TestForLightOn(void) {
  if((raptor.LightLevel() > LIGHT_THRESHOLD)){
    return true;
  }else{
    return false; 
  }
}

void RespToLightOn(void) {
  //raptor.Beep(TIME_INTERVAL,TIME_INTERVAL);
  raptor.RGB(RGB_WHITE);
}

unsigned char TestForLightOff(void) {
    if((raptor.LightLevel() < LIGHT_THRESHOLD)){
    return true;
  }else{
    return false; 
  }
}

void RespToLightOff(void) {
  raptor.RGB(RGB_WHITE);
}

unsigned char TestForFence(void) {
  unsigned char triggerState=raptor.ReadTriggers(LINE_THRESHOLD);
  return (triggerState&CENTER_TRIGGER);
}

void RespToFence(void) {
  raptor.LeftMtrSpeed(0);
  raptor.RightMtrSpeed(0);
  }
