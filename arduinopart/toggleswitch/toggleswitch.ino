//검은선, 125쪽 GND, 반대쪽 5V , 가운데 7번핀
void setup() {
  Serial.begin(9600);
  pinMode(7, INPUT_PULLUP);  // 버튼 7번핀, 내부 풀업저항
  pinMode(13, OUTPUT);       // LED 13번 핀
  digitalWrite(13, LOW);
}
void loop() {
  if (digitalRead(7) == 1) {
    digitalWrite(13, HIGH);  // LED ON 125쪽
    Serial.println("HIGH");
  } 
  else {
    digitalWrite(13, LOW);  // LED OFF 6A쪽
    Serial.println("LOW");
  }
  delay(100);
}