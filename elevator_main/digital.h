//**Digital Display readout defines and function**//
#ifndef DIGITAL_H
#define DIGITAL_H

//code taken from example from Elegoo Super Starter Kit for UNO V1.0.2017.7.9 PDF: Lesson 19 74HC595 And Segment Display
// may eventually make this a class and have it be treated as an object in main code like the buttons and elevator objects
//for now these objects and functions can be called anywhere 

#define DISPLAY_CLOCK_PIN 4
#define DISPLAY_LATCH_PIN 3
#define DISPLAY_DATA_PIN  2

byte digitalNumsArray[10] = 
{ 
    B11111100,  // = 0
    B01100000,  // = 1
    B11011010,  // = 2
    B11110010,  // = 3
    B01100110,  // = 4
    B10110110,  // = 5
    B10111110,  // = 6
    B11100000,  // = 7
    B11111110,  // = 8
    B11100110   // = 9
};
                             
// display a number on the digital segment display
void updateDigitalShiftRegister(byte digit) 
{
  // set the latchPin to low potential, before sending data
  digitalWrite(DISPLAY_LATCH_PIN, LOW);   
  // the original data (bit pattern)
  shiftOut(DISPLAY_DATA_PIN, DISPLAY_CLOCK_PIN, LSBFIRST, digitalNumsArray[digit]);   
  // set the latchPin to high potential, after sending data
  digitalWrite(DISPLAY_LATCH_PIN, HIGH);
}

void setUpDigital()
{
  // Set latchPin, clockPin, dataPin as output
  pinMode(DISPLAY_LATCH_PIN, OUTPUT);
  pinMode(DISPLAY_CLOCK_PIN, OUTPUT);
  pinMode(DISPLAY_DATA_PIN, OUTPUT);  
}

#endif
