#include <TinyGPS++.h>
#include <ESP8266Firebase.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>    

#define _SSID "modem-wifi"          
#define _PASSWORD "12345678"     
#define REFERENCE_URL "https://gps-tracker-6f312-default-rtdb.firebaseio.com/"

Firebase firebase(REFERENCE_URL);
SoftwareSerial neo6m(4, 5);
TinyGPSPlus gps;

void setup() {
  Serial.begin(115200);
  WiFiconnect();
  neo6m.begin(9600);
}

void loop() {
  smartdelay_gps(1000);

  if(gps.location.isValid()) 
  {
    String latitude = String(gps.location.lat(), 6);
    String longitude = String(gps.location.lng(), 6);

  firebase.setString("/GPS/Latitude", latitude);
  firebase.setString("/GPS/Longitude", longitude);

  firebase.json(true);              
  
  String data = firebase.getString("GPS"); 

  const size_t capacity = JSON_OBJECT_SIZE(3) + 50;
  DynamicJsonDocument doc(capacity);

  deserializeJson(doc, data);

  String float_latitude = doc["Latitude"]; 
  String float_longitude = doc["Longitude"];        

  Serial.print("Received String Latitude :\t");
  Serial.println(float_latitude);

  Serial.print("Received String Longitude:\t");
  Serial.println(float_longitude);

  firebase.deleteData("GPS");
  }
  else
  {
    Serial.println("No valid GPS data found.");
  }
  delay(5000);

}
static void smartdelay_gps(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (neo6m.available())
      gps.encode(neo6m.read());
  } while (millis() - start < ms);
}

void WiFiconnect(){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(_SSID);
  WiFi.begin(_SSID, _PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("-");
  }

  Serial.println("");
  Serial.println("WiFi Connected");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}
