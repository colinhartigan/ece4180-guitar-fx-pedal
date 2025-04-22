#include <Arduino.h>
#include "encoder.h"
#include "../effects/effects.h"

// #define button_pin 19
#define encoderA_pin 12
#define encoderB_pin 14

volatile int rotaryCounter;
volatile int rotaryCounterRaw;              // current "position" of rotary encoder (increments CW)
volatile boolean rotaryChange;              // will turn true if rotaryCounter has changed
volatile boolean buttonPressed = false;     // will turn true if the button has been pushed
volatile boolean buttonReleased = false;    // will turn true if the button has been released (sets buttonDowntime)
volatile unsigned long buttonDowntime = 0L; // ms the button was pushed before release
volatile unsigned char previousState = 0;   // previous state
volatile unsigned char currentState = 0;

void initEncoder()
{
    pinMode(encoderA_pin, INPUT_PULLUP);
    pinMode(encoderB_pin, INPUT_PULLUP);
    pinMode(2, OUTPUT); // onboard LED

    attachInterrupt(encoderA_pin, rotaryISR, CHANGE);
    attachInterrupt(encoderB_pin, rotaryISR, CHANGE);

    rotaryCounter = 0;
    rotaryCounterRaw = 0;
    rotaryChange = false;
}

void IRAM_ATTR rotaryISR()
{
    if (rotaryChange)
    {
        return;
    }

    // print current state to serial
    int lookup_table[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

    currentState = digitalRead(encoderA_pin) << 1 | digitalRead(encoderB_pin);
    rotaryCounterRaw = (rotaryCounterRaw - lookup_table[previousState << 2 | currentState]) % 64;
    if (rotaryCounterRaw < 0)
    {
        rotaryCounterRaw += 64;
    }
    previousState = currentState;

    rotaryChange = rotaryCounterRaw / 4 != rotaryCounter;
    rotaryCounter = rotaryCounterRaw / 4; // divide by 4 to get the actual position
}