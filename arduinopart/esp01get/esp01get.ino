#include "WiFiEsp.h"
#include "SoftwareSerial.h"
#include <ArduinoJson.h>

SoftwareSerial Serialw(10, 11); // RX, TX
char SSID_NAME[] = "iptime";
char SSID_PASS[] = "";


char PATH[] = "/api/tutorial"; 

WiFiEspClient client;
char SERVER[] = "";
int HTTPPORT = ;

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
  Serial.println("OK");

  if (client.connect(SERVER, HTTPPORT)) {
    Serial.println("Connected to server");
    
    // Send HTTP POST request
client.println(F("GET /api/tutorial HTTP/1.1")); 
client.println("Connection: close");

client.println();

   bool isBody = false;
    String line;

    while (client.connected() || client.available()) {
      line = client.readStringUntil('\n');
      if (line == "\r") {
        isBody = true; // 헤더가 끝나고 본문이 시작됨
        continue;  // 본문의 첫 줄을 읽기 위해 계속
      }
      if (isBody) {
        Serial.print(line);  // 본문 출력
      }
    }
    
    Serial.println("done");
  } else {
    Serial.println("Connection to server failed");
  }

  // Close connection
  client.stop();
  delay(5000); // Delay before next request
}
