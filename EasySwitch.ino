#include <WiFiUdp.h>
#include <ESP8266WiFiType.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <WiFiServer.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiMulti.h>



#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <EEPROM.h>


int port=80;

ESP8266WebServer server (port);

/* Set these to your desired credentials. */
/* that are the AP credentials  */
const char *ssid = "EasyWifi";
const char *password = "Easy123";

/* that are the client credentials  */
char client_ssid[100];
char  client_pasw[100];

int MAX_SSID=100;
int MAX_PASW=MAX_SSID;

int RELE_PIN=1;

int l_pasw;

void stringToEEPROM(String,int);
String stringFromEEPROM(int,int);

void handleRoot();
void handleControl();
void handleForm();
void handleNotFound();

int saveCredentials(String ssid,String password);
void readCredentials();





void setup() {
    //512kbyte of memory
  EEPROM.begin(512);
    
  pinMode(RELE_PIN,OUTPUT);
  
  Serial.begin ( 115200 );
  
  //while(!Serial.available());
  
  delay(500);
  
  Serial.println("\n\nHello from esp module.");
  
  //it doesn't won't work.
  //WiFi.softAP(ssid,password);
  
  WiFi.softAP(ssid);

  Serial.println("soft AP built. Credentials:");
  Serial.println(ssid);Serial.println(password);
  
  if ( MDNS.begin ( "esp8266" ) ) {
    Serial.println ( "MDNS responder started" );
  }else{ 
    Serial.println("something goes wrong with MDNS"); 
  }

  server.on("/",handleRoot);
  server.on("/form",handleForm);
  server.on("/control",handleControl);
  server.onNotFound(handleNotFound);
  server.begin();  

  Serial.println("server started.");

  readCredentials();

  int l_ssid=EEPROM.read(0);
  l_pasw=EEPROM.read(1);

  Serial.println("logging with :");
  Serial.print(client_ssid);Serial.print('\t');Serial.println(client_pasw);
  
  if( l_pasw<=0){
    WiFi.begin(client_ssid);
  }else{
    WiFi.begin(client_ssid,client_pasw);
  }

  float t = millis();
  while(WiFi.status()!= WL_CONNECTED && millis() < t+5000){ Serial.print('.'); delay(500);}
  if (WiFi.status()== WL_CONNECTED) Serial.println("\nconnected!!");
  else Serial.println("timeout wifi client connection.");
}

float last = millis();
int check_wifi= 10000;
void loop() {
  
  server.handleClient();
  delay(100);
  
  if( millis()>last+check_wifi){
    Serial.println("checking wifi routine.");
    if(WiFi.status()!= WL_CONNECTED) {
      Serial.println("logging with :");
      Serial.print(client_ssid);Serial.print('\t');Serial.println(client_pasw);
      if( l_pasw<=0){
        WiFi.begin(client_ssid);
      }else{
        WiFi.begin(client_ssid,client_pasw);
      }
    }
    int t=millis();
    while(WiFi.status()!= WL_CONNECTED && millis() < t+10000){ Serial.print('.'); delay(500);}
    if (WiFi.status()== WL_CONNECTED) Serial.println("\nconnected!!");
    else Serial.println("timeout wifi client connection.");
    last = millis();
  }

}




void stringToEEPROM(String src,int addr_start){
  
  int l = src.length();
  Serial.println("str "+src);
  Serial.println("str to EE len:");Serial.println(l);
  for(int i=0;i<l;i++){
    EEPROM.write(addr_start+i,src[i]);
  }
  EEPROM.commit();
  
}

String stringFromEEPROM(int addr_start,int len){
  char readed_str[len];
  readed_str[len]='\0';
  Serial.print("readed from EEE:");
  for(int i=0;i<len;i++){
    readed_str[i]=EEPROM.read(addr_start+i);
    Serial.print(readed_str[i]);
  }
  Serial.println();
  return String(readed_str);
  
}

void handleRoot(){
  Serial.println("handle root.");

  String message = "";

  message+="\
    <style>\
  body {\
    width: 300px;\
    margin: 0 auto;\
  }\
  </style>\
    <h2>Hello from Easy Service</h2>\
    <a href='/control'> Service </a>\
    <br>\
    <a href='/form'> Wifi Settings </a>\
  ";
  
  server.send(200,"text/html", message);
}

