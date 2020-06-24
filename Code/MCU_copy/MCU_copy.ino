// This code has been made for the course Mechatronics engineering design F20.
// The code has been made by Group 8 for the final project of the course, Project 3 - Corona (Final project)


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
#include <HX711.h> //Bug
#include <string>
#include<iostream>

static int mixingStatus = 0;

// values needed to read weight sensors
char c;
int e1;
int e2;
String d;
String d1;
String d2;

String Machinenr;

static int ethanolrcp;
static int waterrcp;
static int glycerinrcp;
static int brintoveriltercp;
static int oilrcp;

int ethanolMass;
int waterMass;
int glycerinMass;
int brinoverilteMass;
int oilMass;

int mixingMass;
int mixingMassMin;


int distance;

//FLAME SENSOR
const int flamePin = D8;
int Flame = LOW;

//LED'er

int ledRED = D6;
int ledGREEN = D7;

char pumpe1[5];
char pumpe2[5];
char pumpe3[5];
char pumpe4[5];
char pumpe5[5];

String ethanolrcpS;
String waterrcpS;
String glycerinrcpS;
String brintoveriltercpS;
String oilrcpS;

char* pump1 = pumpe1;
char* pump2 = pumpe2;
char* pump3 = pumpe3;
char* pump4 = pumpe4;
char* pump5 = pumpe5;

char *recipe[5] = {pump1, pump2, pump3, pump4, pump5};




/////// FUNKTIONSOPSÆTNING ////////


// Opretter en placeholder for callback-funktionen til brug senere. Den rigtige funktion ses længere nede.
void callback(char* byteArraytopic, byte* byteArrayPayload, unsigned int length);

// Opretter en klient der kan forbinde til en specifik internet IP adresse.
WiFiClient espClient; // Initialiserer wifi bibloteket ESP8266Wifi, som er inkluderet under "nødvendige bibloteker"

// Opretter forbindelse til mqtt klienten:
PubSubClient client(mqtt_server, mqtt_port, callback, espClient); // Initialiserer bibloteket for at kunne modtage og sende beskeder til mqtt. Den henter fra definerede mqtt server og port. Den henter fra topic og beskeden payload

/////// FUNKTIONSOPSÆTNING SLUT /////////


//connect to the wifi access point

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

  payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload


  //////////////////////////////////////////////////// INGREDIENTS SETUP //////////////////////////////////////////////////////////////


  ///////////////////////// ETHANOL CONFIGURATION /////////////////////////

  if (topic == "Ethanol") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }

    ethanolrcp = payload.toInt() + 1000; //Cryptation for I2C bus
    ethanolrcpS = String(ethanolrcp);
    ethanolrcpS.toCharArray(pumpe1, 5);

    Serial.println(pumpe1);
  }

  ///////////////////////// WATER CONFIGURATION /////////////////////////

  if (topic == "Water") {
    // Nulstiller payload, endhour og endmin variablen så forloopet ikke appender til en allerede eksisterende definationer
    payload = "";
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }

    waterrcp = payload.toInt() + 2000; //Cryptation for I2C bus
    waterrcpS = String(waterrcp);
    waterrcpS.toCharArray(pumpe2, 5);

    Serial.println(pumpe2);
  }

  ///////////////////////// GLYCERIN CONFIGURATION /////////////////////////

  if (topic == "Glycerin") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }

    glycerinrcp = payload.toInt() + 3000; //Cryptation for I2C bus
    glycerinrcpS = String(glycerinrcp);
    glycerinrcpS.toCharArray(pumpe3, 5);

    Serial.print(pumpe3);
  }

  ///////////////////////// H2O2 CONFIGURATION /////////////////////////

  if (topic == "Brintoverilte") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }

    brintoveriltercp = payload.toInt() + 4000; //Cryptation for I2C bus
    brintoveriltercpS = String(brintoveriltercp);
    brintoveriltercpS.toCharArray(pumpe4, 5);

    Serial.print(pumpe4);
  }

  ///////////////////////// Oil CONFIGURATION /////////////////////////

  if (topic == "Oil") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }

    mixingStatus = 1;
    oilrcp = payload.toInt() + 5000; //Cryptation for I2C bus
    oilrcpS = String(oilrcp);
    oilrcpS.toCharArray(pumpe5, 5);

    Serial.print(pumpe5);
  }

  ///////////////////////// CHOOSING MACHINE /////////////////////////

  if (topic == "Machinenr") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }
    String Machinenr = payload;//
    Serial.print(payload);

    // In the ideal setup there would have been more than one machine, and this value Machinenr would have decided the slave addresses

  }
}

