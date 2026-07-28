#include <Arduino.h>
#include <WiFi.h>
#include "Audio.h"

#define I2S_BCLK 26
#define I2S_LRC  25
#define I2S_DOUT 22

Audio audio;

const char* ssid = "";
const char* password = "";

const char* stations[] = {
  "http://ice1.somafm.com/groovesalad-64-aac",
  "http://ice1.somafm.com/groovesalad-32-aac"
};

int current = 0;
int volume = 12;
char lastTitle[128] = "";

void setup() {
  Serial.begin(115200);
  Serial.println("Starting internet radio...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi ok, IP ");
  Serial.println(WiFi.localIP());

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(12);   // 0 to 21
  audio.connecttohost(stations[0]);
}

void loop() {
  audio.loop();

  if (Serial.available()) {
    char c = Serial.read();

    if (c == '1' || c == '2') {
      current = c - '1';
      audio.connecttohost(stations[current]);
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
void audio_showstation(const char *s) { Serial.printf("station: %s\n", s); }
void audio_showstreamtitle(const char *s) {
  if (strcmp(lastTitle, s) != 0) {
    strncpy(lastTitle, s, sizeof(lastTitle) - 1);
    Serial.printf("title: %s\n", s);
  }
}