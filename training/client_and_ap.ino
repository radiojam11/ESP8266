#include <ESP8266WiFi.h>


//this is an example about the coexistence of CLIENT/ AP
//of the ESP8266 module.
//

/*
 * this is an example about the coexistence of CLIENT/ AP
 * of the ESP8266 module.
 * 
 * the loop part is a simple example about connecting to a socket
 * but we don't really care about it now.
 */



void setup() {
  
  Serial.begin(115200);

  //Connect to Wifi test without passwords
  //as a client
  WiFi.begin("test");

  //wait while not connected
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("connected!");

  //after connected start the AP
  WiFi.softAP("node-esp");
  
}

void loop() {
  WiFiClient client;
  while(!client.connect("192.168.43.194",9999)){
    Serial.println("connection failed.");
    delay(1000);
  }
  Serial.println("connected to host!");
  while(client.available() ==0);
  Serial.println(client.readStringUntil('\n'));
  Serial.println("goin to sleep.");
  while(1);
}
