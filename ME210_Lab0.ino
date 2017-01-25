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
void checkState(void);
void handleLightOff(void);
void handleLineOff(void);
void handleLineOn(void);
void moveForward(void);
void turnRight(void);
void turnLeft(void);
/*---------------State Definitions--------------------------*/
typedef enum {
   STATE_LIGHT_OFF, STATE_LINE_ON, STATE_LINE_OFF, 
} States_t;

/*---------------Module Variables---------------------------*/
States_t state;
unsigned char isLEDOn;
unsigned char onLine;

/*---------------Raptor Main Functions----------------*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Hello, world!");
  state = STATE_LINE_OFF;
  isLEDOn = false;
  onLine = false;
  TMRArd_InitTimer(0, TIME_INTERVAL);
  TMRArd_InitTimer(1, TIME_INTERVAL);
}

void loop() {
  checkGlobalEvents();
  checkState();
  //printLightLevel();
  //printLineLevel();

  switch(state) {
    case STATE_LIGHT_OFF:
	  handleLightOff();
      break;
    case STATE_LINE_OFF:
      handleLineOff();
      break;
	case STATE_LINE_ON:
	  handleLineOn();
	  break;
    default: //Should never get into an unhandled state
      Serial.println("What is this I do not even...");
  }

}

/*----------------Module Functions--------------------------*/
void checkGlobalEvents(void) {
  if (TestTimer0Expired()) RespTimer0Expired();
  if (TestForKey()) RespToKey();
}

void handleLineOn(void){
	unsigned char triggerState=raptor.ReadTriggers(LINE_THRESHOLD);
	if(triggerState&RIGHT_TRIGGER){
		turnLeft();
	}else if(triggerState&LEFT_TRIGGER){
		turnRight();
	}else{
		moveForward();
	}
}

void checkState(void){
	if(TestForLightOn){
		if(TestForFence&&onLine==false){
		state=STATE_LINE_ON;
		onLine = true;
		}
	}else{
		state = STATE_LIGHT_OFF;
	}
}

void handleLightOff(void){
	raptor.LeftMtrSpeed(0);
	raptor.RightMtrSpeed(0);
}

void handleLineOff(void){
	moveForward();
}


void moveForward(void) {
  raptor.LeftMtrSpeed(25);
  raptor.RightMtrSpeed(25);
}
void turnLeft(void){
	raptor.LeftMtrSpeed(-25);
	raptor.RightMtrSpeed(25);
}
void turnRight(void){
	raptor.LeftMtrSpeed(25);
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

unsigned char TestForLightOff(void) {
    if((raptor.LightLevel() < LIGHT_THRESHOLD)){
    return true;
  }else{
    return false; 
  }
}
