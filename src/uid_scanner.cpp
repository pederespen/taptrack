// UID scanner — flash with: pio run -e uid_scanner -t upload
//
// Tap each NTAG213 card on the RC522 and copy the printed UID into
// include/cards.h. Only the RC522 needs to be wired for this.

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RC522_SS_PIN = 10;
constexpr uint8_t RC522_RST_PIN = 9;

MFRC522 rfid(RC522_SS_PIN, RC522_RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println(F("UID scanner ready. Tap a card..."));
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  String s;
  for (uint8_t i = 0; i < rfid.uid.size; i++) {
    if (i) s += ' ';
    if (rfid.uid.uidByte[i] < 0x10) s += '0';
    s += String(rfid.uid.uidByte[i], HEX);
  }
  s.toUpperCase();

  Serial.print(F("UID: "));
  Serial.println(s);

  rfid.PICC_HaltA();
  delay(800);
}
