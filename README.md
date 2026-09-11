# Perdio Radio

> **Picking this up?** Start with [HANDOVER.md](HANDOVER.md) — written for someone with no programming background. Then [DEMO.md](DEMO.md) for running it.

Restoring a donated vintage radio cabinet with an ESP32 inside, while keeping the original cabinet, speaker, knobs and dial. Part of my placement with Rebel Punkworx, a charity working in electronic waste repair and sustainability.

The idea is "good repair": the radio should still look original, and the build should be something you can open up and fix later, not glued shut. The old electronics come out, an ESP32 goes in, and the radio gets a second life instead of going to the scrap heap.

**About the name.** The repo is called `perdio-radio` and the Bluetooth device shows up as "Perdio Radio" because a Perdio PR22 was the first cabinet planned. The build actually in progress is a **Philips R4400**. The PR22 comes later.

## What it does

Three functions, all in one program. The board reads which one to start from its own memory at boot, and only one runs at a time.

1. **Bluetooth speaker** — pair a phone with "Perdio Radio"
2. **Internet radio** — streams SomaFM over a phone hotspot
3. **SD card** — plays MP3 files from a micro SD card

Switching is from the serial monitor for now (`b`, `r`, `s`). See `DEMO.md` for how to run it. Wiring the radio's own buttons to do this is the next step.

## Hardware

- ESP32-WROOM-32 DevKit V1, 30 pin. **No PSRAM** — this matters, see below
- 2x MAX98357A I2S amplifier boards, one per speaker, both fed the same signal (dual mono)
- Micro SD card reader module, 5V type with its own regulator
- 32GB micro SD card, FAT32
- Breadboard and jumper wires until the electronics are final

## Wiring

MAX98357A to ESP32 (both amps share the same three signal lines):

| MAX98357A | ESP32 |
|-----------|-------|
| BCLK | GPIO26 |
| LRC  | GPIO25 |
| DIN  | GPIO22 |
| VIN  | 5V / VIN |
| GND  | GND |

SD card module to ESP32:

| SD module | ESP32 |
|-----------|-------|
| CS   | GPIO5 |
| SCK  | GPIO18 |
| MOSI | GPIO23 |
| MISO | GPIO19 |
| VCC  | **5V / VIN**, not 3V3 |
| GND  | GND |

The SD module has its own regulator. Feeding it 3.3V is the usual reason it fails to mount.

Volume pot (tested, not yet in the main code): outer tabs to 3V3 and GND, wiper to GPIO34.

## Software

- VS Code with the PlatformIO extension, Arduino framework, board `esp32dev`
- `board_build.partitions = huge_app.csv` — WiFi plus the audio decoder does not fit the default partition
- [ESP32-audioI2S](https://github.com/schreibfaul1/ESP32-audioI2S) **pinned to 2.0.6**. Do not update it. Newer versions need PSRAM, which this board does not have
- [ESP32-A2DP](https://github.com/pschatzmann/ESP32-A2DP) for Bluetooth

All config is in `platformio.ini`. All code is in `src/main.cpp`.

## Getting started

1. Install VS Code, then the **PlatformIO IDE** extension inside it
2. Clone this repo:
   ```
   git clone https://github.com/EpheGIT/perdio-radio.git
   ```
3. Open the folder in VS Code. PlatformIO downloads the libraries by itself the first time — give it a few minutes
4. Put your WiFi name and password in `src/main.cpp` (they are blank on purpose, never commit them)
5. Plug the board in with a **data** USB cable
6. PlatformIO sidebar → Project Tasks → esp32dev → General → **Upload and Monitor**

The MP3 files are not in the repo. Put your own in the root of the SD card, named `01.mp3`, `02.mp3` and so on.

## Known limits

- **No PSRAM** means a 6399 byte stream buffer. 64k AAC streams stutter, 32k plays cleanly. That is the hardware, not the code
- **Plain HTTP streams only.** No HTTPS, no HLS. That rules out most BBC stations
- **Hotspot must be 2.4GHz.** The ESP32 cannot see 5GHz. Eduroam will not work either
- The 5V rail is marginal with two amps and WiFi. If the board reboots for no reason, suspect power first. A 1000uF capacitor across the 5V rail is the planned fix

## Things that caught me out

- A USB cable that looks fine can be charge only. If the board does not show up as a serial port, check the cable before the board
- Libraries would not install by their short names. Point `lib_deps` at the GitHub URL
- Serial monitor defaults to 9600 baud, code prints at 115200. `monitor_speed = 115200` in `platformio.ini` fixes it
- Open the serial monitor **first**, then press EN on the board. Opening it afterwards misses the boot lines
- Do not put the working copy inside iCloud Drive. The path has colons and spaces in it and PlatformIO breaks
- The `nvs_open failed: NOT_FOUND` line on the very first boot is harmless. It just means no mode was saved yet

## Status

| Date | Milestone |
|------|-----------|
| 14 Jun 2026 | Board flashing, Bluetooth receiver verified |
| 19 Jun 2026 | Real sound through the amplifiers |
| Jul 2026 | Internet radio streaming, station and volume control |
| 21 Aug 2026 | SD card playback with track selection |
| 21 Aug 2026 | All three functions in one program, mode saved across reboot |
| 10 Sep 2026 | Original volume pot read on GPIO34, working |
| next | Find the contact pairs on the original MW / LW keys, wire them in |
| then | 1000uF capacitor on 5V, move everything into the cabinet |
