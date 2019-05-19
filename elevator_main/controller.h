#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "digital.h"
#include "buttons.h"
#include "elevator.h"
#include "lights.h"

#define DEBUG_CONTROLLER
#define NUM_OF_FLOORS 5

//this file "controller.h" is currently doing a lot of heavy lifting because its keeps the main loop code cleaner
//some code might get delegated somewhere else or reconstructed in a better way....eventually!!!!
//controller will get called/used thru thru main file. 
//a few of these functions are still prototypes and are for testing/development

//holds info abut what button was pushed for queue, button state being the most important
//others members may get used based on how efficiant we want the queue to work
typedef struct InputHolder 
{
  bool buttonState;
  uint8_t buttonNum;
  long timeStamp;  
} InputHolder;

//order in which the buttons are laid out I use this when doing LED combinations
enum LED_TYPE { LED_DOOR_OPEN, LED_FLOOR_1, LED_FLOOR_2, LED_FLOOR_3, LED_FLOOR_4, LED_FLOOR_5, LED_UP, LED_DOWN };

class Controller
{
  public:
    //constructors
    Controller();
    ~Controller(void);

    void setDoorRequest();
    bool checkDoorRequest();
    void setTempElevatorState(ElevatorState aState);
    ElevatorState getTempElevatorState();
    void setElevatorFloorNum(uint8_t aNum);
    void setUpButtons(uint8_t aNum);
    void setUpLights();
    void setUpDigital();
    void upDateButtons();
    void checkButtons();
    void addFloorRequest(uint8_t aButtonNum);
    uint8_t getFloorRequest();
    ElevatorState getTargetDirection();
    bool changeFloors();
    void showCurrentFloor(uint8_t aNum,ElevatorState aState);
    bool thisFloorInQueue();
    void openDoor();
    void upDateElevator();
        
  private:
    //this will be the que that we feed floor requests from checkButtons()
    InputHolder buttonQueue[8];
    //temp way of doing this till expermint code is done
    uint8_t numOfFloors = 5;
    Lights lights;
    Digital digital;
    Buttons buttons;
    Elevator elevator;
    int buttonCounter = 0;
    bool doorRequest = false;
    ElevatorState tempElevatorState = NOT_IN_USE;  
};

//***   Constructor functions   **//
Controller::Controller()
{
  #ifdef DEBUG_CONTROLLER
    Serial.println("Controller ready.");
  #endif
}

Controller::~Controller(void) 
{
  #ifdef DEBUG_CONTROLLER
    Serial.println("Destructor of Controller called.");
  #endif  
}

void Controller::setDoorRequest()
{
  doorRequest = true;
}
bool Controller::checkDoorRequest()
{
  bool tempDoorRequest = doorRequest;
  doorRequest = false;
  return tempDoorRequest;  
}
void Controller::setTempElevatorState(ElevatorState aState)
{
  tempElevatorState = aState;
}
ElevatorState Controller::getTempElevatorState()
{
  return tempElevatorState;
}

void Controller::setElevatorFloorNum(uint8_t aNum)
{
  elevator.setNumFloors(aNum);
  #ifdef DEBUG_CONTROLLER
    Serial.println("Elevator ready.");
  #endif 
}

void Controller::setUpButtons(uint8_t aNum)
{
  buttons.setUpButtons(aNum); 
}

void Controller::setUpLights()
{
  lights.setUpLights();
}

void Controller::setUpDigital()
{
  digital.setUpDigital();
}

void Controller::upDateButtons()
{
  buttons.callEveryLoop();
}

//this is where our entry for where que will get called when a button is pressed
void Controller::checkButtons() 
{
  for (uint8_t i = 0; i < elevator.getNumberOfFloors(); i++) 
  {
    uint8_t action = buttons.getButtonAction(i);
    if (action != None) 
    {
      switch (action) 
      {
        case Up:
          break;
        case Down:                           
          addFloorRequest(i);
          buttonCounter++;
          #ifdef DEBUG_CONTROLLER           
            Serial.println("BUTTON "+String(i+1)+" Down!");
            Serial.println("BUTTON COUNT = "+String(buttonCounter));
          #endif  
          break;
        default:
          Serial.println(":in check buttons Huh?");
          break;
      }
    }
  }
}

