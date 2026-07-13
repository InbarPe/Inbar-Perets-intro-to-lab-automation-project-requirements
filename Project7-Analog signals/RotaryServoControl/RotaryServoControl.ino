#include <Servo.h>

const int rotaryPin = A0;
const int servoPin = 7;
const int servoMinAngle = 20;   // adjust to your servo's safe minimum angle
const int servoMaxAngle = 160;  // adjust to your servo's safe maximum angle
Servo myServo;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  Serial.println("Rotary to Servo Control");

  myServo.attach(servoPin);
  myServo.write((servoMinAngle + servoMaxAngle) / 2); // center within safe range
}

void loop() {
  int rotaryValue = analogRead(rotaryPin);
  int angle = map(rotaryValue, 0, 1023, servoMinAngle, servoMaxAngle);
  angle = constrain(angle, servoMinAngle, servoMaxAngle);

  myServo.write(angle);

  Serial.print("Rotary A0 = ");
  Serial.print(rotaryValue);
  Serial.print("  Servo angle = ");
  Serial.println(angle);

  delay(100);
}
