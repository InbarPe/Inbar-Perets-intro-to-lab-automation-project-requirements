// Step 1: Basic servo-based fan motion test for the final project.
// This sketch proves that the servo can move the fan assembly safely before adding sensors or other features.

#include <Servo.h>

const int servoPin = 3;
const int servoMinAngle = 20;
const int servoMaxAngle = 160;
const int servoStartAngle = 90;
const unsigned long movementDelayMs = 750;

Servo myServo;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  Serial.println("Step 1: Servo fan basic test");
  Serial.println("The fan is considered active when the servo moves correctly.");

  myServo.attach(servoPin);
  moveServoToAngle(servoStartAngle);
  Serial.println("Servo initialized at the center position.");
}

void loop() {
  moveServoToAngle(servoMinAngle);
  moveServoToAngle(servoStartAngle);
  moveServoToAngle(servoMaxAngle);
  moveServoToAngle(servoStartAngle);
}

void moveServoToAngle(int requestedAngle) {
  int safeAngle = constrain(requestedAngle, servoMinAngle, servoMaxAngle);
  myServo.write(safeAngle);
  printStatus("Moving servo", safeAngle);
  delay(movementDelayMs);
}

void printStatus(const char *message, int angle) {
  Serial.print(message);
  Serial.print(" -> angle: ");
  Serial.print(angle);
  Serial.println(" degrees");
}
