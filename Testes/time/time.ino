void setup() {
  Serial.begin(9600);
  
}

void loop() {
  unsigned long startTime = millis();
  Serial.println(startTime);

  delay(1000); 

  unsigned long elapsedTime = millis() - startTime;
  Serial.println(elapsedTime);
}
