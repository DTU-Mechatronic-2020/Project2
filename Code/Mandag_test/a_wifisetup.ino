void wifiSetup() { // Setup for the wifi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}

void wifiLoop() { // loop which makes sure the NodeMCu is connected
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
