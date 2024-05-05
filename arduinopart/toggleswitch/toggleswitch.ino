void setup() {
  Serial.begin(9600);
  pinMode(7, INPUT_PULLUP);  // 버튼 7번핀, 내부 풀업저항
  pinMode(13, OUTPUT);       // LED 13번 핀
  digitalWrite(13, LOW);
}
void loop() {
  if (digitalRead(7) == 1) {
    digitalWrite(13, HIGH);  // LED ON 
    Serial.println("HIGH");
  } 
  else {
    digitalWrite(13, LOW);  // LED OFF 스위치 암페어 써진 쪽
    Serial.println("LOW");
  }
  delay(100);
}