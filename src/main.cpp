#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "Audio.h"

#define SD_CS    5
#define I2S_BCLK 26
#define I2S_LRC  25
#define I2S_DOUT 22

Audio audio;

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("SD playback test");

  if (!SD.begin(SD_CS)) {
    Serial.println("mount failed");
    return;
  }

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(12);
  audio.connecttoFS(SD, "/01.mp3");
}

void loop() {
  audio.loop();
}

void audio_info(const char *info) { Serial.printf("info: %s\n", info); }
