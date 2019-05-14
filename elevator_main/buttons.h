#ifndef BUTTONS_H
#define BUTTONS_H

//code from example found here Written by Michael Sokolewicz. https://www.instructables.com/id/Driving-Dozens-of-Buttons/
// this code can only handle 8 buttons because it's set up to use 8 only

              
#define DEBUG_BUTTONS  
#define NUM_OF_SHIFTS  1               // number of shift registers you have chained together.
#define MAX_BUTTONS (8 * NUM_OF_SHIFTS)
#define DEFAULT_DELAY_MILLIS  50L      // this is the default number of millis between button checks.
#define CLOCK_PIN      8               // 74HC595 SH_CP 
#define LATCH_PIN      9               // 74HC595 ST_CP 
#define DATA_PIN      10               // 74HC595 DS    
#define INPUT_PIN     14               // A0

//each button gets a context(info struct)used to get each buttons current status
enum ButtonAction { None, Up, Down };
// used to store context for each button.
typedef struct ButtonContext 
{
  uint8_t button;
  uint8_t state;  // HIGH or LOW
  unsigned long lastStateChangeMillis;
  ButtonAction action;  // last ButtonAction
} ButtonContext;

class Buttons 
{
    public:
        Buttons(uint8_t numButtons, uint8_t inputPin, uint8_t latchPin, uint8_t dataPin, uint8_t clockPin, unsigned long delay = DEFAULT_DELAY_MILLIS);

        ~Buttons(void);
        // this MUST BE CALLED EVERY LOOP in your script, or class will work properly!
        void callEveryLoop();
        // returns a ButtonAction.when you read the current button action, the action is reset to NONE for that button
        ButtonAction getButtonAction(uint8_t whichButton);

    private:
        uint8_t numButtons;
        uint8_t inputPin;
        uint8_t latchPin;
        uint8_t dataPin;
        uint8_t clockPin;
        uint8_t strobeCurButton;
        unsigned long delayMillis;
        unsigned long checkButtonTimeout;
        // malloc'd dynamically in constructor.
        ButtonContext *buttonContexts;
        uint8_t *strobeNextButton;
};

//*** constructors **//
Buttons::Buttons(uint8_t nb, uint8_t ip, uint8_t lp, uint8_t dp, uint8_t cp, unsigned long delay) 
{
    numButtons = nb;
    
    if (numButtons > MAX_BUTTONS) 
    {
        #ifdef DEBUG_BUTTONS
          Serial.print("Too many buttons: ");
          Serial.println(numButtons);
        #endif
          while (true) ;
    }
    
    inputPin = ip;
    latchPin = lp;
    dataPin = dp;
    clockPin = cp;
    delayMillis = delay;

    buttonContexts = (ButtonContext*)malloc(numButtons * sizeof(ButtonContext));
    // next button strobe array (avoids an if stmt in time-critical function).
    strobeNextButton = (uint8_t*)malloc(numButtons * sizeof(uint8_t));
    // initialize the arrays.
    for (uint8_t i=0; i<numButtons; i++) {
        buttonContexts[i].button = i;
        buttonContexts[i].state = LOW;  // means "not pressed".
        buttonContexts[i].lastStateChangeMillis = 0L;
        buttonContexts[i].action = None;
        
        // index of next button (avoids an if stmt).
        strobeNextButton[i] = i+1;
        #ifdef DEBUG_BUTTONS
          Serial.print("Initialized Button ");
          Serial.println(i);
        #endif
    }

    // last one leads back to index 0.
    strobeNextButton[numButtons-1] = 0;
    strobeCurButton = numButtons-1;
    
    // init the Arduino pins.
    pinMode(inputPin, INPUT);
    pinMode(latchPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);

    // prep next strobe action timeout.
    checkButtonTimeout = 0;
    
    #ifdef DEBUG_BUTTONS
        Serial.println("Buttons ready.");
    #endif
}

//*** destructor used to delete object, espicially its pointers **//
Buttons::~Buttons(void) 
{
    #ifdef DEBUG_BUTTONS
        Serial.print("Destructor of Buttons called.");
    #endif
        free(buttonContexts);
}

//*** Functions **//
ButtonAction Buttons::getButtonAction(uint8_t whichButton) 
{
    // sanity check.
    if ((whichButton < 0) || (whichButton >= numButtons)) 
    {
        return None;
    }
    ButtonAction action = buttonContexts[whichButton].action;
    // once the action is read, we reset it to NONE.
    buttonContexts[whichButton].action = None;    
    return action;
}

//*** loop that gets called in main loop **//
void Buttons::callEveryLoop() 
{
    if (millis() > checkButtonTimeout) 
    {
        // check the next button
        strobeCurButton = strobeNextButton[strobeCurButton];
        
        // tell the shift-registers we're sending a new value to shift.
        digitalWrite(latchPin, LOW);
        
        // shift out the pattern which activates only this button.
        shiftOut(dataPin, clockPin, MSBFIRST, (uint8_t)0x01 << strobeCurButton);
        
        // tell the shift register we're done shifting and it can now send the pattern out its output pins.
        digitalWrite(latchPin, HIGH);

        // at this button is the only one which can be sending a signal, So the the input pin reflects the state of just this button: HIGH or LOW.
        uint8_t buttonState = digitalRead(inputPin);

        // interpret the button state.
        if (buttonState != buttonContexts[strobeCurButton].state) {
            // button state has changed since last call.
            buttonContexts[strobeCurButton].action = (buttonState == LOW) ? Up : Down;
            
            // update these.
            buttonContexts[strobeCurButton].state = buttonState;
            buttonContexts[strobeCurButton].lastStateChangeMillis = millis();
        }
        // set timeout for next button check.
        checkButtonTimeout = millis() + delayMillis;
    }
}
    
#endif   
