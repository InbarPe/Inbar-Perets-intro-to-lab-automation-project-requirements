#include <Servo.h>
#include <Wire.h>
#include "Arduino_SensorKit.h"
#define BUZZER 5

const int servoPin = 3;
const int fanPin = 7;
const int buttonPin = 6;
const int oledWidth = 128;
const int oledHeight = 64;
const int servoMinAngle = 20;
const int servoMaxAngle = 160;
const int fanMinSafeAngle = 85; 
const int fanMaxSafeAngle = 105;
const int accelSamples = 10; 
const int angleSensitivity = 30;
const unsigned long readDelayMs = 50;
const unsigned long buttonDebounceMs = 250;

Servo myServo;
float accelCenterX = 0.0;
bool systemRunning = false; 
bool lastButtonState = HIGH;
unsigned long lastButtonPressTime = 0;
int currentServoAngle = 90;
bool buzzerActive = false; 

void setup() {
  // Start serial communication at 9600 baud
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  Serial.println("Place the board flat and wait for calibration...");

  pinMode(fanPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(fanPin, LOW);
  pinMode(BUZZER, OUTPUT);

  Accelerometer.begin();
  myServo.attach(servoPin);
  accelCenterX = calibrateAccelerometer();

  Oled.begin();
  Oled.setFlipMode(true);
  
  myServo.write(90);
  currentServoAngle = 90;

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
    currentServoAngle = servoAngle;
    
    if (servoAngle >= fanMinSafeAngle && servoAngle <= fanMaxSafeAngle) {
      digitalWrite(fanPin, HIGH); 
      noTone(BUZZER);
      buzzerActive = false; 
    } else {
      digitalWrite(fanPin, LOW); 
      tone(BUZZER, 100);
      buzzerActive = true;  
    }

    printStatus(accelX, delta, servoAngle);
  } else {
    myServo.write(90);
    currentServoAngle = 90;
    digitalWrite(fanPin, LOW);
    noTone(BUZZER);
    buzzerActive = false; 
  }

  // Update the display
  Oled.setFont(u8x8_font_chroma48medium8_r); 
  Oled.setCursor(0, 3);
  Oled.print("Angle: ");
  Oled.print(currentServoAngle);

  // Send the CSV data to the PC
  logToSerial();

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

// Sends a single line of CSV data out of the Serial port
void logToSerial() {
  Serial.print(millis());
  Serial.print(",");
  Serial.print(currentServoAngle);
  Serial.print(",");
  Serial.println(buzzerActive ? 1 : 0);
}