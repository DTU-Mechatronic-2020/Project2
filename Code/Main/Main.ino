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
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
      if (i < 2) {
        starthour += (char)byteArrayPayload[i];
      }
      if (i > 2) {
        startminute += (char)byteArrayPayload[i];
      }
    }
    Serial.println(payload);
    starttime = payload;
    rentalperiod = starttime + "-";
    starthour = starthour.toInt();
    startminute = startminute.toInt();
    Serial.println(starthour);
    Serial.println(startminute);
    Serial.println(rentalperiod);



    //client.publish("mqtt", String(payload).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }


  // End time: The time when the booking should end.
  if (topic == "End-time") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
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
    endhours = endhour.toInt();
    endminutes = endmin.toInt();
    rentalperiod = rentalperiod + endtime;
    Serial.println(endhours);
    Serial.println(endminutes);
    Serial.println(rentalperiod);
    //client.publish("mqtt", String(payload).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  // End time: The time when the booking should end.
  if (topic == "Bookingstatus") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }
    bookingstatus = String(payload); // Temporary definition
    Serial.print(payload);
    //client.publish("mqtt", String(payload).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  // Gets the number og the cabinet.
  if (topic == "Skabnr") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }
    skabnr = payload;
    Serial.print(payload);
    //client.publish("mqtt", String(payload).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }





}

///////// CALLBACK SLUT /////////




void setup() {
  Serial.begin(115200);
  lockstatus = "Locked"; // Temporary definition
  skabnr = "11"; // Temporary definition
  bookingstatus = "Booked"; // Temporary definition
  rentalperiod = "12:00-18:00"; // Temporary definition
  endhours = 18; // Temporary definition
  endminutes = 45; // Temporary definition
  pinMode(GreenLedPin, OUTPUT);
  digitalWrite(GreenLedPin, LOW);
  pinMode(BlueLedPin, OUTPUT);
  digitalWrite(BlueLedPin, LOW);
  pinMode(RedLedPin, OUTPUT);
  digitalWrite(RedLedPin, LOW);
  u8g2.begin();
  //connect to the wifi access point

  setup_wifi(); // Kører WiFi loopet og forbinder herved.
  client.setServer(mqtt_server, mqtt_port); // Forbinder til mqtt serveren (defineret længere oppe)
  client.setCallback(callback); // Ingangsætter den definerede callback funktion hver gang der er en ny besked på den subscribede "cmd"- topic
  OLEDScreen();
}

void loop() {
  if (millis() >= time_now + oneminute) {
    time_now += oneminute;
    OLEDScreen();
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
