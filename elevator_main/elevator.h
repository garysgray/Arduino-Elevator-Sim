#ifndef ELEVATOR_H
#define ELEVATOR_H

#define DEBUG_ELEVATOR
//object that will simulate a elevator by holding members that will represent elevator info and state
//most functinality will be getting and setting mebers, controller will do a lot of the work 
//some functions are just placeholders/experimental and may not get actually used, 
//kind of depending on what controller will delegate

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
        void setTargetFloor(uint8_t aTargetFloor);
        void increaseCurrentFloor();
        void decreaseCurrentFloor();

        //getters for private members
        ElevatorState getState(); 
        uint8_t getCurrentFloor();
        uint8_t getNumberOfFloors();
        uint8_t getTargetFloor();
               
    private:
        ElevatorState elevatorState;
        uint8_t numFloors;
        uint8_t currentFloor;
        uint8_t targetFloor;
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

//*** getters functions for private members **//
ElevatorState Elevator::getState() 
{
    return elevatorState;
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

#endif

