#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "digital.h"
#include "buttons.h"
#include "elevator.h"
#include "lights.h"

#define DEBUG_CONTROLLER

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
enum ButtonType { BUTTON_F1, BUTTON_F2, BUTTON_F3, BUTTON_F4, BUTTON_F5, BUTTON_FIRE, BUTTON_STOP ,BUTTON_NO_SERVICE};
enum ControllerState { NORMAL, FIRE, STOP, NO_SERVICE};

class Controller
{
  public:
    //CONSTRUCTORS
    Controller();
    ~Controller(void);
    
    //SET UP SYSTEMS
    void setElevatorFloorNum(uint8_t aNum);
    void setUpButtons(uint8_t aNum);
    void setUpLights();
    void setUpDigital();

    //SETTERS
    void setDoorRequest();
    void setTempElevatorState(ElevatorState aState);
    void setControllerState(ControllerState aState);
    
    //GETTERS
    bool checkDoorRequest();    
    ElevatorState getTempElevatorState();    
    ControllerState getControllerState();

    //MAIN LOOP FUNCTIONS
    void upDateButtons();
    void checkButtons();
    void addRequest(uint8_t aButtonNum);
    uint8_t getModeRequest();
    uint8_t getFloorRequest();    
    ElevatorState getTargetDirection();
    bool changeFloors();
    void showCurrentFloor(uint8_t aNum,ElevatorState aState);
    bool isThisFloorInQueue();
    void openDoor();
    void upDateElevator();
        
