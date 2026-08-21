#include <Arduino.h>
#include <Preferences.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include "Audio.h"
#include "BluetoothA2DPSink.h"

#define SD_CS    5
#define I2S_BCLK 26
#define I2S_LRC  25
#define I2S_DOUT 22

#define MODE_BT    0
#define MODE_RADIO 1
#define MODE_SD    2

#define MAX_TRACKS 16

Preferences prefs;
int mode = MODE_SD;

Audio *audio = nullptr;
BluetoothA2DPSink *a2dp = nullptr;

const char *ssid = "";
const char *password = "";

const char *stations[] = {
  "http://ice1.somafm.com/groovesalad-64-aac",
  "http://ice1.somafm.com/groovesalad-32-aac"
};
const int stationCount = 2;

char tracks[MAX_TRACKS][64];
int trackCount = 0;
int current = -1;
int volume = 18;

void startSD();

void setMode(int m) {
  prefs.begin("radio", false);
  prefs.putUChar("mode", m);
  prefs.end();
  Serial.println("restarting");
  delay(200);
  ESP.restart();
}

void startBluetooth() {
  a2dp = new BluetoothA2DPSink();

  i2s_pin_config_t pins = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_LRC,
    .data_out_num = I2S_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  a2dp->set_pin_config(pins);
  a2dp->start("Perdio Radio");

  Serial.println("bluetooth ready, pair with 'Perdio Radio'");
}

void playStation(int i) {
  if (i == current) return;
  current = i;
  audio->stopSong();
  delay(200);
  audio->connecttohost(stations[current]);
  Serial.printf("station %d\n", current + 1);
}

void startRadio() {
  WiFi.begin(ssid, password);
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nwifi failed, falling back to sd");
    mode = MODE_SD;
    startSD();
    return;
  }
  Serial.printf("\nwifi ok, IP %s\n", WiFi.localIP().toString().c_str());

  audio = new Audio();
  audio->setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio->setVolume(volume);
  playStation(0);
}

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
  audio->stopSong();
  audio->connecttoFS(SD, tracks[current]);
  Serial.printf("playing %s\n", tracks[current]);
}

void startSD() {
  if (!SD.begin(SD_CS)) {
    Serial.println("mount failed");
    return;
  }

  scanTracks();
  if (trackCount == 0) {
    Serial.println("no tracks");
    return;
  }

  audio = new Audio();
  audio->setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio->setVolume(volume);
  play(0);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  prefs.begin("radio", true);
  mode = prefs.getUChar("mode", MODE_SD);
  prefs.end();

  Serial.printf("mode %d  (b bluetooth, r radio, s sd)\n", mode);

  if (mode == MODE_BT)         startBluetooth();
  else if (mode == MODE_RADIO) startRadio();
  else                         startSD();
}

void loop() {
  if (audio) audio->loop();

  if (!Serial.available()) return;
  char c = Serial.read();

  if (c == 'b' && mode != MODE_BT)         setMode(MODE_BT);
  else if (c == 'r' && mode != MODE_RADIO) setMode(MODE_RADIO);
  else if (c == 's' && mode != MODE_SD)    setMode(MODE_SD);

  if (!audio) return;

  if (c == '+') {
    volume = min(volume + 1, 21);
    audio->setVolume(volume);
    Serial.printf("volume %d\n", volume);
  }
  else if (c == '-') {
    volume = max(volume - 1, 0);
    audio->setVolume(volume);
    Serial.printf("volume %d\n", volume);
  }
  else if (mode == MODE_SD) {
    if (c >= '1' && c <= '9')  play(c - '1');
    else if (c == 'n')         play(current + 1);
    else if (c == 'p')         play(current - 1);
  }
  else if (mode == MODE_RADIO) {
    if (c >= '1' && c <= '9' && (c - '1') < stationCount) playStation(c - '1');
  }
}

void audio_info(const char *info) { Serial.printf("info: %s\n", info); }
void audio_eof_mp3(const char *info) { if (mode == MODE_SD) play(current + 1); }
