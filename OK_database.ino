#include <ESP8266WiFi.h>                                                   
#include <FirebaseArduino.h>                                                
#include <DHT.h>                                                          
#include <String.h>
String data, dataOK1, dataOK2, dataOK3, dataOK4, dataOK5, dataOK6;
char equal[6];
unsigned long int timer;
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define FIREBASE_HOST "st-utetracking-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "F0ZEfGuhxAFQNOJlCc5j38UTCgaM6PKIAdPik0w9"
#define WIFI_SSID "**********"                                       //name of wifi
#define WIFI_PASSWORD "**********"                                   //password of wifi ssid
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void setup() {
  Serial.begin(9600);
  delay(1000);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                  
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                           
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                                                                                     //Start reading dht sensor
}
//..............................................................
void receive_uno_data() {
  String str = ""; char c;
  do {
    while (Serial.available() > 0) {
      c = Serial.read();
      str += c;
    }
    if (str[0] != 'L') str = "";
  } while ((c != '\n') || (str.length() <= 35));
  data = str;
  Serial.println(data);
}
//..............................................................
void solve_data() {
  int dem = 1;
  for (int i = 4; i < data.length(); i++) {
    if (data[i] == '=') {
      equal[dem] = i;
      //Serial.println(equal[dem]);
      dem++;
    }
  }
  dataOK1 = ""; dataOK2 = ""; dataOK3 = ""; dataOK4 = ""; dataOK5 = ""; dataOK6 = "";
  for (int k = 4; k < (equal[1] - 3); k++) {
    dataOK1 += data[k];
  }
  for (int k = (equal[1] + 1); k < (equal[2] - 3); k++) {
    dataOK2 += data[k];
  }
  for (int k = (equal[2] + 1); k < (equal[3] - 3); k++) {
    dataOK3 += data[k];
  }
  for (int k = (equal[3] + 1); k < (equal[4] - 3); k++) {
    dataOK4 += data[k];
  }
  for (int k = (equal[4] + 1); k < (equal[5] - 3); k++) {
    dataOK5 += data[k];
  }
  for (int k = (equal[5] + 1); k < (equal[6]); k++) {
    dataOK6 += data[k];
  }
  if (dataOK1 != "") {
    Serial.println(dataOK1);
    Serial.println(dataOK2);
    Serial.println(dataOK3);
    Serial.println(dataOK4);
    Serial.println(dataOK5);
    Serial.println(dataOK6);
    Firebase.setString("Location/Latitude", dataOK1);
    Firebase.setString("Location/Longitude", dataOK2);
    Firebase.setString("Location/Speed", dataOK3);
    Firebase.setString("Status", dataOK4);
    Firebase.setString("Location/Date", dataOK5);
    Firebase.setString("Location/Time", dataOK6);
    while ((millis() - timer) > 120000) {
      Firebase.setString("Storage/Lati", dataOK1);
      Firebase.setString("Storage/Longi", dataOK2);
      timer = millis();
    }
  }
}
//..............................................................
void loop() {
  receive_uno_data();
  solve_data();
}
