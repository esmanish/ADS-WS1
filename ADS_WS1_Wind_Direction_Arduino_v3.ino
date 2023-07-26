const int pin = 34;  // Replace with the appropriate pin number for your wind vane

const int numDirections = 16;

const char* bin[numDirections] = { "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW" };

int getWindDirection() {
  int rawValue = analogRead(pin);
  Serial.println(rawValue);

  int directionIndex = -1;

  if (rawValue >= 140 && rawValue < 218) directionIndex = 10;    // SW
  else if (rawValue >= 218 && rawValue < 296) directionIndex = 11;   // WSW
  else if (rawValue >= 296 && rawValue < 374) directionIndex = 12;   // NNW
  else if (rawValue >= 374 && rawValue < 452) directionIndex = 13;   // NW
  else if (rawValue >= 452 && rawValue < 530) directionIndex = 14;   // WNW
  else if (rawValue >= 530 && rawValue < 608) directionIndex = 15;   // NNE
  else if (rawValue >= 608 && rawValue < 686) directionIndex = 0;    // N
  else if (rawValue >= 686 && rawValue < 764) directionIndex = 1;    // W
  else if (rawValue >= 764 && rawValue < 842) directionIndex = 2;  // SSW
  else if (rawValue >= 842 && rawValue < 920) directionIndex = 3;  // NE
  else if (rawValue >= 920 && rawValue < 998) directionIndex = 4;  // S
  else if (rawValue >= 998 && rawValue < 1076) directionIndex = 5;  // SSE
  else if (rawValue >= 1076 && rawValue < 1154) directionIndex = 6;  // E
  else if (rawValue >= 1154 && rawValue < 1232) directionIndex = 7;  // ESE
  else if (rawValue >= 1232 && rawValue < 1310) directionIndex = 8;  // SE
  else if (rawValue >= 1310 && rawValue <= 1260) directionIndex = 9;  // ENE

  return directionIndex;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  int directionIndex = getWindDirection();

  if (directionIndex >= 0 && directionIndex < numDirections) {
    Serial.print("Wind Direction: ");
    Serial.println(bin[directionIndex]);
  } else {
    Serial.println("Direction not found!");
  }

  delay(1000);  // Delay for 1 second between readings
}