///////// CALLBACK SLUT /////////

int len;

void setup() {
  Serial.begin(115200); /* begin serial for debug */
  setup_wifi(); // Kører WiFi loopet og forbinder herved.
  client.setServer(mqtt_server, mqtt_port); // Forbinder til mqtt serveren (defineret længere oppe)
  client.setCallback(callback); // Ingangsætter den definerede callback funktion hver gang der er en ny besked på den subscribede "cmd"- topic

  bool mixingstatus = LOW;
  bool dispensingstatus = LOW;
  int mixingMassMin = 500;

  //Flame
  pinMode(flamePin, INPUT);

  //LED'er
  pinMode(ledRED, OUTPUT);
  pinMode(ledGREEN, OUTPUT);
  digitalWrite(ledRED, LOW);
  digitalWrite(ledGREEN, LOW);
  Serial.println("Setup Done");
  Wire.begin(D3, D4); /* join i2c bus with SDA=D3 and SCL=D4 of NodeMCU */
  u8g2.begin();
}


void loop() {

  /////////////////// Read function I2C //////////////////
  if (millis() >= time_now + 1000) {
    time_now += 1000;
    Serial.println("One second has passed");
    for (int t = 0; t < 5; t++) {
      Serial.println(recipe[t]);
    }
    Wire.requestFrom(8, 8); // request & read data of size 4 from slave //
    int f = 0;
    while (Wire.available()) {
      char c = Wire.read(); //Read charachters to c
      d += c;
      f ++;
      Serial.print("F is: ");
      Serial.println(f);

      //// Seperate the input to two seperate weight sensors ////
      if (f < 5) {
        d1 += c; //Collect characters to string
      }
      else if (f >= 5) {
        d2 += c;
      }
    }

    if (d1 == "3000" && flamePin == LOW) {
      dispense_time = millis();
      dispensing ();
    }
    else {
      Serial.print("d1 is: ");
      Serial.println(d1);
      Serial.print("d2 is: ");
      Serial.println(d2);
      e1 = d1.toInt(); //Convert received string to integer
      e2 = d2.toInt(); //Convert received string to integer


      ///////////////// Interpretation of I2C weight inputs /////////////////

      //Weight 1
      e1 = e1 - 1000;

      int mixingMass = e1;
      client.publish("Weight 1", String(e1).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.

      //Weight 2
      e2 = e2 - 2000;

      int ethanolMass = e2;
      client.publish("Weight 2", String(e2).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
    }

    Serial.print("e1 is: ");
    Serial.println(e1);
    Serial.print("e2 is: ");
    Serial.println(e2);
    Serial.print("The weight of mixing chamber 1 is: ");
    Serial.println(e1);

    Serial.print("The weight of mixing chamber 2 is: ");
    Serial.println(e2);
  }

  d = ""; //reset d
  d1 = "";
  d2 = "";

  /*

    This would have been recreated for the reamining 4 weights, had they been working...

  */

  /////////////////// Write function I2C /////////////////
  if (mixingStatus == 1) {
    for (int t = 0; t <= 4; t++) {
      Wire.beginTransmission(8); // begin with device address 8 //
      Wire.write(static_cast<char*>(recipe[t]));  // sends string //
      Serial.println(recipe[t]);
      Wire.endTransmission();  // stop transmitting //
      mixingStatus = 0;
    }
  }

  ///////////////////////// MQTT LOOP FUNCTION /////////////////////

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  ////////////////// FLAME SENSOR AND OLED /////////////////////

  //Flame detection
  Flame = digitalRead(flamePin);

  //If Flame is ON, turn ledRED and write "Fire" on OLED
  if (Flame == HIGH)
  {
    flamesensor ();
  }
  digitalWrite(ledRED, LOW);
}
