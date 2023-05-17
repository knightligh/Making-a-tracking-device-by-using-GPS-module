#include <TinyGPS++.h>           
#include <SoftwareSerial.h>       
TinyGPSPlus gps;
#define S_RX    4            
#define S_TX    3            
SoftwareSerial SoftSerial(S_RX, S_TX);   
#include <Wire.h>
#include <Sim800l.h>            
SoftwareSerial sim(10, 11);
int _timeout, sendd = 0;
String _buffer;
Sim800l Sim800l;
String textSms, text;
String number = "+84*********";      //Các bạn thay số điện thoại để module gửi SMS về
uint8_t index1;
String LAT, LON, SPD, STT = "STT=0", DAT, TIM;
int demSms = 0, call = 0;
//..............................................................
void setup(void) {
  Serial.begin(9600);
  _buffer.reserve(50);
  Serial.println("--------");
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(7, INPUT);          //vibration sensor
  pinMode(12, OUTPUT);        //buzzer
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(12, LOW);
  sim.begin(9600);
  Sim800l.begin();
  Sim800l.delAllSms();
  Serial.println("<<Sim Connected>>");
  digitalWrite(A1, HIGH);
}
//..............................................................
void readgps() {
step1:
  SoftSerial.begin(9600);
  while (SoftSerial.available() > 0) {
    if (gps.encode(SoftSerial.read())) {
      if (gps.location.isValid()) {
        if (gps.location.isValid()) {
          LAT = String("LAT=");
          LAT += String(gps.location.lat(), 6);
          LON = String("LON=");
          LON += String(gps.location.lng(), 6);
        }
        if (gps.speed.isValid()) {
          SPD = String("SPD=");
          SPD += String(gps.speed.kmph());
        }
        if (gps.date.isValid()) {
          DAT = String("DAT=");
          if (gps.date.day() < 10)      DAT += String("0");
          DAT += String(gps.date.day());
          DAT += String("/");
          if (gps.date.month() < 10)    DAT += String("0");
          DAT += String(gps.date.month());
          DAT += String("/");
          DAT += String(gps.date.year());
        }
        if (gps.time.isValid()) {
          int hourr = gps.time.hour() + 7;
          if (hourr > 24) hourr = hourr - 24;
          TIM = String("TIM=");
          if (hourr < 10)               TIM += String("0");
          TIM += String(hourr);
          TIM += String(":");
          if (gps.time.minute() < 10)   TIM += String("0");
          TIM += String(gps.time.minute());
          TIM += String(":");
          if (gps.time.second() < 10)   TIM += String("0");
          TIM += String(gps.time.second());
          goto endd;
        }
      }
      else {
        LAT = ""; LON = "", SPD = "", DAT = "", TIM = "";
        goto endd;
      }
    }
  }
endd:
  if (LAT == "") {
    Serial.println("invalid");
    goto step1;
  }
  else {
    digitalWrite(A0, HIGH);
    text = LAT;
    text += LON;
    text += SPD;
    text += STT;
    text += DAT;
    text += TIM;
    text += String("=");
    Serial.println(text);
  }
  SoftSerial.end();
}
//..............................................................
void ReadSMS()
{
  _buffer.reserve(50);
  sim.begin(9600);
  Sim800l.begin();
  textSms = "";
  if (LAT != "") {
    text = LAT;
    text += LON;
    text += SPD;
    text += STT;
    text += DAT;
    text += TIM;
  } else text = "Invalid";
  textSms = Sim800l.readSms(1);
  if ((textSms.length() <= 100) || (call == 1))
  {
    sendd = 0;
    String SMS;
    if (textSms.indexOf("LOCATION") != -1) {
      SMS = String(text); sendd = 1;
      Serial.println("**kiem tra vi tri**");
    } else if (textSms.indexOf("TURN_ON") != -1) {
      SMS = String("ON LOCATION"); sendd = 1; STT = "STT=1"; Serial.println("**bat vi tri**");
    } else if (textSms.indexOf("TURN_OFF") != -1) {
      SMS = String("OFF LOCATION"); sendd = 1; STT = "STT=0"; Serial.println("**tat vi tri**");
    } else if (textSms.indexOf("OFF_BUZ") != -1) {
      SMS = String("BUZZER OFF"); sendd = 1;  Serial.println("**tat bao dong**"); digitalWrite(12, LOW);
    } else if (call == 1) {
      SMS = String("WARNING"); sendd = 1; Serial.println("**canh bao**"); call = 0;
      //    Serial.println("**make a phone call**");
      //    sim.println("AT");
      //    updateSerial();
      //    sim.println("ATD+ +84*********;");
      //    updateSerial();
      //    Serial.println("calling");
      //    delay(20000); // wait for 20 seconds...
      //    sim.println("ATH");
      //    updateSerial();
    }
    if (sendd == 1) {
      sim.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
      delay(1000);
      sim.println("AT+CMGS=\"" + number + "\"\r"); //Mobile phone number to send message
      delay(1000);
      sim.println(SMS);
      Serial.println("**gui tin nhan**");
      delay(100);
      sim.println((char)26);// ASCII code of CTRL+Z
      delay(5000);
      sim.println("AT+CMGDA=\"DEL ALL\"");
      delay(2000);
      updateSerial();
      delay(500);
      Sim800l.delAllSms();
    }
  }
  sim.end();
}
//..............................................................
void updateSerial() {
  delay(500);
  while (Serial.available()) {
    sim.write(Serial.read());
  }
  while (sim.available()) {
    Serial.write(sim.read());
  }
}
//..............................................................
void loop() {
  readgps();
  ReadSMS();
  unsigned long int timer;
  timer = millis();
  while ((millis() - timer) < 2000) {
    if ((digitalRead(7) == HIGH) && (STT == "STT=1")) {
      digitalWrite(12, HIGH);
      Serial.println("**buzzer canh bao**");
      call = 1;
    }
  }
}
