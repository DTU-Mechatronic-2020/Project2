// Defining pins, variables and more. 
const char* ssid = "Powerpuff Pigerne"; // HINT: create a hotspot with your phone..
const char* password = "BlomstogBobbel";

// Definerer information til mqtt serveren
const char *mqtt_server  = "hairdresser.cloudmqtt.com"; //navn på mqtt-server. Find navnet på cloudmqtt-hjemmesiden
const int mqtt_port  = 15743 ; // Definerer porten
const char *mqtt_user  = "Freddy"; // Definerer mqtt-brugeren
const char *mqtt_pass  = "Freddy" ; // Definerer koden til mqtt-brugeren

// For setting time and date:
const long utcOffsetInSeconds = 7200; //For CEST, UTC +2 +2.00: 1*60*60
char daysOfTheWeek[7][12] = {"Sunday" , "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}; //Week days

// Generel variables
String skabnr;
String bookingstatus;
String rentalperiod;
String lockstatus;
String payload; // Definerer variablen 'payload' i det globale scope (payload er navnet på besked-variablen)
String starttime;
String endtime;
int starthour;
int startminute;
String Futurebooking;
String stahour;
String staminute;
int buttonPin = D7;
int buttonState = 0;         // variable for reading the pushbutton status
int GreenLedPin = D4;
int BlueLedPin = D5;
int RedLedPin = D6;
int YellowLedPin = D8;
int remaining_hours;
int remaining_minutes;
String endmin;
String endhour;
int endhours;
int endminutes;
int emergency = 0;

//Milis variables:
int oneminute = 30000;
unsigned long time_now = 0;



WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
