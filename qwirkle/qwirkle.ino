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

#define NAME_QWIRKLE "Qwirkle"

#define MAX_MOVES 120

byte state;

byte players, number, step, back;
byte points[MAX_MOVES];
char* again_msg;

#define STATE_PLAYERS 1
#define STATE_OUTPUT 2
#define STATE_INPUT 3
#define STATE_NUMBER 4

void setup()
{
  setup_nanocal();

  lcd.cursor();

  for(step=0; step<MAX_MOVES; step++)
    points[step]=0;
  step=0;
  back=0;

  lcd.print("Qwirkle");
  lcd.setCursor(0, 1);
  lcd.print("Spieler (2-4)? ");
  state = STATE_PLAYERS;
}

void loop()
{
  char key;
  byte digit;
  switch (state) {
    case STATE_PLAYERS:
      key = keypad.getKey();
      if (key == '2' || key == '3' || key == '4') {
        beep_key();
        players = (int)key-48;
        state = STATE_OUTPUT;
      } else {
        if (key)
          beep_error();
      }
      break;
    case STATE_OUTPUT:
      lcd.clear();
      for(byte player=0; player < players; player++)
      {
        int sum=0;
        for(int i=player; i < MAX_MOVES; i+=players)
          sum += points[i];
        lcd.setCursor(player*4, 0);
        lcd.print(sum);
      }
      lcd.setCursor(((step-back) % players)*4, 1);
      if (back) {
        lcd.print(points[step-back]);
        lcd.setCursor((step-back) % players*4, 1);
        lcd.blink();
        lcd.noCursor();
      }
      state = STATE_INPUT;
    case STATE_INPUT:
    case STATE_NUMBER:
      key = keypad.getKey();
      if (key) {
        beep_key();
      }
      switch (key)
      {
        case '\0':
          break;
        case '*':
          if (step-back > 0) {
            back += 1;
            state = STATE_OUTPUT;
          } else {
            beep_error();
          }
          break;
        case '#':
          if (!back and state == STATE_INPUT) {
            beep_error();
            break;
          }
          if (state == STATE_NUMBER) {
            points[step-back] = number;
          }
          if (back) {
            back -= 1;
            if (!back) {
              lcd.noBlink();
              lcd.cursor();
            }
          } else {
            step += 1;
          }
          state = STATE_OUTPUT;
          break;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
          beep_error();
          break;
        default:
          if (state == STATE_NUMBER && !number) {
            beep_error();
            break;
          }
          if (state == STATE_INPUT) {
            if (step == MAX_MOVES) {
              beep_error();
              break;
            }
            state = STATE_NUMBER;
            number = 0;
            if (back) {
              lcd.print("  ");
              lcd.setCursor((step-back) % players*4, 1);
            }
          }
          digit = (int)key-48;
          if ((number > 9) || (number*10 + digit > 84)) {
            beep_error();
            break;
          }
          lcd.print(key);
          number = number*10 + digit;
      }
  }
}
