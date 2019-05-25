#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "pitches.h"

#include "digital.h"
#include "buttons.h"
#include "elevator.h"
#include "lights.h"

#define DEBUG_CONTROLLER

////****FOR MAIN***// 
#define DELAY_INTERVAL 1000
#define NUM_OF_FLOORS  5
#define NUM_OF_BUTTONS 8

////****FOR LIGHTS***// 
#define OPEN_DOOR_LED 0
#define UP_LED        6
#define DOWN_LED      7

////****FOR BUTTONS***// 
#define BUTTON_FIRE_1     5
#define BUTTON_FIRE_2     6
#define BUTTON_NO_SERVICE 7

////****FOR FLOORS***// 
#define FLOOR_1 1
#define FLOOR_2 2

////****FOR SOUND***//
#define SOUND_PIN        12
#define SOUND_NOTE_DOOR  4000
#define SOUND_NOTE_MODE  1000
#define SOUND_DELAY 300

//this file "controller.h" is doing a lot of heavy lifting, it holds main objects to make things work
//controller will get called/used thru thru main file. 

//holds info abut what button was pushed for queue, button state being the most important
//others members may get used based on how efficiant we want the queue to work
typedef struct InputHolder 
{
  bool buttonState;
  uint8_t buttonNum;
  long timeStamp;  
} InputHolder;

//keeps tarck of controller state, this is how we change 'elevator mode'
enum ControllerState { NORMAL, FIRE_1, FIRE_2, NO_SERVICE};

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
    void displayAllLights();
    void turnOffDirectionLights();
    void addRequest(uint8_t aButtonNum);
    uint8_t getModeRequest();
    uint8_t getFloorRequest(uint8_t tempCurrentFloor);    
    ElevatorState getTargetDirection(uint8_t tempCurrentFloor,uint8_t tempTargetFloor);
    void showCurrentFloor(uint8_t aNum,ElevatorState aState);
    bool isThisFloorInQueue(uint8_t tempCurrrentFloor);
    void openDoor(uint8_t tempCurrentFloor);
    void makeSound(int pin,int sound,int delayValue);
    void resetQues();
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
  for (uint8_t i = 0; i < NUM_OF_BUTTONS; i++) 
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
          getModeRequest();
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

//after user pushes floor buttons we call this outside update elevator
void Controller::displayAllLights()
{
  lights.displayLights();
}

//certain stage in elevator update we turn these off when elevator reaches target
void Controller::turnOffDirectionLights()
{
  lights.setOffLightQue(UP_LED);
  lights.setOffLightQue(DOWN_LED);
}

//addRequest is also now a place where requests get filtered based on what type it is  floor or mode
//e.g. if controller is in fire mode only mode buttons will get responses/requests
//becuse we dont want floor requests during that time.
void Controller::addRequest(uint8_t aButtonNum)
{
  if(getControllerState()== NORMAL)
  {
    buttonQueue[aButtonNum].buttonState = true;

    //sets correct LED based on floor button pressed
    if(aButtonNum < NUM_OF_FLOORS)
    {
      //floor LED start at index 1 so we have to +1
      lights.setOnLightQue(aButtonNum+1);
    }
  }
  ///we may not want user floor requests but still want mode buttons
  else 
  {
    if(aButtonNum >= NUM_OF_FLOORS)
    {
      buttonQueue[aButtonNum].buttonState = true;
    }
  }
}

