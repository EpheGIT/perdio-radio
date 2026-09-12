# Perdio Radio — handover notes for Richard

Written for someone with no programming background. You should not need to change any code to use the radio, add music, or change the stations. Where you do need to edit a file, it is plain text and this document tells you exactly which line.

---

## 1. Where everything is

| What | Where |
|------|-------|
| The code, and all these documents | https://github.com/EpheGIT/perdio-radio |
| Copy on your laptop | the `perdio-radio` folder in your home directory |
| The music | on the SD card in the reader module, files `01.mp3`, `02.mp3` |
| How to run it, step by step | `DEMO.md` in the same folder |
| Wiring tables, parts list, known limits | `README.md` in the same folder |

If your laptop copy is ever lost, download it again: open a terminal and run
`git clone https://github.com/EpheGIT/perdio-radio.git`
Then open that folder in VS Code.

---

## 2. What it does

One program with three functions. Only one runs at a time, and the radio remembers which one you chose last time, even after power off.

| Function | What happens |
|----------|--------------|
| **SD card** | plays the MP3 files on the card, one after another, looping |
| **Internet radio** | streams a station over a phone hotspot |
| **Bluetooth** | shows up on a phone as "Perdio Radio", plays whatever the phone plays |

For now you switch between them by pressing a key on the laptop (`s`, `r`, `b`). Wiring the radio's own MW / LW keys to do this is the next job, see section 7.

---

## 3. Running it

Short version. `DEMO.md` has the full version.

1. Plug the board into the laptop with the USB cable. Put the SD card in.
2. In VS Code, click the alien-head icon in the left bar → Project Tasks → esp32dev → General → **Upload and Monitor**.
3. Text appears in the panel at the bottom. Music starts.
4. Click in that panel and press `s`, `r` or `b` to change function. `+` and `-` for volume. `n` for next track.

For internet radio you need a phone hotspot switched on and set to **2.4GHz**. The board cannot see 5GHz. Your WiFi name and password go in the file `src/main.cpp`, lines 26 and 27, between the quote marks. That is the only code edit you will ever need to make.

---

## 4. Adding more music

The MP3 files on the card came from **Pixabay Music** (pixabay.com/music), which is free to download and free to play in public. Two other good sources:

- **Internet Archive** — archive.org/details/musopen — public domain classical recordings
- **Free Music Archive** — freemusicarchive.org — filter by Creative Commons licence

Any of these are safe to play in the Royal Cinema or in front of a school. Ordinary chart music is not, so please stick to these.

To add music:

1. Download the **MP3** version of the track. Not FLAC, not OGG, not WAV.
2. Put the SD card in a computer. Copy the files into the top level of the card, not inside a folder.
3. Name them `01.mp3`, `02.mp3`, `03.mp3` and so on. Short names, no spaces.
4. Eject the card properly before pulling it out, then put it back in the radio.

Up to 16 tracks. The radio finds them on its own at power on. Nothing to edit.

---

## 5. Changing the radio stations

The station list is in `src/main.cpp`, lines 29 to 32. It looks like this:

```
const char *stations[] = {
  "http://ice1.somafm.com/groovesalad-64-aac",
  "http://ice1.somafm.com/groovesalad-32-aac"
};
```

Each line between the braces is one station. Replace a web address, keep the quote marks and the comma, save, then Upload and Monitor again.

Rules for a station address, because of the board's limits:

- It must begin with **`http://`**, not `https://`. Most BBC stations are https, so they will not work. This was tested.
- Pick the **lowest bitrate** offered, 32k if there is one. 64k plays but stutters. This is a hardware limit of the board, not something the code can fix.
- SomaFM (somafm.com) lists direct stream links for every station and they all work. That is the easiest place to look.

If you add a third station, change `const int stationCount = 2;` on line 33 to `3`.

---

## 6. Things that will trip you up

| Symptom | Cause | Fix |
|---------|-------|-----|
| No sound at all | board did not start properly | press the **EN** button on the board |
| `wifi failed` in the panel | hotspot off, or on 5GHz | check the hotspot, set to 2.4GHz |
| `mount failed` | SD card out, or module on 3.3V | card in; module VCC wire must be on **VIN (5V)** |
| Board keeps restarting mid-song | power supply, not code | see the capacitor note in section 7 |
| Music sounds rough | bench speakers, no enclosure | expected; better in the cabinet |
| Board does not show up in VS Code | USB cable is charge-only | use a data cable |
| VS Code shows no PlatformIO buttons | bottom-left says Restricted Mode | click it, choose Trust |

**One rule above all others:** never update the `ESP32-audioI2S` library. It is pinned to version 2.0.6 in `platformio.ini` because newer versions need memory this board does not have. If VS Code or anyone offers to update it, say no.

---

## 7. What is finished and what is left

**Done and tested**

- All three functions, in one program
- Original volume knob read by the board (tested, works cleanly, not yet wired into the main program)
- The SD card, both amplifiers, both speakers, all wired and working on the breadboard

**Left to do, in order**

1. **The MW and LW keys.** Each key needs one pair of contacts on the brown wafer switch that touch when the key is down. I have not found them yet. Any electronics person with a multimeter on continuity mode can find them in ten minutes. Once found, they wire to GPIO32 and GPIO33 on the board, and the code change to read them is small.
2. **The tune dial.** Your sketch has six positions. Simplest way: fit a small 10k potentiometer on the dial shaft so it turns with the cord, read it on GPIO35 exactly like the volume pot, and split the range into six zones. Position 1 to 6 becomes station 1 to 6 or track 1 to 6. The dial pointer keeps moving as it always did. A rotary encoder is the alternative if the dial turns freely with no end stops.
3. **Bluetooth has no key.** The front panel has MW, LW and ON/OFF. Your notes assign MW to internet radio and LW to MP3. Bluetooth is left over. Options: drop it, or make "no key pressed" mean Bluetooth. Your call.
4. **A 1000µF capacitor across the 5V rail**, near the amplifiers. This is the fix for the restarting problem. One can be salvaged from the Intempo board.
5. **Move it into the cabinet.** Female header strips so the boards plug in rather than solder down, stripboard for the shared signal wires, and a proper power socket on the back so nobody is pushing a USB plug into the dev board.

---

## 8. Getting help

Anyone who can program will understand this project from `README.md` in about ten minutes. The wiring is all documented there.

If nobody is around, paste `README.md` into an AI assistant such as Claude or ChatGPT and ask your question. It has enough context to give a useful answer, including the board's limits so it will not suggest things that have already been ruled out.

---

Huawei Huang, September 2026
