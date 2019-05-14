#ifndef LIGHTS_H
#define LIGHTS_H

//code taken from example from Elegoo Super Starter Kit for UNO V1.0.2017.7.9 PDF: Lesson 16 Eight LED with 74HC595
// may eventually make this a class and have it be treated as an object in main code like the buttons and elevator objects
//for now these objects and functions can be called anywhere 

#define LIGHTS_CLOCK_PIN 5 // (5) SH_CP [SCK] on 74HC595
#define LIGHTS_LATCH_PIN 6 // (6) ST_CP [RCK] on 74HC595
#define LIGHTS_DATA_PIN  7 // (7) DS [S1] on 74HC595

byte lightsArray[8] = 
{ 
    B10000000,  // = OPEN DOOR
    B01000000,  // = 1
    B00100000,  // = 2
    B00010000,  // = 3
    B00001000,  // = 4
    B00000100,  // = 5
    B00000010,  // = UP
    B00000001,  // = DOWN
};

void updateLightShiftRegister(byte digit)
{
   digitalWrite(LIGHTS_LATCH_PIN, LOW);
   shiftOut(LIGHTS_DATA_PIN, LIGHTS_CLOCK_PIN, LSBFIRST, lightsArray[digit]);
   digitalWrite(LIGHTS_LATCH_PIN, HIGH);
}

void setUpLights() 
{
  pinMode(LIGHTS_LATCH_PIN, OUTPUT);
  pinMode(LIGHTS_DATA_PIN, OUTPUT);  
  pinMode(LIGHTS_CLOCK_PIN, OUTPUT);
}

#endif
