// This code has been made for the course Mechatronics engineering design F20.
// The code has been made by Group 8 for the second project of the course, Project 2 - IoT


// Import af biblioteker

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <PubSubClient.h>
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include "Setup.h"


/////// FUNKTIONSOPSÆTNING ////////


// Opretter en placeholder for callback-funktionen til brug senere. Den rigtige funktion ses længere nede.
void callback(char* byteArraytopic, byte* byteArrayPayload, unsigned int length);

// Opretter en klient der kan forbinde til en specifik internet IP adresse.
WiFiClient espClient; // Initialiserer wifi bibloteket ESP8266Wifi, som er inkluderet under "nødvendige bibloteker"

// Opretter forbindelse til mqtt klienten:
PubSubClient client(mqtt_server, mqtt_port, callback, espClient); // Initialiserer bibloteket for at kunne modtage og sende beskeder til mqtt. Den henter fra definerede mqtt server og port. Den henter fra topic og beskeden payload

/////// FUNKTIONSOPSÆTNING SLUT /////////

//
//
//
//
//
//

///////// CALLBACKFUNKTION ////////

// Definerer callback funktionen der modtager beskeder fra mqtt
// OBS: den her funktion kører hver gang MCU'en modtager en besked via mqtt
void callback(char* byteArraytopic, byte* byteArrayPayload, unsigned int length) {
  // Konverterer indkomne besked (topic) til en string:
  String topic;
  topic = String(byteArraytopic);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  // Konverterer den indkomne besked (payload) fra en array til en string:
  // Topic == Temperaturmaaler, Topic == Kraftsensor
  payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload

  // Takes the given topics and uses to define times.

  // Start time: The time when the booking should start from.
  if (topic == "Start-time") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    stahour = ""; // Nulstiller de to variabler, stahour og staminute som bliver brugt til at finde booking tidsrummet.
    staminute = "";
    for (int i = 0; i < length; i++) {

      payload += (char)byteArrayPayload[i];
      // Det regnes med at beskeden er i et bestemt 5 char format således: 12:30
      if (i < 2) {

        stahour += (char)byteArrayPayload[i]; //Stahour bliver derfor time antallet 1+2 = 12
      }
      if (i > 2) {
        staminute += (char)byteArrayPayload[i]; //Staminute bliver til minut antallet 3+0 = 30
      }
    }
    Serial.println(payload);
    starttime = payload;
    rentalperiod = starttime + "-"; //Lejeperioden bliver defineret som en string med startstiden først.

    // Time- og minutværdierne bliver omdannet fra string til int-værdier så de kan bruges i udregninger.
    starthour = stahour.toInt();
    startminute = staminute.toInt();
    client.publish("RecievedStartTime", String(payload).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }


  // End time: The time when the booking should end.
  if (topic == "End-time") {
    // Nulstiller payload, endhour og endmin variablen så forloopet ikke appender til en allerede eksisterende definationer
    payload = "";
    endhour = "";
    endmin = "";



    // Som med Start-time, forventes end-time at være i et 5-char format lignende 12:30.
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
      if (i < 2) {
        endhour += (char)byteArrayPayload[i];
      }
      if (i > 2) {
        endmin += (char)byteArrayPayload[i];
      }
    }
    Serial.println(payload);
    endtime = payload;
    // Konverterer string værdier til int værdier.
    endhours = endhour.toInt();
    endminutes = endmin.toInt();
    rentalperiod = rentalperiod + endtime; //Afslutter lejeperioden med stringen for slutlejeperioden.
    Serial.println(endhours);
    Serial.println(endminutes);
    Serial.println(rentalperiod);
    client.publish("RecievedEndTime", String(payload).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  // Giver clienten en mulighed for at ændre bookingstatus. Tænkes at skal bruges når en booking anulleres.
  if (topic == "Bookingstatus") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }
    if (payload == "Check") {
      client.publish("CurrentBookingstatus", String(bookingstatus).c_str()); //Lader NodeRed checke hvad skabets nuværende status er.
    }
    else {
      bookingstatus = String(payload); //Sætter bookingstatus til at være payloaden.
      client.publish("CurrentBookingstatus", String(payload).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
    }
    Serial.print(payload);
  }

  // Gets the number og the cabinet : I tilfælde af at der er flere skabe, så kan skabet nr. defineres.
  if (topic == "Skabnr") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }
    skabnr = payload; //
    Serial.print(payload);
    client.publish("LokcerNumber", String(payload).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }
  // Giver clienten mulighed for at åbne og lukke skabet.
  if (topic == "Lockstatus") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }
    if (payload == "Check") {
      client.publish("CurrentLockstatus", String(lockstatus).c_str()); //Lader NodeRed checke hvad skabets nuværende status er.
    }
    else {
      lockstatus = payload; // Lockstatus bliver defineret som payloaden.
      client.publish("CurrentLockstatus", String(payload).c_str()); // Publish besked fra MCU til et valgt topic. //Husk at subscribe til topic'et i NodeRed.
    }
    Serial.print(payload);
  }





}

