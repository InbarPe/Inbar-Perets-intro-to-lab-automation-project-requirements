const int ledPin = 4;
const int buttonPin = 6;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Button + LED debug started");
}

void loop() {
  int buttonState = digitalRead(buttonPin);

  Serial.print("Button state: ");
  Serial.println(buttonState);

  if (buttonState == HIGH) {
    digitalWrite(ledPin, HIGH);
    Serial.println("LED ON");
  } else {
    digitalWrite(ledPin, LOW);
    Serial.println("LED OFF");
  }

  delay(200);
}
