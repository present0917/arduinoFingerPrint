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

void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  char key = save.getKey();
  if (key)
  {
    Serial.println(key);
  }
}
