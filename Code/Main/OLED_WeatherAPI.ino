void OLEDScreen() {
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    HTTPClient http; //Declare an object of class HTTPClient

    // REMEMBER to replace YOURKEY from the url with your own key

    http.begin("http://api.openweathermap.org/data/2.5/forecast?q=Copenhagen,dk&appid=95fb8a0ca887dc68309f34e8d20c8ceb&units=metric&cnt=3"); //Specify request destination
    int httpCode = http.GET(); //Send the request

    if (httpCode > 0) { //Check the returning code

      String payload = http.getString(); //Get the request response payload - the json object as a plain text

      //define a json object buffer
      DynamicJsonDocument jsonBuffer(1024);

      //try convert the plain text to an arduino - json object
      DeserializationError success = deserializeJson(jsonBuffer, payload);

      // if conversion not successful:
      // NOTE: there are two cases this can fail: if the json object is too big for the arduino
      // OR the communication protocol to the api is secure: e.g. 'https' instead of 'http'
      // There are solutions to both, but not relevant for this exercise..

      if (!success) {
        Serial.println("deserialization failed");
        delay(1000);
        return;
      }
      //if it is successful, let's grab a data fragment and show it on the serial terminal:

      // the first forecast in the json object is at jsonBuffer['list'][0]
      String timestamp = jsonBuffer["list"][0]["dt_txt"];
      //Serial.print("The time for this forecast: ");
      //Serial.println(timestamp);

      // The 'weather' in the first position in the list, is actually a list, with only one element...
      String desc = jsonBuffer["list"][0]["weather"][0]["description"];
      //Serial.print("The weather will be: ");
      //Serial.println(desc);

      // The ArduinoJson library also has a utility function to 'pretty print' json objects, try:
      //serializeJsonPretty(jsonBuffer["list"][0], Serial);
      //or
      //Serial.println("\nThe full forecast Json looks like this: ");
      //serializeJsonPretty(jsonBuffer, Serial);
      float temp = jsonBuffer["list"][0]["main"]["temp"];
      timeClient.update();

      Serial.print(daysOfTheWeek[timeClient.getDay()]);
      Serial.print(", ");
      Serial.print(timeClient.getHours());
      Serial.print(":");
      Serial.println(timeClient.getMinutes());
      remaining_hours = endhours - int(timeClient.getHours());
      if (endminutes - int(timeClient.getMinutes()) <= 0) {
        remaining_hours = remaining_hours - 1;
        remaining_minutes = 60 - abs(endminutes - int(timeClient.getMinutes()));
      }
      else {
        remaining_minutes = endminutes - int(timeClient.getMinutes());
      }
      u8g2.clearBuffer();          // clear the internal memory
      u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      Serial.println("Time Remaining!");
      Serial.print(remaining_hours);
      Serial.print(":");
      Serial.println(remaining_minutes);

      Serial.println("Hours and Minutes untill booking:");
      Serial.println(timeClient.getHours() - starthour);
      Serial.println(timeClient.getMinutes() - startminute);

      if (timeClient.getHours() - starthour <= 0 && timeClient.getMinutes() - startminute >= 0) {
        Futurebooking = "True";
      }
      else {
        Futurebooking = "False";
      }
      if (float(remaining_hours) < 0 && remaining_minutes == 60) {
        bookingstatus = "Free";
        lockstatus = "Unlocked";
      }
      else {
        if (float(remaining_hours) < 0) {
          remaining_hours = 0;
        }
      }
      if (bookingstatus == "Free") {
        u8g2.setCursor(0, 10);
        u8g2.print(timeClient.getHours());
        u8g2.print(":");
        u8g2.printf("%02d", timeClient.getMinutes()); //Makes sure that there's always being printed with two dicimals.
        u8g2.setCursor(70, 10);
        u8g2.print(daysOfTheWeek[timeClient.getDay()]);
        u8g2.setCursor(40, 20);
        u8g2.print(desc);
        u8g2.setCursor(0, 20);
        u8g2.print(String(temp));
        u8g2.print("C");
        u8g2.setCursor(0, 30);
        u8g2.print("Skab nr: ");
        u8g2.print(skabnr);
        u8g2.setCursor(75, 30);
        u8g2.print(lockstatus);
        u8g2.setCursor(0, 40);
        u8g2.print("Status: ");
        u8g2.print(bookingstatus);
        Serial.println("Futurebooking = ");
        Serial.print(Futurebooking);
        if (Futurebooking == "True") {
          u8g2.setCursor(0, 50);
          u8g2.print("Next booking at: ");
          u8g2.print(starttime);


        }






        u8g2.setCursor(0, 60);
        u8g2.print("Use our app to rent this!");
      }
      else if (bookingstatus == "Booked") {
        u8g2.setCursor(0, 10);
        u8g2.print(timeClient.getHours());
        u8g2.print(":");
        u8g2.printf("%02d", timeClient.getMinutes()); //Makes sure that there's always being printed with two dicimals.
        u8g2.setCursor(70, 10);
        u8g2.print(daysOfTheWeek[timeClient.getDay()]);
        u8g2.setCursor(40, 20);
        u8g2.print(desc);
        u8g2.setCursor(0, 20);
        u8g2.print(String(temp));
        u8g2.print("C");
        u8g2.setCursor(0, 30);
        u8g2.print("Skab nr: ");
        u8g2.print(skabnr);
        u8g2.setCursor(80, 30);
        u8g2.print(lockstatus);
        u8g2.setCursor(0, 40);
        u8g2.print("Status: ");
        u8g2.print(bookingstatus);
        u8g2.setCursor(0, 50);
        u8g2.print("Period: ");
        u8g2.print(rentalperiod);
        u8g2.setCursor(0, 60);
        u8g2.print("Remaining time: ");
        u8g2.printf("%02d", remaining_hours);
        u8g2.print(":");
        u8g2.printf("%02d", remaining_minutes);
      }
      u8g2.sendBuffer();          // transfer internal memory to the display
      LEDControl();
    }
    http.end(); //Close connection
  }
  /*
    Forecast time date
    Forecast description: e.g. 'few clouds'
    Forecast temperature in C
  */
}
