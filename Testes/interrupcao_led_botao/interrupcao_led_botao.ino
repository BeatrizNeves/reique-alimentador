// definir o número dos pinos
const int buttonPin = 2;  // o número do pino do botão
const int ledPin = 11;    // o número do pino do led
int t_pisca = 1;
// mudança de variável:
int buttonState = 0;  // variável para ler o status do botão

void setup() {
  Serial.begin(9600);
  // indicando o pino do led como uma saída:
  pinMode(ledPin, OUTPUT);
  // indicando o pino do botão de pressão como uma entrada:
  pinMode(buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), muda_estado, RISING);
}

void loop() {
  Serial.print(t_pisca);
  // ler o estado do valor do botão:
  // buttonState = digitalRead(buttonPin);
  // verificar se o botão está pressionado.
  // se sim, o estado do botão é alto:
  if (t_pisca == 1) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  } else if (t_pisca == 2) {
    digitalWrite(ledPin, HIGH);
    delay(2000);
    digitalWrite(ledPin, LOW);
    delay(2000);
  } else if (t_pisca == 3) {
    digitalWrite(ledPin, HIGH);
    delay(5000);
    digitalWrite(ledPin, LOW);
    delay(5000);
  }
}

void muda_estado() {
  if (t_pisca == 1) {
    Serial.print("ehum");
    t_pisca = 2;
    Serial.print(t_pisca);
  } else if (t_pisca == 2) {
    t_pisca = 3;
  } else if (t_pisca == 3) {
    t_pisca = 1;
  }
}
