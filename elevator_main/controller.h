#ifndef CONTROLLER_H
#define CONTROLLER_H

//this file "controller.h" is currently doing a lot of heavy lifting because its keeps the main loop code cleaner
//most of this code will eventually be delegated somewhere else or reconstructed in a better way
//possibly controller will eventually hold all the objects(elevator,buttons,light,digital) and then
//controller will get called/used thru thru main file. 

//these functions are mostly prototypes and will either be redone or there just for testing/development
//for eaxmple passing in objects (elevator for example) because its quicker and easier, when I should be
//passing there information as arguments, which I will change/fix after more functinality is working as intended

//show user thru LED and Digital Display
void showCurrentFloor(uint8_t aNum)
{
  //this is how we tell digital to display current floor num 
  updateDigitalShiftRegister(aNum);
  //this is how we tell lights to light up currect floor LED
  updateLightShiftRegister(aNum);
}

//function that will simulate elevator going up
void goUpFloor(Elevator *aElevator)
//this is still work in progress
{
    if(aElevator->getCurrentFloor() != aElevator->targetFloor)
    {
      aElevator->increaseCurrentFloor();
      showCurrentFloor(aElevator->getCurrentFloor());
      Serial.println("Show Floor "+String(aElevator->getCurrentFloor())+" with led and number!");
      delay(2000);
    }
    else
    {
      aElevator->setState(TARGET_REACHED);
    }    
}
//function that will simulate elevator going up
//this is still work in progress
void goDownFloor(Elevator *aElevator)
{
    if(aElevator->getCurrentFloor() != aElevator->targetFloor)
    {
      aElevator->decreaseCurrentFloor();
      showCurrentFloor(aElevator->getCurrentFloor());
      Serial.println("Show Floor "+String(aElevator->getCurrentFloor())+" with led and number!");
      delay(2000);
    }
    else
    {
      aElevator->setState(TARGET_REACHED);
    }  
}

//after we get a floor from que (currently a random number in NOT_IN_USE state)
//we figure out what floor/direction will be our next destination
//this is still work in progress
void checkForTarget(Elevator *aElevator)
{
  if (aElevator->targetFloor > aElevator->getCurrentFloor())
  {
    Serial.println("we need to go up");
    aElevator->setState(GOING_UP);
  }
  if (aElevator->targetFloor < aElevator->getCurrentFloor())
  {
    Serial.println("we need to go down");
    aElevator->setState(GOING_DOWN);
  }  
}


//this function/switch is like a lifecycle for the elevator in which its state dictates what it should be doing
//this is where it will be told to go up,down,check que,calculate a new taget floor, display current floor
//this is still work in progress
//use of random is for development and will eventually be replased by use of a que
void updateElevator(Elevator *aElevator)
{
  //temp use
  long randNumber;
  //Serial.println(aElevator->getState());
  switch(aElevator->getState())
  {
    case NOT_IN_USE:
      Serial.println("NOT IN USE");
      showCurrentFloor(aElevator->getCurrentFloor());
      //random us temp use and will eventually be replaced by a que
      randNumber = random(1, 6);
      Serial.println("Elevator new Target is "+String(randNumber));
      Serial.println("Elevator current floor is "+String(aElevator->getCurrentFloor()));
      aElevator->targetFloor = randNumber;      
      delay(2000);
      //temp check elevator que
      checkForTarget(aElevator);     
      break;
    case GOING_UP:
      Serial.println("GOING UP"); 
      goUpFloor(aElevator); 
      break;
    case GOING_DOWN:
      Serial.println("GOING DOWN");
      goDownFloor(aElevator);    
      break;
    case PICK_TARGET_FLOOR:
      Serial.println("PICKING TARGET FLOOR");
      break;
    case TARGET_REACHED:
      Serial.println("TARGET REACHED");
      //if no new target for now goto not in use
      delay(2000);
      aElevator->setState(NOT_IN_USE);
      break;
    default:
      //Serial.println(": Huh?");
      break;
  }
}

//this funtion mainly for expermintaion may get used somehow
//this is still work in progress
void showFloorNum(uint8_t buttonNum)
{
  switch(buttonNum)
  {
    case 0:
      Serial.println(1);
      updateDigitalShiftRegister(1);
      updateLightShiftRegister(1);
      break;
    case 1:
      Serial.println(2);
      updateDigitalShiftRegister(2);
      updateLightShiftRegister(2);
      break;
    case 2:
      Serial.println(3);
      updateDigitalShiftRegister(3);
      updateLightShiftRegister(3);
      break;
    case 3:
      Serial.println(4);
      updateDigitalShiftRegister(4);
      updateLightShiftRegister(4);
      break;
    case 4:
      Serial.println(5);
      updateDigitalShiftRegister(5);
      updateLightShiftRegister(5);
      break;
    default:
      Serial.println(": Huh?");
      break;
  }
}

//this is where our entry for where que will get called when a button is pressed
//for now its when button is pressed down, que will take the floor number
//the que will eventually be checked by elevator/controller and updated
//this is still work in progress
void checkButtons(uint8_t aNumOfFloors,Buttons *aButtons) 
{
  for (uint8_t i=0; i<aNumOfFloors; i++) 
  {
    //uint8_t action = elevator->getButtons()->getButtonAction(i);
    uint8_t action = aButtons->getButtonAction(i);
    if (action != None) 
    {
      Serial.print("Button ");
      Serial.print(i);
      switch (action) 
      {
        case Up:
          Serial.println(" Up!");
          break;
        case Down:
          Serial.println(" Down!");
          //function that shows what button is pressed
          //mainly for development and will not be used in this way for final version
          showFloorNum(i);
          //function call for que update eventaully here instead of showFloorNum
          break;
        default:
          Serial.print(": ");
          Serial.print(action);
          Serial.println(": Huh?");
          break;
      }
    }
  }
}
#endif

////****END OF CODE***//

////****EXAMPLE CODE or SCRAP CODE***//
//elevator->getButtons()->callEveryLoop();

//Buttons * Elevator::getButtons()
//{
  //Serial.println("getButtons function");
  //return buttons;
//}

