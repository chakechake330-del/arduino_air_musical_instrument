#include <FastLED.h>

#define NUM_LEDS           // LEDの数（Processingのbandsと合わせる）
#define DATA_PIN 6            // LEDのデータピン

#define TRIG_PIN 9            // 超音波センサーのTRIGピン
#define ECHO_PIN 10           // 超音波センサーのECHOピン

CRGB leds[NUM_LEDS];

String inputString = "";
bool receiving = false;

unsigned long lastNoteTime = 0;
bool noteOnSent = false;
byte currentNote = 0;

long lastDistance = 0;  // 前回の距離を記録

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  handleLEDSerial();
  handleUltrasonicMIDI();
}

void handleLEDSerial() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      processInput(inputString);
      inputString = "";
      receiving = false;
    } else {
      inputString += inChar;
      receiving = true;
    }
  }
}

void processInput(String data) {
  if (data.charAt(0) != 'L') return;

  int values[NUM_LEDS];
  int index = 0;
  int lastComma = 0;

  for (int i = 2; i < data.length(); i++) {
    if (data.charAt(i) == ',' || i == data.length() - 1) {
      String numStr = data.substring(lastComma + 1, (data.charAt(i) == ',') ? i : i + 1);
      values[index] = numStr.toInt();
      index++;
      lastComma = i;
      if (index >= NUM_LEDS) break;
    }
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(i * 8, 255, values[i]);  // 色相を細かく分けてカラフルに
  }
  FastLED.show();
}

void handleUltrasonicMIDI() {
  long duration, distance;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 20000); // タイムアウト20ms
  distance = duration * 0.034 / 2;

  // 0cmが出たら前回の値を使う
  if (distance == 0) {
    distance = lastDistance;
  } else {
    lastDistance = distance;
  }

  if (distance > 5 && distance < 50 && !noteOnSent) {
    currentNote = map(distance, 5, 50, 72, 48);  // C5〜C3
    Serial.write((byte)(0x90));  // Note On
    Serial.write(currentNote);
    Serial.write((byte)100);
    noteOnSent = true;
    lastNoteTime = millis();
  }

  if (noteOnSent && millis() - lastNoteTime > 300) {
    Serial.write((byte)(0x80));  // Note Off
    Serial.write(currentNote);
    Serial.write((byte)0);
    noteOnSent = false;
  }
}
