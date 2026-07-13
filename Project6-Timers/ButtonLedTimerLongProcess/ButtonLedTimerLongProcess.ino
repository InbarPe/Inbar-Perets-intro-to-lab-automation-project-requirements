const int ledPin = 4;
const int buttonPin = 2; // use a pin that supports interrupts on your board

volatile bool ledState = false;
unsigned long ledOnTime = 0;
const unsigned long ledDuration = 5000UL; // 5 seconds
const unsigned long debounceDelay = 50UL;
volatile unsigned long lastInterruptTime = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonPress, RISING);
  Serial.begin(9600);
}

void loop() {
  if (ledState) {
    unsigned long currentMillis = millis();
    if (currentMillis - ledOnTime >= ledDuration) {
      digitalWrite(ledPin, LOW);
      ledState = false;
    }
  }
  for (int i = 0; i< 10000; i++){
    Serial.println("calculating...");
  }
}

void handleButtonPress() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime < debounceDelay) {
    return;
  }
  lastInterruptTime = interruptTime;

  digitalWrite(ledPin, HIGH);
  ledState = true;
  ledOnTime = millis();
}
