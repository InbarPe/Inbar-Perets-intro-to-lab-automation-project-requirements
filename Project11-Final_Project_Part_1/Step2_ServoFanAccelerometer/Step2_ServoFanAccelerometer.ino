// Step 2: Servo angle controlled by an accelerometer input.
// This sketch reads the accelerometer value and uses it to move the servo,
// so the fan position changes based on sensor input.

#include <Servo.h>
#include "Arduino_SensorKit.h"

const int servoPin = 3;
const int fanPin = 7;
const int buttonPin = 6;
const int servoMinAngle = 20;
const int servoMaxAngle = 160;
const int accelSamples = 20;
const int angleSensitivity = 30;
const unsigned long readDelayMs = 50;
const unsigned long buttonDebounceMs = 250;

Servo myServo;
float accelCenterX = 0.0;
bool systemRunning = false;
bool lastButtonState = HIGH;
unsigned long lastButtonPressTime = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  Serial.println("Step 2: Accelerometer-controlled servo");
  Serial.println("Place the board flat and wait for calibration...");

  pinMode(fanPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(fanPin, LOW);

  Accelerometer.begin();
  myServo.attach(servoPin);
  accelCenterX = calibrateAccelerometer();
  myServo.write(90);
  Serial.print("Calibration complete. Center X value = ");
  Serial.println(accelCenterX);
  Serial.println("Initial state: movement stopped.");
  Serial.println("Press the button to start movement.");
}

void loop() {
  handleButtonToggle();

  if (systemRunning) {
    float accelX = readSmoothedAccelerometerX();
    float delta = accelX - accelCenterX;
    int servoAngle = 90 + int(delta * angleSensitivity);
    servoAngle = constrain(servoAngle, servoMinAngle, servoMaxAngle);

    myServo.write(servoAngle);
    digitalWrite(fanPin, HIGH);
    printStatus(accelX, delta, servoAngle);
  } else {
    myServo.write(90);
    digitalWrite(fanPin, LOW);
    Serial.println("System stopped. Press the button to start movement.");
  }

  delay(readDelayMs);
}

void handleButtonToggle() {
  bool buttonState = digitalRead(buttonPin);
  unsigned long now = millis();

  if (buttonState != lastButtonState && (now - lastButtonPressTime) > buttonDebounceMs) {
    lastButtonPressTime = now;
    lastButtonState = buttonState;

    if (buttonState == LOW) {
      systemRunning = !systemRunning;
      if (systemRunning) {
        Serial.println("Button pressed: movement started.");
      } else {
        Serial.println("Button pressed: movement stopped.");
      }
    }
  }
}

float calibrateAccelerometer() {
  float total = 0.0;

  for (int i = 0; i < accelSamples; i++) {
    total += Accelerometer.readX();
    delay(20);
  }

  return total / accelSamples;
}

float readSmoothedAccelerometerX() {
  float total = 0.0;

  for (int i = 0; i < accelSamples; i++) {
    total += Accelerometer.readX();
    delay(5);
  }

  return total / accelSamples;
}

void printStatus(float accelX, float delta, int servoAngle) {
  Serial.print("Accelerometer X = ");
  Serial.print(accelX);
  Serial.print(" | Delta = ");
  Serial.print(delta);
  Serial.print(" | Servo angle = ");
  Serial.print(servoAngle);
  Serial.println(" degrees");
}
