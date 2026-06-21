# taptrack

An RFID music box for a toddler — inspired by the Toniebox. Placing an
RFID-tagged card on a wooden box plays a specific MP3 (nursery rhyme, animal
sound, song). Built with an Arduino Nano, an RC522 RFID reader, and a DFPlayer
Mini.

Full hardware, wiring, and enclosure notes: [docs/rfid_music_box.md](docs/rfid_music_box.md).

## Project layout

```
platformio.ini      build environments (nano / uno / uid_scanner)
include/cards.h      UID -> track mapping table
src/main.cpp         main firmware
src/uid_scanner.cpp  utility sketch to read tag UIDs
docs/                project summary
```

## Build & flash

Requires [PlatformIO](https://platformio.org/).

```bash
# Final build (Arduino Nano)
pio run -e nano -t upload

# Breadboard prototype (Arduino Uno)
pio run -e uno -t upload

# Read tag UIDs, then copy them into include/cards.h
pio run -e uid_scanner -t upload
pio device monitor
```

## Registering cards

1. Flash the `uid_scanner` environment and open the serial monitor.
2. Tap each card; copy the printed `UID:` value.
3. Add an entry to `CARDS[]` in [include/cards.h](include/cards.h) with the UID
   and its track number.
4. Re-flash the `nano` (or `uno`) environment.

## SD card

Format FAT32, create a folder `01`, and name files `001.mp3`, `002.mp3`, … —
track `N` in the mapping plays `/01/00N.mp3`.