  private:
    //this will be the que that we feed floor requests from checkButtons()
    InputHolder buttonQueue[8];
    Lights lights;
    Digital digital;
    Buttons buttons;
    Elevator elevator;
    int buttonCounter = 0;
    bool doorRequest = false;
    ElevatorState tempElevatorState = NOT_IN_USE;
    ControllerState controllerState = NORMAL;  
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

//***   SET UP SYSTEMS   **//
void Controller::setElevatorFloorNum(uint8_t aNum)
{
  elevator.setNumFloors(aNum);
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

//***   SETTERS   **//
void Controller::setDoorRequest()
{
  doorRequest = true;
}

void Controller::setTempElevatorState(ElevatorState aState)
{
  tempElevatorState = aState;
}

void Controller::setControllerState(ControllerState aState)
{
  controllerState = aState;
}

//***   GETTERS  **//
//sets to false auto when user checks it
bool Controller::checkDoorRequest()
{
  bool tempDoorRequest = doorRequest;
  doorRequest = false;
  return tempDoorRequest;  
}

ElevatorState Controller::getTempElevatorState()
{
  return tempElevatorState;
}

ControllerState Controller::getControllerState()
{
  return controllerState;
}

//***   MAIN LOOP FUNCTIONS   **//
void Controller::upDateButtons()
{
  buttons.callEveryLoop();
}

//this is where our entry for where que will get called when a button is pressed
void Controller::checkButtons() 
{
  for (uint8_t i = 0; i < buttons.getNumberOfButtons(); i++) 
  {
    uint8_t action = buttons.getButtonAction(i);
    if (action != None) 
    {
      switch (action) 
      {
        case Up:
          break;
        case Down:                           
          addRequest(i);
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

//addRequest is also now a place where requests get filtered based on what type it is  floor or mode
//e.g. if controller is in fire mode only mode buttons will get responses/requests
//becuse we dont want floor request during that time.
void Controller::addRequest(uint8_t aButtonNum)
{
  if(getControllerState()== NORMAL)
  {
    buttonQueue[aButtonNum].buttonState = true;
  }
  ///we may not want user floor requests but still want mode buttons
  else if(getControllerState() == STOP || getControllerState()== NO_SERVICE)
  {
    if(aButtonNum >= elevator.getNumberOfFloors())
    {
      buttonQueue[aButtonNum].buttonState = true;
    }
  }
  else if(getControllerState() == FIRE)
  {
    //floor requests lower then elevator to force going down, but still need mode buttons
    if(aButtonNum < elevator.getCurrentFloor()||aButtonNum >= elevator.getNumberOfFloors())
    {
      buttonQueue[aButtonNum].buttonState = true;
    }
  }
  //so if a mode button gets pushed we deal with it in this function
  getModeRequest();
}

//this is where non floor request buttons "Mode buttons" wil get told what to do when pressed
//for example fire mode, we shoudl set the target to floor 1 have it hit each floor on way down
//so we fill the Queue which is why que needs a little more functinality
//when back to normal mode, set target to current floor and empty queue
uint8_t Controller::getModeRequest()
{
  for(uint8_t i = elevator.getNumberOfFloors(); i < buttons.getNumberOfButtons();i++)
  {
    if(buttonQueue[i].buttonState == true)
    {
      switch(i)
      {
        case BUTTON_FIRE:
        //we set the target to floor 1 and fill the Queue to hit each floor down
        //we set the addRequest() to not take floor request during Fire Mode as well
        //when set back to normal set current floor to target and empty queue
        if(getControllerState()!= FIRE)
        {
          setControllerState(FIRE);
          for(uint8_t i =1;i< elevator.getCurrentFloor();i++)
          {
            buttonQueue[i].buttonState = true;
          }  
          elevator.setTargetFloor(1);
          #ifdef DEBUG_CONTROLLER           
            Serial.println("CONTROLLER FIRE MODE");
          #endif
        }
        else
        {
          setControllerState(NORMAL);
          elevator.setTargetFloor(elevator.getCurrentFloor());
          for(uint8_t i =0;i< elevator.getNumberOfFloors();i++)
          {
            buttonQueue[i].buttonState = false;
          }  
          #ifdef DEBUG_CONTROLLER           
            Serial.println("CONTROLLER NORMAL MODE");           
          #endif 
        }        
        break;
        case BUTTON_STOP:
          if(getControllerState()!= STOP)
        {
          setControllerState(STOP);
          #ifdef DEBUG_CONTROLLER           
            Serial.println("CONTROLLER STOP MODE");
          #endif
        }
        else
        {
          setControllerState(NORMAL);
          #ifdef DEBUG_CONTROLLER           
            Serial.println("CONTROLLER NORMAL MODE");
          #endif

        }
        break;
        case BUTTON_NO_SERVICE:
          if(getControllerState()!= NO_SERVICE)
        {
          setControllerState(NO_SERVICE);
          #ifdef DEBUG_CONTROLLER           
            Serial.println("CONTROLLER NO SERVICE MODE");
          #endif
        }
        else
        {
          setControllerState(NORMAL);
          #ifdef DEBUG_CONTROLLER           
            Serial.println("CONTROLLER NORMAL MODE");
          #endif
        }
        break;
        default:
        Serial.println("in getModeRequest: Huh?");
        break;
      }
    }
    //what ever button set it to false now so code does not repeat
    buttonQueue[i].buttonState = false;
  }
}

//this gets called when elevator is in NOT IN USE state
//buttons start at 0, floor count starts at 1 so we and add 1  return floor from queue
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
      
      //if elevator is on same floor as request already, take floor out of queue, and open doors
      //current floor gets returned as target floor
      if(++tempIndex == tempCurrentFloor)
      {
        //take floor out of queue
        buttonQueue[i].buttonState = false;       
        setDoorRequest();
        return tempCurrentFloor;
      }
      else //other floors where requested so get the delta
      {
        //we need to know if this looproutine or actual button press
        buttonPush = true;
        tempIndex = i;
        tempIndex++;
        //biggestDelta gets saved and used to get correct index
        uint8_t delta = abs(tempCurrentFloor - tempIndex);
        if(delta > biggestDelta)
        {
          biggestDelta = delta;
          biggestDeltaIndex = i;  
        }
      }     
    }
  }
  //this helps confirm biggest delta was from a user on another floor, so we make it the target floor
  if(buttonPush == true)
  {
    //take out of queue basiclly
    buttonQueue[biggestDeltaIndex].buttonState = false;
    //increase index so floor count is correct on return
    return ++biggestDeltaIndex;
  }
  else //elavtor was checking loop, no buttons where pushed return currentFloor back as target
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
bool Controller::isThisFloorInQueue()
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
      else{elevator.setState(PICK_TARGET_DIRECTION);}         
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
        if(isThisFloorInQueue())
        {         
          setTempElevatorState(elevator.getState());
          elevator.setState(FLOOR_STOP);    
        }
        else{elevator.increaseCurrentFloor();} 
      }
      else{setDoorRequest();elevator.setState(TARGET_REACHED);}      
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
        if(isThisFloorInQueue())
        {
          setTempElevatorState(elevator.getState());
          elevator.setState(FLOOR_STOP);        
        }
        else{elevator.decreaseCurrentFloor();}  
      }
      else{setDoorRequest(); elevator.setState(TARGET_REACHED);}
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
      else{elevator.setState(NOT_IN_USE);}     
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
