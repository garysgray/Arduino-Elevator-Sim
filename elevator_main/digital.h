//code taken from example from Elegoo Super Starter Kit for UNO V1.0.2017.7.9 PDF: Lesson 19 74HC595 And Segment Display

//**Digital Display readout defines and function**//
#ifndef DIGITAL_H
#define DIGITAL_H

#define DEBUG_DIGITAL

#define DISPLAY_CLOCK_PIN 4
#define DISPLAY_LATCH_PIN 3
#define DISPLAY_DATA_PIN  2

class Digital
{
  public:
    //constructors
    Digital();
    ~Digital(void);   
    void updateDigitalShiftRegister(byte aDigit);
    void setUpDigital();
  private:
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
};

//***   Constructor functions   **//
Digital::Digital()
{
  #ifdef DEBUG_DIGITAL
      Serial.println("constructor of Digital called.");
  #endif  
}
Digital::~Digital(void) 
{
  #ifdef DEBUG_DIGITAL
      Serial.println("Destructor of Digital called.");
  #endif  
}

void Digital::setUpDigital()
{
  // Set latchPin, clockPin, dataPin as output
  pinMode(DISPLAY_LATCH_PIN, OUTPUT);
  pinMode(DISPLAY_CLOCK_PIN, OUTPUT);
  pinMode(DISPLAY_DATA_PIN, OUTPUT);
  #ifdef DEBUG_DIGITAL
      Serial.println("Digital ready.");
  #endif 
}

void Digital::updateDigitalShiftRegister(byte aDigit) 
{
  // set the latchPin to low potential, before sending data
  digitalWrite(DISPLAY_LATCH_PIN, LOW);   
  // the original data (bit pattern)
  shiftOut(DISPLAY_DATA_PIN, DISPLAY_CLOCK_PIN, LSBFIRST, digitalNumsArray[aDigit]);   
  // set the latchPin to high potential, after sending data
  digitalWrite(DISPLAY_LATCH_PIN, HIGH);
}
#endif














