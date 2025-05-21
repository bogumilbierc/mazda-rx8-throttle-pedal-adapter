# Mazda RX-8 throttle pedal to M113 ECU adapter

Mazda RX-8 throttle pedal adapter for Mercedes M113 ECU (M113 V8 swap into RX-8).

## OEM Pedals documentation

- [RX-8 pedal](./docs/rx-8-pedal/rx-8-pedal.md)
- [M113 pedal](./docs/m113-pedal/m113-pedal.md)

## Adapter

### How it works

Mazda RX-8 pedal has two sensors (potentiometers) that have voltage offset between each other.

M113 pedal has two sensors (potentiometers) that have one sensor being 2*voltage of the other sensor.

Adapter reads the voltage from Mazda pedal, converts it to M113 voltage with DAC (for one sensor) and then takes output
from DAC and divides it by two using a simple resistor-based voltage divider for the other sensor.

### Basic logic (runs in loop)

1. Read voltages from both sensors on RX-8 pedal
2. Calculate percentage from both sensors
3. Validate if percentages are not out of sync (safety feature)
    - if they are, set the throttle limit to 30%
    - if they are not, clear the throttle limit
4. Set output for DAC (for "high" pedal)

## Components

- Arduino Nano <br> ![arduino nano](./docs/elements/arduino-nano.png)
- MCP4725 DAC <br> ![mcp 4725 dac](./docs/elements/mcp-4725-dac.png)
- RX-8 pedal plug
- two resistors for voltage divider (I choose 4.7 kOhm, but it does not really matter)
- some wires

## Circuit diagram

![circuit diagram](./docs/circuit-diagram.png)

Circuit was drawn using https://www.circuit-diagram.org/. 

Source code for the diagram is located in [circuit cddx file](./docs/circuit.cddx)

## Arduino IDE setup

- Install Adafruit_MCP4725 library
- Load project from `arduino/pedal_adapter/pedal_adapter.ino`
