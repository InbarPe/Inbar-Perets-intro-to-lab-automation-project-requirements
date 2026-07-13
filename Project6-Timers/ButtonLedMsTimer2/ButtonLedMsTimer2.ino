/*
  ButtonLedMsTimer2.ino
  - Turns on LED on pin 4 when button pressed (interrupt)
  - Uses MsTimer2 to call a callback after 5 seconds to turn LED off
  - Requires MsTimer2 library installed
*/

#include <MsTimer2.h>

const int ledPin = 4;
const int buttonPin = 2; // use a pin that supports interrupts on your board

volatile bool ledState = false;
const unsigned long ledDuration = 5000UL; // 5 seconds
const unsigned long debounceDelay = 50UL;
volatile unsigned long lastInterruptTime = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonPress, RISING);
}

void loop() {
  // Main loop stays free; timer callback will run in interrupt context
}

void handleButtonPress() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime < debounceDelay) {
    return; // simple debounce
  }
  lastInterruptTime = interruptTime;

  digitalWrite(ledPin, HIGH);
  ledState = true;

  // (Re)start a one-shot timer for ledDuration milliseconds
  MsTimer2::stop();
  MsTimer2::set(ledDuration, timerCallback);
  MsTimer2::start();
}

void timerCallback() {
  digitalWrite(ledPin, LOW);
  ledState = false;
  MsTimer2::stop(); // ensure timer is not repeating
}
