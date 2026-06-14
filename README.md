# Perdio Radio

Repurposing a 1960s Perdio transistor radio into a Bluetooth speaker, while keeping the original cabinet, speaker, knobs and dial. Part of my internship with Rebel Punkworx (Sustainable Product and Service Design).

The idea is "good repair": the radio should still look original, and the build should be something you can open up and fix later, not glued shut. The old electronics come out, an ESP32 goes in, and the radio gets a second life instead of going to the scrap heap.

## What it does right now

The ESP32 acts as a Bluetooth speaker. You pair your phone to "Perdio Radio", play music, and the board receives the audio stream. The audio output stage (amplifier and speaker) is the next step, so for now the received stream is just confirmed in the serial monitor.

Build order I am following (simplest first):
1. Bluetooth audio in — **working**
2. Audio out through the amplifier to the original speaker — next
3. SD card playback — later
4. Internet radio — optional, last

## Hardware

- ESP32 dev board (ESP-WROOM-32, CP2102 USB chip)
- MAX98357A I2S amplifier (converts the digital audio and drives the speaker)
- Original Perdio speaker, reused
- Breadboard and jumper wires for testing before anything is soldered

## Wiring (planned, for the audio output stage)

MAX98357A to ESP32:

| MAX98357A | ESP32 |
|-----------|-------|
| LRC  | GPIO25 |
| BCLK | GPIO26 |
| DIN  | GPIO22 |
| VIN  | 5V |
| GND  | GND |

Speaker connects to the amplifier's + and - output.

## Software

- VS Code with the PlatformIO extension
- C++ on the Arduino framework
- Bluetooth audio handled by the [pschatzmann ESP32-A2DP](https://github.com/pschatzmann/ESP32-A2DP) library

All the config is in `platformio.ini`. The test code is in `src/main.cpp`.

## How to build it

1. Open the folder in VS Code with PlatformIO installed
2. Plug the board in with a **data** USB cable (a charge only cable will not work)
3. Hit Upload
4. Open the serial monitor at 115200 baud
5. On your phone, pair with "Perdio Radio" and play music

You should see `Audio streaming... received 4096 bytes` scrolling as the audio comes in.

## Notes to myself (things that caught me out)

- A USB cable that looks fine can be charge only with no data wires. If the board does not show up as a serial port, the cable is the first thing to check, not the board.
- The library would not install by its short name. Pointing `lib_deps` straight at the GitHub URL fixed it.
- The serial monitor opened at 9600 baud by default and showed garbage. The code prints at 115200, so the speeds have to match. Set `monitor_speed = 115200` in `platformio.ini`.
- The warning `I2S0 has not installed yet` is expected until the amplifier and I2S pins are wired in. It does not mean the Bluetooth side is broken.

## Status

| Date | Milestone |
|------|-----------|
| 14 Jun 2026 | Dev environment set up, board flashing, Bluetooth receiver verified |
| next | Wire the MAX98357A and speaker, get real sound out |