//this is where non floor request buttons "Mode buttons" wil get told what to do when pressed
//for example fire mode, we shoudl set the target to floor 1 no stopping
uint8_t Controller::getModeRequest()
{
  for(uint8_t i = NUM_OF_FLOORS; i < NUM_OF_BUTTONS;i++)
  {
    if(buttonQueue[i].buttonState == true)
    {
      //noise that indicates mode button presses
      makeSound(SOUND_PIN, SOUND_NOTE_MODE, SOUND_DELAY);
      
      switch(i)
      {
        case BUTTON_FIRE_1:
        if(getControllerState()!= FIRE_1)
        {
          setControllerState(FIRE_1);
          resetQues();
          elevator.setTargetFloor(FLOOR_1);
          lights.setOnLightQue(FLOOR_1);
          elevator.setState(GOING_DOWN);         
        }
        else
        {
          setControllerState(NORMAL);
          elevator.setTargetFloor(elevator.getCurrentFloor());         
        }        
        break;
        case BUTTON_FIRE_2:
        if(getControllerState()!= FIRE_2)
        {
          setControllerState(FIRE_2);
          resetQues();
          //make sure elavtor is not below floor 2          
          if(elevator.getCurrentFloor() > FLOOR_2)
          {
            elevator.setTargetFloor(FLOOR_2);
            elevator.setState(GOING_DOWN);
            lights.setOnLightQue(FLOOR_2);
          }          
        }
        else
        {
          setControllerState(NORMAL);
          elevator.setTargetFloor(elevator.getCurrentFloor());
        }
        break;
        case BUTTON_NO_SERVICE:
          if(getControllerState()!= NO_SERVICE)
        {
          setControllerState(NO_SERVICE);
          resetQues();
          elevator.setTargetFloor(elevator.getCurrentFloor());
          //setDoorRequest();
          elevator.setState(TARGET_REACHED);
          //lights.setOnLightQue(elevator.getCurrentFloor());
        }
        else
        {
          setControllerState(NORMAL);
          elevator.setTargetFloor(elevator.getCurrentFloor());
          //lights.setOffLightQue(elevator.getCurrentFloor());
          elevator.setState(NOT_IN_USE);
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
//buttons start at 0, floor count starts at 1 so we and add 1  when returning floor from queue
uint8_t Controller::getFloorRequest(uint8_t tempCurrentFloor)
{
  //uint8_t tempCurrentFloor = elevator.getCurrentFloor();
  uint8_t biggestDelta = 0;
  uint8_t biggestDeltaIndex = 0;
  bool buttonPush = false;
  for(uint8_t i =0; i < NUM_OF_FLOORS;i++)
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
        //we need to know if this part of looping routine or actual button press
        buttonPush = true;
        tempIndex = i+1;
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
ElevatorState Controller::getTargetDirection(uint8_t tempCurrentFloor,uint8_t tempTargetFloor)
{
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

//show user floor and direction with LED and Digital Display
void Controller::showCurrentFloor(uint8_t aNum,ElevatorState aState)
{
  //this is how we tell digital to display current floor num 
  digital.updateDigitalShiftRegister(aNum);
  
  //this is how we tell lights to light up currect floor LED and elevator direction LED
  if(aState == GOING_UP)
  {
    lights.setOnLightQue(UP_LED);
  }
  else if(aState == GOING_DOWN)
  {
    lights.setOnLightQue(DOWN_LED);
  }
}

//basicly see if floor elevator is passing is in queue
//if it is we return bool, then set this to false to take it out of que
bool Controller::isThisFloorInQueue(uint8_t tempCurrrentFloor)
{ 
  bool tempButtonState= buttonQueue[tempCurrrentFloor-1].buttonState;
  if(buttonQueue[tempCurrrentFloor-1].buttonState == true)
  {
    buttonQueue[tempCurrrentFloor-1].buttonState = false;
  }
  return tempButtonState;
}

//open door called when target reached or floor stop
//or if same floor as button press while elevator.state is NOT_IN_USE 
void Controller::openDoor(uint8_t tempCurrentFloor)
{
  unsigned long then = millis();
  unsigned long now = 0;
  unsigned long delta = 0;
  
  lights.setOffLightQue(tempCurrentFloor);
  lights.setOnLightQue(OPEN_DOOR_LED);
  while(delta < DELAY_INTERVAL)
  { 
    lights.displayLights();
       
    delta = abs((now= millis()) - then);
  } 
  lights.setOffLightQue(OPEN_DOOR_LED); 
}

//makes sounds for key event notifications 
//wrapped it in function for readability
void Controller::makeSound(int pin,int sound,int delayValue)
{
  tone(pin, sound, delayValue);
}

//helps clears out the fllor and LED ques when we need to change controller modes
void Controller::resetQues()
{
  for(int i = 0;i < NUM_OF_FLOORS;i++)
  {
    buttonQueue[i].buttonState = false;
  }          
  for(int i = 0;i < NUM_OF_BUTTONS;i++)
  {
    //floor lights start at index 1 so need to add 1
    lights.setOffLightQue(i+1);
  }
}

//this function/switch is the lifecycle of the elevator in which elevator.state decides actions
void Controller::upDateElevator()
{   
  switch(elevator.getState())
  {
    case NOT_IN_USE:    
      showCurrentFloor(elevator.getCurrentFloor(),elevator.getState());
      elevator.setTargetFloor(getFloorRequest(elevator.getCurrentFloor()));
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
      showCurrentFloor(elevator.getCurrentFloor(),elevator.getState());
      if(elevator.getCurrentFloor() != elevator.getTargetFloor())
      {
        if(isThisFloorInQueue(elevator.getCurrentFloor()))
        {         
          setTempElevatorState(elevator.getState());
          elevator.setState(FLOOR_STOP);    
        }
        else{elevator.increaseCurrentFloor();} 
      }
      else{setDoorRequest();elevator.setState(TARGET_REACHED);} 
           
      #ifdef DEBUG_CONTROLLER
        Serial.println("STATE = GOING UP");
        Serial.println("SHOW FLOOR "+String(" #"));
      #endif        
      break;
    case GOING_DOWN:
      showCurrentFloor(elevator.getCurrentFloor(),elevator.getState());
      if(elevator.getCurrentFloor() != elevator.getTargetFloor())
      {
        if(isThisFloorInQueue(elevator.getCurrentFloor()))
        {
          setTempElevatorState(elevator.getState());
          elevator.setState(FLOOR_STOP);        
        }
        else{elevator.decreaseCurrentFloor();}  
      }
      else{setDoorRequest(); elevator.setState(TARGET_REACHED);}
      
      #ifdef DEBUG_CONTROLLER
        Serial.println("STATE = GOING DOWN");
        Serial.println("SHOW FLOOR "+String(" #"));
      #endif        
      break;     
      case FLOOR_STOP:
      makeSound(SOUND_PIN, SOUND_NOTE_DOOR, SOUND_DELAY);
      openDoor(elevator.getCurrentFloor());
      elevator.setState(getTempElevatorState());
      
      #ifdef DEBUG_CONTROLLER
        Serial.println("STATE = FLOOR_STOP");
        Serial.println("DOORS OPEN ON FLOOR "+String(elevator.getCurrentFloor()));
      #endif         
      break;   
    case PICK_TARGET_DIRECTION:
      elevator.setState(getTargetDirection(elevator.getCurrentFloor(),elevator.getTargetFloor()));
      
      #ifdef DEBUG_CONTROLLER
        Serial.println("STATE = PICKING TARGET DIRECTION");
        Serial.println("NEW TARGET = "+String(elevator.getTargetFloor()));
      #endif         
      break;     
    case TARGET_REACHED:
      turnOffDirectionLights();
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