//this gets called when user presses button in check buttons function
//may pass other info if needed e.g. timevalue
void Controller::addFloorRequest(uint8_t aButtonNum)
{
  buttonQueue[aButtonNum].buttonState = true;  
}

//this will be called from the elevator update function
//for now it gets called when elevator is in NOT IN USE state
//button count starts at 0, but floor count starts at 1
//so we and add 1 to the floor value we return from queue
uint8_t Controller::getFloorRequest()
{
  uint8_t tempCurrentFloor = elevator.getCurrentFloor();
  uint8_t biggestDelta = 0;
  uint8_t biggestDeltaIndex = 0;
  bool buttonPush = false;
  for(uint8_t i =0; i < elevator.getNumberOfFloors();i++)
  {
    if(buttonQueue[i].buttonState == true) 
    {
      //keeps index from getting messed up
      //but have floor value we can test with or pass
      uint8_t tempIndex = i;
      
      //if elevator is on same floor as request already, take floor out of queue,
      //open the door and return the current floor back
      if(++tempIndex == tempCurrentFloor)
      {
        //take floor out of queue
        buttonQueue[i].buttonState = false;
        
        setDoorRequest();
        return tempCurrentFloor;
      }
      else //other floors where requested
      {
        //we need to know if this routine or actual button press//elevator loops thru cycle for now it 
        //always button checks even when none presses this helps with that
        buttonPush = true;
        tempIndex = i;
        //get the differance between current floor and requested floor
        //and compare, biggest gets saved and used to get correct index
        uint8_t delta = abs(tempCurrentFloor - ++tempIndex);
        if(delta > biggestDelta)
        {
          biggestDelta = delta;
          biggestDeltaIndex = i;  
        }
      }     
    }
  }
  //this means an actual button on another floor was pressed
  if(buttonPush == true)
  {
    //take out of queue basiclly
    buttonQueue[biggestDeltaIndex].buttonState = false;
    //increase so floor count is right on return
    return ++biggestDeltaIndex;
  }
  else //elavtor was checking loop, no buttons where pushed return currentFloor back as is
  {
    return tempCurrentFloor;
  }  
}

//after we get a floor from queue we pick floor direction
ElevatorState Controller::getTargetDirection()
{
  uint8_t tempCurrentFloor = elevator.getCurrentFloor();
  uint8_t tempTargetFloor = elevator.getTargetFloor();
  if (tempTargetFloor > tempCurrentFloor)
  {
    return GOING_UP;
  }
  else if (tempTargetFloor < tempCurrentFloor)
  {
    return GOING_DOWN;
  }
  else
  {
     return NOT_IN_USE; //means we are on still on same floor just waiting
  }
}

//see if we need to change floors if its same floor, no need
bool Controller::changeFloors()
{  
  return elevator.getCurrentFloor() != elevator.getTargetFloor();
}

//show user thru LED and Digital Display
void Controller::showCurrentFloor(uint8_t aNum,ElevatorState aState)
{
  //this is how we tell digital to display current floor num 
  digital.updateDigitalShiftRegister(aNum);
  //this is how we tell lights to light up currect floor LED and elevator direction LED
  if(aState == GOING_UP)
  {
    lights.comboLight(aNum,LED_UP);
  }
  else if(aState == GOING_DOWN)
  {
    lights.comboLight(aNum,LED_DOWN);
  }
  else
  { //just sitting at floor no direction LED needed
    lights.updateLightShiftRegister(aNum);
  }
}

//basicly see if floor elevator is passing is in queue
bool Controller::thisFloorInQueue()
{ 
  uint8_t tempCurrrentFloor = elevator.getCurrentFloor();
  bool tempButtonState= buttonQueue[tempCurrrentFloor-1].buttonState;
  if(buttonQueue[tempCurrrentFloor-1].buttonState == true)
  {
    buttonQueue[tempCurrrentFloor-1].buttonState = false;   
  }
  return tempButtonState;
}

