#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "";  // 여기에 WiFi 이름 입력
const char* password = "";  // 여기에 WiFi 비밀번호 입력

String serverIP = "";  // 서버의 IP 주소를 입력하세요

WiFiClient client;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;  
    Serial.println("begin");
    http.begin(client, serverIP + "/api/busstop");  // 서버의 주소와 경로 수정
    http.addHeader("Content-Type", "application/json");  // JSON 데이터 전송을 위한 헤더 설정
    String httpRequestData = "{\"busId\":\"33\",\"message\":\"testmessage\"}";

    // GET 요청 보내기
    int httpCode = http.POST(httpRequestData);
    
    while(http.connected() && httpCode == -1) {
        // 계속 연결 상태 확인
        delay(1000); // 너무 빠른 반복을 피하기 위해 약간 지연
        Serial.println("something wrong. repeat");
        httpCode = http.GET(); // 다시 시도
    }

    if (httpCode > 0) {
      // 서버로부터 응답 받기
      String payload = http.getString();
      Serial.println("Received: " + payload);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(http.errorToString(httpCode).c_str());
    }

    http.end();  // HTTP 연결 종료
    Serial.println("end");
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(10000);  // 다음 요청까지 0.5초 대기
}