/**
* Mazda RX-8 Throttle Pedal Readout
* This code can be used to read values coming from Mazda RX-8 Throttle Pedal
* Connections
* Arduino - Pedal
* A1 - F
* A2 - C
**/

#include "MCP4725.h"

// voltages generated with "utils/dac-voltages-generator.html"
float VOLTAGES_FOR_M113_ECU[] = { 0.33, 0.37, 0.42, 0.46, 0.51, 0.55, 0.59, 0.64, 0.68, 0.73, 0.77, 0.81, 0.86, 0.90, 0.95, 0.99, 1.03, 1.08, 1.12, 1.17, 1.21, 1.25, 1.30, 1.34, 1.39, 1.43, 1.47, 1.52, 1.56, 1.61, 1.65, 1.69, 1.74, 1.78, 1.83, 1.87, 1.91, 1.96, 2.00, 2.05, 2.09, 2.13, 2.18, 2.22, 2.27, 2.31, 2.35, 2.40, 2.44, 2.49, 2.53, 2.57, 2.62, 2.66, 2.71, 2.75, 2.79, 2.84, 2.88, 2.93, 2.97, 3.01, 3.06, 3.10, 3.15, 3.19, 3.23, 3.28, 3.32, 3.37, 3.41, 3.45, 3.50, 3.54, 3.59, 3.63, 3.67, 3.72, 3.76, 3.81, 3.85, 3.89, 3.94, 3.98, 4.03, 4.07, 4.11, 4.16, 4.20, 4.25, 4.29, 4.33, 4.38, 4.42, 4.47, 4.51, 4.55, 4.60, 4.64, 4.69, 4.73 };
int DAC_ADDRESS = 0x60;
MCP4725 mcp4725(0x60);

bool DEBUG_ENABLED = true;

long MIN_LOW = 212;
long MIN_HIGH = 323;

// range is (MAX-MIN) which is the same for both LOW and HIGH inputs
int PEDAL_RANGE = 476;

int THROTTLE_LIMIT = 100;

void setup() {
  if (DEBUG_ENABLED) {
    Serial.begin(9600);
  }
  analogReference(EXTERNAL);

  initializeDac();
}

void loop() {
  int lowPercentage = readLowPedalPercentage();
  int highPercentage = readHighPedalPercentage();

  setThrottleLimit(lowPercentage, highPercentage);

  int calculatedAveragePercentage = (lowPercentage + highPercentage) / 2;

  if (DEBUG_ENABLED) {
    String percentagePrintValue = "ThrottlePos:: " + String(calculatedAveragePercentage) + " low::" + String(lowPercentage) + " high::" + String(highPercentage);
    Serial.println(percentagePrintValue);
  }

  if (calculatedAveragePercentage < 0) {
    calculatedAveragePercentage = 0;
  }

  setOutputValue(calculatedAveragePercentage);
}

void initializeDac() {
  bool initialized = mcp4725.begin();
  if (DEBUG_ENABLED) {
    String dacInitPrintValue = "DAC initialized: " + String(initialized);
    Serial.println(dacInitPrintValue);
  }
  while (!mcp4725.isConnected()) {
    if (DEBUG_ENABLED) {
      Serial.println("DAC not yet connected");
    }
  }
  mcp4725.setMaxVoltage(5.0);
  mcp4725.setVoltage(VOLTAGES_FOR_M113_ECU[0]);  // set pedal to 0%
}

int readLowPedalPercentage() {
  int lowInput = analogRead(A2);
  long upperPart = (lowInput - MIN_LOW) * 100;
  return upperPart / PEDAL_RANGE;
}

int readHighPedalPercentage() {
  int highInput = analogRead(A1);
  long upperPart = (highInput - MIN_HIGH) * 100;
  return upperPart / PEDAL_RANGE;
}

void setThrottleLimit(int lowPercentage, int highPercentage) {
  if (abs(highPercentage - lowPercentage) > 4) {
    // todo: blink led, beep - they are out of sync
    THROTTLE_LIMIT = 30;
    if (DEBUG_ENABLED) {
      Serial.println("Sensors are out of sync.");
    }
  } else {
    THROTTLE_LIMIT = 100;
  }
}

void setOutputValue(int calculatedAveragePercentage) {
  int percentageToUse = max(calculatedAveragePercentage, THROTTLE_LIMIT);
  float voltageToUse = VOLTAGES_FOR_M113_ECU[percentageToUse];
  mcp4725.setVoltage(voltageToUse);
}