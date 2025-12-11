import processing.serial.*;
import processing.sound.*;

AudioIn in;
FFT fft;
int bands = 8;
float[] spectrum = new float[bands];
Serial myPort;

void setup() {
  size(512, 200);
  in = new AudioIn(this, 0);
  fft = new FFT(this, bands);
  in.start();
  fft.input(in);

  printArray(Serial.list());
  myPort = new Serial(this, Serial.list()[2], 9600); // ← ポート番号は確認してね！
}

void draw() {
  background(0);
  fft.analyze(spectrum);

  // データを文字列に変換して送信
  String data = "";
  for (int i = 0; i < bands; i++) {
    int level = int(spectrum[i] * 255); // 0〜255にスケーリング
    level = constrain(level, 0, 255);
    data += level;
    if (i < bands - 1) data += ",";
  }
  myPort.write(data + "\n"); // 改行で区切って送信！

  // 可視化（オプション）
  for (int i = 0; i < bands; i++) {
    float h = spectrum[i] * height * 5;
    rect(i * (width / bands), height - h, width / bands, h);
  }

  delay(50); // 送信間隔を少し空ける
}test
