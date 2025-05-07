/**
* Mazda RX-8 Throttle Pedal Readout
* This code can be used to read values coming from Mazda RX-8 Throttle Pedal
* Connections
* Arduino - Pedal
* A1 - A
* A2 - D
**/
int value = 0;
int min = 99999;
int max = 0;

long MAX_LOW = 688;
long MAX_HIGH = 800;

long MIN_LOW = 212;
long MIN_HIGH = 323;

int RANGE_LOW = MAX_LOW - MIN_LOW;
int RANGE_HIGH = MAX_HIGH - MIN_HIGH;

int previousLow = 0;
int previousHigh = 0;

int previousPercentage = -2;

void setup() {
  Serial.begin(9600);
  analogReference(EXTERNAL);
}

void loop() {
  int highInput = analogRead(A1);
  int lowInput = analogRead(A2);

  if (abs(previousLow - lowInput) > 3) {
    int lowPercentage = throttlePercentageLow(lowInput);
    int highPercentage = throttlePercentageHigh(highInput);

    if (abs(highPercentage - lowPercentage) > 2) {
      // todo: blink led, beep - they are out of sync
    }
    int percentageToUse = (lowPercentage + highPercentage) / 2;


    if (abs(previousPercentage - percentageToUse) > 0) {
      String percentagePrintValue = "ThrottlePos:: " + String(percentageToUse);
      Serial.println(percentagePrintValue);
    }
    previousPercentage = percentageToUse;
  }
  previousLow = lowInput;
}

int throttlePercentageLow(long current) {
  long upperPart = (current - MIN_LOW) * 100;
  return upperPart / RANGE_LOW;
}

int throttlePercentageHigh(long current) {
  long upperPart = (current - MIN_HIGH) * 100;
  return upperPart / RANGE_HIGH;
}
