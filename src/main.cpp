#include <Arduino.h>
#include "BluetoothA2DPSink.h"

BluetoothA2DPSink a2dp_sink;

volatile uint32_t last_audio_ms = 0;   // when audio last arrived
volatile uint32_t total_bytes = 0;

// Fires only when real audio data streams in
void audio_data_callback(const uint8_t *data, uint32_t len) {
  last_audio_ms = millis();
  total_bytes += len;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Bluetooth speaker...");

  i2s_pin_config_t my_pins = {
    .bck_io_num = 26,    // BCLK
    .ws_io_num = 25,     // LRC
    .data_out_num = 22,  // DIN
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  a2dp_sink.set_pin_config(my_pins);

  a2dp_sink.set_stream_reader(audio_data_callback, true);  // true = still play
  a2dp_sink.start("Perdio Radio");

  Serial.println("Ready. Look for 'Perdio Radio' on your phone Bluetooth.");
}

void loop() {
  bool playing = (millis() - last_audio_ms) < 500;  // audio in last half second?

  if (playing) {
    Serial.printf("PLAYING - audio flowing, total %u bytes\n", total_bytes);
  } else {
    Serial.println("IDLE - connected but no audio (paused or stopped)");
  }

  delay(1000);
}