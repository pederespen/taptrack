#pragma once
#include <Arduino.h>

// Maps an RFID tag UID to a track number inside SD folder "01".
// Track N plays /01/00N.mp3 (e.g. track 1 -> /01/001.mp3).
struct CardMap {
  const char *uid;  // space-separated, uppercase hex, e.g. "AB CD EF 01"
  int track;        // file number within folder 01
};

// Populate this table using the `uid_scanner` environment:
//   1. pio run -e uid_scanner -t upload
//   2. pio device monitor
//   3. Tap each card, copy the printed UID below, set its track.
const CardMap CARDS[] = {
    {"AB CD EF 01", 1},  // Old MacDonald
    {"AB CD EF 02", 2},  // Twinkle Twinkle
    {"AB CD EF 03", 3},  // Animal sounds
    // add more cards here
};
