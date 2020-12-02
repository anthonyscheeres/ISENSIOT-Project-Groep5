
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>

// The static info used to connect with firebase
#define FIREBASE_HOST "isensiot.firebaseio.com"
#define FIREBASE_AUTH "mj33QNFCJaR92P8ywIltvjVy00jifRhytxR4J8Fe"
#define ID_HASH "/21232f297a57a5a743894a0e4a801fc3e033ba9cd693f9e833c72e9055d2c491"

// Change this to your own wifi credentials and make sure to keep them private
#define WIFI_SSID "*****"
#define WIFI_PASSWORD "****"



// Define FirebaseESP8266 data object for data sending and receiving
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


  // Set your Firebase info

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  // Enable auto reconnect the WiFi when connection lost
  Firebase.reconnectWiFi(true);

}


void sendJson(FirebaseJson json, String doc){

  // Doc path esample like: doc="/test" 

if (Firebase.pushJSON(firebaseData, doc, json)) {

  Serial.println(firebaseData.dataPath() + "/"+ firebaseData.pushName());

} else {
  Serial.println(firebaseData.errorReason());
}

}


void loop()
{
  // Initialize json object 
  FirebaseJson json;

  // Initialize sensor data objects
  String time = "0";
  String ultrasoonSensor = "0";
  String lightSensor = "0";
  String moistureSensor = "0";

// You need to get the sensor data here 



 // The data is copied to the json opject from here  
  json.set("time", time);
  json.set("ultrasoonSensor", ultrasoonSensor);
  json.set("lightSensor", lightSensor);
  json.set("moistureSensor", moistureSensor);
  
  // The is hash is used to identify devices
  sendJson(json,ID_HASH);

  //60 * 60 * 1000 ms
  int hour = 3600000;

  delay(hour);

}
