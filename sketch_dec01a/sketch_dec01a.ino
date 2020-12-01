
/*
 * 
 * 
 *
*/


#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>

//1. Change the following info
#define FIREBASE_HOST "isensiot.firebaseio.com"
#define FIREBASE_AUTH "mj33QNFCJaR92P8ywIltvjVy00jifRhytxR4J8Fe"
#define WIFI_SSID "****"
#define WIFI_PASSWORD "****"
#define ID_HASH "/21232f297a57a5a743894a0e4a801fc3e033ba9cd693f9e833c72e9055d2c491"



//2. Define FirebaseESP8266 data object for data sending and receiving
FirebaseData firebaseData;


void setup()
{

  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();


  //3. Set your Firebase info

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //4. Enable auto reconnect the WiFi when connection lost
  Firebase.reconnectWiFi(true);

}


void sendJson(FirebaseJson json, String doc){

//doc path esample like: doc="/test" 

if (Firebase.pushJSON(firebaseData, doc, json)) {

  Serial.println(firebaseData.dataPath());

  Serial.println(firebaseData.pushName());

  Serial.println(firebaseData.dataPath() + "/"+ firebaseData.pushName());

} else {
  Serial.println(firebaseData.errorReason());
}

}


void loop()
{
  //initialize json object 
  FirebaseJson json;

  //initialize sensor data objects
  String time = "0";
  String ultrasoonSensor = "0";
  String lightSensor = "0";
  String moistureSensor = "0";
  
  json.set("time", time);
  json.set("ultrasoonSensor", ultrasoonSensor);
  json.set("lightSensor", lightSensor);
  json.set("moistureSensor", moistureSensor);

  sendJson(json,ID_HASH);

  //60 * 60 * 1000
  int hour = 3600000;

  delay(hour);

}
