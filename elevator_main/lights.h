//code taken from example from Elegoo Super Starter Kit for UNO V1.0.2017.7.9 PDF: Lesson 16 Eight LED with 74HC595 

#ifndef LIGHTS_H
#define LIGHTS_H

#define DEBUG_LIGHTS

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
        void setOnLightQue(uint8_t aBit);
        void setOffLightQue(uint8_t aBit);
        void displayLights();
                      
    private:
    byte    lightsQueue;
    
};
//***   Constructor functions   **//
Lights::Lights()
{
  #ifdef DEBUG_LIGHTS
      Serial.println("constructor of lights called.");
  #endif 
}
Lights::~Lights(void) 
{
  #ifdef DEBUG_LIGHTS
      Serial.println("Destructor of lights called.");
  #endif  
}

void Lights::setUpLights()
{
  pinMode(LIGHTS_LATCH_PIN, OUTPUT);
  pinMode(LIGHTS_DATA_PIN, OUTPUT);  
  pinMode(LIGHTS_CLOCK_PIN, OUTPUT);
  #ifdef DEBUG_LIGHTS
      Serial.println("Lights ready.");
  #endif
}

void Lights::setOnLightQue(uint8_t aBit)
{
  bitWrite(lightsQueue, aBit, HIGH);
}

void Lights::setOffLightQue(uint8_t aBit)
{
  bitWrite(lightsQueue, aBit, LOW);
}

void Lights::displayLights()
{
  digitalWrite(LIGHTS_LATCH_PIN, LOW);
  shiftOut(LIGHTS_DATA_PIN, LIGHTS_CLOCK_PIN, MSBFIRST , lightsQueue);
  digitalWrite(LIGHTS_LATCH_PIN, HIGH);
}

#endif
