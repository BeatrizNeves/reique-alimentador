#include <SoftwareSerial.h>
#include <RDM6300.h>

#define CONFIG_STATE 0
#define DISPENSER_STATE 1

#define CONFIG_LOW 0
#define CONFIG_MEDIUM 1
#define CONFIG_HIGH 2

#define FOOD_LOW_INTERVAL 10000
#define FOOD_MEDIUM_INTERVAL 15000
#define FOOD_HIGH_INTERVAL 20000

const int PIN_CONFIG_TIME_BUTTON = 2;
const int PIN_CONFIG_QUANTITY_BUTTON = 3;
const int PIN_CONFIG_BUTTON = 7;
const int PIN_RFID_TX = 9;
const int PIN_RFID_RX = 10;

int state = DISPENSER_STATE;
int foodQuantity = CONFIG_LOW;
int foodInterval = CONFIG_LOW;

bool isMealAvailable = false;

unsigned long nextMealTime = FOOD_LOW_INTERVAL;

SoftwareSerial RFID(PIN_RFID_TX, PIN_RFID_RX);
uint8_t Payload[6];
RDM6300 RDM6300(Payload);

void setup() {
  RFID.begin(9600);
  Serial.begin(9600);

  pinMode(PIN_CONFIG_BUTTON, INPUT);
  pinMode(PIN_CONFIG_TIME_BUTTON, INPUT);
  pinMode(PIN_CONFIG_QUANTITY_BUTTON, INPUT);

  attachInterrupt(digitalPinToInterrupt(PIN_CONFIG_TIME_BUTTON), configInterval, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_CONFIG_QUANTITY_BUTTON), configQuantity, RISING);
}

void loop() {
  if (state == DISPENSER_STATE) {
    dispenserMode();
  }

  if (digitalRead(PIN_CONFIG_BUTTON) == HIGH) {
    changeState();
    delay(500);
  }
}

void configInterval() {
  if (state != CONFIG_STATE) {
    Serial.println("Change to config mode to be able to change interval");
  }
  Serial.println("Changing interval");

  switch (foodInterval) {
    case CONFIG_LOW:
      foodInterval = CONFIG_MEDIUM;
      nextMealTime = nextMealTime + (FOOD_MEDIUM_INTERVAL - FOOD_LOW_INTERVAL);
      break;
    case CONFIG_MEDIUM:
      foodInterval = CONFIG_HIGH;
      nextMealTime = nextMealTime + (FOOD_HIGH_INTERVAL - FOOD_MEDIUM_INTERVAL);
      break;
    case CONFIG_HIGH:
      foodInterval = CONFIG_LOW;
      nextMealTime = nextMealTime + (FOOD_LOW_INTERVAL - FOOD_HIGH_INTERVAL);
      break;
  }
}

void configQuantity() {
  if (state != CONFIG_STATE) {
    Serial.println("Change to config mode to be able to change quantity");
  };
  Serial.println("Changing quantity");

  switch (foodQuantity) {
    case CONFIG_LOW:
      foodQuantity = CONFIG_MEDIUM;
      break;
    case CONFIG_MEDIUM:
      foodQuantity = CONFIG_HIGH;
      break;
    case CONFIG_HIGH:
      foodQuantity = CONFIG_LOW;
      break;
  }
}

void dispenserMode() {
  // Verificar limite do unsigned long e ver o que acontece com o milis se tiver no limite
  unsigned long currentTime = millis();

  if (currentTime >= nextMealTime) {
    updateNextMealTime();
    isMealAvailable = true;
    Serial.println("Meal is available");
  }

  if (isMealAvailable) {
    while (RFID.available() > 0) {
      uint8_t c = RFID.read();
      if (RDM6300.decode(c)) {
        dispenser();
      }
    }
  }
}

void dispenser() {
  Serial.println("Meal time!");
  Serial.print("foodQuantity: ");
  Serial.println(foodQuantity);
  Serial.print("foodInterval: ");
  Serial.println(foodInterval);

  isMealAvailable = false;
}

void updateNextMealTime() {
  switch (foodInterval) {
    case CONFIG_LOW:
      nextMealTime = nextMealTime + FOOD_LOW_INTERVAL;
      break;
    case CONFIG_MEDIUM:
      nextMealTime = nextMealTime + FOOD_MEDIUM_INTERVAL;
      break;
    case CONFIG_HIGH:
      nextMealTime = nextMealTime + FOOD_HIGH_INTERVAL;
      break;
  }
}

void changeState() {
  Serial.println("Change state");
  state = !state;

  if (state == DISPENSER_STATE)
    Serial.println("Entering dispenser state");

  if (state == CONFIG_STATE)
    Serial.println("Entering config state");
}
