#define RAIN_PIN 33
#define CALC_INTERVAL 1000
#define DEBOUNCE_TIME 25

unsigned long nextCalc;
unsigned long timer;

volatile unsigned int rainCounter = 0;
volatile unsigned long last_micros_rg;

float rainAmount = 0.0;
float rain = 0.0;

void IRAM_ATTR countingRain() {
  if ((long)(micros() - last_micros_rg) >= DEBOUNCE_TIME * 1000) {
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

  pinMode(RAIN_PIN, INPUT);
  attachInterrupt(RAIN_PIN, countingRain, RISING);

  nextCalc = millis() + CALC_INTERVAL;
}

void loop() {
  timer = millis();
  if (timer > nextCalc) {
    nextCalc = timer + CALC_INTERVAL;
    rainAmount = readRainAmmount();
    Serial.print("Rain Amount: ");
    Serial.print(rainAmount);
    Serial.println(" mm");
  }
}