void lidMotor() { // Sends messages to the slave if the Lid should open
  buttonState = digitalRead(button);
  if (chipOut == 1) {
    Wire.beginTransmission(0x08); /* begin with device address 0x08 */
    Wire.write("1");
    Wire.endTransmission();
    chipOut = 0;                // makes sure the loop exits
  }
  if (buttonState == 1) {
    Wire.beginTransmission(0x08); /* begin with device address 0x08 */
    Wire.write("0");
    Wire.endTransmission();
  }
}

void distanceSensor() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  // Serial.print("Distance: "); // Included for debugging purpose
  // Serial.println(distance);
}

void pumpAndGate() {

  digitalWrite(relayPin, HIGH); // turns the relay (and therfore the punp) on
  client.publish("countIn", "1"); // Sends message to MQTT that a perosn entered the shop
  loadingAnim(strip.Color(0,   0,   255), sanTime); // Red (wait)
  strip.clear();
  digitalWrite(relayPin, LOW); // Turns off the pump
  enterOLED();

  Serial.print("jeg ser noget");
  Serial.print(allowedP);

  for (int i = 0; i < 360; i++) {  // Open the gate, made in a for loop so the speed can be controlled
    i++;
    servo.write(i / 4);
    delay (5.55555);
  }
  countdownAnim(strip.Color(  0, 255,   0), gateTime); // Green (go)

  for (int i = 0; i < 360; i++) { // Closes the gate
    i++;
    servo.write(90 - i / 4);
    delay (5.55555);
  }
  // Serial.print("Jeg ser intet længere"); - included for debugging purposes

}

void waterLevel() { // Checks the level of sanitizer
  waterState = digitalRead(floatswitch);
  if (waterState != test) {
    if (waterState == 1) {
      client.publish("currW", "1"); // Sends message to MQTT, that the container should be filled
      Serial.print("Please fill container"); //help to debug
    }
    if (waterState == 0) {
      client.publish("currW", "0");
      Serial.print("Container has been filled"); //help to debug
    }
    test = waterState;
  }
}
