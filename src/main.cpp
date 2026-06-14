#include <Arduino.h>
#include "BluetoothA2DPSink.h"

BluetoothA2DPSink a2dp_sink;

// Called every time audio data arrives from the phone
void audio_data_callback(const uint8_t *data, uint32_t len) {
  static uint32_t counter = 0;
  counter++;
  if (counter % 100 == 0) {   // print every 100 packets so it is readable
    Serial.printf("Audio streaming... received %u bytes\n", len);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Bluetooth speaker...");

  a2dp_sink.set_stream_reader(audio_data_callback, false);
  a2dp_sink.start("Perdio Radio");

  Serial.println("Ready. Look for 'Perdio Radio' on your phone Bluetooth.");
}

void loop() {
  delay(1000);
}
