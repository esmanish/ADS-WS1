#include <Filter.h>

#define DEBOUNCE_TIME 15
#define DEBOUNCE_TIME_RAIN 25
#define CALC_INTERVAL 1000
#define ANEMOMETER_PIN 12  // Interrupt pin tied to anemometer reed switch
#define RAIN_PIN 33
const int pin = 34;

unsigned long nextCalc;
unsigned long timer;

volatile int anemometerCounter;
volatile unsigned long last_micros_an;
volatile unsigned int rainCounter = 0;
volatile unsigned long last_micros_rg;

float windSpeed;
float rainAmount = 0.0;
float rain = 0.0;
int cardinalDir, dir = 0, dirDig = 0, i;

AnalogFilter<100, 0> filter;

const double voltageValues[16] = { 0.63, 0.52, 1.03, 1.01, 1.11, 1.1, 1.07, 1.09, 0.81, 0.88, 0.14, 0.17, 0.09, 0.4, 0.26, 0.0 };
const char *direction[] = { "N", "NE", "NE", "NE", "E", "SE", "SE", "SE", "S", "SW", "SW", "SW", "W", "NW", "NW", "NW" };
const double dirDegrees[16] = { 0.0, 22.5, 45.0, 67.5, 90.0, 112.5, 135.0, 157.5, 180.0, 202.5, 225.0, 247.5, 270.0, 292.5, 315.0, 337.5 };

double round(double value) {
  return (int)(value * 100 + 0.5) / 100.0;
}

int checkWindDir() {

  int value = analogRead(pin);
  int filtered = filter.update(value);
  double voltage = filtered * (3.3 / 4095.0);

  for (i = 0; i <= 14; i++) {
    if (round(voltage) == voltageValues[i]) {
      dir = i;
    }
    // else {
    //   Serial.println("Direction not Found!!!");
    // }
  }
  return dir;
}

void IRAM_ATTR countAnemometer() {
  if ((long)(micros() - last_micros_an) >= DEBOUNCE_TIME * 1000) {
    anemometerCounter++;
    last_micros_an = micros();
  }
}

//returns the wind speed since the last calcInterval.
float readWindSpd() {
  unsigned char i;
  float spd = 24.0;  // one turn = 2.4 km per hour;
  spd *= anemometerCounter;
  spd /= 10.0;
  anemometerCounter = 0;
  return spd;
}

void IRAM_ATTR countingRain() {
  if ((long)(micros() - last_micros_rg) >= DEBOUNCE_TIME_RAIN * 1000) {
    rainCounter += 1;
    last_micros_rg = micros();
  }
}

float readRainAmmount() {
  rain = 0.2794 * rainCounter;
  return rain;
}

void setup() {
  Serial.begin(115200);

  pinMode(ANEMOMETER_PIN, INPUT);
  attachInterrupt(ANEMOMETER_PIN, countAnemometer, FALLING);

  pinMode(RAIN_PIN, INPUT);
  attachInterrupt(RAIN_PIN, countingRain, RISING);

  nextCalc = millis() + CALC_INTERVAL;
}

void loop() {

  timer = millis();

  cardinalDir = checkWindDir();

  if (timer > nextCalc) {
    nextCalc = timer + CALC_INTERVAL;

    Serial.print("Direction: ");
    Serial.print(dirDegrees[cardinalDir]);
    Serial.print(" ");
    Serial.print(direction[cardinalDir]);

    windSpeed = readWindSpd();
    Serial.print("\tWind speed: ");
    Serial.print(windSpeed);
    Serial.print(" kmph");

    rainAmount = readRainAmmount();
    Serial.print("\tRain Amount: ");
    Serial.print(rainAmount);
    Serial.println(" mm");
  }
}