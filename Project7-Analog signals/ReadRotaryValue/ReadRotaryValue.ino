const int rotaryPin = A0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for Serial port to connect (only needed on some boards)
  }
  Serial.println("Rotary value reader starting...");
}

void loop() {
  int rotaryValue = analogRead(rotaryPin);
  Serial.print("Rotary A0 = ");
  Serial.println(rotaryValue);
  delay(100);
}
