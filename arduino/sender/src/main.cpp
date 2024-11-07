#include <Arduino.h>
#include "Arduino_LED_Matrix.h"

const int INPUT_BUTTON_PIN = 2;
const int OUTPUT_LED_PIN = 4;

ArduinoLEDMatrix matrix;
const unsigned long frames[6][3] = {
  {0x3184a444, 0x42081100, 0xa0040000 },
  {0x3184E440, 0x42081101, 0x100A0000 },
  {0x3184E440, 0x42081101, 0x10110110 },
  {0x3184E440, 0x42081101, 0x10208110 },
  {0x3184E440, 0x42081101, 0x102081F0 },
  {0x00000000, 0x00000000, 0x00000000 },
};
int current_frame = -1;

void setup() {
  pinMode(INPUT_BUTTON_PIN, INPUT);
  pinMode(OUTPUT_LED_PIN, OUTPUT);

  matrix.begin();

  Serial.begin(9600);
}

void loop() {

  int button = digitalRead(INPUT_BUTTON_PIN);
  //Serial.println(button);
  if(button == HIGH){
    digitalWrite(OUTPUT_LED_PIN, HIGH);
    current_frame = current_frame < 0 ? 0 : current_frame;
  }
  else {
     digitalWrite(OUTPUT_LED_PIN, LOW);
  }

  if(current_frame >= 0){
    matrix.loadFrame(frames[current_frame]);
    current_frame = current_frame == 5 ? -1 : current_frame + 1;
  }

  delay(1000);
}