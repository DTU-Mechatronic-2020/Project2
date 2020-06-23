
// CODE FOR THE OLED SCREEN
void closeOLED() {
  Wire.beginTransmission(0x3C); /* begin with device address 8 */
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB10_te); // choose a suitable font
  u8g2.drawStr(0, 15, "Please Wait!"); // write something to the internal memory
  u8g2.drawStr(0, 30, "To Many"); //
  u8g2.drawStr(0, 45, "People Inside.");
  u8g2.drawStr(0, 60, "Allowed:");
  u8g2.setCursor(70, 60);
  u8g2.print(PeopleAllowed);
  u8g2.sendBuffer();          // transfer internal memory to the display
  Wire.endTransmission();    /* stop transmitting */
}
void openOLED() {
  stringOne = "/";
  stringTwo = currentP + stringOne + PeopleAllowed;
  Wire.beginTransmission(0x3C); /* begin with device address 8 */
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB10_te); // choose a suitable font
  u8g2.drawStr(0, 15, "Please sanitize,"); // write something to the internal memory
  u8g2.drawStr(0, 30, "before entering"); //
  u8g2.drawStr(0, 45, "Now in store:");
  u8g2.setCursor(0, 60);
  u8g2.print(stringTwo);
  u8g2.sendBuffer();          // transfer internal memory to the display
  Wire.endTransmission();    /* stop transmitting */
}

void enterOLED() {
  Wire.beginTransmission(0x3C); /* begin with device address 8 */
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB10_te); // choose a suitable font
  u8g2.drawStr(0, 15, "Please Enter!"); // write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display
  Wire.endTransmission();    /* stop transmitting */
}

// LED STRIP CODE
// Loading/wait animation
void loadingAnim(uint32_t color, int wait) {
  strip.clear();
  unsigned long neoRefTime = millis();
  int p = 0;
  while (p <= 8) { // truns on the 8 LED one at a time
    if (millis() >= (neoRefTime + wait / 8)) {
      neoRefTime = millis();
      strip.setPixelColor(p, color);
      strip.show();
      p++;
    }
    yield();
  }
}

// Countdown animation
void countdownAnim(uint32_t color, int wait) {
  strip.clear();
  for (int p = 0; p < strip.numPixels(); p++) {
    strip.setPixelColor(p, color);
    strip.show();
    yield();
  }
  unsigned long neoRefTime = millis();
  int p = 7;
  while (p >= 0) {
    if (millis() >= (neoRefTime + wait / 8)) {
      neoRefTime = millis();
      strip.setPixelColor(p, strip.Color(0, 0, 0));
      strip.show();
      p--;
    }
    yield();
  }
  strip.clear();
}

void welcomeAnim() { //White light when people can enter
  strip.clear();
  for (int i = 0; i < 8; i++) {
    strip.setPixelColor(i, highLED);
    strip.show();
  }
}

void stopAnim() { // RED lights when store is filled 
  strip.clear();
  for (int i = 0; i < 8; i++) {
    strip.setPixelColor(i, redLED);
    strip.show();
  }
}
