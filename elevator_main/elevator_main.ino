#include <Wire.h>
#include <Arduino.h>
#include "controller.h"

////****FOR DEBUGGING***//
#define DEBUG_MAIN 
#define SERIAL_BAUD 115200
 
//controller file is currently doing a lot of heavy lifting because its keeps the main loop code cleaner
//most of that code will eventually be delegated somewhere else or reconstructed in a better way
//possibly controller will eventually hold all the objects(elevator,buttons,light,digital) and then
//controller will get called/used thru this file. For now this file declares main objects and some
//simple functions, then it helps set up objectsand finally runs the main Arduino loop

////****declares stuff that will be used in Ardunio loop***//
//this is used for throttling elevator update
unsigned long previousMillis=0;
//our controller that runs the show basiclly
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
  ////****FOR DEBUGGING***//
  setUpSerial();
  
  //Wire library used for buttons.h
  Wire.begin();
  
  //set up systems 
  controller.setElevatorFloorNum(NUM_OF_FLOORS); 
  controller.setUpButtons(NUM_OF_BUTTONS);  
  controller.setUpLights(); 
  controller.setUpDigital();  
}

void loop() 
{
  //this is how we throttle the elevator update so 
  //the program has a chance to get button presses
  unsigned long currentMillis = millis();    
  if ((unsigned long)(currentMillis - previousMillis) >= DELAY_INTERVAL) 
  {
    //update function that runs elevator
    controller.upDateElevator();
    previousMillis = currentMillis;
  }
  //update all other systems 
  controller.upDateController();      
}

