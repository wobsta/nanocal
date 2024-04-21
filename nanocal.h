// This file is part of nanocal.
//
// Copyright (C) 2022-2024 André Wobst <project.nanocal@wobsta.de>
//
// nanocal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// nanocal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with nanocal.  If not, see <http://www.gnu.org/licenses/>.

#ifndef nanocal_h
#define nanocal_h

#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

const byte COLS = 4;
const byte ROWS = 4;
char hexaKeys[ROWS][COLS]={
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};
byte colPins[COLS] = {5, 4, 3, 2};
byte rowPins[ROWS] = {9, 8, 7, 6};
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
byte first=1;

LiquidCrystal_I2C lcd(0x27, 16, 2);

byte charNEG[] = {
  B00000,
  B00000,
  B00000,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000
};
byte charMUL[] = {
  B00000,
  B00000,
  B00000,
  B00100,
  B00000,
  B00000,
  B00000,
  B00000
};
byte charDIV[] = {
  B00000,
  B00100,
  B00000,
  B11111,
  B00000,
  B00100,
  B00000,
  B00000
};
byte charAE[] = {
  B01010,
  B00000,
  B01110,
  B10001,
  B10001,
  B10011,
  B01101,
  B00000
};
byte charOE[] = {
  B01010,
  B00000,
  B01110,
  B10001,
  B10001,
  B10001,
  B01110,
  B00000
};
byte charUE[] = {
  B01010,
  B00000,
  B10001,
  B10001,
  B10001,
  B10011,
  B01101,
  B00000
};
byte charLOW[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111
};
byte charFULL[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

#define GREETING "Hallo nanocal.de"

void setup_nanocal()
{
  byte i, b;
  if (first) {
    for(i=0; i<16; i++) {
      // test for invalid data
      b = EEPROM.read(i+4);
      if (b < 32 || b > 130)
        i = 254;
    }
    if (i == 255) {
      // fill defaults
      EEPROM.update(i+4, (byte)GREETING[i]);
      EEPROM.update(0, 70);
      EEPROM.update(1, 5);
      EEPROM.update(2, 60);
      EEPROM.update(3, 15);
      for(i=0; i<16; i++)
        if (i < strlen(GREETING))
          EEPROM.update(i+4, (byte)GREETING[i]);
        else
          EEPROM.update(i+4, (byte)' ');
    }

    lcd.init();
    lcd.createChar(0, charNEG);
    lcd.createChar(1, charMUL);
    lcd.createChar(2, charDIV);
    lcd.createChar(3, charAE);
    lcd.createChar(4, charOE);
    lcd.createChar(5, charUE);
    lcd.createChar(6, charFULL);
    lcd.createChar(7, charLOW);
    lcd.clear();
    lcd.backlight();
  }
  first = 0;
}

#define SPK_PIN 10
#define MAX_HEIGHT 110
#define MAX_LENGTH 21

int height2freq(byte height)
{
  int i, freq=100;
  for(i=10; i && height; i--, height--)
    freq += 10;
  for(i=40; i && height; i--, height--)
    freq += 20;
  for(i=40; i && height; i--, height--)
    freq += 50;
  for(i=10; i && height; i--, height--)
    freq += 100;
  for(i=10; i && height; i--, height--)
    freq += 200;
  return freq;
}

int length2duration(byte length)
{
  int i, duration=0;
  for(i=10; i && length; i--, length--)
    duration += 10;
  for(i=5; i && length; i--, length--)
    duration += 20;
  for(i=6; i && length; i--, length--)
    duration += 50;
  return duration;
}

void beep_key()
{
  byte length = EEPROM.read(1);
  if (length)
    tone(SPK_PIN, height2freq(EEPROM.read(0)), length2duration(length));
}

void beep_error()
{
  byte length = EEPROM.read(3);
  if (length)
    tone(SPK_PIN, height2freq(EEPROM.read(2)), length2duration(length));
}

#endif
