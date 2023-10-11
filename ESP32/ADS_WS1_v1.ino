#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define RAIN_PIN 25
#define DEBOUNCE_TIME 25
#define WIND_VANE_PIN 27
#define ANEMOMETER_PIN 12
#define CALC_INTERVAL 1000

#define DHTPIN 2       // D2 pin on ESP32 where the signal pin of the DHT sensor is connected
#define DHTTYPE DHT11  // Change this to DHT11 if you are using DHT11 sensor

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

// Create an instance of the BMP280 sensor
Adafruit_BMP280 bmp;

const char* ssid = "CSD";
const char* password = "csd@NITK2014";
const char* mqttServer = "10.100.83.226"; // Local MQTT broker IP address
const int mqttPort = 1883;                   // Port for local MQTT broker
const char* mqttUser = "";     // MQTT broker username
const char* mqttPassword = ""; // MQTT broker password (if any)

WiFiClient espClient;
PubSubClient client(espClient);

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

void setupWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupMQTT() {
  client.setServer(mqttServer, mqttPort);
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
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
  int directionIndex = map(rawValue, 0, 4095, 0, numDirections);
  return directionIndex;
}

void publishData(float rain, float wind, int direction, float pressure, float temp) {
  String topic = "weather_data";
  String payload = String("{");
  payload += "\"rainAmount\":" + String(rain);
  payload += ",\"windSpeed\":" + String(wind);
  payload += ",\"windDirection\":" + String(direction);
  payload += ",\"pressure\":" + String(pressure);
  payload += ",\"averageTemperature\":" + String(temp);
  payload += "}";

  if (client.publish(topic.c_str(), payload.c_str())) {
    Serial.println("Data published successfully.");
  } else {
    Serial.println("Failed to publish data.");
  }
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

  // Initialize the I2C communication for the BMP280 sensor
  Wire.begin();

  // Initialize the BMP280 sensor
  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1)
      ;
  }

  setupWiFi();
  setupMQTT();
}

void loop() {
  timer = millis();

  if (timer > nextCalc) {
    nextCalc = timer + CALC_INTERVAL;
    rainAmount = readRainAmount();
    windSpeed = readWindSpeed();
    windDirection = readWindDirection();

    // Read temperature and pressure values from the BMP280 sensor
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure() / 100.0;  // Divide by 100 to convert Pa to hPa (hectopascals)

    // Read temperature and humidity from the DHT sensor.
    float dht_temperature = dht.readTemperature();  // in Celsius
    float humidity = dht.readHumidity();            // in percentage

    // Check if any reads failed and exit early (to try again).
    if (isnan(dht_temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Calculate the average temperature from both sensors
    float average_temperature = (temperature + dht_temperature) / 2.0;

    // Publish data to local MQTT broker
    reconnectMQTT();
    publishData(rainAmount, windSpeed, windDirection, pressure, average_temperature);
    client.loop();
  }
}
