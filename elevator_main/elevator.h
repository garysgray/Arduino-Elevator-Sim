#ifndef ELEVATOR_H
#define ELEVATOR_H

#define DEBUG_ELEVATOR
//object that will simulate a elevator by holding members that will represent elevator info and state
//most functinality will be getting, setting, and checking  members, controller will do a lot of the work 


enum ElevatorState { NOT_IN_USE, GOING_UP, GOING_DOWN, FLOOR_STOP, PICK_TARGET_DIRECTION, TARGET_REACHED };

class Elevator
{
   public:  
        //temp members that will be made public at some point
              
        //constructors
        Elevator();
        ~Elevator(void);

        //setters for private members
        void setNumFloors(uint8_t aNum);
        void setState(ElevatorState aState);
        void setTempElevatorState(ElevatorState aState);
        void setTargetFloor(uint8_t aTargetFloor);
        void increaseCurrentFloor();
        void decreaseCurrentFloor();
        void setDoorRequest();

        //getters for private members
        ElevatorState getState();
        ElevatorState getTempElevatorState(); 
        uint8_t getCurrentFloor();
        uint8_t getNumberOfFloors();
        uint8_t getTargetFloor();

        //checkers for elevator update in control
        bool checkDoorRequest(); 
        ElevatorState getTargetDirection();
        bool isThisTargetFloor();
        void changeFloorProcess(bool isItinQue);
        void doFloorStop(bool isThereDoorRequest);
               
    private:
        ElevatorState elevatorState = NOT_IN_USE;
        //this comes in handy when different states can lead to same place
        //use of temp will help get back to where we started in those situations
        ElevatorState tempElevatorState = NOT_IN_USE;
        uint8_t numFloors;
        uint8_t currentFloor;
        uint8_t targetFloor;
        bool doorRequest = false;
};

//***   Constructor functions   **//
Elevator::Elevator()
{
    numFloors = 0;
    elevatorState = NOT_IN_USE;
    currentFloor = 1;
    targetFloor = 1;     
}

Elevator::~Elevator(void) 
{
    #ifdef DEBUG_ELEVATOR
        Serial.println("Destructor of Elevator called.");
    #endif
}

//*** setters functions for private members **//
void Elevator::setNumFloors(uint8_t aNum) 
{
    numFloors = aNum;
    #ifdef DEBUG_ELEVATOR
        Serial.println("Elevator Ready");
    #endif
}

void Elevator::setState(ElevatorState aState) 
{
    elevatorState = aState;
}

void Elevator::setTempElevatorState(ElevatorState aState)
{
  tempElevatorState = aState;
}

void Elevator::setTargetFloor(uint8_t aTargetFloor)
{
    targetFloor = aTargetFloor;
}

void Elevator::increaseCurrentFloor() 
{
    currentFloor++;
}

void Elevator::decreaseCurrentFloor() 
{
    currentFloor--;
}

void Elevator::setDoorRequest()
{
  doorRequest = true;
}

//*** getters functions for private members **//
ElevatorState Elevator::getState() 
{
    return elevatorState;
}

ElevatorState Elevator::getTempElevatorState()
{
  return tempElevatorState;
}

uint8_t Elevator::getCurrentFloor() 
{
    return currentFloor;
}

uint8_t Elevator::getNumberOfFloors() 
{
    return numFloors;
}

uint8_t Elevator::getTargetFloor() 
{
    return targetFloor;
}

bool Elevator::checkDoorRequest()
{
  bool tempDoorRequest = doorRequest;
  doorRequest = false;
  return tempDoorRequest;  
}

//after we get a floor from queue we pick floor direction
ElevatorState Elevator::getTargetDirection()
{
  if (targetFloor > currentFloor)
  {
    return GOING_UP;
  }
  else if (targetFloor < currentFloor)
  {
    return GOING_DOWN;
  }
  else
  {
     return NOT_IN_USE; //means we are on still on same floor just waiting
  }
}
bool Elevator::isThisTargetFloor()
{
  return targetFloor != currentFloor;
}

void Elevator::changeFloorProcess(bool isItinQue)
{
  if(isThisTargetFloor())
  {
    if(isItinQue) 
    {
      setTempElevatorState(elevatorState);
      setState(FLOOR_STOP);        
    }
    else
    {
      if(elevatorState== GOING_UP)
      {
        increaseCurrentFloor();
      }
      else if(elevatorState== GOING_DOWN)
      {
        decreaseCurrentFloor(); 
      }             
    }
  }
  else
  {
    setDoorRequest();
    setState(TARGET_REACHED);
  }      
}

void Elevator::doFloorStop(bool isThereDoorRequest)
{
  if(isThereDoorRequest)
  {
    setTempElevatorState(elevatorState);
    setState(FLOOR_STOP); 
  }
  else
  {
    if(elevatorState == NOT_IN_USE)
    {
       setState(PICK_TARGET_DIRECTION);
    }  
    else if(elevatorState == TARGET_REACHED)
    {
      setState(NOT_IN_USE);
    }    
  }
}
#endif

