#ifndef CONTROLLER_H
#define CONTROLLER_H

#define DEBUG_CONTROLLER
#define NUM_OF_FLOORS 5
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

class Controller
{
  public:
    //constructors
    Controller();
    ~Controller(void);

    void checkButtons();
    void addFloorRequest(uint8_t aButtonNum);
    uint8_t getFloorRequest();
    ElevatorState getTargetDirection();
    bool changeFloors();
    void showCurrentFloor(uint8_t aNum);
    bool thisFloorInQueue();
    void openDoor();
    void upDateElevator();
    void upDateButtons();
    void setElevatorFloorNum(uint8_t aNum);
    
    
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
  
};

//***   Constructor functions   **//
Controller::Controller()
{
    //setUpElevator();
}
Controller::~Controller(void) 
{
  Serial.println("Destructor of Controller called.");
}

void Controller::upDateButtons()
{
  buttons.callEveryLoop();
}

//this is where our entry for where que will get called when a button is pressed
//this is still work in progress
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
          #ifdef DEBUG_CONTROLLER
            
            //add the 1 because arrays count from 0 but our floors start a 1
            Serial.println("BUTTON "+String(i+1)+" Down!");
            Serial.println("BUTTON COUNT = "+String(buttonCounter));
          #endif          
          addFloorRequest(i);
          buttonCounter++;
          break;
        default:
          Serial.println(": Huh?");
          break;
      }
    }
  }
}

void Controller::setElevatorFloorNum(uint8_t aNum)
{
  elevator.setNumFloors(aNum); 
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
        openDoor();
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
     return NOT_IN_USE;
  }
}

//see if we need to change floors if its same floor, no need
bool Controller::changeFloors()
{  
  return elevator.getCurrentFloor() != elevator.getTargetFloor();
}

//show user thru LED and Digital Display
void Controller::showCurrentFloor(uint8_t aNum)
{
  //this is how we tell digital to display current floor num 
  digital.updateDigitalShiftRegister(aNum);
  //this is how we tell lights to light up currect floor LED
  lights.updateLightShiftRegister(aNum);
}

//basicly see if floor elevator is passing is in queue
bool Controller::thisFloorInQueue()
{ 
  uint8_t tempCurrrentFloor = elevator.getCurrentFloor();
  bool tempState= buttonQueue[tempCurrrentFloor-1].buttonState;
  if(buttonQueue[tempCurrrentFloor-1].buttonState == true)
  {
    buttonQueue[tempCurrrentFloor-1].buttonState = false;   
  }
  return tempState;
}

//delay usage is temp will change to  millis()
void Controller::openDoor()
{
  uint8_t tempCurrentFloor = elevator.getCurrentFloor();
  //the door LED is 0 when door open show both door and floor LED's
  //functions from lights.h
  lights.comboLight(0,tempCurrentFloor);
  delay(3000);
  lights.updateLightShiftRegister(tempCurrentFloor); 
  delay(1000);
}

//this function/switch is like a lifecycle for the elevator in which its state dictates what it should be doing
//this is still work in progress
void Controller::upDateElevator()
{ 
  ElevatorState tempState;
  switch(elevator.getState())
  {
    case NOT_IN_USE:
      showCurrentFloor(elevator.getCurrentFloor());
      elevator.setTargetFloor(getFloorRequest()); 
      elevator.setState(PICK_TARGET_FLOOR);    
      #ifdef DEBUG_CONTROLLER
        Serial.println("NOT IN USE");
        Serial.println("CURRENT FLOOR = "+String(elevator.getCurrentFloor()));
      #endif  
      break;
    case GOING_UP:
      //show floor with LED, Digit, and Serial if debug is defined
      showCurrentFloor(elevator.getCurrentFloor());
      if(changeFloors())
      {
        //we check to see if this floor is in queue if so we simulate stopping
        if(thisFloorInQueue())
        {         
          openDoor();
          #ifdef DEBUG_CONTROLLER
            Serial.println("GOING UP");
            Serial.println("SHOW FLOOR "+String(elevator.getCurrentFloor())+" LED + #");
          #endif          
        }
        else
        {
           elevator.increaseCurrentFloor();   
        }  
      }
      else
      {
        elevator.setState(TARGET_REACHED);
      }
      #ifdef DEBUG_CONTROLLER
        Serial.println("GOING UP");
        Serial.println("SHOW FLOOR "+String(elevator.getCurrentFloor())+" LED + #");
      #endif        
      break;
    case GOING_DOWN:
      //show floor with LED, Digit, and Serial if debug is defined
      showCurrentFloor(elevator.getCurrentFloor());
      if(changeFloors())
      {
        //we check to see if this floor is in queue if so we simulate stopping
        if(thisFloorInQueue())
        {
          openDoor();
          #ifdef DEBUG_CONTROLLER
            Serial.println("FLOOR STOP");
            Serial.println("OPEN DOORS");
          #endif         
        }
        else
        {
           elevator.decreaseCurrentFloor();   
        }  
      }
      else
      {
        elevator.setState(TARGET_REACHED);
      }
      #ifdef DEBUG_CONTROLLER
        Serial.println("GOING DOWN");
        Serial.println("SHOW FLOOR "+String(elevator.getCurrentFloor())+" LED + #");
      #endif        
      break;   
    case PICK_TARGET_FLOOR:
      tempState = getTargetDirection();
      elevator.setState(tempState);
      #ifdef DEBUG_CONTROLLER
        Serial.println("PICKING TARGET FLOOR");
        Serial.println("NEW TARGET = "+String(elevator.getTargetFloor()));
      #endif         
      break;     
    case TARGET_REACHED:
      openDoor();
      elevator.setState(NOT_IN_USE);
      #ifdef DEBUG_CONTROLLER
        Serial.println("TARGET REACHED");
      #endif
      break;
    default:
      Serial.println(": Huh?");
      break;
  }
}
#endif

