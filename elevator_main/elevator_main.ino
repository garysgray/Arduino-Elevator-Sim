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
#define NUM_OF_FLOORS 5 
#define DELAY_INTERVAL 4000

//controller file is currently doing a lot of heavy lifting because its keeps the main loop code cleaner
//most of that code will eventually be delegated somewhere else or reconstructed in a better way
//possibly controller will eventually hold all the objects(elevator,buttons,light,digital) and then
//controller will get called/used thru this file. For now this file declares main objects and some
//simple functions, then it helps set up objectsand finally runs the main Arduino loop

////****declares stuff that will be used in Ardunio loop***//
//this is used for throttling elevator update
unsigned long previousMillis=0;
Buttons *buttons;
Elevator *elevator;

////****functions to set up objects before going into the loop***/
void setupElevator(uint8_t aNumofFloors)
{
    elevator = new Elevator(aNumofFloors);
}

void setupButtons() 
{
    buttons = new Buttons(NUM_OF_FLOORS, INPUT_PIN, LATCH_PIN, DATA_PIN, CLOCK_PIN);
}

////****FOR DEBUGGING***//
void setUpSerial() 
{
    Serial.begin(SERIAL_BAUD);
    #ifdef DEBUG_MAIN
    Serial.println("Serial ready!");
    #endif
}

////****ARDUINO CODE that sets up and runs the loop***//
void setup() 
{
  //Wire library used for buttons.h
  Wire.begin();
  //this is temp for now being used to create random floors in updateElevator function
  //which will eventually be replaced with a que that will be held by controller or elevator
  randomSeed(analogRead(0));
  ////****FOR DEBUGGING***//     
  setUpSerial(); 
  ////****set up all the sub-systems***//  
  ////****these functions are in this file***//
  setupElevator(NUM_OF_FLOORS);
  setupButtons();  
  ////*******this function is found in digital.h***//
  setUpDigital();  
  ////*******this function is found in lights.h***//
  setUpLights();    
}

void loop() 
{
  //this is how we throttle the elevator update so 
  //the program has a chance to get button presses
  unsigned long currentMillis = millis();    
  if ((unsigned long)(currentMillis - previousMillis) >= DELAY_INTERVAL) 
  {
    updateElevator(elevator);
    previousMillis = currentMillis;
  } 
  buttons->callEveryLoop();
  checkButtons(NUM_OF_FLOORS,buttons);       
}

