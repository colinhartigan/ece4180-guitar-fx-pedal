
void initEncoder();
void rotaryISR();

extern volatile int rotaryCounter; // current "position" of rotary encoder (increments CW)
extern volatile bool rotaryChange; // will turn true if rotary_counter has changed