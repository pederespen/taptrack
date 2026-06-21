// RFID Music Box (taptrack) — main firmware
//
// Places an RFID-tagged card on the box -> looks up the tag UID -> plays the
// matching MP3 from the DFPlayer Mini (SD folder "01"). Removing the card
// stops playback.
//
// Wiring (see docs/rfid_music_box.md):
//   RC522:  SDA=D10  SCK=D13  MOSI=D11  MISO=D12  RST=D9  (3.3V !)
//   DFPlayer: Arduino D2 -> DFPlayer RX (via 1k resistor), DFPlayer TX -> D3

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#include "cards.h"

// ---- Pin configuration ----
constexpr uint8_t RC522_SS_PIN = 10;
constexpr uint8_t RC522_RST_PIN = 9;
constexpr uint8_t DF_RX_PIN = 3;  // Arduino RX  <- DFPlayer TX
constexpr uint8_t DF_TX_PIN = 2;  // Arduino TX  -> DFPlayer RX (1k resistor inline)

// ---- Behaviour tuning ----
constexpr uint8_t SD_FOLDER = 1;          // files live in /01
constexpr uint8_t VOLUME = 20;            // 0..30
constexpr uint16_t POLL_INTERVAL_MS = 150;
constexpr uint8_t ABSENT_THRESHOLD = 3;   // consecutive misses before "removed"

MFRC522 rfid(RC522_SS_PIN, RC522_RST_PIN);
SoftwareSerial dfSerial(DF_RX_PIN, DF_TX_PIN);
DFRobotDFPlayerMini dfplayer;

String currentUid = "";
uint8_t absentCount = 0;

String uidToString(const MFRC522::Uid &uid) {
  String s;
  for (uint8_t i = 0; i < uid.size; i++) {
    if (i) s += ' ';
    if (uid.uidByte[i] < 0x10) s += '0';
    s += String(uid.uidByte[i], HEX);
  }
  s.toUpperCase();
  return s;
}

int trackForUid(const String &uid) {
  for (const CardMap &c : CARDS) {
    if (uid == c.uid) return c.track;
  }
  return -1;
}

// Detects a card whether it was just placed or is still resting on the reader.
// Returns true and fills `uid` when a card is in the field.
bool pollCard(String &uid) {
  byte atqa[2];
  byte atqaLen = sizeof(atqa);
  MFRC522::StatusCode rc = rfid.PICC_WakeupA(atqa, &atqaLen);
  if (rc != MFRC522::STATUS_OK && rc != MFRC522::STATUS_COLLISION) {
    return false;
  }
  if (!rfid.PICC_ReadCardSerial()) {
    return false;
  }
  uid = uidToString(rfid.uid);
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  return true;
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  dfSerial.begin(9600);
  if (!dfplayer.begin(dfSerial)) {
    Serial.println(F("DFPlayer init failed - check wiring and SD card."));
    while (true) {
      delay(1000);
    }
  }
  dfplayer.volume(VOLUME);

  Serial.println(F("Ready. Place a card on the box."));
}

void loop() {
  static uint32_t lastPoll = 0;
  if (millis() - lastPoll < POLL_INTERVAL_MS) return;
  lastPoll = millis();

  String uid;
  if (pollCard(uid)) {
    absentCount = 0;
    if (uid != currentUid) {
      currentUid = uid;
      int track = trackForUid(uid);
      if (track > 0) {
        Serial.print(F("Card "));
        Serial.print(uid);
        Serial.print(F(" -> track "));
        Serial.println(track);
        dfplayer.playFolder(SD_FOLDER, track);
      } else {
        Serial.print(F("Unknown card: "));
        Serial.println(uid);
      }
    }
  } else if (currentUid.length()) {
    if (++absentCount >= ABSENT_THRESHOLD) {
      Serial.println(F("Card removed -> stop"));
      dfplayer.stop();
      currentUid = "";
      absentCount = 0;
    }
  }
}
