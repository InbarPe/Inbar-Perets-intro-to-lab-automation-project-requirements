const int ledPin = 4;
const int buttonPin = 2;  // Interrupt pin on Arduino Uno
volatile bool ledState = false;

void handleButtonChange() {
  ledState = (digitalRead(buttonPin) == HIGH);  // change to LOW if your circuit uses INPUT_PULLUP
  digitalWrite(ledPin, ledState);
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);  // use INPUT_PULLUP if your button is wired that way
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonChange, CHANGE);
  Serial.println("Button press LED control started");
}

void loop() {
  for (int i = 0; i< 10000; i++){
    Serial.println("calculating...");
  }
  delay(10);
}
