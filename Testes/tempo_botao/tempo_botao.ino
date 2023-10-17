// definir o número dos pinos
#define CONFIG_STATE 0
#define DISPENSER_STATE 1

#define CONFIG_LOW 0
#define CONFIG_MEDIUM 1
#define CONFIG_HIGH 2

#define FOOD_LOW_INTERVAL 2000
#define FOOD_MEDIUM_INTERVAL 5000
#define FOOD_HIGH_INTERVAL 10000

const int PIN_CONFIG_BUTTON = 2;
const int PIN_CONFIG_TIME_BUTTON = 3;
const int PIN_CONFIG_QUANTITY_BUTTON = 4;
const int PIN_LED = 11;

int state = DISPENSER_STATE;
int foodQuantity = FOOD_QUANTITY_LOW;
int foodInterval = FOOD_INTERVAL_LOW;

unsigned long nextMealTime = FOOD_LOW_INTERVAL;

void setup() {
  Serial.begin(9600);

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_CONFIG_BUTTON, INPUT);

  attachInterrupt(digitalPinToInterrupt(PIN_CONFIG_BUTTON), changeState, RISING);
}

void loop() {
  if (state == DISPENSER_STATE) {
    dispenserMode();
  } else {
    configMode();
  }

  Serial.println("foodQuantity: " + foodQuantity);
  Serial.println("foodInterval: " + foodInterval);
  Serial.println("nextMealTime: " + nextMealTime);
}

void configMode() {
  if (digitalRead(PIN_CONFIG_QUANTITY_BUTTON) == HIGH) {
    configFoodMode(foodQuantity);
  }

  if (digitalRead(PIN_CONFIG_TIME_BUTTON) == HIGH) {
    configFoodMode(foodInterval);
  }

  delay(500);
}

void configInterval() {
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
    dispenser();
  }
}

void dispenser() {
  Serial.println("Meal time!");
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
  state = !state;
}
