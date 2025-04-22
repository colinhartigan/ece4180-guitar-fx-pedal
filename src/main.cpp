#include <Arduino.h>
#include <interface.h>
#include "driver/adc.h"
#include "effects/effects.h"

#define PWMOut 21
#define PWMFreq 19000
#define PWMRes 12

// some variables
TaskHandle_t interfaceTaskHandle = NULL;
TaskHandle_t adcTaskHandle = NULL;

// some prototypes
void adcLoop(void *param);

void setup()
{
    // set up serial
    Serial.begin(115200);

    // run screen setup
    interfaceSetup();

    // run other setup code here
    xTaskCreatePinnedToCore(
        interfaceLoop,        // Function to implement the task
        "Interface Task",     // Name of the task
        10000,                // Stack size in bytes
        NULL,                 // Task input parameter
        1,                    // Priority of the task
        &interfaceTaskHandle, // Task handle
        0                     // core
    );

    // set up ADC pin
    pinMode(35, INPUT);

    // set up PWM pins for audio output
    pinMode(PWMOut, OUTPUT);       // D21
    ledcSetup(0, PWMFreq, PWMRes); // PWM channel 0
    ledcAttachPin(PWMOut, 0);      // attach PWM Channel 0 to pin 21

    // pin processADC to core 1
    xTaskCreatePinnedToCore(
        adcLoop,        // Function to implement the task
        "ADC",          // Name of the task
        10000,          // Stack size in bytes
        NULL,           // Task input parameter
        1,              // Priority of the task
        &adcTaskHandle, // Task handle
        1               // core
    );

    // set up periodic timer/interrupt for sampling, trigger every PWMfreq
}

unsigned long prevTime = 0;
int prevValue = 0;

void adcLoop(void *param)
{
    while (true)
    {
        unsigned long currentTime = micros();
        if (currentTime - prevTime >= (1000000 / PWMFreq))
        {
            uint16_t rawInput = analogRead(35);
            uint16_t inputVal = (rawInput + prevValue) / 2;
            prevValue = rawInput;
            uint16_t outputVal = bypass ? inputVal : activeEffect->process(inputVal, pot1Raw, pot2Raw, pot3Raw);
            ledcWrite(0, outputVal);
            prevTime = currentTime;
        }
        // vTaskDelay(1);
    }
}

void loop()
{
    vTaskDelete(NULL);
}
