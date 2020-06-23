void setupCode() {
  client.setServer(mqtt_server, mqtt_port); // Connects to MQTT    server
  client.setCallback(callback); // Runs the callback function "callback" every time a new subcribed topic appears from MQTT
  servo.attach(15);//D8, controls the gate motor
  pinMode(button, INPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(relayPin, OUTPUT); 
  pinMode(floatswitch, INPUT_PULLUP);
  digitalWrite(relayPin, LOW); // makes sure the relay is off 
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  servo.write(0);
  u8g2.begin();
  strip.begin();           // Initialize NeoPixel strip
  strip.setBrightness(10); // Set BRIGHTNESS to about 1/25 (max = 255)
  strip.clear();
}
