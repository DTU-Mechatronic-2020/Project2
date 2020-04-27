// This code has been made for the course Mechatronics engineering design F20.
// The code has been made by Group 8 for the second project of the course, Project 2 - IoT

// Function that controls and changes the OLED screen, aswell as uses an weather API.
void OLEDScreen() {
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    HTTPClient http; //Declare an object of class HTTPClient

    // Får vejrdata fra en vejr-api. Her er vejret fra københavn.
    http.begin("http://api.openweathermap.org/data/2.5/forecast?q=Copenhagen,dk&appid=95fb8a0ca887dc68309f34e8d20c8ceb&units=metric&cnt=3"); //Specify request destination
    int httpCode = http.GET(); //Send the request

    // Hvis der er data modtaget, så kører koden.
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

      // the first forecast in the json object is at jsonBuffer['list'][0]
      String timestamp = jsonBuffer["list"][0]["dt_txt"];

      // The 'weather' in the first position in the list, is actually a list, with only one element...
      String desc = jsonBuffer["list"][0]["weather"][0]["description"];

      // The ArduinoJson library also has a utility function to 'pretty print' json objects, try:
      //serializeJsonPretty(jsonBuffer["list"][0], Serial);
      //or
      //Serial.println("\nThe full forecast Json looks like this: ");
      //serializeJsonPretty(jsonBuffer, Serial);

      float temp = jsonBuffer["list"][0]["main"]["temp"]; //Tager temperaturen fra vejr APIen
      timeClient.update(); //Opdaterer tiden.
      u8g2.clearBuffer();          // clear the internal memory
      u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font

      // Checker for fremtidige bookinger
      // Hvis der bliver modtaget en booking i fremtiden, så skal skabet stadigt være ledigt indtil bookingstidsrummet.
      if ((timeClient.getHours() * 60 + timeClient.getMinutes()) - (starthour * 60 + startminute) < 0) {
        Futurebooking = "True";
        bookingstatus = "true";
      }
      else {
        // Hvis bookingen er igang, så skal skabet være betegnet som værende false.
        Futurebooking = "False";
        bookingstatus = "false";
        //Start tidspunktet når en booking først er igang, skal ikke have indflydelse.
        starthour = 0;
        startminute = 0;
        // Hvis bookingen er igang, men der er mindre end 1 time tilbage således at følgende gælder:
        // Slut tidspunkt: 15:30 | Nuværende tidspunkt: 15:20 | Så vil der være 10 minutter tilbage af bookingen, men grundet funktionen længere oppe, så kan det restrende timeantal godt være vist som -1 time.
        // I dette tilfælde skal timeantallet der ikke kunne være under 0.
      }

      // Udregner hvor meget tid der er tilbage af en aktuel booking.
      if (bookingstatus == "false") {
        remaining_hours = floor(((endhours * 60 + endminutes) - (timeClient.getHours() * 60 + timeClient.getMinutes())) / 60); //Finder hvor mange timer der er tilbage og runder ned. 1.15 timer bliver til 1 time.
        remaining_minutes =60- abs(endminutes - int(timeClient.getMinutes())); //Finder hvor mange minutter der er tilbage.
        // Hvis en igangværende booking udløber, så skal skabet igen være ledigt og låsen skal åbnes.
        if (remaining_hours <= 0 && remaining_minutes <= 0 || remaining_hours < 0 ) {
          bookingstatus = "true";
          lockstatus = "Unlocked";
          endhours = 0;
          endminutes = 0;
        }
      }
      Serial.println(remaining_hours);
      Serial.println(remaining_minutes);
      // Skærmen skal vise forskellige ting ud fra om der er en igangværende booking.
      if (bookingstatus == "true") {
        u8g2.setCursor(0, 10);
        u8g2.printf("%02d", timeClient.getHours()); //Makes sure that there's always being printed with two dicimals.
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
        u8g2.print("Status: Free");

        // I tilfælde af at skabet er ledigt, men at der senere vil være en booking, så vises det hvornår denne booking forekommer.
        if (Futurebooking == "True") {
          u8g2.setCursor(0, 50);
          u8g2.print("Next booking at: ");
          u8g2.print(starthour);
          u8g2.print(":");
          u8g2.print(startminute);
        }
        u8g2.setCursor(0, 60);
        u8g2.print("You can book this!");
      }
      else if (bookingstatus == "false") {
        u8g2.setCursor(0, 10);
        u8g2.printf("%02d", timeClient.getHours()); //Makes sure that there's always being printed with two dicimals.
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
        u8g2.print("Status: Booked");
        u8g2.setCursor(0, 50);
        u8g2.print("Period: ");
        u8g2.print(rentalperiod); // Printer bookingperioden på skærmen.
        u8g2.setCursor(0, 60);
        u8g2.print("Remaining time: ");
        u8g2.printf("%02d", remaining_hours); //Makes sure that there's always being printed with two dicimals.
        u8g2.print(":");
        u8g2.printf("%02d", remaining_minutes); //Makes sure that there's always being printed with two dicimals.
      }
      u8g2.sendBuffer();          // transfer internal memory to the display
      LEDControl(); // Kører LEDerne sammen med skærmen
    }
    http.end(); //Close connection
  }
}
