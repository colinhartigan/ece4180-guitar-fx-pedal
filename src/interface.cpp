#include <SPI.h>
#include <TFT_eSPI.h>
#include "rotary_encoder/encoder.h"
#include "interface.h"
#include "effects/effects.h"

#define POT1_PIN 33
#define POT2_PIN 26
#define POT3_PIN 27

#define FOOTSWITCH 34
#define RPG_Button 5

#define CIRCLE_Y 170

// input defs
TFT_eSPI tft = TFT_eSPI();

// Function prototypes
void interfaceSetup(void);
void interfaceLoop(void *param);
void changeEffect(Effect *newEffect);
void toggleBypass(void);

// variables
int pot1Raw = 0;
int pot2Raw = 0;
int pot3Raw = 0;

String pot1Value;
String pot2Value;
String pot3Value;

// --------------------------------------------------

void interfaceSetup()
{
    // setup display
    tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(0, 0);
    tft.setTextSize(3);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("guitar pedal!", tft.width() / 2, tft.height() / 2);

    // setup rotary encoder
    initEncoder();

    // setup pots
    pinMode(POT1_PIN, INPUT);
    pinMode(POT2_PIN, INPUT);
    pinMode(POT3_PIN, INPUT);
    pinMode(RPG_Button, INPUT_PULLUP);

    // set pin 34 to be an interrupt
    pinMode(FOOTSWITCH, INPUT);
    attachInterrupt(FOOTSWITCH, toggleBypass, CHANGE);

    delay(500);

    changeEffect(effects[0]); // set default effect
}

void toggleBypass()
{
    bypass = !bypass;

    if (bypass)
    {
        tft.setTextSize(3);
        tft.fillRect(0, 70, tft.width(), 50, TFT_RED);
        tft.setTextDatum(MC_DATUM);
        tft.drawString("BYPASS", tft.width() / 2, 95);
    }
    else
    {
        tft.fillRect(0, 70, tft.width(), 50, TFT_BLACK);
    }
}

void changeEffect(Effect *newEffect)
{
    // change effect
    activeEffect = newEffect;

    // update display
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.fillRoundRect(10, 10, 50, 50, 8, activeEffect->color);

    // draw effect name
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(3);
    tft.drawNumber(activeEffect->id, 35, 35);
    tft.setTextDatum(CL_DATUM);
    tft.drawString(activeEffect->name, 70, 35);

    // draw param names
    tft.setTextSize(1.5);
    tft.setTextDatum(TC_DATUM);
    if (activeEffect->param1_enable)
    {
        tft.drawString(activeEffect->param1_name, 60, CIRCLE_Y + 50);
    }
    if (activeEffect->param2_enable)
    {
        tft.drawString(activeEffect->param2_name, 160, CIRCLE_Y + 50);
    }
    if (activeEffect->param3_enable)
    {
        tft.drawString(activeEffect->param3_name, 260, CIRCLE_Y + 50);
    }
    activeEffect->setup(pot1Raw, pot2Raw, pot3Raw);

    pot1Raw = 0;
    pot2Raw = 0;
    pot3Raw = 0;
}

unsigned long buttonSamplePrev = 0;
int buttonSampleInterval = 0.1e6; // 100ms
int buttonPrev = 0;

void interfaceLoop(void *param)
{
    while (true)
    {
        // update pots
        int newPot1Raw = map(analogRead(POT1_PIN), 0, 4095, activeEffect->param1_min, activeEffect->param1_max);
        int newPot2Raw = map(analogRead(POT2_PIN), 0, 4095, activeEffect->param2_min, activeEffect->param2_max);
        int newPot3Raw = map(analogRead(POT3_PIN), 0, 4095, activeEffect->param3_min, activeEffect->param3_max);

        boolean pot1Change = abs(newPot1Raw - pot1Raw) > (int(float(activeEffect->param1_max - activeEffect->param1_min) / 50.0));
        boolean pot2Change = abs(newPot2Raw - pot2Raw) > (int(float(activeEffect->param2_max - activeEffect->param2_min) / 50.0));
        boolean pot3Change = abs(newPot3Raw - pot3Raw) > (int(float(activeEffect->param3_max - activeEffect->param3_min) / 50.0));

        Serial.printf("%d %d %d\n", pot1Change, pot2Change, pot3Change);

        unsigned long time = micros();
        if ((time - buttonSamplePrev) > buttonSampleInterval)
        {
            int newButtonRPG = digitalRead(RPG_Button);
            if (newButtonRPG != buttonPrev)
            {
                // toggleBypass();
                activeEffect->button;
            }
        }

        if (pot1Change || pot2Change || pot3Change)
        {
            tft.setTextSize(3);
            tft.setTextColor(TFT_BLACK);
            tft.setTextDatum(MC_DATUM);
            activeEffect->setup(pot1Raw, pot2Raw, pot3Raw);
        }

        if (activeEffect->param1_enable && pot1Change)
        {
            pot1Raw = newPot1Raw;
            uint32_t pot1Angle = (float(pot1Raw) / float(activeEffect->param1_max)) * float(360);
            tft.fillCircle(60, CIRCLE_Y, 45, TFT_WHITE);
            tft.drawArc(60, CIRCLE_Y, 40, 35, 45, map(pot1Angle, 0, 360, 45, 315), activeEffect->color, activeEffect->color);
            tft.drawString(String(pot1Raw), 60, CIRCLE_Y);
        }
        if (activeEffect->param2_enable && pot2Change)
        {
            pot2Raw = newPot2Raw;
            uint32_t pot2Angle = (float(pot2Raw) / float(activeEffect->param2_max)) * float(360);
            tft.fillCircle(160, CIRCLE_Y, 45, TFT_WHITE);
            tft.drawArc(160, CIRCLE_Y, 40, 35, 45, map(pot2Angle, 0, 360, 45, 315), activeEffect->color, activeEffect->color);
            tft.drawString(String(pot2Raw), 160, CIRCLE_Y);
        }
        if (activeEffect->param3_enable && pot3Change)
        {
            pot3Raw = newPot3Raw;
            uint32_t pot3Angle = (float(pot3Raw) / float(activeEffect->param3_max)) * float(360);
            tft.fillCircle(260, CIRCLE_Y, 45, TFT_WHITE);
            tft.drawArc(260, CIRCLE_Y, 40, 35, 45, map(pot3Angle, 0, 360, 45, 315), activeEffect->color, activeEffect->color);
            tft.drawString(String(pot3Raw), 260, CIRCLE_Y);
        }

        // check if rotary encoder has changed
        if (rotaryChange)
        {
            rotaryChange = false;
            Serial.printf("rotaryCounter: %d\n", rotaryCounter % effectCount);

            changeEffect(effects[rotaryCounter % effectCount]);
        }

        vTaskDelay(1000 / 60 / portTICK_PERIOD_MS);
    }
}