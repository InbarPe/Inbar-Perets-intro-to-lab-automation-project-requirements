void setup() {
  Serial.begin(115200);
  int error = 5;
  int Kp = 2;
  int output = Kp*error;
  Serial.println("output = ");
  Serial.println(output);

}

void loop() {

}
