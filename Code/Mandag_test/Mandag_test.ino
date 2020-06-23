// Libraries used
#include <SPI.h>
#include <Wire.h>
#include <Servo.h>
#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, D2, D1, U8X8_PIN_NONE); // Used for OLED Screen
Servo servo; // controls the motors


// Global variabels
const int trigPin = D6; const int echoPin = D7; const int relayPin = D3; const int floatswitch = 14; // D5
const int neoPin = 2; // LED strip D4
const int button = 16; // D0

// Strings
String payload; String allowedP; String currentP; String stringOne; String stringTwo;
// Integers
int sanTime = 2250; //Time which the pump shoukd run, start value set to goverment recommendation
int gateTime = 5000; //Used by the LED strip
int currP; int PinS; int countIn; int currW; int sanOut; // int used to communicate with MQTT
// variables used throughout the code
int i = 0; int test = 0; long duration; int distance; int chipOut; int buttonState = 0; int waterState; int PeopleInside; int PeopleAllowed; int y = 1;


Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, neoPin, NEO_GRB + NEO_KHZ800); // Used for the LED strip
uint32_t highLED = strip.Color(255, 255, 255); // Sets the color for the waiting light
uint32_t redLED = strip.Color(255, 0, 0); // Sets the color for the waiting light

// WiFi connection details
const char* ssid = "1312";
const char* password = "1007A-s1007";

// MQTT connection details
const char *mqtt_server = "hairdresser.cloudmqtt.com";
const int mqtt_port = 17358;
const char *mqtt_user = "Node1";
const char *mqtt_pass = "test";

// Callback function intialized
void callback(char* byteArraytopic, byte* byteArrayPayload, unsigned int length);

// Loading WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(mqtt_server, mqtt_port, callback, espClient);











void callback(char* byteArraytopic, byte* byteArrayPayload, unsigned int length) {

  // Defines topic string
  String topic;
  topic = String(byteArraytopic);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");

  // Converts the payload of the given topic into a string
  if (topic == "PinS") {
    payload = ""; // Resetting payload variable
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }
    allowedP = payload;
    Serial.print(allowedP);
  }


  if (topic == "currP") {
    payload = ""; // Resetting payload variable
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }
    currentP = payload;
    Serial.print(currentP);
  }

  if (topic == "sanTime") {
    payload = ""; // Resetting payload variable
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }
    sanTime = payload.toInt();

    Serial.print(sanTime);
  }
  if (topic == "chipOut") {
    payload = ""; // Resetting payload variable
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }
    chipOut = payload.toInt();
    Serial.print(chipOut);
  }
  Serial.println("");
}

// WiFi setup function
void setup_wifi() {
  wifiSetup();
}

// MQTT re-connection function in case of missing WiFi
void reconnect() {
  reconnectCode();
}

void setup() {
  Serial.begin(9600);
  setup_wifi(); // WiFi connection loop. Needed to maintain connection.
  setupCode(); // contains code that should be run in setup
}

void loop() {
  wifiLoop(); // Keeps the NodeMCU connected
  PeopleAllowed = allowedP.toInt(); // Convert message from MQTT to a Int
  PeopleInside = currentP.toInt(); // Convert message from MQTT to a Int
  waterLevel(); //Checks level of sanitizer

  if (PeopleInside >= PeopleAllowed) {
    stopAnim(); 
    closeOLED();
    lidMotor();
  }

  else if (PeopleInside <= PeopleAllowed) {
    welcomeAnim();
    distanceSensor();
    openOLED();
    lidMotor();
    
    if (distance < 10) {
      pumpAndGate(); // Runs the code for our pump and the gate, if the sensor triggers
    }
  }
}
