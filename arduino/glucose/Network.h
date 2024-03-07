#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <Preferences.h>
#include <FirebaseESP32.h>  //Firebase Library
#include <NTPClient.h>
#include <WiFiUdp.h>

#define DATABASE_URL "glucose-b9d1c-default-rtdb.firebaseio.com" //url firebase
#define FIREBASE_AUTH "SzkT6Dphy7Sy1MWZYCNFCNLW6tQ5fBkO44gilMMC"            //credential firebase
FirebaseData fb;
FirebaseAuth auth;
FirebaseConfig config;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200);

Preferences pf;

const char* ssid_ap     = "ESP32-Connection";

WiFiServer server(80);

String header;

void reconnect();

void wifi_CONNECT(){
  
  pf.begin("credentials", false);
  ssid = pf.getString("ssid", DEFAULT_SSID);
  pass = pf.getString("pass", DEFAULT_PASS);
  uid = pf.getString("uid", DEFAULT_UID);
  pf.end();
  
  if(ssid=="" || pass=="" || uid==""){
    Serial.print("[" + String(millis())+"] ");
    Serial.println("WiFi Credentials not found in memory!");
    delay(1000);
    reconnect();
  } else {
    Serial.print("[" + String(millis())+"] ");
    Serial.print("Connecting to : ");
    Serial.println(ssid);
    // Serial.print("Password : ");
    // Serial.println(pass);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(),pass.c_str());
    int connect_failed = 0;
    while(WiFi.status() != WL_CONNECTED){
      Serial.print(".");
      connect_failed++;
      if(connect_failed > 20) {
        Serial.print("[" + String(millis())+"] ");
        Serial.println("Failed to connect!\nCheck your WiFi credentials...");
        reconnect();
      }
      delay(500);
    }

    digitalWrite(23, HIGH);
    delay(100);
    digitalWrite(23, LOW);
    delay(100);

    Serial.println();
    Serial.print("[" + String(millis())+"] ");
    Serial.println(WiFi.localIP());

    digitalWrite(23, HIGH);
    delay(100);
    digitalWrite(23, LOW);
    delay(100);
    
    timeClient.begin();
    config.database_url = DATABASE_URL;
    config.signer.test_mode = true;
    Firebase.reconnectWiFi(true);
    Firebase.begin(&config, &auth);

  }
}

void reconnect(){
  connection_status = false;
  WiFi.mode(WIFI_OFF);
  delay(500);

  tft.fillScreen(TFT_BLACK);
  tft.drawString("Access App", 80, 25, 4);
  tft.drawString("192.168.4.1", 80, 55, 4);
  Serial.print("[" + String(millis())+"] ");
  Serial.print("Opening server ");
  Serial.println(ssid_ap);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid_ap);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("[" + String(millis())+"] ");
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  while(connection_status == false){
    WiFiClient client = server.available();
    String s[3];
    
    if (client) {
      Serial.print("[" + String(millis())+"] ");
      Serial.println("New Client.");
      String currentLine = "";
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          header += c;
          if (c == '\n') {
            if (currentLine.length() == 0) {
              client.println("HTTP/1.1 200 OK");
              
              if(header.indexOf("GET /wifi") >= 0){
                String b = header.substring(header.indexOf("?")+1, header.indexOf("+"));
                Serial.print("[" + String(millis())+"] ");
                Serial.print("ssid = ");
                Serial.println(b);
                s[0]=b;
                b = header.substring(header.indexOf("+")+1, header.indexOf(" HTTP/1.1"));
                s[1]=b;
                Serial.print("[" + String(millis())+"] ");
                Serial.print("pass = ");
                Serial.println(b);

                pf.begin("credentials", false);
                pf.putString("ssid", s[0]);
                pf.putString("pass", s[1]);
                pf.end();
      
                digitalWrite(23, HIGH);
                delay(100);
                digitalWrite(23, LOW);
                delay(100);

                client.stop();
                Serial.print("[" + String(millis())+"] ");
                Serial.println("Client disconnected.");
                Serial.println("");

                digitalWrite(23, HIGH);
                delay(100);
                digitalWrite(23, LOW);
                delay(100);
                          
              } else if(header.indexOf("GET /uid") >= 0){
                String b = header.substring(header.indexOf("?")+1, header.indexOf(" HTTP/1.1"));
                s[2]=b;
                Serial.print("[" + String(millis())+"] ");
                Serial.print("uid = ");
                Serial.println(b);

                pf.begin("credentials", false);
                pf.putString("uid", s[2]);
                pf.end();

                digitalWrite(23, HIGH);
                delay(100);
                digitalWrite(23, LOW);
                delay(100);

                client.stop();
                Serial.print("[" + String(millis())+"] ");
                Serial.println("Client disconnected.");
                Serial.println("");

                digitalWrite(23, HIGH);
                delay(100);
                digitalWrite(23, LOW);
                delay(100);
                              
              } else if(header.indexOf("GET /finish") >= 0){
                Serial.print("[" + String(millis())+"] ");
                Serial.println("Setting complete, returning to stand by mode");
                
                digitalWrite(23, HIGH);
                delay(100);
                digitalWrite(23, LOW);
                delay(100);

                client.stop();
                Serial.print("[" + String(millis())+"] ");
                Serial.println("Client disconnected.");
                Serial.println("");
                
                digitalWrite(23, HIGH);
                delay(100);
                digitalWrite(23, LOW);
                delay(100);

                server.stop();
                
                WiFi.mode(WIFI_OFF);
                
                wifi_CONNECT();
                
                state=2;    
                connection_status = true;
              }
              
              // The HTTP response ends with another blank line
              client.println();
              // Break out of the while loop
              break;
            } else { // if you got a newline, then clear currentLine
              currentLine = "";
            }
          } else if (c != '\r') {  // if you got anything else but a carriage return character,
            currentLine += c;      // add it to the end of the currentLine
          }
        }
      }
      header = "";
      client.stop();
    }
  }
}
