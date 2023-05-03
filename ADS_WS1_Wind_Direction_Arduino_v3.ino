#include <Filter.h>

AnalogFilter<100, 0> filter;

const int pin = 34;

const double voltageValues[16] = { 0.63, 0.52, 1.03, 1.01, 1.11, 1.1, 1.07, 1.09, 0.81, 0.88, 0.14, 0.17, 0.09, 0.4, 0.26, 0.0 };
const char *direction[] = { "N", "NE", "NE", "NE", "E", "SE", "SE", "SE", "S", "SW", "SW", "SW", "W", "NW", "NW", "NW" };
const double dirDegrees[16] = { 0.0, 22.5, 45.0, 67.5, 90.0, 112.5, 135.0, 157.5, 180.0, 202.5, 225.0, 247.5, 270.0, 292.5, 315.0, 337.5 };

int dir = 0, dirDig = 0, i;

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
  Serial.print("Direction: ");
  Serial.print(dirDegrees[dir]);
  Serial.print(" ");
  Serial.println(direction[dir]);

  return dir + 1;
}


void setup() {
  Serial.begin(115200);
}

void loop() {
  int Direction = checkWindDir();
}