#include <Adafruit_MAX31865.h>
#include <MD_PWM.h>

const int rotaryPin = A0;
const int pwmPin = 10;
const uint16_t pwmFrequency = 30; // PWM frequency in Hz (0..300)

MD_PWM pwmLed(pwmPin);

// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 thermo = Adafruit_MAX31865(4, 11, 12, 13);
// use hardware SPI, just pass in the CS pin
//Adafruit_MAX31865 thermo = Adafruit_MAX31865(10);

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit MAX31865 PT100 Sensor Test!");

  thermo.begin(MAX31865_4WIRE);  // set to 2WIRE or 4WIRE as necessary

  if (!pwmLed.begin(pwmFrequency)) {
    Serial.println("ERROR: MD_PWM begin failed");
  }
  if (!pwmLed.enable()) {
    Serial.println("ERROR: MD_PWM enable failed");
  }
}


void loop() {
  uint16_t rtd = thermo.readRTD();

  Serial.print("RTD value: "); Serial.println(rtd);
  float ratio = rtd;
  ratio /= 32768;
  Serial.print("Ratio = "); Serial.println(ratio,8);
  Serial.print("Resistance = "); Serial.println(RREF*ratio,8);
  Serial.print("Temperature = "); Serial.println(thermo.temperature(RNOMINAL, RREF));

  // Potentiometer
  int rotaryValue = analogRead(rotaryPin);
  int dutyValue = map(rotaryValue, 0, 1023, 0, 255);
  dutyValue = constrain(dutyValue, 0, 255);

  pwmLed.write((uint8_t)dutyValue);

  Serial.print("Rotary A0 = ");
  Serial.print(rotaryValue);
  Serial.print("  PWM duty = ");
  Serial.println(dutyValue);

  // Check and print any faults
  uint8_t fault = thermo.readFault();
  if (fault) {
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold"); 
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold"); 
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias"); 
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage"); 
    }
    thermo.clearFault();
  }
  Serial.println();
  delay(1000);
}
