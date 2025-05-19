/**
* Mazda RX-8 Throttle Pedal Readout
* This code can be used to read values coming from Mazda RX-8 Throttle Pedal
* Connections
* Arduino - Pedal
* A1 - F
* A2 - C
**/

#include <Wire.h>
//MCP4725 library
#include <Adafruit_MCP4725.h>

// fractions generated with "utils/dac-voltages-generator.html"
int FRACTIONS_FOR_M113_ECU[] = { 270, 303, 344, 376, 417, 450, 483, 524, 557, 598, 630, 663, 704, 737, 778, 811, 843, 884, 917, 958, 991, 1024, 1064, 1097, 1138, 1171, 1204, 1245, 1277, 1318, 1351, 1384, 1425, 1458, 1499, 1531, 1564, 1605, 1638, 1679, 1712, 1744, 1785, 1818, 1859, 1892, 1925, 1966, 1998, 2039, 2072, 2105, 2146, 2179, 2220, 2252, 2285, 2326, 2359, 2400, 2433, 2465, 2506, 2539, 2580, 2613, 2646, 2686, 2719, 2760, 2793, 2826, 2867, 2899, 2940, 2973, 3006, 3047, 3080, 3121, 3153, 3186, 3227, 3260, 3301, 3334, 3366, 3407, 3440, 3481, 3514, 3547, 3588, 3620, 3661, 3694, 3727, 3768, 3801, 3842, 3874 };

int DAC_ADDRESS = 0x60;

Adafruit_MCP4725 mcp4725;

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
  bool initialized = mcp4725.begin(DAC_ADDRESS);
  if (DEBUG_ENABLED) {
    String dacInitPrintValue = "DAC initialized: " + String(initialized);
    Serial.println(dacInitPrintValue);
  }
  mcp4725.setVoltage(FRACTIONS_FOR_M113_ECU[0], false);  // set pedal to 0%
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
  int percentageToUse = min(calculatedAveragePercentage, THROTTLE_LIMIT);
  float voltageToUse = FRACTIONS_FOR_M113_ECU[percentageToUse];
  mcp4725.setVoltage(voltageToUse, false);
}