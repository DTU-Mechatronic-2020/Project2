#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1

const char* ssid = "WIFI NAME"; // HINT: create a hotspot with your phone..
const char* password = "PASSWORD";

void setup() {
  Serial.begin(115200);

  //connect to the wifi access point
  WiFi.begin(ssid, password);

  //wait until connected - might take a while

  Serial.print("Connecting  ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected ! \n");
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    HTTPClient http; //Declare an object of class HTTPClient

    // REMEMBER to replace YOURKEY from the url with your own key
    http.begin("http://api.openweathermap.org/data/2.5/forecast?q=Copenhagen,dk&appid=YOURKEY&units=metric&cnt=3"); //Specify request destination
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
        return;
      }

      //if it is successful, let's grab a data fragment and show it on the serial terminal:

      // the first forecast in the json object is at jsonBuffer['list'][0]
      String timestamp = jsonBuffer["list"][0]["dt_txt"];
      Serial.print("The time for this forecast: ");
      Serial.println(timestamp);

      // The 'weather' in the first position in the list, is actually a list, with only one element...
      String desc = jsonBuffer["list"][0]["weather"][0]["description"];
      Serial.print("The weather will be: ");
      Serial.println(desc);

      // The ArduinoJson library also has a utility function to 'pretty print' json objects, try:
      serializeJsonPretty(jsonBuffer["list"][0], Serial);
      //or
      Serial.println("\nThe full forecast Json looks like this: ");
      serializeJsonPretty(jsonBuffer, Serial);
    }
    http.end(); //Close connection
  }
  //Send a request every 5 min
  delay(5 * 60 * 1000);
}
