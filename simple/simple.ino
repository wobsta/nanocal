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

int number1, number2, result;
byte current_number;
char op;

void setup()
{
  setup_nanocal();

  lcd.print("Hello, world!");
  delay(3000);

  lcd.clear();
  lcd.cursor();
  number1 = 0;
  current_number = 1;
}

void loop()
{
  char key;
  byte digit;
  key = keypad.getKey();
  if (key) beep_key();
  switch (key) {
    case '\0':
      break;
    case 'A':
    case 'B':
    case 'C':
    case 'D':
      switch (key) {
        case 'A':
          op = '+';
          break;
        case 'B':
          op = '-';
          break;
        case 'C':
          op = '*';
          break;
        case 'D':
          op = '/';
          break;
      }
      lcd.print(op);
      number2 = 0;
      current_number = 2;
      break;
    case '*':
      lcd.clear();
      lcd.cursor();
      number1 = 0;
      current_number = 1;
      break;
    case '#':
      lcd.print('=');
      switch (op)
      {
        case '+':
          result = number1 + number2;
          break;
        case '-':
          result = number1 - number2;
          break;
        case '*':
          result = number1 * number2;
          break;
        case '/':
          result = number1 / number2;
          break;
      }
      lcd.setCursor(0, 1);
      lcd.print(result);
      lcd.noCursor();
      break;
    default:
      lcd.print(key);
      digit = (int)key-48;
      if (current_number == 1) {
          number1 = 10*number1 + digit;
      } else {
          number2 = 10*number2 + digit;
      }
  }
}
