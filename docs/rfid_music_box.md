# RFID Music Box — Project Summary

## Overview
A DIY gift for a toddler (age ~2). Inspired by the Toniebox: placing an RFID-tagged card on top of a wooden box triggers audio playback. Each card has unique artwork and plays a specific MP3 file (nursery rhyme, animal sound, song, etc.).

---

## Hardware

| Component | Details |
|---|---|
| Arduino Nano (CH340, ATMEGA328P) | Final build microcontroller — lives inside the box |
| Arduino Uno R3 (CH340) | Prototyping only — not in final build |
| RC522 RFID reader module | SPI interface, reads NTAG213 tags |
| DFPlayer Mini (16P) | MP3 playback from microSD card |
| Speaker | 3W, 4 ohm, 40mm round — mounted inside box |
| TP4056 (USB-C) | LiPo battery charging module |
| LiPo battery | 3.7V, 1200mAh, 103040 form factor, PH 2.0 connector |
| MicroSD card | FAT32 formatted, stores MP3 files |
| NTAG213 sticker tags | One per card, 13.56MHz, compatible with RC522 |
| Blank PVC cards (CR80) | Card backing — sticker tag on back, printed artwork on front |

---

## Enclosure
- **Wooden box** — approx 80x80x50mm or similar craft store box (e.g. from Panduro)
- Hinged lid with small magnetic clasp on the front
- Speaker grille hole drilled/routed into the top or front face
- USB-C port hole on the side (for charging)
- RC522 mounted just inside the lid surface — reads cards placed on top
- All components mounted inside, accessible by opening the lid

> Note: No 3D printing involved. Wooden enclosure chosen for aesthetic and simplicity.

---

## Wiring (Prototype — Arduino Uno + Breadboard)

### RC522 → Arduino
| RC522 Pin | Arduino Pin |
|---|---|
| SDA | D10 |
| SCK | D13 |
| MOSI | D11 |
| MISO | D12 |
| GND | GND |
| RST | D9 |
| 3.3V | 3.3V |

> ⚠️ RC522 runs on 3.3V — do not connect to 5V

### DFPlayer Mini → Arduino
| DFPlayer Pin | Arduino Pin |
|---|---|
| VCC | 5V |
| GND | GND |
| RX | D2 (via 1kΩ resistor) |
| TX | D3 |

### Speaker → DFPlayer Mini
| Speaker | DFPlayer Pin |
|---|---|
| + | SPK_1 |
| - | SPK_2 |

---

## MicroSD Card Setup
- Format as **FAT32**
- Create folder `01` in root
- Name files `001.mp3`, `002.mp3`, `003.mp3` etc.
- Each file corresponds to one RFID card/tag

---

## Firmware (PlatformIO + VS Code)

### Libraries needed
- `MFRC522` by GithubCommunity — RFID reader
- `DFRobotDFPlayerMini` by DFRobot — MP3 player

### Core logic
1. On startup, initialise RC522 and DFPlayer
2. In main loop, poll for RFID tag presence
3. When tag detected, read UID
4. Look up UID in a mapping table → get track number
5. Send play command to DFPlayer for that track
6. When card removed, optionally pause/stop playback

### Card/track mapping (example)
```cpp
struct CardMap {
  String uid;
  int track;
};

CardMap cards[] = {
  {"AB CD EF 01", 1},  // Old MacDonald
  {"AB CD EF 02", 2},  // Twinkle Twinkle
  {"AB CD EF 03", 3},  // Animal sounds
  // add more cards here
};
```

> UIDs are read from serial monitor during setup — scan each card and note the UID, then populate the mapping table.

---

## Development Workflow

1. **Prototype phase** — wire everything on breadboard with Arduino Uno, test code
2. **Card registration** — scan each NTAG213 tag, record UIDs via serial monitor, populate mapping
3. **Audio prep** — source/record MP3 files, name correctly, load onto microSD
4. **Final build** — replicate circuit with Arduino Nano, solder connections, mount in wooden box
5. **Enclosure** — drill speaker grille + USB-C port hole in wooden box, mount components, close up

---

## Notes
- RC522 reads through wood fine — keep lid surface under ~5mm above the reader
- Avoid metal hardware on the top surface directly above the RC522
- Battery connects to TP4056 via PH 2.0 connector — or solder directly
- DFPlayer RX pin needs a 1kΩ resistor inline to avoid damage
- Volume can be set in code via `myDFPlayer.volume(20)` — range 0–30