void handleControl(){
    String message="";

    if(server.method()== HTTP_POST){
      
        if(server.args()==2 && 
        server.argName(0)=="pin" &&
        server.argName(1)=="value"){
          
          Serial.println(server.arg(0));
          Serial.println(server.arg(1));
          String readed=server.arg(0);
          
          //im not sure that his is a good idea.
          int user_chosed_pin=readed.toInt();
          
          int chosed_pin=13;
          
          digitalWrite(chosed_pin,server.arg(1)=="on" ? HIGH : LOW );
          
        }
    }

    message+="\
   <style>\
  form {\
    width: 300px;\
    margin: 0 auto;\
  }\
  </style>\
    <form action='' method='post'>\
       <input type=hidden value=13 name='pin'>\
       <input type=hidden value='on' name='value'>\
       <input type='submit' value='ON'>\
       </form><form action='' method='post'>\
       <input type=hidden value=13 name='pin'>\
       <input type=hidden value='off' name='value'>\
       <input type='submit' value='OFF'>\
       </form>\
       <br><a href='/'>back</a>\
        ";
    server.send(200,"text/html",message);
    
}



void handleForm(){
  String message="";

  if(server.method()== HTTP_POST){
    
    /*
     * BEST WAY TO CHECK GET/POST requests.
     * 
    for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
    }
    */
    
    if(server.args()==2 && 
        server.argName(0)=="wifiname" &&
        server.argName(1)=="password"){
          
          //Serial.print("wifi name: ");Serial.println(server.arg(0));
          //Serial.println("wifi pass:");Serial.println(server.arg(1));

          //save ssid and password in order to connect afer reboot.
          if(saveCredentials(server.arg(0),server.arg(1))){
            readCredentials();
            message+="<h2>credential setted. please reboot the device. </h2><br>";
          }else 
            message+="<h2>error saving credentials. please retry.</h2>";
          
        }else{
          message+="<h2>input was not recognized.</h2><br>";
        }
  }
  
  message+="\
  <style>\
  form {\
    width: 300px;\
    margin: 0 auto;\
  }\
  </style>\
  <form action='' method='post'>\
  <p>wifi credentials</p>\
  <p>wifi name</p>\
  <input name='wifiname' type='text'><br>\
  <p>wifi password</p>\
  <input name='password' type='password'><br>\
  <input type='submit' value='submit'>\
  </form>\
  <br><a href='/'>back</a>\
  ";

  server.send(200,"text/html",message);
}

void handleNotFound() {
  Serial.println("handle not found.");
  
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 200, "text/html", message );
  
}


int saveCredentials(String ssid,String password){
  int l_ssid = ssid.length();
  int l_pasw = password.length();
  
  if (l_ssid>MAX_SSID) {Serial.println("max ssid len = 100");return 0;}
  if (l_pasw>MAX_PASW) {Serial.println("max pasw len = 100");return 0;}
  
  EEPROM.write(0,l_ssid);
  EEPROM.write(1,l_pasw);
  
  stringToEEPROM(ssid,2);
  stringToEEPROM(password,l_ssid+2);
  
  return 1;
}



void readCredentials(){
  int l_ssid=EEPROM.read(0);
  int l_pasw=EEPROM.read(1);

  String wssid_t;
  String wpasw_t;

  if (l_ssid>MAX_SSID || l_ssid==0 || l_pasw>MAX_PASW){
   Serial.println("exceeded ssid or pasw max size");
   //client_ssid=client_pasw="";
   client_ssid[0]='\0';
   return;
  }

  wssid_t=stringFromEEPROM(2,l_ssid);
  
  wssid_t.toCharArray(client_ssid,l_ssid+1);

  if (l_pasw==0) { return;}
  
  wpasw_t =stringFromEEPROM(l_ssid+2,l_pasw);
  wpasw_t.toCharArray(client_pasw,l_pasw+1);

  Serial.println("readCredentials Debug");
  Serial.print(wssid_t);Serial.print(" ");Serial.println(client_ssid);
  Serial.print(wpasw_t);Serial.print(" ");Serial.println(client_pasw);
  
  return;
}
