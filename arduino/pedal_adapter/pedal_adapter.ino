/**
* Mazda RX-8 Throttle Pedal Adapter
* Check README in main folder to verify wiring diagram
**/
#define DEBUG_ENABLED false
#define FASTADC true

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


#include <Wire.h>
//MCP4725 library
#include <Adafruit_MCP4725.h>

// fractions generated with "utils/dac-voltages-generator.html"
const int FRACTIONS_FOR_M113_ECU[] = { 270, 303, 344, 376, 417, 450, 483, 524, 557, 598, 630, 663, 704, 737, 778, 811, 843, 884, 917, 958, 991, 1024, 1064, 1097, 1138, 1171, 1204, 1245, 1277, 1318, 1351, 1384, 1425, 1458, 1499, 1531, 1564, 1605, 1638, 1679, 1712, 1744, 1785, 1818, 1859, 1892, 1925, 1966, 1998, 2039, 2072, 2105, 2146, 2179, 2220, 2252, 2285, 2326, 2359, 2400, 2433, 2465, 2506, 2539, 2580, 2613, 2646, 2686, 2719, 2760, 2793, 2826, 2867, 2899, 2940, 2973, 3006, 3047, 3080, 3121, 3153, 3186, 3227, 3260, 3301, 3334, 3366, 3407, 3440, 3481, 3514, 3547, 3588, 3620, 3661, 3694, 3727, 3768, 3801, 3842, 3874 };

const int DAC_ADDRESS = 0x60;

const long MIN_LOW = 212;
const long MIN_HIGH = 323;

// range is (MAX-MIN) which is the same for both LOW and HIGH inputs
const int PEDAL_RANGE = 476;

// used to limit throttle / put adapter into safety mode in case sensors go out of sync
int THROTTLE_LIMIT = 100;

// used to speed up the code -> don't call DAC if same value is passed multiple times in a row
int lastValue = -1;

Adafruit_MCP4725 mcp4725;

void setup() {

#if FASTADC == true
  // set prescale to 16
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  cbi(ADCSRA, ADPS0);
#endif

#if DEBUG_ENABLED == true
  Serial.begin(9600);
#endif

  // analogReference(EXTERNAL); // this should not matter since board is powered via 5V pin
  initializeDac();
}

void loop() {
#if DEBUG_ENABLED == true
  int startMillis = millis();
#endif

  int lowPercentage = readLowPedalPercentage();
  int highPercentage = readHighPedalPercentage();

  setThrottleLimit(lowPercentage, highPercentage);

  int calculatedAveragePercentage = (lowPercentage + highPercentage) / 2;

#if DEBUG_ENABLED == true
  String percentagePrintValue = "ThrottlePos:: " + String(calculatedAveragePercentage) + " low::" + String(lowPercentage) + " high::" + String(highPercentage);
  Serial.println(percentagePrintValue);

  int endMillis = millis();
  String millisPrintValue = "Loop took:: " + String(endMillis - startMillis);
  Serial.println(millisPrintValue);
#endif

  if (calculatedAveragePercentage < 0) {
    calculatedAveragePercentage = 0;
  }

  setOutputValue(calculatedAveragePercentage);
}

void initializeDac() {
  bool initialized = mcp4725.begin(DAC_ADDRESS);
#if DEBUG_ENABLED == true
  String dacInitPrintValue = "DAC initialized: " + String(initialized);
  Serial.println(dacInitPrintValue);
#endif
  setOutputValue(0);  // set pedal to 0%
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
#if DEBUG_ENABLED == true
    Serial.println("Sensors are out of sync.");
#endif
  } else {
    THROTTLE_LIMIT = 100;
  }
}

void setOutputValue(int calculatedAveragePercentage) {
  if (calculatedAveragePercentage == lastValue) {
#if DEBUG_ENABLED == true
    Serial.println("Passed value is the same as previous value. Skipping DAC call.");
#endif
    return;
  }
  int percentageToUse = min(calculatedAveragePercentage, THROTTLE_LIMIT);
  float voltageToUse = FRACTIONS_FOR_M113_ECU[percentageToUse];
  mcp4725.setVoltage(voltageToUse, false);
  lastValue = calculatedAveragePercentage;
}
