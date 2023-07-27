#define DEBOUNCE_TIME 15
#define CALC_INTERVAL 1000
#define ANEMOMETER_PIN 12  // Interrupt pin tied to anemometer reed switch

unsigned long nextCalc;
unsigned long timer;

volatile int anemometerCounter;
volatile unsigned long last_micros_an;

float windSpeed;

//returns the wind speed since the last calcInterval.
float readWindSpd() {
  unsigned char i;
  float spd = 24.0;  // one turn = 2.4 km per hour;
  spd *= anemometerCounter;
  spd /= 10.0;
  anemometerCounter = 0;
  return spd;
}

void IRAM_ATTR countAnemometer() {
  if ((long)(micros() - last_micros_an) >= DEBOUNCE_TIME * 1000) {
    anemometerCounter++;
    last_micros_an = micros();
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(ANEMOMETER_PIN, INPUT);
  attachInterrupt(ANEMOMETER_PIN, countAnemometer, RISING);
}

void loop() {
  timer = millis();

  if (timer > nextCalc) {
    nextCalc = timer + CALC_INTERVAL;
    windSpeed = readWindSpd();
    Serial.print("Wind speed: ");
    Serial.print(windSpeed);
    Serial.println(" kmph");
  }
}
