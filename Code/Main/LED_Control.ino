void LEDControl() {
  Serial.println("LED Staus:");
  if (bookingstatus == "Free") {
    digitalWrite(GreenLedPin, HIGH);
    digitalWrite(RedLedPin, LOW);
    digitalWrite(BlueLedPin, LOW);
    Serial.println("Green On, Red Off, Blue Off");
  }
  else if (bookingstatus == "Booked") {
    digitalWrite(BlueLedPin, HIGH);

    if (lockstatus == "Locked") {
      digitalWrite(GreenLedPin, LOW);
      digitalWrite(RedLedPin, HIGH);
      Serial.println("Green Off, Red On, Blue On");
    }
    else if (lockstatus == "Unlocked") {
      digitalWrite(GreenLedPin, HIGH);
      digitalWrite(RedLedPin, LOW);
      Serial.println("Green On, Red Off, Blue On");
    }
  }
}