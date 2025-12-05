#include <Adafruit_NeoPixel.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
#include <FFT.h>
#include <ErriezTM1637.h>

#define LED_PIN     6
#define AUDIO_PIN   A0
#define DF_TX       10
#define DF_RX       11
#define KP_CLK      3
#define KP_DIO      4

#define LED_WIDTH   32
#define LED_HEIGHT  8
#define NUM_LEDS    (LED_WIDTH * LED_HEIGHT)
Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

SoftwareSerial dfSerial(DF_RX, DF_TX);
DFRobotDFPlayerMini dfplayer;

ErriezTM1637 keypad(KP_CLK, KP_DIO);

#define SAMPLES         128
#define SAMPLING_FREQ   4000
unsigned int sampling_period_us;
unsigned long microseconds;
byte peak[LED_WIDTH];

void setup() {
  Serial.begin(9600);

  leds.begin();
  leds.setBrightness(40);
  leds.show();

  dfSerial.begin(9600);
  if (!dfplayer.begin(dfSerial)) {
    Serial.println("DFPlayer Mini not found!");
    while (1);
  }
  dfplayer.volume(20);
  Serial.println("DFPlayer ready");

  keypad.begin();
  keypad.setBrightness(7);

  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQ));
}

void loop() {
  int key = keypad.getKey();
  if (key != -1) {
    Serial.print("Key pressed: ");
    Serial.println(key);
    dfplayer.play(key + 1);  // キー0→0001.mp3
  }

  for (int i = 0; i < SAMPLES; i++) {
    microseconds = micros();
    int val = analogRead(AUDIO_PIN);
    fft_input[i] = val;
    while (micros() - microseconds < sampling_period_us);
  }

  fft_window();
  fft_reorder();
  fft_run();
  fft_mag_log();

  leds.clear();
  for (int x = 0; x < LED_WIDTH; x++) {
    int bin = map(x, 0, LED_WIDTH - 1, 2, SAMPLES / 2 - 1);
    int level = map(fft_log_out[bin], 0, 100, 0, LED_HEIGHT);
    level = constrain(level, 0, LED_HEIGHT);

    for (int y = 0; y < level; y++) {
      int idx = getLedIndex(x, y);
      leds.setPixelColor(idx, leds.Color(0, 150, 255));  // 青系
    }
  }
  leds.show();

  delay(30);
}

int getLedIndex(int x, int y) {
  if (x % 2 == 0) {
    return x * LED_HEIGHT + y;
  } else {
    return x * LED_HEIGHT + (LED_HEIGHT - 1 - y);
  }
}