///////// CALLBACK SLUT /////////




void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);

  ////////// Temporary definitions start: /////////
  lockstatus = "Unlocked"; // Temporary definition
  skabnr = "11"; // Temporary definition
  bookingstatus = "true"; // Temporary definition
  timeClient.update(); //Opdaterer tiden.

  ////////// Temporary definitions end: /////////

  ///////// LED SETUP /////////////
  pinMode(GreenLedPin, OUTPUT);
  digitalWrite(GreenLedPin, LOW);
  pinMode(BlueLedPin, OUTPUT);
  digitalWrite(BlueLedPin, LOW);
  pinMode(RedLedPin, OUTPUT);
  digitalWrite(RedLedPin, LOW);

  ///////// Den gule LED repræsenterer låsen. //////////
  pinMode(YellowLedPin, OUTPUT);
  digitalWrite(YellowLedPin, LOW);
  u8g2.begin(); //Tænder skærmen

  //connect to the wifi access point
  setup_wifi(); // Kører WiFi loopet og forbinder herved.
  client.setServer(mqtt_server, mqtt_port); // Forbinder til mqtt serveren (defineret længere oppe)
  client.setCallback(callback); // Ingangsætter den definerede callback funktion hver gang der er en ny besked på den subscribede "cmd"- topic
  OLEDScreen(); // Kører OLEDScreen funktionen ved start, således at der er information på skærmen.
}

void loop() {
  // Der er monteret en fysisk knap der kan bruges til at tvinge skabet op. //
  buttonState = digitalRead(buttonPin); // Læser knapværdi hele tiden
  // Hvis knappen er holdt inde i 4 sekunder, så åbnes skabet og OLED printer besked.
  if (buttonState == HIGH) {
    delay(1000);
    emergency ++;
    if (emergency >= 4) {
      u8g2.clearBuffer();          // clear the internal memory
      u8g2.setFont(u8g2_font_maniac_tf);
      u8g2.setCursor(20, 40);
      u8g2.print("!LOCK!");
      u8g2.setCursor(20, 64);
      u8g2.print("!OPEN!");
      u8g2.sendBuffer();          // transfer internal memory to the display
      lockstatus == "Unlocked";   // Unlocker Lås
      digitalWrite(GreenLedPin, HIGH);
      digitalWrite(RedLedPin, LOW);
      digitalWrite(YellowLedPin, HIGH); // Lås bliver åbnet
      Serial.println("LOCK HAS BEEN OPENED!");
      delay(15000); // Laver et 15 sekunders delay hvor funktionerne ikke kører, mens låsen er åben.
    }
  }
  else if (buttonState == LOW) {
    emergency = 0;
  }
  if (millis() >= time_now + tenseconds) { //Opdaterer skærm, vejr, tid mm. hvert 10. sekund.
    time_now += tenseconds;
    OLEDScreen();
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
