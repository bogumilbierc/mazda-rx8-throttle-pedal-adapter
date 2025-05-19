/**
* Mazda RX-8 Throttle Pedal Readout
* This code can be used to read values coming from Mazda RX-8 Throttle Pedal
* Connections
* Arduino - Pedal
* A1 - F
* A2 - C
**/

int DAC_ADDRESS = 0x60;


bool DEBUG_ENABLED = true;

long MAX_LOW = 688;
long MAX_HIGH = 800;

long MIN_LOW = 212;
long MIN_HIGH = 323;

int RANGE_LOW = MAX_LOW - MIN_LOW;
int RANGE_HIGH = MAX_HIGH - MIN_HIGH;

int THROTTLE_LIMIT = 100;

int previousPercentage = -2;

void setup() {
  if (DEBUG_ENABLED) {
    Serial.begin(9600);
  }
  analogReference(EXTERNAL);
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

  previousPercentage = calculatedAveragePercentage;
}

int readLowPedalPercentage() {
  int lowInput = analogRead(A2);
  long upperPart = (lowInput - MIN_LOW) * 100;
  return upperPart / RANGE_LOW;
}

int readHighPedalPercentage() {
  int highInput = analogRead(A1);
  long upperPart = (highInput - MIN_LOW) * 100;
  return upperPart / RANGE_LOW;
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