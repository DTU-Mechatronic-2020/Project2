void reconnectCode() {
  // Will retry to a connection is established
  while (!client.connected()) {
    Serial.print("Attempting to establish connection to MQTT server...");

    if (client.connect("Node1", mqtt_user, mqtt_pass)) { // Connects to MQTT client
      Serial.println("connected");
      ;
      // Subscribed topics
      client.subscribe("currP");
      client.subscribe("PinS");
      client.subscribe("chipOut");
      client.subscribe("sanTime");
      // In case of errors, re-run the loop at 5 second intervals
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
    // Ping MQTT server to get current values in case of a restart (People allowed in the store, current number of people, amount of sanitizer)
    client.publish("alt", "1");
  }
}
