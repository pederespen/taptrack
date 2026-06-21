# RFID Music Box — Project Summary

## Overview
A DIY gift for a toddler (age ~2). Inspired by the Toniebox: placing an RFID-tagged card on top of a wooden box triggers audio playback. Each card has unique artwork and plays a specific MP3 file (nursery rhyme, animal sound, song, etc.).

The build uses **a fixed set of ~20 pre-programmed cards** (IDs 1–20). Each card's UID is hardcoded to an ID in firmware. Songs live on the microSD card named by ID, so the recipient can **swap songs by editing files on the SD card** — no code, no app, no re-flashing. Songs aren't expected to change often.

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
| NTAG213 sticker tags | One per card (~20), 13.56MHz, compatible with RC522 |
| Blank PVC cards (CR80) | Card backing — sticker tag on back, printed artwork + ID on front |

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

## MicroSD Card Setup & Card Scheme

**~20 fixed cards, IDs 1–20.** Each card's UID is hardcoded to an ID in firmware (done once by you). Each card gets a **visible printed ID** (1–20). The DFPlayer plays files by number, so the ID *is* the track number.

- Format the card as **FAT32**
- Create folder `01` in the root
- Name files by ID, zero-padded to three digits: `001.mp3` (card 1), `002.mp3` (card 2), … `020.mp3` (card 20)
- Tap card with ID `N` → plays `/01/00N.mp3`

**Swapping a song (recipient):** pop the microSD into a computer, replace the file for that ID (e.g. overwrite `007.mp3` with a new song, keeping the same name), put the card back. The card-to-slot binding never changes — only the audio file does.

> If a card's ID has no matching file, the box stays silent (or plays a short "empty slot" cue if added in firmware).

## Firmware (PlatformIO + VS Code)

### Libraries needed
- `MFRC522` by GithubCommunity — RFID reader
- `DFRobotDFPlayerMini` by DFRobot — MP3 player

### Core logic
1. On startup, initialise RC522 and DFPlayer
2. In main loop, poll for RFID tag presence
3. When tag detected, read UID
4. Look up UID in the fixed card table → get ID (1–20)
5. Play that track number on the DFPlayer (`/01/0NN.mp3`)
6. When card removed, stop playback

### Card mapping (fixed 20 cards)
UIDs are bound to a permanent ID once. The recipient never edits this — they only swap the matching MP3 on the SD card.

```cpp
struct CardMap {
  String uid;
  int id;  // 1..20, also the track number -> /01/0{id}.mp3
};

CardMap cards[] = {
  {"AB CD EF 01", 1},  // Card 1
  {"AB CD EF 02", 2},  // Card 2
  {"AB CD EF 03", 3},  // Card 3
  // ... up to 20
};
```

> UIDs are read from the serial monitor during setup — scan each of the 20 cards, note the UID, and assign it the next ID. Print a matching ID label on each card's artwork.

---

## Development Workflow

1. **Prototype phase** — wire everything on breadboard with Arduino Uno, test code
2. **Card registration** — scan all ~20 NTAG213 tags, record UIDs via serial monitor, assign each a fixed ID 1–20
3. **Label cards** — print artwork with a visible ID (1–20) on each card
4. **Audio prep** — source/record MP3 files, name by ID (`001.mp3`…`020.mp3`), load into folder `01` on the microSD
5. **Final build** — replicate circuit with Arduino Nano, solder connections, mount in wooden box
6. **Enclosure** — drill speaker grille + USB-C port hole in wooden box, mount components, close up
7. **Gift extras** — preload songs and include a short printed note: "to change a song, swap the numbered file on the SD card"

---

## Notes
- RC522 reads through wood fine — keep lid surface under ~5mm above the reader
- Avoid metal hardware on the top surface directly above the RC522
- Battery connects to TP4056 via PH 2.0 connector — or solder directly
- DFPlayer RX pin needs a 1kΩ resistor inline to avoid damage
- Volume can be set in code via `myDFPlayer.volume(20)` — range 0–30

---

# Design Options (decisions log)

Alternatives considered, with the chosen path noted. Kept for reference.

## Card → song binding
| Option | How songs are managed | Verdict |
|---|---|---|
| Learn-mode button | Drag MP3, hold button, tap card → box writes UID→file to SD | Foolproof but needs a button + persistent mapping; overkill |
| Write track onto the tag | Box programs the filename/number into NTAG213 memory | Elegant, no box state; lost tag = lost binding |
| **Fixed ID 1–20, file named by ID** | Hardcode UID→ID once; recipient swaps the numbered MP3 on the SD card | **Chosen** — simplest; no UIDs, no button, no re-flashing for the recipient |

## Loading / swapping audio
| Option | Requires | Verdict |
|---|---|---|
| **Edit microSD on a computer** | Current parts only | **Chosen** — songs rarely change; recipient overwrites a numbered file |
| Removable microSD slot | Panel-mount microSD extender (~$2–4) | Optional nicety so the card is easy to reach without opening the box |
| USB-MSC over the controller | RP2040/ESP32-S3 (native USB) + I2S amp | Rejected — drag-and-drop is nicer but needs a controller + audio re-platform |

## Controller / audio platform
| Option | Notes | Verdict |
|---|---|---|
| **Arduino Nano + DFPlayer Mini** | Current parts; plays SD files by number | **Chosen** — matches the fixed-ID scheme, no new hardware |
| RP2040-Zero + MAX98357A I2S | Native USB-C, SD-as-USB-drive | Rejected for now — more parts/cost; revisit only if drag-and-drop becomes a must |
