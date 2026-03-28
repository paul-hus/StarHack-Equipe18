#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

const byte RFID_SS_PIN = 53;  // RC522 SDA/SS -> Mega 2560 pin 53
const byte RFID_RST_PIN = 5;   // RC522 RST   -> Mega 2560 pin 5

const byte LCD_RS = 7;
const byte LCD_EN = 8;
const byte LCD_D4 = 9;
const byte LCD_D5 = 10;
const byte LCD_D6 = 11;
const byte LCD_D7 = 12;

const byte AUTH_UID[] = {0xF0, 0x0D, 0x15, 0x5F};
const byte AUTH_UID_LEN = sizeof(AUTH_UID) / sizeof(AUTH_UID[0]);

MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void showMessage(const char *line1, const char *line2 = "") {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

bool isAuthorizedCard() {
  if (rfid.PICC_GetType(rfid.uid.sak) != MFRC522::PICC_TYPE_MIFARE_1K) {
    return false;
  }

  if (rfid.uid.size != AUTH_UID_LEN) {
    return false;
  }

  for (byte i = 0; i < AUTH_UID_LEN; i++) {
    if (rfid.uid.uidByte[i] != AUTH_UID[i]) {
      return false;
    }
  }

  return true;
}

void setup() {
  pinMode(RFID_SS_PIN, OUTPUT);
  SPI.begin();
  rfid.PCD_Init();

  lcd.begin(16, 2);
  showMessage("RFID ready", "Tap a card");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  if (isAuthorizedCard()) {
    showMessage("Access granted", "Welcome");
  } else {
    showMessage("Access denied", "Try again");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(1000);
}
