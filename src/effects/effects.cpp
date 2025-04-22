#include "effects.h"

#define BUFFER_SIZE 5000
#define LOOPER_BUFFER_SIZE 20000

uint16_t passthrough(uint16_t inputValue, int param1, int param2, int param3)
{
    return inputValue;
}

void defaultSetup(int param1, int param2, int param3)
{
}

void defaultButton(void)
{
}

Effect::Effect(int id, String name, uint16_t color, int param1_min, int param1_max, bool param1_enable, String param1_name, int param2_min, int param2_max, bool param2_enable, String param2_name, int param3_min, int param3_max, bool param3_enable, String param3_name, uint16_t (*process)(uint16_t inputValue, int param1, int param2, int param3), void (*setup)(int param1, int param2, int param3) = defaultSetup, void (*button)(void) = defaultButton)
{
    this->id = id;
    this->name = name;
    this->color = color;
    this->param1_min = param1_min;
    this->param1_max = param1_max;
    this->param1_enable = param1_enable;
    this->param1_name = param1_name;
    this->param2_min = param2_min;
    this->param2_max = param2_max;
    this->param2_enable = param2_enable;
    this->param2_name = param2_name;
    this->param3_min = param3_min;
    this->param3_max = param3_max;
    this->param3_enable = param3_enable;
    this->param3_name = param3_name;

    this->process = process;
    this->setup = setup;
    this->button = button;
}

int effectCount = 4;
int activeEffectIndex = 0;
Effect *activeEffect = nullptr;
boolean bypass = false;

int bufPointer = 0;
uint16_t audioBuffer[BUFFER_SIZE];
uint16_t reverbBuffer[BUFFER_SIZE];
uint16_t looperBuffer[LOOPER_BUFFER_SIZE];

void shiftBuffer(uint16_t sample)
{
    bufPointer++;
    if (bufPointer >= BUFFER_SIZE)
    {
        bufPointer = 0;
    }
    audioBuffer[bufPointer] = sample;
}

uint16_t recallSample(int offset)
{
    int index = bufPointer - offset;
    if (index < 0)
    {
        index += BUFFER_SIZE;
    }
    return audioBuffer[index];
}

uint16_t delay(uint16_t inputValue, int param1, int param2, int param3)
{
    shiftBuffer(inputValue);
    if (param1 == 0)
        return inputValue;
    uint16_t outputValue = recallSample(map(param1, 0, 100, 0, BUFFER_SIZE));
    return (outputValue + inputValue) / 2;
}

uint16_t threshold, outputValue;
uint16_t distortion(uint16_t inputValue, int param1, int param2, int param3)
{
    // Apply distortion effect
    if (param1 == 0)
        return inputValue;
    uint16_t outputValue = inputValue;
    int threshold = map(100 - map(param1, 0, 100, 0, 40), 0, 100, 0, 4095);
    // Serial.printf("%d\t%d\t%d\n", inputValue, threshold, outputValue);
    if (outputValue > threshold)
        outputValue = 4095;
    if (outputValue < 4095 - threshold)
        outputValue = 0;

    return outputValue;
}

short tremoloOffset = 0;
short tremeloPrescale = 0;
uint16_t tremolo(uint16_t inputValue, int param1, int param2, int param3)
{
    // Serial.printf("%d\n", inputValue);
    uint16_t outputValue = map(inputValue, 0, 4095, 0, sin_waveform[tremoloOffset]);
    outputValue = map(outputValue, 0, 255, 0, 4095);

    tremeloPrescale++;
    if (tremeloPrescale >= map(100 - param1, 0, 100, 0, 10))
    {
        tremeloPrescale = 0;
        tremoloOffset += 1;
        if (tremoloOffset >= 1024)
        {
            tremoloOffset = 0;
        }
    }

    return outputValue;
}

Effect *
    effects[] = {
        new Effect(1, "Passthrough", TFT_GREEN, 0, 100, false, "hi", 0, 0, false, "", 0, 0, false, "", passthrough),
        new Effect(2, "Distortion", TFT_RED, 0, 100, true, "Threshold", 0, 40, false, "Param 2", 0, 60, false, "Param 3", distortion),
        new Effect(3, "Tremolo", TFT_PINK, 0, 100, true, "Speed", 0, 40, false, "Param 2", 0, 60, false, "Param 3", tremolo),
        new Effect(4, "Delay", TFT_YELLOW, 0, 100, true, "Delay", 0, 40, false, "Param 2", 0, 60, false, "Param 3", delay),
        // new Effect(5, "Reverb", TFT_PURPLE, 0, 100, true, "Delay", 0, 100, true, "Decay", 0, 60, false, "Param 3", reverb, reverbSetup),
        // new Effect(6, "Looper", TFT_GOLD, 0, 100, true, "Input Volume", 0, 100, true, "Looper Volume", 0, 100, false, "", loopMain, defaultSetup, loopToggle)};
};