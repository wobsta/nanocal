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

#define NAME_SETTINGS "Einstellungen"

#define NAME_KEY_TONE "Tastenton"
#define NAME_ERROR_TONE "Fehlerton"
#define NAME_GREETING "Willkommenstext"

const char* menu = "1:" NAME_KEY_TONE
                  " 2:" NAME_ERROR_TONE
                  " 3:" NAME_GREETING
                  " ";

void (*exit_ptr)();
byte state, pos, height, length;
char menu_scroll[17], hold_k;
unsigned long last_millis, hold_t;

#define STATE_MENU 0
#define STATE_KEY_TONE 1
#define STATE_ERROR_TONE 2
#define STATE_GREETING 3

#define ACTION_NONE 0
#define ACTION_PRINT 1
#define ACTION_TONE 2
#define ACTION_PRINT_AND_TONE 3

void setup(void (*use_exit_ptr)())
{
  setup_nanocal();

  state = STATE_MENU;

  last_millis = millis() + 1000;

  strncpy(menu_scroll, menu, 16);
  menu_scroll[16] = 0;

  lcd.setCursor(0, 0);
  lcd.print(NAME_SETTINGS);
  lcd.setCursor(0, 1);
  lcd.print(menu_scroll);

  pos = 0;
  exit_ptr = use_exit_ptr;
}

void setup()
{
  setup(0);
}

void loop()
{
  if (state == STATE_MENU && millis() > last_millis + 500)
  {
    last_millis = millis();
    pos += 1;
    if (!strlen(menu + pos))
      pos = 0;

    strncpy(menu_scroll, menu + pos, 16);
    if (strlen(menu + pos) < 16) {
      strncpy(menu_scroll + strlen(menu + pos), menu, 16 - strlen(menu + pos));
    }
    lcd.setCursor(0, 1);
    lcd.print(menu_scroll);
  }

  char key;
  byte action = ACTION_NONE;
  key = keypad.getKey();
  if (keypad.getState() == HOLD && hold_t && ((millis() - hold_t) > 100))
    key = hold_k;
  if (key) {
    hold_t = 0;
    switch (state) {
      case STATE_MENU:
        switch (key) {
          case '1':
            state = STATE_KEY_TONE;
            height = EEPROM.read(0);
            length = EEPROM.read(1);
          case '2':
            if (key == '2') {
              state = STATE_ERROR_TONE;
              height = EEPROM.read(2);
              length = EEPROM.read(3);
            }
            beep_key();
            lcd.clear();
            lcd.setCursor(0, 0);
            if (key == '1')
              lcd.print(NAME_KEY_TONE);
            else
              lcd.print(NAME_ERROR_TONE);
            action = ACTION_PRINT;
            break;
          case '3':
            beep_key();
            state = STATE_GREETING;
            for(int pos=0; pos < 16; pos++)
              menu_scroll[pos] = EEPROM.read(pos+4);

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(NAME_GREETING);
            lcd.setCursor(0, 1);
            for(int pos=0; pos < 16; pos++) {
              if ((byte)menu_scroll[pos] > 127) {
                lcd.write((byte)menu_scroll[pos]-125);
              } else {
                lcd.print(menu_scroll[pos]);
              }
            }

            pos = 0;
            lcd.setCursor(pos, 1);
            lcd.blink();
            break;
          case '*':
            if (exit_ptr) {
              beep_key();
              exit_ptr();
              break;
            }
          default:
            beep_error();
        }
        if (key != '1' && key != '2')
          break;
      case STATE_KEY_TONE:
      case STATE_ERROR_TONE:
        switch (key) {
          case 'A':
            if (height < MAX_HEIGHT) {
              height++;
              action = ACTION_PRINT_AND_TONE;
            } else
              beep_error();
            break;
          case 'B':
            if (height) {
              height--;
              action = ACTION_PRINT_AND_TONE;
            } else
              beep_error();
            break;
          case 'C':
            if (length < MAX_LENGTH) {
              length++;
              action = ACTION_PRINT_AND_TONE;
            } else
              beep_error();
            break;
          case 'D':
            if (length) {
              length--;
              action = ACTION_PRINT_AND_TONE;
            } else
              beep_error();
            break;
          case '#':
            if (state == STATE_KEY_TONE) {
              EEPROM.update(0, height);
              EEPROM.update(1, length);
            } else {
              EEPROM.update(2, height);
              EEPROM.update(3, length);
            }
          case '*':
            beep_key();
            setup(exit_ptr);
            break;
          case '0':
            action = ACTION_TONE;
            break;
          default:
            if (key && !action)
              beep_error();
        }
        if (action) {
          int freq = height2freq(height);
          int duration = length2duration(length);
          if (action & ACTION_PRINT) {
            lcd.setCursor(0, 1);
            if (freq < 1000)
              lcd.print(' ');
            lcd.print(freq);
            lcd.print(" Hz   ");
            if (duration < 100)
              lcd.print(' ');
            if (duration < 10)
              lcd.print(' ');
            lcd.print(duration);
            lcd.print(" ms");
          }
          if ((action & ACTION_TONE) && duration)
            tone(10, freq, duration);
        }
        break;
      case STATE_GREETING:
        hold_k = key;
        hold_t = millis();
        switch (key) {
          case 'A':
            if ((byte)menu_scroll[pos] < 130) {
              beep_key();
              menu_scroll[pos]++;
              if ((byte)menu_scroll[pos] > 127) {
                lcd.write((byte)menu_scroll[pos]-125);
              } else {
                lcd.print(menu_scroll[pos]);
              }
              lcd.setCursor(pos, 1);
            } else
              beep_error();
            break;
          case 'B':
            if ((byte)menu_scroll[pos] > 32) {
              beep_key();
              menu_scroll[pos]--;
              if ((byte)menu_scroll[pos] > 127) {
                lcd.write((byte)menu_scroll[pos]-125);
              } else {
                lcd.print(menu_scroll[pos]);
              }
              lcd.setCursor(pos, 1);
            } else
              beep_error();
            break;
          case 'C':
            if (pos < 15) {
              beep_key();
              pos++;
              lcd.setCursor(pos, 1);
            } else
              beep_error();
            break;
          case 'D':
            if (pos) {
              beep_key();
              pos--;
              lcd.setCursor(pos, 1);
            } else
              beep_error();
            break;
          case '#':
            for(int pos=0; pos < 16; pos++)
              EEPROM.update(pos+4, menu_scroll[pos]);
          case '*':
            beep_key();
            lcd.noBlink();
            lcd.clear();
            setup(exit_ptr);
            break;
          default:
            beep_error();
        }
        break;
    }
  }
}

