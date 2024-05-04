#include "WiFiEsp.h"
#include "SoftwareSerial.h"
#include <ArduinoJson.h>

#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;

char keys [ROWS][COLS] = {
  {'1' , '2' , '3' , 'A'},
  {'4' , '5' , '6' , 'B'},
  {'7' , '8' , '9' , 'C'},
  {'*' , '0' , '#' , 'D'},
};
byte rowPins[ROWS] = {52, 50, 48, 46};
byte colPins[COLS] = {44, 42, 40, 38};
Keypad save = Keypad (makeKeymap(keys), rowPins, colPins, ROWS, COLS);

SoftwareSerial Serialw(10, 11); // RX, TX
char SSID_NAME[] = "iptime";
char SSID_PASS[] = "";
char SERVER[] = "";
int HTTPPORT = ;
char PATH[] = "/api/busstop"; 

WiFiEspClient client;

void setup() {
  Serial.begin(9600);
  Serialw.begin(9600);
  WiFi.init(&Serialw);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SSID_NAME);
    WiFi.begin(SSID_NAME, SSID_PASS);
    delay(1000);
  }
  Serial.println("Connected to wifi");
}



void loop() {
  String postData;
  String keypadData="";
  while(1)
  {
    char key = save.getKey();
    if(key=='A') break;
    if (key)
    {
      keypadData+=key;
      Serial.println("keypadData = "+keypadData);
    }
  }
  Serial.print(keypadData);

  Serial.println("OK");
  postData = "{\"busId\": \"" + keypadData + "\", \"message\": \"" + "test" + "\"}";
  Serial.println(postData);
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

  } else {
    Serial.println("Failed to connect to server");
  }
  Serial.println("For Check A");
  bool isBody = false;
  String line;
  while (client.available()) {
    line = client.readStringUntil('\n');
      if (line == "\r") {
        isBody = true; // 헤더가 끝나고 본문이 시작됨
        continue;  // 본문의 첫 줄을 읽기 위해 계속
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
  delay(5000); // Wait 5 seconds before next request
}
