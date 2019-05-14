#ifndef ELEVATOR_H
#define ELEVATOR_H

//object that will simulate a elevator by holding members that will represent elevator info and state
//most functinality will be getting and setting mebers, controller will do a lot of the work 
//some functions are just placeholders/experimental and may not get actually used, 
//kind of depending on what controller will delegate

enum ElevatorState { NOT_IN_USE, GOING_UP, GOING_DOWN,PICK_TARGET_FLOOR,TARGET_REACHED };

class Elevator
{
   public:  
        //temp members that will be made public at some point
        uint8_t targetFloor;
        //members that may be added later
        //Buttons * getButtons()
        //que
        
        //constructors
        Elevator(uint8_t aNumFloors);
        ~Elevator(void);

        //setters for private members
        void setState(uint8_t aState);
        void increaseCurrentFloor();
        void decreaseCurrentFloor();

        //getters for private members
        ElevatorState getState(); 
        uint8_t getCurrentFloor();
        uint8_t getNumberOfFloors();

        //functions 
        //this function are placeholders for now and eventaully will do stuff
        void updateQue();
        void calculateTarget();
        void updateElevator();
        
    private:
        ElevatorState elevatorState;
        uint8_t numFloors;
        uint8_t currentFloor;
};

//***   Constructor functions   **//
Elevator::Elevator(uint8_t aNumFloors)
{
    numFloors = aNumFloors;
    elevatorState = NOT_IN_USE;
    currentFloor = 1;
    targetFloor = 0;
    //buttons = new Buttons(NUM_BUTTONS, INPUT_PIN, LATCH_PIN, DATA_PIN, CLOCK_PIN);
    Serial.println("Constructor of Elevator called.");   
}

Elevator::~Elevator(void) 
{
    #ifdef DEBUG_BUTTONS
        Serial.println("Destructor of Elevator called.");
    #endif
}

//*** setters functions for private members **//
void Elevator::setState(uint8_t aState) 
{
    elevatorState = aState;
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

//*** Functions **//
void Elevator::updateQue()
{
  Serial.println("updateQue function");
}

void Elevator::updateElevator()
{
  Serial.println("updateElevator function");
}

void Elevator::calculateTarget()
{
  Serial.println("calculateTarget function");
}
#endif

