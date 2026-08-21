#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "Audio.h"

#define SD_CS    5
#define I2S_BCLK 26
#define I2S_LRC  25
#define I2S_DOUT 22

#define MAX_TRACKS 16

Audio audio;

char tracks[MAX_TRACKS][64];
int trackCount = 0;
int current = 0;
int volume = 18;

void scanTracks() {
  File dir = SD.open("/");
  File f = dir.openNextFile();
  while (f && trackCount < MAX_TRACKS) {
    const char *n = f.name();
    if (!f.isDirectory() && n[0] != '.' && strstr(n, ".mp3")) {
      snprintf(tracks[trackCount], sizeof(tracks[0]), "/%s", n);
      Serial.printf("%d: %s\n", trackCount + 1, tracks[trackCount]);
      trackCount++;
    }
    f = dir.openNextFile();
  }
}

void play(int i) {
  if (trackCount == 0) return;
  current = (i + trackCount) % trackCount;
  audio.stopSong();
  audio.connecttoFS(SD, tracks[current]);
  Serial.printf("playing %s\n", tracks[current]);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("SD player");

  if (!SD.begin(SD_CS)) {
    Serial.println("mount failed");
    return;
  }

  scanTracks();
  if (trackCount == 0) {
    Serial.println("no tracks");
    return;
  }

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(volume);
  play(0);
}

void loop() {
  audio.loop();

  if (Serial.available()) {
    char c = Serial.read();

    if (c >= '1' && c <= '9') {
      play(c - '1');
    }
    else if (c == 'n') {
      play(current + 1);
    }
    else if (c == 'p') {
      play(current - 1);
    }
    else if (c == '+') {
      volume = min(volume + 1, 21);
      audio.setVolume(volume);
      Serial.printf("volume %d\n", volume);
    }
    else if (c == '-') {
      volume = max(volume - 1, 0);
      audio.setVolume(volume);
      Serial.printf("volume %d\n", volume);
    }
  }
}

void audio_info(const char *info) { Serial.printf("info: %s\n", info); }
void audio_eof_mp3(const char *info) { play(current + 1); }
