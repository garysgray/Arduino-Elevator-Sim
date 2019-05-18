#ifndef LIGHTS_H
#define LIGHTS_H

//code taken from example from Elegoo Super Starter Kit for UNO V1.0.2017.7.9 PDF: Lesson 16 Eight LED with 74HC595
// may eventually make this a class and have it be treated as an object in main code like the buttons and elevator objects
//for now these objects and functions can be called anywhere 

#define LIGHTS_CLOCK_PIN 5 // (5) SH_CP [SCK] on 74HC595
#define LIGHTS_LATCH_PIN 6 // (6) ST_CP [RCK] on 74HC595
#define LIGHTS_DATA_PIN  7 // (7) DS [S1] on 74HC595

class Lights
{
    public:
        
        //constructors
        Lights();
        ~Lights(void);
        
        void setUpLights();
        void comboLight(uint8_t aBit1,uint8_t aBit2);
        void updateLightShiftRegister(byte aDigit);
        
        
    private:
    byte    lightsArray[8] = 
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
};
//***   Constructor functions   **//
Lights::Lights()
{
    setUpLights();
}
Lights::~Lights(void) 
{
  Serial.println("Destructor of lights called.");
}

void Lights::setUpLights() 
{
  pinMode(LIGHTS_LATCH_PIN, OUTPUT);
  pinMode(LIGHTS_DATA_PIN, OUTPUT);  
  pinMode(LIGHTS_CLOCK_PIN, OUTPUT);
}

void Lights::updateLightShiftRegister(byte aDigit)
{
   digitalWrite(LIGHTS_LATCH_PIN, LOW);
   shiftOut(LIGHTS_DATA_PIN, LIGHTS_CLOCK_PIN, LSBFIRST, lightsArray[aDigit]);
   digitalWrite(LIGHTS_LATCH_PIN, HIGH);
}

//so way i layed out LED's they go left-right 
//0=door open 1=floor 1 2= floor 2 ect.ect.
//Need MSBFIRST to make this work or combo is backwards
void Lights::comboLight(uint8_t aBit1,uint8_t aBit2)
{
  byte bitBuffer = 0;
  bitWrite(bitBuffer, abs(aBit1), HIGH);
  bitWrite(bitBuffer, abs(aBit2), HIGH);
  digitalWrite(LIGHTS_LATCH_PIN, LOW);
  shiftOut(LIGHTS_DATA_PIN, LIGHTS_CLOCK_PIN, MSBFIRST , bitBuffer);
  digitalWrite(LIGHTS_LATCH_PIN, HIGH);
}

#endif
