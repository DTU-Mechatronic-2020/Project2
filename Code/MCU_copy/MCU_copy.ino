
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

bool mixingStatus = false;

char c;
int e;
String d;
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
/*
  char* WhatIsTheWeight;
*/
//int recipe[5];
int distance;

//OLED
//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED
//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ D1, /* data=*/ D2);   // pin remapping with ESP8266 HW I2C
//U8G2_SSD1306_128X32_WINSTAR_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C
//FLAME SENSOR
const int flamePin = D5;
int Flame = HIGH;

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

  /*String payloada;
    payloada = String(byteArrayPayload);
    Serial.print("Message payload is: ");
    Serial.print(payloada);
  */
  // Konverterer den indkomne besked (payload) fra en array til en string:
  // Topic == Kraftsensor
  payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload


  /////////////////////////////// INGREDIENTS SETUP ///////////////////////////////


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

    oilrcp = payload.toInt() + 5000; //Cryptation for I2C bus
    oilrcpS = String(oilrcp);
    oilrcpS.toCharArray(pumpe5, 5);


    mixingStatus = true;

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
  }




  /*
    char ethanolrcpchar[3] = {ethanolrcp};
    char waterrcpchar[3] = {waterrcp};
    char glycerinrcpchar[3] = {glycerinrcp};
    char brintoveriltercpchar[3] = {brintoveriltercp};
    char oilrcpchar[3] = {oilrcp};



    int recipe[5] = {ethanolrcpchar[3], waterrcpchar[3], glycerinrcpchar[3], brintoveriltercpchar[3], oilrcpchar[3]};
  */
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
  for (int t = 0; t < 5; t++) {
    Serial.println(recipe[t]);
  }

  /////////////////// Read function //////////////////
  Wire.requestFrom(8, 5); // request & read data of size 4 from slave //
  while (Wire.available()) {
    char c = Wire.read(); //Read charachters to c
    Serial.println(c);

    //// Read weightsensors ////
    d += c; //Collect characters to string
    int e = d.toInt(); //Convert received string to integer
    Serial.println("The weight of mixing chamber is: ");
    Serial.println(e);

    Serial.println();

    //Her skal den indlæse signalet, som Arduino-koden sender ved dispensering


    if (c == 1 ) {
      digitalWrite(ledGREEN, HIGH);
      u8g2.clearBuffer();          // clear the internal memory
      u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      u8g2.drawStr(0, 10, "Dispensing"); // write something to the internal memory
      u8g2.sendBuffer();          // transfer internal memory to the display
      delay(3000);
      digitalWrite(ledGREEN, LOW);
    }


  }
  Serial.println(d);
  d = ""; //reset d


  ///////////////// Interpretation of I2C weight inputs /////////////////

  //Weight 1
  if (2000 >= e && e > 1000) {
    e = e - 1000;
    int mixingMass = e;
    client.publish("Weight 1", String(e).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  //Weight 2
  else if (3000 >= e && e > 2000) {
    e = e - 2000;
    int ethanolMass = e;
    client.publish("Weight 2", String(e).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  //Weight 3
  else if (4000 >= e && e > 3000) {
    e = (e - 3000);
    int waterMass = e;
    client.publish("Weight 3", String(e).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  //Weight 4
  else if (5000 >= e && e > 4000) {
    e = e - 4000;
    int glycerinMass = e;
    client.publish("Weight 4", String(e).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  //Weight 5
  else if (5000 >= e && e > 4000) {
    e = e - 4000;
    int H2O2Mass = e;
    client.publish("Weight 5", String(e).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  //Weight 6
  else if (6000 >= e && e > 5000) {
    e = e - 5000;
    int oilMass = e;
    client.publish("Weight 6", String(e).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  int e = 0;



  for (int t = 0; t <= 4; t++) {
    Wire.beginTransmission(8); // begin with device address 8 //
    Wire.write(static_cast<char*>(recipe[t]));  // sends string //
    Serial.println(recipe[t]);
    Wire.endTransmission();  // stop transmitting //
  }

  /////////////////// Write function /////////////////
  if (mixingStatus == true) {
    for (int t = 0; t <= 4; t++) {
      Wire.beginTransmission(8); // begin with device address 8 //
      Wire.write(static_cast<char*>(recipe[t]));  // sends string //
      Serial.println(recipe[t]);
      Wire.endTransmission();  // stop transmitting //
      mixingStatus = false;
    }
  }


  ///////////////////////// MQTT LOOP FUNCTION /////////////////////

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  /*                FLAME SENSOR AND OLED                 */
  /*
    //Flame detection
    Flame = digitalRead(flamePin);

    //If Flame is ON, turn ledRED and write "Fire" on OLED
    if (Flame == HIGH)
    {
      digitalWrite(ledRED, HIGH);
      digitalWrite(ledGREEN, LOW);

      u8g2.clearBuffer();          // clear the internal memory
      u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      u8g2.drawStr(0, 10, "FIRE guys, run!!"); // write something to the internal memory
      u8g2.sendBuffer();          // transfer internal memory to the display
      delay(10);
    }

    //When dispensing
    else if (distance < 15)
    {
      digitalWrite(ledRED, LOW);
      digitalWrite(ledGREEN, HIGH);

      u8g2.clearBuffer();          // clear the internal memory
      u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      u8g2.drawStr(0, 10, "Dispensing"); // write something to the internal memory
      u8g2.sendBuffer();          // transfer internal memory to the display
      delay(10);
    }

    // If there is no fire and no dispensing, write nothing on the OLED and turn OFF the LED's
    else
    {
      digitalWrite(ledRED, LOW);
      digitalWrite(ledGREEN, LOW);
      u8g2.clearBuffer();          // clear the internal memory
      u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      u8g2.drawStr(0, 10, ""); // write something to the internal memory
      u8g2.sendBuffer();          // transfer internal memory to the display
      delay(10);

    }
  */
}
