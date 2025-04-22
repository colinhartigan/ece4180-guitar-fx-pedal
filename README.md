## ECE 4180 Final Project

Colin Hartigan, Nicholas Candello
### Overview

Our project is a digital guitar effects pedal that emulates effects created by traditional analog pedals. Our goal was to create a low-cost alternative to traditional pedals through a single-device solution so users would not have to carry multiple pedals to achieve multiple effects.

Typical digital pedals don't have a screen or any visual feedback to the user beyond the orientation of the potentiometers, but with our solution, we can map the 0-100 values of the pots to more understandable values on the display.

### Design

From ideation, we knew the system would have three major components:

1. a pre-amp circuit to get the low voltage guitar output to LINE level
2. a microcontroller to faciliate the real-time signal processing and user interface
3. a power amplifier to convert the PWM output signal from the microcontroller to a continuous voltage for the guitar amplifier

For the microcontroller we used an ESP32, which has much higher performance than an Mbed or Arduino. There are specialized versions of the ESP32 that have dedicated audio/DSP hardware, such as I2S communication with the ADC, but we decided against those upgrades to accessibility and simplicity.

The ESP32 has a 12-bit ADC which makes the audio sound a little bit like it's from a retro console, but this could be fixed with a better MCU with more ADC bits. One of the main issues we ran into was the lackluster 8-bit onboard DACs. We decided to instead use a 11-bit PWM output with a circuit that would smooth the PWM into a continuous analog voltage.

The ESP32 also has 2 cores, so we decided to pin one core to the audio effects tasks to ensure low latency streaming, and the other to the remaining configuration/display tasks.

We also spent a lot of time tuning the input and output circuits and trying to understand the voltage characteristics of a guitar's output signal. We were able to find a circuit schematic from a similar project that met our requirements.

On the DSP side, it took some time to understand how to manipulate the signal to achieve certain effects, but with some experimentation and research, we were able to get the main effects working:

-   clean
-   distortion
-   delay
-   tremolo

We wanted to implement more effects like reverb and a simple looper, but the hardware limitations of the ESP32, specifically its 4MB of RAM, made them unfeasable to implement.

For the user to interact with the system, there are 3 potentiometers to change effect parameters with high precision, a rotary encoder to select the effect, and an LCD to display the selected effect and its parameters.

### Schematic
![f7b2be95-79b7-40ee-afc6-f1a3b131c148](https://github.com/user-attachments/assets/81ce6194-6b75-4525-ad64-39981af0ff22)

### If we had more time and resources

If we had more time and resources, we would make a housing and a proper PCB to miniturize the footprint of the system. We were originally planning to CAD and 3D print a housing, however, the size of our protoboard quickly ballooned and we decided it made more sense to keep the electronics exposed for the demo.

### Libraries/Projects used

-   [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) for the display
-   [Arduino Uno Pedalshield Effects Pedal](https://www.electrosmash.com/pedalshield-uno) for the input and output amplifier circuitry
