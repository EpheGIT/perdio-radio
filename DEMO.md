# Perdio Radio — how to run the demo

## Before you start

1. Open `src/main.cpp`. Put the hotspot name and password in lines 26–27.
2. Turn on the phone hotspot. Must be 2.4GHz.
3. Put the SD card in the reader. Plug the board into the laptop.

## Run it

1. In VS Code, click the alien-head icon in the left bar.
2. Project Tasks → esp32dev → General → **Upload and Monitor**.
3. Wait for text to appear in the terminal. Music starts on its own (SD mode).

## Switch function

Click in the terminal, press one key. The board restarts, about two seconds.

| Key | Function |
|-----|----------|
| `s` | SD card — plays MP3s |
| `r` | Internet radio |
| `b` | Bluetooth — pair phone with "Perdio Radio" |

## Controls

| Key | Does |
|-----|------|
| `1` `2` | pick track or station |
| `n` `p` | next / previous track |
| `+` `-` | volume |

Bluetooth volume comes from the phone, not the keyboard.

## If something is wrong

- No sound → press the **EN** button on the board.
- `wifi failed` → hotspot is off, or on 5GHz.
- `mount failed` → SD card not in, or module not on 5V.
