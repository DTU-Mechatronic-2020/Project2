// This code has been made for the course Mechatronics engineering design F20. 
// The code has been made by Group 8 for the second project of the course, Project 2 - IoT  

//Funktion der styrer skabets LEDer.

void LEDControl() {
  //Hvis skabet er ledigt vil en blå LED være slukket.
  if (bookingstatus == "true") {
    digitalWrite(BlueLedPin, LOW);
  }
  //Hvis skabet derimod er booked vil en blå LED være tændt.
  else if (bookingstatus == "false") {
    digitalWrite(BlueLedPin, HIGH);
  }
  // Hvis skabets lås er låst vil den røde LED lyse op.
  if (lockstatus == "Locked") {
    digitalWrite(GreenLedPin, LOW);
    digitalWrite(RedLedPin, HIGH);
    digitalWrite(YellowLedPin, LOW);
  }
  // Hvis skabets lås er ulåst, så vil den grønne lampe lyse op. Den gule lampe repræsenterer låsen som også er tændt når den er låst.
  else if (lockstatus == "Unlocked") {
    digitalWrite(GreenLedPin, HIGH);
    digitalWrite(RedLedPin, LOW);
    digitalWrite(YellowLedPin, HIGH);
  }
}
