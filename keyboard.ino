#include <Keyboard.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <SPI.h>

#define buzzer 9
#define first 8
#define second 7
#define third 6
#define fourth 5

#define SS 10
#define RST 4

LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(SS, RST);

String lastCard = "";
int total = 0;
float finalTotal = 0;
int charsSent = 0;

const char* bu1 = "This ";
const char* bu2 = "Keyboard ";
const char* bu3 = "Is ";
const char* bu4 = "GREAT!!! ";

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();

  SPI.begin();
  rfid.PCD_Init();

  pinMode(buzzer, OUTPUT);
  pinMode(first, INPUT_PULLUP);
  pinMode(second, INPUT_PULLUP);
  pinMode(third, INPUT_PULLUP);
  pinMode(fourth, INPUT_PULLUP);

  Keyboard.begin();

  lcd.setCursor(0, 0);
  lcd.print("Please scan card");
  lcd.setCursor(0, 1);
  lcd.print("AMEX or VISA");
}

void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    playBuzzer();
    String currentCard = uidToString(&rfid.uid);
    lastCard = currentCard;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card declined.");
    delay(3000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Please scan card");
    lcd.setCursor(0, 1);
    lcd.print("AMEX or VISA");

    while (true) {
      if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        playBuzzer();
        String newCard = uidToString(&rfid.uid);

        lcd.clear();
        if (newCard == lastCard) {
          lcd.print("Card declined.");
          lcd.setCursor(0, 1);
          lcd.print("Nice try, broke.");
          delay(3000);
        } else {
          lcd.print("Card accepted!");
          delay(2000);
          total = 0;
          selectAmount();
        }

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Please scan card");
        lcd.setCursor(0, 1);
        lcd.print("AMEX or VISA");
        lastCard = "";
        break;
      }
      delay(50);
    }
  }
}

void selectAmount() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select amount:");
  lcd.setCursor(0, 1);
  lcd.print("1 2 5 10 (buttons)");

  while (true) {
    if (buttonPressed(first)) {
      total += 1;
      updateTotal();
    }
    if (buttonPressed(second)) {
      total += 2;
      updateTotal();
    }
    if (buttonPressed(third)) {
      total += 5;
      updateTotal();
    }
    if (buttonPressed(fourth)) {
      total += 10;
      updateTotal();
    }

    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
      playBuzzer();
      askForTip();
      break;
    }

    delay(100);
  }
}

void askForTip() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Add a tip?");
  lcd.setCursor(0, 1);
  lcd.print("0 5 10 50%");

  while (true) {
    if (buttonPressed(first)) {
      playBuzzer();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sorry, I can't");
      lcd.setCursor(0, 1);
      lcd.print("help you.");
      delay(3000);
      return;
    }
    if (buttonPressed(second)) {
      finalTotal = total * 1.05;
      if (checkBalance(5)) return;
      break;
    }
    if (buttonPressed(third)) {
      finalTotal = total * 1.10;
      if (checkBalance(10)) return;
      break;
    }
    if (buttonPressed(fourth)) {
      finalTotal = total * 1.50;
      if (checkBalance(50)) return;
      break;
    }

    delay(100);
  }

  showFinalTotalAndProceed();
}

bool checkBalance(int tipPercent) {
  if (finalTotal > 110.0) {
    playBuzzer();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Insufficient funds");
    lcd.setCursor(0, 1);
    lcd.print("LOL");
    delay(4000);
    return true;
  }
  return false;
}

void showFinalTotalAndProceed() {
  playBuzzer();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tip added.");
  lcd.setCursor(0, 1);
  lcd.print("Total: $");
  lcd.print(finalTotal, 2);
  delay(3000);

  expensiveKeyboardMode();
}

void expensiveKeyboardMode() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press buttons");
  lcd.setCursor(0, 1);
  lcd.print("to send");

  charsSent = 0;
  unsigned long start = millis();

  while (millis() - start < 20000UL) {
    if (buttonPressed(first)) {
      if (!sendWordLimited(bu1)) return;
    }
    if (buttonPressed(second)) {
      if (!sendWordLimited(bu2)) return;
    }
    if (buttonPressed(third)) {
      if (!sendWordLimited(bu3)) return;
    }
    if (buttonPressed(fourth)) {
      if (!sendWordLimited(bu4)) return;
    }

    delay(50);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Please scan card");
  lcd.setCursor(0, 1);
  lcd.print("AMEX or VISA");
}

bool sendWordLimited(const char *word) {
  int wordLen = strlen(word) + 1;

  if (charsSent + wordLen > total) {
    playBuzzer();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Limit exceeded!");
    lcd.setCursor(0, 1);
    lcd.print("Deleting text...");

    for (int i = 0; i < 1000; i++) {
      Keyboard.write(KEY_BACKSPACE);
      delay(10);
    }

    delay(2000);
    return false;
  }

  playBuzzer();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sent:");
  lcd.setCursor(0, 1);
  lcd.print(word);

  Keyboard.print(word);
  Keyboard.print(" ");
  charsSent += wordLen;

  delay(300);
  while (digitalRead(first) == LOW || digitalRead(second) == LOW || digitalRead(third) == LOW || digitalRead(fourth) == LOW) {
    delay(10);
  }
  delay(150);
  return true;
}

bool buttonPressed(uint8_t pin) {
  if (digitalRead(pin) == LOW) {
    delay(20);
    if (digitalRead(pin) == LOW) {
      while (digitalRead(pin) == LOW) delay(10);
      delay(20);
      return true;
    }
  }
  return false;
}

void updateTotal() {
  playBuzzer();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Total: $");
  lcd.print(total);
  lcd.setCursor(0, 1);
  lcd.print("Add more or scan");
}

String uidToString(MFRC522::Uid *uid) {
  String s = "";
  for (byte i = 0; i < uid->size; i++) {
    if (uid->uidByte[i] < 0x10) s += "0";
    s += String(uid->uidByte[i], HEX);
  }
  s.toUpperCase();
  return s;
}

void playBuzzer() {
  tone(buzzer, 2500);
  delay(200);
  noTone(buzzer);
}
