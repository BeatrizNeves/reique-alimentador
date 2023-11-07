#include <SoftwareSerial.h>
#include <RDM6300.h>
#include <Servo.h>

#define CONFIG_STATE 0
#define DISPENSER_STATE 1

#define CONFIG_LOW 1
#define CONFIG_MEDIUM 2
#define CONFIG_HIGH 3

#define FOOD_LOW_INTERVAL 10000
#define FOOD_MEDIUM_INTERVAL 15000
#define FOOD_HIGH_INTERVAL 20000

Servo servo;

const int PIN_CONFIG_TIME_BUTTON = 2;
const int PIN_CONFIG_QUANTITY_BUTTON = 3;
const int PIN_BLUE_RX = 5;
const int PIN_BLUE_TX = 6;
const int PIN_CONFIG_BUTTON = 7;
const int PIN_BUZZER = 8;
const int PIN_RFID_TX = 9;
const int PIN_RFID_RX = 10;
const int PIN_LED = 11;
const int PIN_SERVO = 12;

int state = DISPENSER_STATE;
int foodQuantity = CONFIG_LOW;
int foodInterval = CONFIG_LOW;

bool isMealAvailable = false;
bool btConfigMode = false;

unsigned long nextMealTime = FOOD_LOW_INTERVAL;

SoftwareSerial RFID(PIN_RFID_TX, PIN_RFID_RX);
SoftwareSerial BLUE(PIN_BLUE_TX, PIN_BLUE_RX);

uint8_t Payload[6];
RDM6300 RDM6300(Payload);

void setup() {
  Serial.begin(9600);
  RFID.begin(9600);
  BLUE.begin(9600);

  pinMode(PIN_CONFIG_BUTTON, INPUT);
  pinMode(PIN_CONFIG_TIME_BUTTON, INPUT);
  pinMode(PIN_CONFIG_QUANTITY_BUTTON, INPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  servo.attach(PIN_SERVO);
  servo.write(5);
  
  tone(PIN_BUZZER, 1000, 1000);
  delay(1000);

  attachInterrupt(digitalPinToInterrupt(PIN_CONFIG_TIME_BUTTON), configInterval, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_CONFIG_QUANTITY_BUTTON), configQuantity, RISING);

  Serial.println("   ");
  Serial.println("Setup completo");
}

void loop() {
  BLUE.listen();

  delay(10);
  while (BLUE.isListening()) {
    int incomingByte = BLUE.read();

    if (incomingByte != -1) {
      Serial.println("Configuring by bluetooth");
      delay(100);

      int lastState = state;
      state = CONFIG_STATE;

      if(incomingByte == 'A'){
        configInterval();
      } else if(incomingByte == 'B') {
        configQuantity();
      }
      
      state = lastState;
    }
    break;
  }

  RFID.listen();
  delay(20);

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
    Serial.println("Unable to configure, change to config mode to be able to change interval");
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
    Serial.println("Unable to configure, change to config mode to be able to change quantity");
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
    // Passa 0 pro Payload[0] para poder reutilizar a tag
    Payload[0] = 0;
    Serial.println("Meal is available");

    tone(PIN_BUZZER, 1000, 500);
    digitalWrite(PIN_LED, HIGH);
  }

  if (isMealAvailable) {
    while (RFID.available() > 0) {
      int d = RFID.read();
      if (RDM6300.decode(d)) {
        Serial.print("ID TAG: ");
        //Mostra os dados no serial monitor
        for (int i = 0; i < 5; i++) {
          Serial.print(Payload[i], HEX);
          Serial.print(" ");
        }
        Serial.println();
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

  digitalWrite(PIN_LED, LOW);

  for (int pos = 5; pos < 90; pos++) {
    servo.write(pos);
    delay(15);
  }

  delay(foodQuantity * 500);

  for (int pos = 89; pos > 5; pos--) {
    servo.write(pos);
    delay(15);
  }

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
  Serial.println("Changing state");
  state = !state;

  if (state == DISPENSER_STATE)
    Serial.println("Entering dispenser state");

  if (state == CONFIG_STATE)
    Serial.println("Entering config state");
}