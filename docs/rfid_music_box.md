# RFID Music Box — Project Summary

## Overview
A DIY gift for a toddler (age ~2). Inspired by the Toniebox: placing an RFID-tagged card on top of a wooden box triggers audio playback. Each card has unique artwork and plays a specific MP3 file (nursery rhyme, animal sound, song, etc.).

---

## Build Tracks

The project is built in two stages so progress isn't blocked while new parts ship.

| | Track A — Prototype | Track B — Final build |
|---|---|---|
| **Goal** | Validate the concept now with parts on hand | Gift-ready, user-friendly unit |
| **Controller** | Arduino Uno / Nano (ATmega328P) | RP2040-Zero (native USB-C) |
| **Audio** | DFPlayer Mini | MAX98357A I2S amp + microSD (SPI) |
| **Add songs** | Edit SD card on a computer (numbered files) | Plug in USB-C → drag songs onto the box as a USB drive |
| **Card binding** | UID → track table in firmware | 20 pre-programmed cards, fixed UID → ID 1–20 |
| **Charging** | TP4056 (charge only) | Single USB-C, load-sharing charger (IP5306) |
| **Status** | Build now | Design locked, parts on order |

> Track A proves the RFID-read → play → stop loop and the enclosure. Track B is the design that ships as the gift. The two share the RC522, speaker, tags, and wooden box.

---

## Hardware — Track A (Prototype, current parts)

| Component | Details |
|---|---|
| Arduino Nano (CH340, ATMEGA328P) | Prototype microcontroller |
| Arduino Uno R3 (CH340) | Breadboard prototyping |
| RC522 RFID reader module | SPI interface, reads NTAG213 tags |
| DFPlayer Mini (16P) | MP3 playback from microSD card |
| Speaker | 3W, 4 ohm, 40mm round — mounted inside box |
| TP4056 (USB-C) | LiPo battery charging module (charge only) |
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

**Track A (now, current parts):**
1. **Prototype phase** — wire everything on breadboard with Arduino Uno, test code
2. **Card registration** — scan each NTAG213 tag, record UIDs via serial monitor, populate mapping
3. **Audio prep** — source/record MP3 files, name correctly, load onto microSD
4. **Solder up** — replicate circuit with Arduino Nano, mount in wooden box, test the full loop
5. **Enclosure** — drill speaker grille + USB-C port hole in wooden box, mount components, close up

**Track B (when parts arrive):**
6. **Re-platform** — move to RP2040-Zero + MAX98357A I2S amp + microSD (SPI)
7. **USB-MSC** — expose the SD card as a USB drive over the single USB-C port
8. **Pre-program 20 cards** — bind each UID to a fixed ID 1–20 in firmware, print ID labels
9. **Single-port power** — wire RP2040 VBUS into the IP5306 load-sharing charger
10. **Pack the gift** — preload songs, include a printed "how to add a song" card

---

## Notes
- RC522 reads through wood fine — keep lid surface under ~5mm above the reader
- Avoid metal hardware on the top surface directly above the RC522
- Battery connects to TP4056 via PH 2.0 connector — or solder directly
- DFPlayer RX pin needs a 1kΩ resistor inline to avoid damage
- Volume can be set in code via `myDFPlayer.volume(20)` — range 0–30

---

# Track B — Final Build (RP2040 + USB-MSC)

The gift-ready design. Goal: the recipient can add songs with **no app, no code, no UIDs** — plug in USB-C, drag a numbered MP3 onto the box, unplug, tap the matching card.

## Hardware — Track B

| Component | Search term (AliExpress) | ~Price | Notes |
|---|---|---|---|
| RP2040-Zero | `RP2040-Zero` | $2.50–4 | Native **USB-C**, runs USB Mass Storage + audio. The single port. |
| I2S amplifier | `MAX98357A I2S amplifier` | $1–2 | Mono 3W class-D; drives the 4Ω speaker. Replaces the DFPlayer. |
| MicroSD module (SPI) | `Micro SD card module SPI` | $0.40–1 | RP2040 reads card over SPI and shares it over USB. |
| Charger (load-sharing) | `IP5306 module` | $1–2 | Single-port charge + power-path so the MCU can run while charging from a PC. |
| RC522 RFID module | `RC522 RFID module` | $1 | Unchanged from Track A. |
| NTAG213 sticker tags | `NTAG213 sticker` | — | One per card. |
| Speaker, LiPo, box | — | — | Reused from Track A. |

> New spend is roughly **$5–9**. Buy 2× of the cheap bits (RP2040-Zero, MAX98357A, SD module) as spares.

## Single USB-C port — power + data

One port does everything:
- **Wall plug** → charges only.
- **PC** → charges **and** the box appears as a USB drive for dragging songs on.
- Playing audio *while plugged in* is **not** required.

The RP2040-Zero's onboard USB-C is the single port. Its **VBUS (5V)** feeds the IP5306, which charges the LiPo and supplies the MCU's load while plugged in (needed because the RP2040 must run to present the SD drive). When unplugged, the box runs from battery and plays cards.

> ⚠️ Confirm the RP2040-Zero listing exposes a **`5V`/`VBUS` pad** — that's the wire tapped for charging.

## How songs play (ID scheme)

- **20 pre-programmed cards.** You bind each card's UID to a fixed **ID 1–20** once, baked into firmware. The recipient never sees a UID.
- Each card has a **visible printed ID** (1–20).
- **Recipient rule:** name the file with the card's number, e.g. `07 - Old MacDonald.mp3` in `/songs/`.
- Playback: tap card → firmware resolves UID → ID → plays the file in `/songs/` whose name starts with that zero-padded ID (`07`).
- If a card's ID has no matching file → play a gentle "nothing here yet" chime instead of silence.
- Standardize on **two-digit IDs** (`01`–`20`) for clean sorting/matching.

---

# Design Options (decisions log)

Alternatives considered, with the chosen path noted. Kept for reference.

## Card → song binding
| Option | How recipient adds a song | Verdict |
|---|---|---|
| **A. Learn-mode button** | Drag MP3, hold button, tap card → box writes UID→file to SD | Most foolproof; needs a button + persistent mapping |
| **B. Write track onto the tag** | Box programs the filename/number into NTAG213 memory | Elegant, no box state; lost tag = lost binding |
| **C. Fixed ID 1–20, name file by ID** | Drag MP3 named with the card number | **Chosen** — no UIDs, no button, no learn mode for the recipient |

## Loading audio
| Option | Requires | Verdict |
|---|---|---|
| TP4056 USB-C (current) | — | Charge only — **cannot** transfer files |
| Removable microSD slot | Panel-mount microSD extender (~$2–4) | Cheap; keeps Nano, but recipient handles a tiny card |
| **USB-MSC over the controller** | RP2040/ESP32-S3 (native USB) | **Chosen** — drag-and-drop, no card handling |

## USB-C ports
| Option | Requires | Verdict |
|---|---|---|
| Two ports (data + charge) | RP2040 USB-C + separate TP4056 | Simplest wiring, but two ports on the box |
| **Single port** | Load-sharing charger (IP5306) fed from RP2040 VBUS | **Chosen** — one clean port, charge + data |
| Single dedicated breakout port | USB-C breakout + 2× 5.1kΩ CC resistors | Tidiest physically; more soldering |
