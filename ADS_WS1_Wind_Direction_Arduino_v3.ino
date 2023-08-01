const int pin = 34;  // Replace with the appropriate pin number for your wind vane

const int numDirections = 16;

const char* bin[numDirections] = { "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW" };

int getWindDirection() {
  int rawValue = analogRead(pin);
  Serial.println(rawValue);

  int directionIndex = -1;

  if (rawValue >= 140 && rawValue < 218) directionIndex = 10;         // SW
  else if (rawValue >= 218 && rawValue < 296) directionIndex = 11;    // WSW
  else if (rawValue >= 296 && rawValue < 374) directionIndex = 12;    // NNW
  else if (rawValue >= 374 && rawValue < 452) directionIndex = 13;    // NW
  else if (rawValue >= 452 && rawValue < 530) directionIndex = 14;    // WNW
  else if (rawValue >= 530 && rawValue < 608) directionIndex = 15;    // NNE
  else if (rawValue >= 608 && rawValue < 686) directionIndex = 0;     // N
  else if (rawValue >= 686 && rawValue < 764) directionIndex = 1;     // W
  else if (rawValue >= 764 && rawValue < 842) directionIndex = 2;     // SSW
  else if (rawValue >= 842 && rawValue < 920) directionIndex = 3;     // NE
  else if (rawValue >= 920 && rawValue < 998) directionIndex = 4;     // S
  else if (rawValue >= 998 && rawValue < 1076) directionIndex = 7;    // ESE
  else if (rawValue >= 1076 && rawValue < 1154) directionIndex = 6;   // E
  else if (rawValue >= 1154 && rawValue < 1232) directionIndex = 5;   // SSE
  else if (rawValue >= 1232 && rawValue < 1310) directionIndex = 8;   // SE
  else if (rawValue >= 1310 && rawValue <= 1260) directionIndex = 9;  // ENE

  return directionIndex;
}

void setup() {
  Serial.begin(115200);
  pinMode(pin, INPUT);  // Set the wind vane pin to INPUT_PULLUP mode
}


void loop() {
  static int previousDirectionIndex = -1;  // Variable to store the previous direction

  int directionIndex = getWindDirection();

  if (directionIndex >= 0 && directionIndex < numDirections) {
    Serial.print("Wind Direction: ");
    Serial.println(bin[directionIndex]);
    previousDirectionIndex = directionIndex;  // Store the current direction as the previous direction
  } else {
    if (previousDirectionIndex != -1) {  // Check if there was a previous direction
      Serial.print("Previous Direction: ");
      Serial.println(bin[previousDirectionIndex]);
    } else {
      Serial.println("Direction not found!");  // Print this only when no direction is found yet
    }
  }

  delay(1000);  // Delay for 1 second between readings
}