//delay usage is temp will change to  millis()....Eventually!!!
void Controller::openDoor()
{
  uint8_t tempCurrentFloor = elevator.getCurrentFloor();
  //the door LED is 0 when door open show both door and floor LED's
  //functions from lights.h
  lights.comboLight(tempCurrentFloor,LED_DOOR_OPEN);
  delay(3000);
  lights.updateLightShiftRegister(tempCurrentFloor); 
  delay(1000);
}

//this function/switch is the lifecycle of the elevator in which elevator.state decides actions
//this is still work in progress
void Controller::upDateElevator()
{   
  switch(elevator.getState())
  {
    case NOT_IN_USE:
      showCurrentFloor(elevator.getCurrentFloor(),elevator.getState());
      elevator.setTargetFloor(getFloorRequest());
      if(checkDoorRequest())
      {
        setTempElevatorState(elevator.getState());
        elevator.setState(FLOOR_STOP); 
      }
      else
      {
        elevator.setState(PICK_TARGET_DIRECTION);
      }         
      #ifdef DEBUG_CONTROLLER
        Serial.println("STATE = NOT IN USE");
        Serial.println("CURRENT FLOOR = "+String(elevator.getCurrentFloor()));
      #endif  
      break;
    case GOING_UP:
      //show floor with LED, Digit, and Serial if debug is defined
      showCurrentFloor(elevator.getCurrentFloor(),elevator.getState());
      if(changeFloors())
      {
        //we check to see if this floor is in queue if so we simulate stopping
        if(thisFloorInQueue())
        {         
          setTempElevatorState(elevator.getState());
          elevator.setState(FLOOR_STOP);    
        }
        else
        {
           elevator.increaseCurrentFloor();   
        }  
      }
      else
      {
        setDoorRequest();
        elevator.setState(TARGET_REACHED);
      }
      #ifdef DEBUG_CONTROLLER
        Serial.println("STATE = GOING UP");
        Serial.println("SHOW FLOOR "+String(elevator.getCurrentFloor())+" LED + #");
      #endif        
      break;
    case GOING_DOWN:
      //show floor with LED, Digit, and Serial if debug is defined
      showCurrentFloor(elevator.getCurrentFloor(),elevator.getState());
      if(changeFloors())
      {
        //we check to see if this floor is in queue if so we simulate stopping
        if(thisFloorInQueue())
        {
          setTempElevatorState(elevator.getState());
          elevator.setState(FLOOR_STOP);        
        }
        else
        {
           elevator.decreaseCurrentFloor();   
        }  
      }
      else
      {
        setDoorRequest();
        elevator.setState(TARGET_REACHED);
      }
      #ifdef DEBUG_CONTROLLER
        Serial.println("STATE = GOING DOWN");
        Serial.println("SHOW FLOOR "+String(elevator.getCurrentFloor())+" LED + #");
      #endif        
      break;     
      case FLOOR_STOP:
      openDoor();
      elevator.setState(getTempElevatorState());
      #ifdef DEBUG_CONTROLLER
        Serial.println("STATE = FLOOR_STOP");
        Serial.println("DOORS OPEN ON FLOOR "+String(elevator.getCurrentFloor()));
      #endif         
      break;   
    case PICK_TARGET_DIRECTION:
      elevator.setState(getTargetDirection());
      #ifdef DEBUG_CONTROLLER
        Serial.println("STATE = PICKING TARGET DIRECTION");
        Serial.println("NEW TARGET = "+String(elevator.getTargetFloor()));
      #endif         
      break;     
    case TARGET_REACHED:
      if(checkDoorRequest())
      {
        setTempElevatorState(elevator.getState());
        elevator.setState(FLOOR_STOP);
      }
      else
      {
        elevator.setState(NOT_IN_USE);
      }     
      #ifdef DEBUG_CONTROLLER
        Serial.println("STATE = TARGET REACHED");
        Serial.println("WELCOME TO FLOOR "+String(elevator.getCurrentFloor()));
      #endif
      break;
    default:
      Serial.println("in upDateElevator: Huh?");
      break;
  }
}
#endif
