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

#include "nanocal.h"

#define NAME_DICE "W\5rfel"

unsigned long last_millis;
byte error, prevs[5];

#define DICE_TOO_FAST_MSG "zu schnell gedr\5ckt"
#define SCROLL_TIME 500
#define SCROLL_START 2

void setup()
{
  setup_nanocal();

  prevs[0] = 0;
  last_millis = millis() - SCROLL_TIME*SCROLL_START;
  error = 0;
}

void loop()
{
  char key;
  byte i;
  if (error) {
    i = (millis() - last_millis) / SCROLL_TIME;
    if (error < i) {
      lcd.setCursor(0, 1);
      if ((byte)strlen(DICE_TOO_FAST_MSG) + SCROLL_START < 16 + i) {
        for(i=0; i<16; i++)
          lcd.print(' ');
        error = 0;
      } else {
        lcd.print(DICE_TOO_FAST_MSG + i - SCROLL_START);
        error = i;
      }
    }
    key = keypad.getKey();
    if (key) {
      beep_error();
      last_millis = millis();
      lcd.setCursor(0, 1);
      lcd.print(DICE_TOO_FAST_MSG);
      error = 1;
    }
    return;
  }
  key = keypad.getKey();
  if (key) {
    lcd.clear();
    lcd.print("zuvor:");
    for(i=0; i<5 && prevs[i]; i++) {
      lcd.print(" ");
      lcd.print(prevs[i]);
    }
    lcd.setCursor(0, 1);
    if (millis() - last_millis > 1000)
    {
      beep_key();
      last_millis = millis();
      for(i=4; i; i--) {
        prevs[i] = prevs[i-1];
      }
      prevs[0] = 1+(last_millis % 6);
      lcd.print("jetzt: ==> ");
      lcd.print(prevs[0]);
      lcd.print(" <==");
    } else {
      beep_error();
      last_millis = millis();
      lcd.print(DICE_TOO_FAST_MSG);
      error = SCROLL_START;
    }
  }
}
