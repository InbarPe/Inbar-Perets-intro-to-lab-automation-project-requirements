#include <MD_PWM.h>

const int rotaryPin = A0;
const int pwmPin = 4;
const uint16_t pwmFrequency = 50; // PWM frequency in Hz (0..300)

MD_PWM pwmLed(pwmPin);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  Serial.println("Rotary LED PWM using MD_PWM");
  Serial.print("PWM frequency: ");
  Serial.print(pwmFrequency);
  Serial.println(" Hz");

  if (!pwmLed.begin(pwmFrequency)) {
    Serial.println("ERROR: MD_PWM begin failed");
  }
  if (!pwmLed.enable()) {
    Serial.println("ERROR: MD_PWM enable failed");
  }
}

void loop() {
  int rotaryValue = analogRead(rotaryPin);
  int dutyValue = map(rotaryValue, 0, 1023, 0, 255);
  dutyValue = constrain(dutyValue, 0, 255);

  pwmLed.write((uint8_t)dutyValue);

  Serial.print("Rotary A0 = ");
  Serial.print(rotaryValue);
  Serial.print("  PWM duty = ");
  Serial.println(dutyValue);

  delay(100);
}
