#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

#define full 4
#define threefourth 9
#define empty 24


#define WIFI_SSID "Omkar"
#define WIFI_PASSWORD "reddy519"

#define ownertoken "5982109878:AAGzuL31DtSVJLKIruPOKQLfVKo6GUzsLWo"
#define ownerid "1694002474"
#define municipalid "5137812486"

int trig=D6;
int echo=D5;

String ownermsgFull="Alert from Septic Tank, your tank is going to be full. Clean it before it overflows";
String muncipalmsgFull="Alert from Septic Tank located at Lt: 15.8283837, Lg: 77.588802, it is going to be full";
String ownermsg3="Alert from Septic Tank, your tank is going to be 3/4th full. Clean it before it overflows";

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(ownertoken, secured_client);

String serverName = "https://makeskilled.com/projects/mockingbirds/driver/septictank.php";


void setup() {
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);
  Serial.begin(9600);
  secured_client.setTrustAnchors(&cert);
  configTime(0,0,"pool.ntp.org");
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  digitalWrite(trig,0);
  delayMicroseconds(2);

  digitalWrite(trig,1);
  delayMicroseconds(10);
  digitalWrite(trig,0);

  float duration=pulseIn(echo,1);
  float distance=(duration/2)*0.034;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(1000);
  if(distance<threefourth && distance>full) {
    Serial.println("3/4th Detected");
    bot.sendMessage(ownerid,ownermsg3, "Markdown");
  } else if(distance<full) {
    Serial.println("Full Detected");
    bot.sendMessage(ownerid,ownermsgFull, "Markdown");
    bot.sendMessage(municipalid,muncipalmsgFull,"Markdown");
  }
   while(1) {
      int stat=upload_sensory_feed("15.8283837","77.588802","Omkar","7396060373",String(distance));
      if(stat==1) {
        break;
      }
      delay(100);
   }
}

int upload_sensory_feed(String a, String g, String s, String d, String st) {
  
  if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      WiFiClientSecure client;
      client.setInsecure(); // this is the magical line that makes everything work


      String serverPath = serverName +"?latitude=" + String(a) + "&longitude=" + String(g) + "&owner=" + String(s) + "&contact=" + String(d) + "&status="+String(st)+"&slot=" + String(1);
            
      http.begin(client,serverPath.c_str());
       
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        return(1);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
        return(0);        
      }
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
      return(0);
    }
}
