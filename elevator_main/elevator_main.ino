#include <Wire.h>
#include <Arduino.h>
#include "digital.h"
#include "buttons.h"
#include "elevator.h"
#include "lights.h"
#include "controller.h"

////****FOR DEBUGGING***//
#define DEBUG_MAIN 
#define SERIAL_BAUD 115200
 
////****FOR MAIN***// 
#define DELAY_INTERVAL 4000
#define NUM_OF_FLOORS 5

//controller file is currently doing a lot of heavy lifting because its keeps the main loop code cleaner
//most of that code will eventually be delegated somewhere else or reconstructed in a better way
//possibly controller will eventually hold all the objects(elevator,buttons,light,digital) and then
//controller will get called/used thru this file. For now this file declares main objects and some
//simple functions, then it helps set up objectsand finally runs the main Arduino loop

////****declares stuff that will be used in Ardunio loop***//
//this is used for throttling elevator update
unsigned long previousMillis=0;
Controller controller;

////****FOR DEBUGGING***//
void setUpSerial() 
{
  Serial.begin(SERIAL_BAUD);
  Serial.println("Serial ready!");
}

////****ARDUINO CODE that sets up and runs the loop***//
void setup() 
{
  //Wire library used for buttons.h
  Wire.begin();
  ////****FOR DEBUGGING***//
  setUpSerial(); 
  controller.setElevatorFloorNum(NUM_OF_FLOORS); 
  //controller.setUpButtons(NUM_OF_FLOORS);  
}

void loop() 
{
  //this is how we throttle the elevator update so 
  //the program has a chance to get button presses
  unsigned long currentMillis = millis();    
  if ((unsigned long)(currentMillis - previousMillis) >= DELAY_INTERVAL) 
  {
    controller.upDateElevator();
    previousMillis = currentMillis;
  } 
  controller.upDateButtons();
  controller.checkButtons();       
}

