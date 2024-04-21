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

#define NAME_CALC "Rechner"

byte state, neg, overflow;
long number1, number2, result;
char op;

#define STATE_BEGIN 1
#define STATE_NUMBER1 2
#define STATE_OP 3
#define STATE_NUMBER2 4
#define STATE_RESULT 5
#define STATE_ERROR 6
#define STATE_ERROR_DONE 7

void setup()
{
  setup_nanocal();
  lcd.cursor();
  state = STATE_BEGIN;
  neg = 0;
  overflow = 0;
}

void negprint(long x)
{
  if (x < 0)
  {
    lcd.print('\0');
    if (x == INT32_MIN)
      lcd.print("2147483648");
    else
      lcd.print(-x);
  } else {
    lcd.print(x);
  }
}

void loop()
{
  char key;
  byte digit;
  if (state == STATE_ERROR || overflow) 
  {
    beep_error();
    lcd.noCursor();
    lcd.setCursor(5, 1);
    for(digit=5; digit<16; digit++) lcd.print(" ");
    lcd.setCursor(0, 1);
    if (overflow)
      lcd.print("overflow");
    else
      lcd.print("error");
    overflow = 0;
    state = STATE_ERROR_DONE;
  }
  key = keypad.getKey();
  if (state == STATE_ERROR_DONE && key != '*')
  {
    if (key)
      beep_error();
    return;
  } else
    if (key)
      beep_key();
  switch (key)
  {
    case '\0':
      break;
    case 'A':
    case 'B':
    case 'C':
    case 'D':
      if (key == 'B' && (state == STATE_BEGIN || state == STATE_OP))
      {
        lcd.print('\0');
        if (neg) {
          state = STATE_ERROR;
          return;
        }
        neg = 1;
        return;
      }
      if (state == STATE_RESULT)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        state = STATE_NUMBER1;
        number1 = result;
        negprint(number1);
        lcd.cursor();
      }
      switch (key)
      {
        case 'A':
          lcd.print('+');
          op = '+';
          break;
        case 'B':
          lcd.print('-');
          op = '-';
          break;
        case 'C':
          lcd.print('\1');
          op = '*';
          break;
        case 'D':
          lcd.print('\2');
          op = '/';
          break;
      }
      if (state != STATE_NUMBER1)
      {
        state = STATE_ERROR;
        return;
      }
      state = STATE_OP;
      neg = 0;
      break;
    case '*':
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.cursor();
      state = STATE_BEGIN;
      neg = 0;
      break;
    case '#':
      lcd.print('=');
      if (state != STATE_NUMBER2)
      {
        state = STATE_ERROR;
        return;
      }
      switch (op)
      {
        case '+':
          overflow = __builtin_saddl_overflow(number1, number2, &result);
          break;
        case '-':
          overflow = __builtin_ssubl_overflow(number1, number2, &result);
          break;
        case '*':
          overflow = __builtin_smull_overflow(number1, number2, &result);
          break;
        case '/':
          if (!number2)
          {
            state = STATE_ERROR;
            return;
          }
          result = number1 / number2;
          break;
      }
      if (overflow)
        return;
      lcd.setCursor(0, 1);
      negprint(result);
      if (op == '/') {
        long remainder = number1 - number2 * result;
        if (remainder) {
          lcd.print(" Rest ");
          negprint(remainder);
        }
      }
      lcd.noCursor();
      state = STATE_RESULT;
      break;
    default:
      lcd.print(key);
      if (state == STATE_BEGIN)
      {
        state = STATE_NUMBER1;
        number1 = 0;
      }
      if (state == STATE_OP)
      {
        state = STATE_NUMBER2;
        number2 = 0;
      }
      switch (state)
      {
        case STATE_NUMBER1:
          overflow = __builtin_smull_overflow(number1, 10, &number1);
          break;
        case STATE_NUMBER2:
          overflow = __builtin_smull_overflow(number2, 10, &number2);
          break;
        default:
          state = STATE_ERROR;
          return;
      }
      if (overflow)
        return;
      digit = (int)key-48;
      if (state == STATE_NUMBER1)
        if (neg)
          overflow = __builtin_ssubl_overflow(number1, digit, &number1);
        else
          overflow = __builtin_saddl_overflow(number1, digit, &number1);
      else
        if (neg)
          overflow = __builtin_ssubl_overflow(number2, digit, &number2);
        else
          overflow = __builtin_saddl_overflow(number2, digit, &number2);
  }
}

