/*
  UserInputLedOn.ino
  - Reads a duration from the serial port as a decimal number.
  - Prints the received value back to the serial port.
  - Turns on an LED when a button interrupt occurs.
  - Uses MsTimer2 to turn the LED off after the received delay.
  - Requires the MsTimer2 library to be installed.
*/

#include <MsTimer2.h>

// Hardware pins
const int ledPin = 4;
const int buttonPin = 2;

// Debounce configuration for the button interrupt
const unsigned long debounceDelay = 50UL;

// State variables
volatile bool buttonPressed = false;
volatile unsigned long lastInterruptTime = 0;
volatile bool ledActive = false;
int buttonStateOnce = HIGH;  // Track if we already sent state '2'
unsigned long receivedDuration = 0;
bool hasValidDuration = false;
char serialInputBuffer[16];
size_t serialInputIndex = 0;

// Forward declarations
void handleButtonPress();
void handleButtonPressState();
long parseDuration(const char* inputText);
void turn_off();

void setup() {
  // Configure the LED pin as an output and keep it off initially
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Configure the button pin with an internal pull-up resistor
  pinMode(buttonPin, INPUT_PULLUP);

  // Start serial communication for user input and feedback
  Serial.begin(9600);
  
  // Send initial state: LED is off
  Serial.println("0");

  // Attach interrupt handler for button press (FALLING edge - pin goes LOW)
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonPress, FALLING);
}

void loop() {
  // Read serial input into a buffer until a full line is available
  while (Serial.available() > 0) {
    char incomingChar = Serial.read();

    if (incomingChar == '\n' || incomingChar == '\r') {
      if (serialInputIndex > 0) {
        serialInputBuffer[serialInputIndex] = '\0';
        long parsedDuration = parseDuration(serialInputBuffer);

        if (parsedDuration > 0) {
          receivedDuration = static_cast<unsigned long>(parsedDuration);
          hasValidDuration = true;

          Serial.print("I received: ");
          Serial.println(receivedDuration);
        }

        serialInputIndex = 0;
      }
    } else {
      if (serialInputIndex < sizeof(serialInputBuffer) - 1) {
        serialInputBuffer[serialInputIndex++] = incomingChar;
      } else {
        Serial.println("Input too long.");
        serialInputIndex = 0;
      }
    }
  }

  // If the interrupt flag was set, process the button press
  if (buttonPressed) {
    handleButtonPressState();
  }
  
  // Check if button is released while LED is on
  if (ledActive && digitalRead(buttonPin) == HIGH && buttonStateOnce == LOW) {
    // Button was just released - send state '2'
    Serial.println("2");
    buttonStateOnce = HIGH;
  }
  
  // If button is pressed, update buttonStateOnce
  if (digitalRead(buttonPin) == LOW) {
    buttonStateOnce = LOW;
  }
}

// Interrupt service routine: keep it short and avoid heavy work
void handleButtonPress() {
  unsigned long currentTime = millis();

  if (currentTime - lastInterruptTime < debounceDelay) {
    return;
  }

  lastInterruptTime = currentTime;
  buttonPressed = true;
}

// Main button-handling logic called from loop()
void handleButtonPressState() {
  buttonPressed = false;

  if (!hasValidDuration) {
    Serial.println("No valid duration available.");
    return;
  }

  // Turn on the LED and start a one-shot timer
  digitalWrite(ledPin, HIGH);
  ledActive = true;
  buttonStateOnce = LOW;  // Reset the release detector

  // Send state code '1': button pressed, LED on
  Serial.println("1");

  MsTimer2::stop();
  MsTimer2::set(receivedDuration + 1UL, turn_off);
  MsTimer2::start();
}

// Parse a decimal string into a positive integer
long parseDuration(const char* inputText) {
  if (inputText == nullptr || inputText[0] == '\0') {
    Serial.println("Invalid input");
    return -1;
  }

  for (size_t i = 0; inputText[i] != '\0'; ++i) {
    if (!isdigit(static_cast<unsigned char>(inputText[i]))) {
      Serial.println("Invalid input");
      return -1;
    }
  }

  long parsedValue = atol(inputText);

  if (parsedValue <= 0) {
    Serial.println("Invalid input");
    return -1;
  }

  return parsedValue;
}

// Timer callback: turn the LED off and stop the timer
void turn_off() {
  digitalWrite(ledPin, LOW);
  ledActive = false;
  
  // Send state code '0': LED off
  Serial.println("0");
  
  MsTimer2::stop();
}
