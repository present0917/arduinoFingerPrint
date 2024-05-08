#include "WiFiEsp.h"
#include <Adafruit_Fingerprint.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <ArduinoJson.h>

#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' },
};
byte rowPins[ROWS] = { 52, 50, 48, 46 };
byte colPins[COLS] = { 44, 42, 40, 38 };
Keypad save = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

SoftwareSerial Serialw(10, 11);  // RX, TX
char SSID_NAME[] = "iptime";
char SSID_PASS[] = "";
char SERVER[] = "";
int HTTPPORT = 917;
char PATH[] = "/api/busstop";
WiFiEspClient client;

//SoftwareSerial fingerSerial(10, 11);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);
int id;
int returned_id;

//SoftwareSerial mp3Serial(12, 13); // RX, TX
DFRobotDFPlayerMini myDFPlayer;


void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  Serialw.begin(9600);
  WiFi.init(&Serialw);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SSID_NAME);
    WiFi.begin(SSID_NAME, SSID_PASS);
    delay(1000);
  }
  Serial.println("Connected to wifi");
  //wifi 파트

  while (!Serial)
    ;
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Device address: "));
  Serial.println(finger.device_addr, HEX);
  //여까지 지문파트


  //mp3Serial.begin(9600);
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(Serial2)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true)
      ;
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(25);  //Set volume value. From 0 to 30
  //mp3 파트

  pinMode(4, INPUT);
  pinMode(5, OUTPUT);
}


void loop() {
  int choicenum;
  Serial.println("input commandNum");
  choicenum = readnumber();
  if (choicenum == 1) enroll();
  else if (choicenum == 2) readFinger();
  else if (choicenum == 3) mp3test();
  else if (choicenum == 4) postmethod();
  else if (choicenum == 5) personcheck();
}
void personcheck() {
  for (int i = 0; i < 100; i++) {
    int value = digitalRead(4);
    Serial.println(value);
    delay(100);
    if (value == 1) digitalWrite(5, HIGH);
    else digitalWrite(5, LOW);
  }
}
String keypad() {
  String postData;
  String keypadData = "";
  while (1) {
    char key = save.getKey();
    if (key == 'A') break;
    if (key) {
      keypadData += key;
      Serial.println("keypadData = " + keypadData);
    }
  }
  return keypadData;
}
String checkmessage(){
  String padNum = keypad();
  if(padNum=="1"){
    myDFPlayer.playMp3Folder(3); //1번버스?
  }
  else if(padNum=="2"){
    myDFPlayer.playMp3Folder(2); //2번버스?
  }
  String checkCom = keypad();
  if(checkCom=="1") return padNum;
  else checkmessage();
}
void postmethod() {
  myDFPlayer.playMp3Folder(1);//안내
  delay(3000);
  readFinger();
  myDFPlayer.playMp3Folder(2);//인식됐다. 키패드로 버스번호 입력좀.
  String postId= String(returned_id);
  String padNum=checkmessage();


  String postData;
  Serial.println("OK" + padNum);
  postData = "{\"busId\": \"" + padNum + "\", \"Id\": \"" + postId + "\", \"message\": \"" + "test" + "\"}";
  int contentLength = postData.length();

  if (client.connect(SERVER, HTTPPORT)) {
    Serial.println("Connected to server");

    // Send HTTP POST request
    client.print("POST ");
    client.print(PATH);
    client.print(" HTTP/1.1\r\n");
    client.print("Host: ");
    client.print(SERVER);
    client.print("\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Content-Length: ");
    client.print(contentLength);
    client.print("\r\n\r\n");
    client.print(postData);
    //client.print("\r\n");
    // Close connection

  } else {
    Serial.println("Failed to connect to server");
  }
  Serial.println("For Check A");
  bool isBody = false;
  String line;
  while (client.available()) {
    line = client.readStringUntil('\n');
    if (line == "\r") {
      isBody = true;  // 헤더가 끝나고 본문이 시작됨
      continue;       // 본문의 첫 줄을 읽기 위해 계속
    }
    if (isBody) {
      DynamicJsonDocument doc(1024);
      //Serial.println(line);  // 본문 출력
      DeserializationError error = deserializeJson(doc, line);
      String response = doc["message"];
      Serial.println(response);
    }
  }

  client.stop();
  delay(1000);
}

void enroll() {
  int check = 0;
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  id = readnumber();
  if (id == 0) {  // ID #0 not allowed, try again!
    return;
  }
  Serial.print("Enrolling ID #");
  Serial.println(id);
  //while (!  getFingerprintEnroll() );
  check = getFingerprintEnroll();
  if (check == 25) { //정상적으로 읽어졌다면
    Serial.println("Input disabled Type");
    char key = 'G';
    int keyloop=0;
    String disabledType;
    while (keyloop==0) {
      key=save.getKey();
      switch (key) {
        case '1':
          Serial.println("leg");
          disabledType = "leg";
          keyloop=1;
          break;
        case '2':
          Serial.println("eye");
          disabledType = "eye";
          keyloop=1;
          break;
        case '3':
          Serial.println("other");
          disabledType = "other";
          keyloop=1;
          break;
        
      }
    }
    postEnroll(String(id), disabledType);
  }
  else{
    enroll();
  }
}

void postEnroll(String idnum, String distype) {
  
  String postData;
  postData = "{\"Id\": \"" + idnum + "\", \"type\": \"" + distype + "\"}";
  int contentLength = postData.length();

  if (client.connect(SERVER, HTTPPORT)) {
    Serial.println("Connected to server");

    // Send HTTP POST request
    client.print("POST ");
    client.print("/api/enroll");
    client.print(" HTTP/1.1\r\n");
    client.print("Host: ");
    client.print(SERVER);
    client.print("\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Content-Length: ");
    client.print(contentLength);
    client.print("\r\n\r\n");
    client.print(postData);
    //client.print("\r\n");
    // Close connection

  } else {
    Serial.println("Failed to connect to server");
  }
  Serial.println("For Check A");
  bool isBody = false;
  String line;
  while (client.available()) {
    line = client.readStringUntil('\n');
    if (line == "\r") {
      isBody = true;  // 헤더가 끝나고 본문이 시작됨
      continue;       // 본문의 첫 줄을 읽기 위해 계속
    }
    if (isBody) {
      DynamicJsonDocument doc(1024);
      //Serial.println(line);  // 본문 출력
      DeserializationError error = deserializeJson(doc, line);
      String response = doc["message"];
      Serial.println(response);
    }
  }

  client.stop();
  delay(1000);
}



void readFinger() {
  returned_id=500;
  while (returned_id==500){
    getFingerprintID();
    Serial.println("returned_id : "+String(returned_id));
    delay(100);
  }
}
int getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  Serial.print("Creating model for #");
  Serial.println(id);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  Serial.print("ID ");
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    return 25;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  return true;
}
int getFingerprintID() {
  int p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  returned_id=finger.fingerID;
  return finger.fingerID;
}
int getFingerprintIDez() {
  int p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}
int readnumber(void) {  // 시리얼 입력받아 반환하는 함수
  int num = 0;
  while (num == 0) {
    while (!Serial.available())
      ;
    num = Serial.parseInt();
  }
  return num;
}
//여기까지지문함수들

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void mp3test() {
  myDFPlayer.playMp3Folder(1);  //Play the first mp3
  delay(5000);
  myDFPlayer.playMp3Folder(3);
  delay(5000);
  myDFPlayer.playMp3Folder(5);
  delay(5000);
}
