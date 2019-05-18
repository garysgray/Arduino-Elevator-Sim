#ifndef CONTROLLER_H
#define CONTROLLER_H

#define DEBUG_CONTROLLER

//this file "controller.h" is currently doing a lot of heavy lifting because its keeps the main loop code cleaner
//most of this code will eventually be delegated somewhere else or reconstructed in a better way
//possibly controller will eventually hold all the objects(elevator,buttons,light,digital) and then
//controller will get called/used thru thru main file. 

//these functions are mostly prototypes and will either be redone or there just for testing/development
//for eaxmple passing in objects (elevator for example) because its quicker and easier, when I should be
//passing there information as arguments, which I will change/fix after more functinality is working as intended

//holds info abut what button was pushed for queue, button state being the most important
//others members may get used based on how efficiant we want the queue to work
typedef struct InputHolder 
{
  bool buttonState;
  uint8_t buttonNum;
  long timeStamp;  
} InputHolder;

///
//this will be the que that we feed floor requests from checkButtons()
InputHolder buttonQueue[8];
//temp way of doing this till expermint code is done
uint8_t numOfFloors = 5;

//we can count button presses in debug
#ifdef DEBUG_CONTROLLER
  int buttonCounter = 0;            
#endif 

//this gets called when user presses button in check buttons function
//may pass other info if needed e.g. timevalue
void addFloorRequest(uint8_t aButtonNum)
{
  buttonQueue[aButtonNum].buttonState = true;  
}

//delay usage is temp will change to  millis()
void openDoor(uint8_t aCurrentFloor)
{
  //the door LED is 0 when door open show both door and floor LED's
  //functions from lights.h
    comboLight(0,aCurrentFloor);
    delay(3000);
    updateLightShiftRegister(aCurrentFloor); 
    delay(1000);
}

//this will be called from the elevator update function
//for now it gets called when elevator is in NOT IN USE state
//button count starts at 0, but floor count starts at 1
//so we and add 1 to the floor value we return from queue
uint8_t getFloorRequest(uint8_t aCurrentFloor)
{
  uint8_t biggestDelta = 0;
  uint8_t biggestDeltaIndex = 0;
  bool buttonPush = false;
  for(uint8_t i =0; i < numOfFloors;i++)
  {
    if(buttonQueue[i].buttonState == true) 
    {
      //keeps index from getting messed up
      //but have floor value we can test with or pass
      uint8_t tempIndex = i;
      
      //if elevator is on same floor as request already, take floor out of queue,
      //open the door and return the current floor back
      if(++tempIndex == aCurrentFloor)
      {
        //take floor out of queue
        buttonQueue[i].buttonState = false;
        openDoor(aCurrentFloor);
        return aCurrentFloor;
      }
      else //other floors where requested
      {
        //we need to know if this routine or actual button press//elevator loops thru cycle for now it 
        //always button checks even when none presses this helps with that
        buttonPush = true;
        tempIndex = i;
        //get the differance between current floor and requested floor
        //and compare, biggest gets saved and used to get correct index
        uint8_t delta = abs(aCurrentFloor - ++tempIndex);
        Serial.println("SHOW DELTA "+String(delta));
        if(delta > biggestDelta)
        {
          biggestDelta = delta;
          biggestDeltaIndex = i;  
        }
      }     
    }
  }
  //this means an actual button on another floor was pressed
  //if(buttonPush ==true && biggestDelta != 0)
  if(buttonPush ==true)
  {
    //take out of queue basiclly
    buttonQueue[biggestDeltaIndex].buttonState = false;
    //increase so floor count is right on return
    return ++biggestDeltaIndex;
  }
  else //elavtor was checking loop, no buttons where pushed return currentFloor back
  {
    return aCurrentFloor;
  }  
}

//show user thru LED and Digital Display
void showCurrentFloor(uint8_t aNum)
{
  //this is how we tell digital to display current floor num 
  updateDigitalShiftRegister(aNum);
  //this is how we tell lights to light up currect floor LED
  updateLightShiftRegister(aNum);
}

