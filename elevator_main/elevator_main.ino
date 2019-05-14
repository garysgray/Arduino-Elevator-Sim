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
//comment this out if you dont want the elevator simulation running
//for example if you wanted to test the LED and Display w/button presses
#define ELEVATOR_LIVE
////****FOR MAIN***//
#define NUM_OF_FLOORS 5 

//controller file is currently doing a lot of heavy lifting because its keeps the main loop code cleaner
//most of that code will eventually be delegated somewhere else or reconstructed in a better way
//possibly controller will eventually hold all the objects(elevator,buttons,light,digital) and then
//controller will get called/used thru this file. For now this file declares main objects and some
//simple functions, then it helps set up objectsand finally runs the main Arduino loop

////****declares main objects that will be used in Ardunio loop***//
Buttons *buttons;
#ifdef ELEVATOR_LIVE
    Elevator *elevator;
#endif

////****functions to set up objects before going into the loop***/
void setupElevator(uint8_t aNumofFloors)
{
  #ifdef ELEVATOR_LIVE
    elevator = new Elevator(aNumofFloors);
  #endif
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
  #ifdef ELEVATOR_LIVE
    setupElevator(NUM_OF_FLOORS);
  #endif
  setupButtons();  
  ////*******this function is found in digital.h***//
  setUpDigital();  
  ////*******this function is found in lights.h***//
  setUpLights();    
}

void loop() 
{
    ////****these functions are currently found in controller.h***//
    checkButtons(NUM_OF_FLOORS,buttons);
    #ifdef ELEVATOR_LIVE
        updateElevator(elevator);
    #endif
    ////****this function is found in buttons.h***//
    buttons->callEveryLoop();       
}

