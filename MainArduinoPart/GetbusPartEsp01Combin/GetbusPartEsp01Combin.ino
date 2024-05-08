#include <ArduinoJson.h>
#include "WiFiEsp.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini myDFPlayer;

char ssid[] = "iptime";                        
char pass[] = "";                      
int status = WL_IDLE_STATUS;                  
char server[] = "";              
unsigned long lastConnectionTime = 0;          // 마지막 연결 시간
const unsigned long postingInterval = 10000L;  // 연결 간격 10초

int open=0;

WiFiEspClient client;



void setup() {
  pinMode(10,OUTPUT);

  Serial.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);

  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  
  WiFi.init(&Serial3);
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true)
      ;
  }
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  }
  Serial.println("You're connected to the network");
  printWifiStatus();

  
  pinMode(7, INPUT_PULLUP);  //버튼 7번핀, 내부 풀업저항
  //검은선, 125쪽 GND, 반대쪽 5V , 가운데 7번핀

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(Serial2)) {  //Use softwareSerial to communicate with mp3.
  Serial.println(F("Unable to begin:"));
  Serial.println(F("1.Please recheck the connection!"));
  Serial.println(F("2.Please insert the SD card!"));
  while (true);
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(25);  //Set volume value. From 0 to 30
  //mp3 파트


  lcd.print("start");
}

void loop() {
  getbody();
  if (open==0) {  
    if (digitalRead(7) == 1) {//닫혀있다가 갑자기열리면. 1 되는게 125쪽.
      open=1;
      Serial.println("opened");
      digitalWrite(10,LOW);
      myDFPlayer.playMp3Folder(1);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("opened");
      delay(1000);
    }
  }
  else if (open==1){
    if(digitalRead(7) == 0) {   // 125방향 열려있다가 닫히면.
      open=0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("closed");
      Serial.println("closed");
      delay(1000);
   }
  }
  
  if ((millis() - lastConnectionTime > postingInterval) && (!open)) {
    httpRequest();
  }
}

void getbody() {
  bool isBody = false;
  String line;
  while (client.available()) {
    // char c = client.read();
    // Serial.write(c);
    line = client.readStringUntil('\n');
    if (line == "\r") {
      isBody = true;  // 헤더가 끝나고 본문이 시작됨
      continue;       // 본문의 첫 줄을 읽기 위해 계속
    }
    if (isBody) {
      DynamicJsonDocument doc(1024);
      //Serial.println(line);  // 본문 출력
      DeserializationError error = deserializeJson(doc, line);
      String response = doc["type"];
      if (response!="null"){
      if (response.length() > 2) {
        digitalWrite(10,HIGH);
        lcd.clear();
        Serial.println(response);
        lcd.setCursor(0, 0);
        lcd.print("type "+response);
      }
      }
    } 
  }
}

void httpRequest() {

  
  client.stop();
  client.flush();
  client.setTimeout(10000);
  if (client.connect(server, 917)) {

    Serial.println("Connecting...");
    client.println("GET /api/bus/1 HTTP/1.1");
    //client.println("Host: ");
    client.println("Connection: close");
    client.println();
    lcd.setCursor(1, 3);
    lcd.print("wating for res...");
    lastConnectionTime = millis();
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connection failed");
    Serial.println("Connection failed");
  }
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