//basicly see if floor elevator is passing is in queue
void floorCheck(uint8_t aCurrentFloor)
{ 
  if(buttonQueue[aCurrentFloor-1].buttonState == true)
  {
    Serial.println("STOP AT FLOOR "+String(aCurrentFloor));
    openDoor(aCurrentFloor);
    buttonQueue[aCurrentFloor-1].buttonState = false;   
  }
}

//function that will simulate elevator going up and down
void changeFloors(Elevator *aElevator)
{
  if(aElevator->getCurrentFloor() != aElevator->targetFloor)
  {
    if(aElevator->getState()== GOING_UP)
    {
      aElevator->increaseCurrentFloor();
    }
    else if(aElevator->getState()== GOING_DOWN)
    {
      aElevator->decreaseCurrentFloor();
    }
    //show floor with LED, Digit, and Serial if debug is defined
    showCurrentFloor(aElevator->getCurrentFloor());
    //we check to see if this floor is in queue if so we simulate stopping
    floorCheck(aElevator->getCurrentFloor());
    #ifdef DEBUG_CONTROLLER
      Serial.println("SHOW FLOOR "+String(aElevator->getCurrentFloor())+" LED + #");
    #endif
  }
  else
  {
    aElevator->setState(TARGET_REACHED);
  }  
}

//after we get a floor from queue we pick floor direction
ElevatorState getTargetDirection(uint8_t aCurrentFloor(),uint8_t aTargetFloor)
{
  if (aTargetFloor > aCurrentFloor)
  {
    return GOING_UP;
  }
  else if (aTargetFloor < aCurrentFloor)
  {
    return GOING_DOWN;
  }
  else
  {
     return NOT_IN_USE;
  }
}

//this function/switch is like a lifecycle for the elevator in which its state dictates what it should be doing
//this is still work in progress
void updateElevator(Elevator *aElevator)
{ 
  ElevatorState tempState;
  switch(aElevator->getState())
  {
    case NOT_IN_USE:
      showCurrentFloor(aElevator->getCurrentFloor());
      aElevator->targetFloor = getFloorRequest(aElevator->getCurrentFloor());
      aElevator->setState(PICK_TARGET_FLOOR);    
      #ifdef DEBUG_CONTROLLER
        Serial.println("NOT IN USE");
        Serial.println("CURRENT FLOOR = "+String(aElevator->getCurrentFloor()));
      #endif  
      break;
    case GOING_UP:
      changeFloors(aElevator);
      #ifdef DEBUG_CONTROLLER
        Serial.println("GOING UP");
      #endif        
      break;
    case GOING_DOWN:
      changeFloors(aElevator); 
      #ifdef DEBUG_CONTROLLER
        Serial.println("GOING DOWN");
      #endif        
      break;
    case PICK_TARGET_FLOOR:
      tempState = getTargetDirection(aElevator->getCurrentFloor(),aElevator->targetFloor);
      aElevator->setState(tempState);
      #ifdef DEBUG_CONTROLLER
        Serial.println("PICKING TARGET FLOOR");
        Serial.println("NEW TARGET = "+String(aElevator->targetFloor));
      #endif         
      break;     
    case TARGET_REACHED:
      openDoor(aElevator->getCurrentFloor());
      aElevator->setState(NOT_IN_USE);
      #ifdef DEBUG_CONTROLLER
        Serial.println("TARGET REACHED");
      #endif
      //if no new target for now goto not in use
      break;
    default:
      Serial.println(": Huh?");
      break;
  }
}

//this is where our entry for where que will get called when a button is pressed
//this is still work in progress
void checkButtons(uint8_t aNumOfFloors, Buttons *aButtons) 
{
  for (uint8_t i = 0; i < aNumOfFloors; i++) 
  {
    uint8_t action = aButtons->getButtonAction(i);
    if (action != None) 
    {
      switch (action) 
      {
        case Up:
          break;
        case Down:          
          #ifdef DEBUG_CONTROLLER
            buttonCounter++;
            //add the 1 because arrays count from 0 but our floors start a 1
            Serial.println("BUTTON "+String(i+1)+" Down!");
            Serial.println("BUTTON COUNT = "+String(buttonCounter));
          #endif          
          addFloorRequest(i);
          break;
        default:
          Serial.println(": Huh?");
          break;
      }
    }
  }
}
#endif













