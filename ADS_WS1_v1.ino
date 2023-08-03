#include <DHT.h>

#define RAIN_PIN 25
#define DEBOUNCE_TIME 25
#define WIND_VANE_PIN 27
#define ANEMOMETER_PIN 12
#define CALC_INTERVAL 1000

#define DHTPIN 2          // D2 pin on ESP32 where the signal pin of the DHT sensor is connected
#define DHTTYPE DHT11     // Change this to DHT11 if you are using DHT11 sensor

unsigned long nextCalc;
unsigned long timer;

volatile unsigned int rainCounter = 0;
volatile unsigned long last_micros_rg;
volatile int anemometerCounter;
volatile unsigned long last_micros_an;

float rainAmount = 0.0;
float windSpeed;
int windDirection;

const int numDirections = 16;
const char* bin[numDirections] = { "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW" };

DHT dht(DHTPIN, DHTTYPE);

void IRAM_ATTR countingRain() {
  if ((long)(micros() - last_micros_rg) >= DEBOUNCE_TIME * 1000) {
    rainCounter += 1;
    last_micros_rg = micros();
  }
}

void IRAM_ATTR countAnemometer() {
  if ((long)(micros() - last_micros_an) >= DEBOUNCE_TIME * 1000) {
    anemometerCounter++;
    last_micros_an = micros();
  }
}

float readRainAmount() {
  float rain = 0.2794 * rainCounter;
  return rain;
}

float readWindSpeed() {
  float spd = 24.0 * anemometerCounter / 10.0;
  anemometerCounter = 0;
  return spd;
}

int readWindDirection() {
  int rawValue = analogRead(WIND_VANE_PIN);
  Serial.println(rawValue);
  int directionIndex = -1;

  // Direction mapping code remains the same...

  return directionIndex;
}

void setup() {
  Serial.begin(115200);
  pinMode(RAIN_PIN, INPUT);
  pinMode(ANEMOMETER_PIN, INPUT);
  pinMode(WIND_VANE_PIN, INPUT);
  attachInterrupt(RAIN_PIN, countingRain, RISING);
  attachInterrupt(ANEMOMETER_PIN, countAnemometer, RISING);
  nextCalc = millis() + CALC_INTERVAL;
  dht.begin();
}

void loop() {
  timer = millis();

  if (timer > nextCalc) {
    nextCalc = timer + CALC_INTERVAL;
    rainAmount = readRainAmount();
    windSpeed = readWindSpeed();
    windDirection = readWindDirection();

    Serial.print("Rain Amount: ");
    Serial.print(rainAmount);
    Serial.println(" mm");

    if (windDirection >= 0 && windDirection < numDirections) {
      Serial.print("Wind Direction: ");
      Serial.println(bin[windDirection]);
    } else {
      Serial.println("Direction not found!");
    }

    Serial.print("Wind Speed: ");
    Serial.print(windSpeed);
    Serial.println(" kmph");

    // Read temperature and humidity from the sensor.
    float temperature = dht.readTemperature(); // in Celsius
    float humidity = dht.readHumidity();       // in percentage

    // Check if any reads failed and exit early (to try again).
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Print temperature and humidity.
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C");

    Serial.print("   Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }
}
