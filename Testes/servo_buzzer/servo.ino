#include <SoftwareSerial.h>
#include <RDM6300.h>
#include <Servo.h>

#define SERVO 6  // Porta Digital 6 PWM

Servo s;  // Variável Servo
int pos = 5;  // Posição Servo
const int buzzer = 9;

//Inicializa a serial nos pinos 2 (RX) e 3 (TX)
SoftwareSerial RFID(2, 3);
uint8_t Payload[6];  // used for read comparisons
RDM6300 RDM6300(Payload);


void setup() {
  //Inicializa a serial para o leitor RDM6300
  RFID.begin(9600);
  //Inicializa a serial para comunicacao com o PC
  Serial.begin(9600);  //;
  //Informacoes iniciais
  Serial.println("Leitor RFID RDM6300n");
  s.attach(SERVO);
  s.write(pos);  // Inicia motor posição inicial
  pinMode(buzzer,OUTPUT);
}


void loop() {
  //Aguarda a aproximacao da tag RFID
  while (RFID.available() > 0) {
    
    uint8_t c = RFID.read();
    if (RDM6300.decode(c)) {
      tone(buzzer, 1000,300);
      Serial.print("ID TAG: ");
      //Mostra os dados no serial monitor
      for (int i = 0; i < 5; i++) {
        Serial.print(Payload[i], HEX);
        Serial.print(" ");
      }
      Serial.println();

      if (pos == 5) {
        Serial.println("começou com 0");
        for (pos = 5; pos < 90; pos++) {
          s.write(pos);
          delay(15);
        }
      } else {
        Serial.print("começou com");
        Serial.println(pos);
        for (pos = 89; pos > 5; pos--) {
          s.write(pos);
          delay(15);
        }
      }
    }
  }
  
  delay(1000);
}
