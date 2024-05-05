#include <ArduinoJson.h>
#include "WiFiEsp.h"
char ssid[] = "iptime";                        // WiFi 이름
char pass[] = "";                      // WiFi 비밀번호
int status = WL_IDLE_STATUS;                   // WiFi 연결 상태
char server[] = "";              // 서버 주소
unsigned long lastConnectionTime = 0;          // 마지막 연결 시간
const unsigned long postingInterval = 10000L;  // 연결 간격 (10초)
WiFiEspClient client;
void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);
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
}

void loop() {
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
      Serial.println(line);  // 본문 출력
      DeserializationError error = deserializeJson(doc, line);
      String response = doc["message"];
      Serial.println(response);
    }
  }
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }
}

void httpRequest() {
  client.stop();
  client.flush();
  client.setTimeout(10000);
  if (client.connect(server, 917)) {

    Serial.println("Connecting...");
    client.println("GET /api/bus/3 HTTP/1.1");
    //client.println("Host: ");
    client.println("Connection: close");
    client.println();
    lastConnectionTime = millis();
  } else {
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
