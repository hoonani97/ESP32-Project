#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <stdio.h>
#include <stdlib.h>
#include <DHT.h>
#include <DS1302.h>
#include <TimeLib.h>
#include <UnixTime.h>

#define DHTTYPE DHT22

SoftwareSerial esp32(2, 3);
DHT dht(13, DHTTYPE);
LiquidCrystal lcd(4, 5, 8, 9, 10, 11);
DS1302 rtc(6,7,12); //rst, dat, clk
UnixTime stamp(3);

Time t;

const String ssid = "iptime";
const String password = "";

//AT 명령어 작동 테스트
void cmdtest() {
  esp32.println("AT");

  if (esp32.find("OK")) {
    Serial.println("Test Success");
  } else if (esp32.find("ERROR")) {
    Serial.println("Test Error");
  }
}

//Esp32 모듈 Wifi 연결
void wifitest() {
  //esp32.println("AT+CWMODE=1");
  //delay(100);
  //esp32.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");
  //delay(100);
  esp32.println("AT+CWSTATE?");
  if (esp32.find("2")) {
    Serial.println("Wifi Conncected");
  } else {
    Serial.println("Wifi Error");
  }
}

//체크인
//void checkin() {
  //String cmd = "AT+HTTPCLIENT=3,0,\"";
  //cmd += "http://52.192.183.203/checkin.php\""; //url
  //cmd += ",\"";
  //cmd += "http://52.192.183.203\""; //host
  //cmd += ",\"";
  //cmd += "/checkin.php\""; //path
  //cmd += ",1,\""; //transport type
  //cmd += "mac=111133335555";
  //cmd += "&ver=20170131";
  //cmd += "&model=RN400H2PS";
  //cmd += "&ip=192.168.100.11";
  //cmd += "&splrate=60"; //측정 주기
  //cmd += "&interval=60"; //서버 전송 주기
  //cmd += "&tags=TEMP|HUMI";
  //cmd += "\"\n";
    
  //esp32.print(cmd);
//}

void setup() {
  rtc.halt(false);
  rtc.writeProtect(true);
  //rtc.setDate(27,5,2022);
  //rtc.setTime(15,13,30);
  dht.begin();
  lcd.begin(16,2);
  Serial.begin(9600);
  esp32.begin(9600);
  cmdtest();
  delay(300);
  wifitest();
  delay(300);
  //checkin();
}

void loop() {
  delay(300);
  
  float humi = dht.readHumidity();
  float temp = dht.readTemperature();

  //온습도 값 전달을 위해 문자형으로 변환
  char x[16];

  String strHumi = dtostrf(humi, 4, 1, x);
  String strTemp = dtostrf(temp, 4, 1, x);

  lcd.setCursor(0, 0);
  lcd.print("Humi:");
  lcd.setCursor(6, 0);
  lcd.print(strHumi);
  lcd.setCursor(11, 0);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Temp:");
  lcd.setCursor(6, 1);
  lcd.print(strTemp);
  lcd.setCursor(11, 1);
  lcd.print("*C");
  
  //현재 시간을 UNIX 시간으로 변환
  t = rtc.getTime();
  
  stamp.setDateTime(t.year, t.mon, t.date, t.hour - 6, t.min, t.sec);
  uint32_t current_unix = stamp.getUnix();
  String strTime = String(current_unix);
  
  //데이터인
  String cmd = "AT+HTTPCLIENT=3,0,\"http://52.192.181.203/datain.php\"";
  cmd += ",\"http://52.192.181.203\",\"/datain.php\"";
  cmd += ",1,";
  cmd += "\"mac=111133335555&sig=-40&bat=255&volt=1|2&SMODEL=RN400H2EX&";
  cmd += "C000=";
  cmd += strTime;
  cmd += "|";
  cmd += strTemp;
  cmd +="|";
  cmd += strHumi;
  cmd += "|";
  cmd += "\"";

  esp32.println(cmd);
  delay(60000);
}
