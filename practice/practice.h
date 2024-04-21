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
#include "exercises.h"

#define NAME_FOUR "4-1er-plus"
#define NAME_ADD  "3er-plus-3er"
#define NAME_SUB  "3er-minus-3er"
#define NAME_101  "1\1""1"
#define NAME_MUL  "2er-mal-1er"
#define NAME_DIV  "3er-durch-1er"

int pos;
unsigned long last_millis, start_millis, error_until;

byte state, flash;
long number1, number2, result;

byte start, step, reverse;
char again_msg[30], *again_msg_ptr;
void (*exit_ptr)();

#define STATE_START 1
#define STATE_OUTPUT 2
#define STATE_INPUT 3
#define STATE_ERROR 4
#define STATE_DONE 5
#define STATE_AGAIN 6
#define STATE_SCROLL 7

// practice output layout (# marks the initial cursor position):
//     column position:      0123456789    mode   start   reverse
//     4-1-digits-add:       1+2+3+4=#_     4       8        0
//     2-3-digits-add:       100+100=__#    +      10        1
//     2-4-digits-sub:       200-100=__#    -      10        1
//     101:                  5x2=#_         1       4        0
//     2-digits-times-digit: 50x2=__#       *       7        1
//     3-digits-by-digit:    100/2=#_       /       6        0
// The number of result digits plus the reverse flag is 3 all the time.

void setup_practice(char mode, void (*use_exit_ptr)())
{
  // start is the output position at step 0
  switch (mode) {
    case '4':
      start = 8;
      break;
    case '+':
    case '-':
      start = 10;
      break;
    case '1':
      start = 4;
      break;
    case '*':
      start = 7;
      break;
    case '/':
      start = 6;
      break;
  }
  reverse = (mode == '+' || mode == '-' || mode == '*');

  strcpy(again_msg, "Noch mal ");
  switch (mode)
  {
    case '4':
      strcat(again_msg, NAME_FOUR);
      break;
    case '+':
      strcat(again_msg, NAME_ADD);
      break;
    case '-':
      strcat(again_msg, NAME_SUB);
      break;
    case '1':
      strcat(again_msg, NAME_101);
      break;
    case '*':
      strcat(again_msg, NAME_MUL);
      break;
    case '/':
      strcat(again_msg, NAME_DIV);
      break;
  }

  if (use_exit_ptr) {
    state = STATE_START;
    exit_ptr = use_exit_ptr;
  } else {
    keypad.getKey();
    lcd.print("Willkommen zu");
    lcd.setCursor(0, 1);
    again_msg_ptr = again_msg + 9;
    lcd.print(again_msg_ptr);
    lcd.setCursor(strlen(again_msg_ptr), 1);
    lcd.print('!');
    state = STATE_SCROLL;
  }

  strcat(again_msg, "?");
}

void loop_practice(char mode)
{
  unsigned long m = millis();
  char key;
  switch (state) {
    case STATE_START:
      if (mode == '4') {
        pos = 0;
      } else {
        pos = (m % 256)*10;
      }
      lcd.clear();
      lcd.print("Aufgabe     0,0s");
      start_millis = m;
      state = STATE_OUTPUT;

    case STATE_OUTPUT:
      lcd.setCursor(8, 0);
      lcd.print((pos%10)+1);
      if (mode == '4') {
        do {
          // repeat old task for pos > 9
          if (pos < 10)
            number1 = millis();
          number2 = 0;
          lcd.setCursor(0, 1);
          result = (number1%101)%8+1;
          number2 += result;
          lcd.print(result);
          lcd.print('+');
          result = (number1%103)%8+1;
          number2 += result;
          lcd.print(result);
          lcd.print('+');
          result = (number1%107)%8+1;
          number2 += result;
          lcd.print(result);
          lcd.print('+');
          result = (number1%109)%8+1;
          number2 += result;
          lcd.print(result);
        } while (number2 < 10);
        lcd.print("= \7");
        pos = pos % 10; // reset to new task next time
      } else {
        switch (mode)
        {
          case '+':
          case '-':
            if (pos%2) {
              result = ((long)pgm_read_byte(DATA_ADD+pos+pos+pos/2)%16)*256*256 +
                       (long)pgm_read_byte(DATA_ADD+pos+pos+pos/2+1)*256 +
                       (long)pgm_read_byte(DATA_ADD+pos+pos+pos/2+2);
            } else {
              result = (long)pgm_read_byte(DATA_ADD+pos+pos+pos/2)*16*256 +
                       (long)pgm_read_byte(DATA_ADD+pos+pos+pos/2+1)*16 +
                       (long)pgm_read_byte(DATA_ADD+pos+pos+pos/2+2)/16;
            }
            number1 = result/1024;
            number2 = result%1024;
            if (mode == '-') {
              number1 += number2;
            }
            break;
          case '1':
            switch (pos%4) {
              case 0:
                result = (long)pgm_read_byte(DATA_101+(pos*3)/4)/4;
                break;
              case 1:
                result = ((long)pgm_read_byte(DATA_101+(pos*3)/4)%4)*16 +
                         (long)pgm_read_byte(DATA_101+(pos*3)/4+1)/16;
                break;
              case 2:
                result = ((long)pgm_read_byte(DATA_101+(pos*3)/4)%16)*4 +
                         (long)pgm_read_byte(DATA_101+(pos*3)/4+1)/64;
                break;
              case 3:
                result = (long)pgm_read_byte(DATA_101+(pos*3)/4)%64;
                break;
            }
            number1 = (result/8) + 2;
            number2 = (result%8) + 2;
            break;
          case '*':
          case '/':
            switch (pos%4) {
              case 0:
                result = (long)pgm_read_byte(DATA_MUL+pos+pos/4)*4 +
                         (long)pgm_read_byte(DATA_MUL+pos+pos/4+1)/64;
                break;
              case 1:
                result = ((long)pgm_read_byte(DATA_MUL+pos+pos/4)%64)*16 +
                         (long)pgm_read_byte(DATA_MUL+pos+pos/4+1)/16;
                break;
              case 2:
                result = ((long)pgm_read_byte(DATA_MUL+pos+pos/4)%16)*64 +
                         (long)pgm_read_byte(DATA_MUL+pos+pos/4+1)/4;
                break;
              case 3:
                result = ((long)pgm_read_byte(DATA_MUL+pos+pos/4)%4)*256 +
                         (long)pgm_read_byte(DATA_MUL+pos+pos/4+1);
                break;
            }
            number1 = result/8;
            number2 = (result%8) + 2;
            if (mode == '/') {
              number1 *= number2;
            }
            break;
        }
        lcd.setCursor(0, 1);
        lcd.print(number1);
        switch (mode) {
          case '+':
            lcd.print('+');
            break;
          case '-':
            lcd.print('-');
            break;
          case '1':
          case '*':
            lcd.print('\1');
            break;
          case '/':
            lcd.print('\2');
            break;
        }
        lcd.print(number2);
        switch (mode) {
          case '+':
          case '-':
          case '*':
            lcd.print("=\7\7");
            break;
          case '1':
          case '/':
            lcd.print("= \7");
            break;
        }
      }
      step = 0; // character counter (0 and 1 for modes '4', '1', and '/'; 0, 1, 2 for '+', '-', and '*')
      flash = 2; // flash cursor (0 or 1), >1 forces redraw
      result = 0;
      state = STATE_INPUT;

    case STATE_INPUT:
      // flash cursor
      if ((m/500)%2 != flash)
      {
        flash = (m/500)%2;
        lcd.setCursor(start+(1-2*reverse)*step, 1);
        lcd.print(flash ? '\6' : '\7');
      }
      key = keypad.getKey();
      if (key >= '0' && key <= '9') {
        beep_key();
        lcd.setCursor(start+(1-2*reverse)*step, 1);
        lcd.print(key);
        step += 1;
        if (reverse) {
          switch (step) {
            case 1:
              result = (int)key-48;
              break;
            case 2:
              result += ((int)key-48)*10;
              break;
            case 3:
              result += ((int)key-48)*100;
              if ((mode == '+' && result == number1 + number2) || (mode == '-' && result == number1 - number2) || (mode == '*' && result == number1 * number2)) {
                pos += 1;
                if (pos%10) {
                  state = STATE_OUTPUT;
                } else {
                  state = STATE_DONE;
                }
              }
              else
              {
                state = STATE_ERROR;
                lcd.setCursor(0, 1);
                lcd.print("falsch 3s Strafe");
                error_until = m + 3000;
              }
          }
        } else {
          result = result*10 + (int)key-48;
          if (step == 2) {
            if ((mode == '4' && result == number2) || (mode == '1' && result == number1 * number2) || (mode == '/' && result == number1 / number2)) {
              pos += 1;
              if (pos%10) {
                state = STATE_OUTPUT;
              } else {
                state = STATE_DONE;
              }
            }
            else
            {
              state = STATE_ERROR;
              lcd.setCursor(0, 1);
              lcd.print("falsch 3s Strafe");
              error_until = m + 3000;
              if (mode == '4')
                pos += 10; // repeat task
            }
          }
        }
      } else {
        if (key == '*') {
          if (step == 1) {
            beep_key();
            switch (mode) {
              case '4':
              case '+':
              case '-':
                lcd.setCursor(9, 1);
                break;
              case '1':
                lcd.setCursor(5, 1);
                break;
              case '*':
                lcd.setCursor(6, 1);
                break;
              case '/':
                lcd.setCursor(7, 1);
                break;
            }
            lcd.print('\7');
            step = 0;
            flash = 2;
            result = 0;
          } else {
            if (step == 2) {
              beep_key();
              if (mode == '*') {
                lcd.setCursor(5, 1);
              } else {
                lcd.setCursor(8, 1);
              }
              lcd.print('\7');
              step = 1;
              flash = 2;
              result %= 10;
            } else
              beep_error();
          }
        } else
          if (key)
            beep_error();
      }

    case STATE_ERROR:

      if (m-start_millis >= 100000) {
        lcd.setCursor(0, 0);
        lcd.print("Zeitbegrenzung! ");
        state = STATE_AGAIN;
      } else {
        if ((m-start_millis)/1000 != (last_millis-start_millis/1000))
        {
          if (m-start_millis < 10000)
            lcd.setCursor(12, 0);
          else
            lcd.setCursor(11, 0);
          lcd.print((m-start_millis)/1000);
        }
        if ((m-start_millis)/100 != (last_millis-start_millis/100)) {
          lcd.setCursor(14, 0);
          lcd.print(((m-start_millis)/100)%10);
        }
        last_millis = m;
      }

      if (state != STATE_ERROR)
        break;

      if (m > error_until) {
        lcd.setCursor(0, 1);
        for(flash=0; flash<16; flash++)
          lcd.print(' ');
        state = STATE_OUTPUT;
      } else {
        if (keypad.getKey())
          beep_error();
      }
      break;

    case STATE_DONE:
      lcd.setCursor(0, 0);
      lcd.print("geschafft:");

    case STATE_AGAIN:
      lcd.setCursor(0, 1);
      lcd.print(again_msg);
      last_millis = m + 1000;
      again_msg_ptr = again_msg;
      state = STATE_SCROLL;

    case STATE_SCROLL:
      if ((m > last_millis + 500) && (strlen(again_msg_ptr) > 16))
      {
        again_msg_ptr += 1;
        lcd.setCursor(0, 1);
        lcd.print(again_msg_ptr);
        last_millis += 500;
      }
      key = keypad.getKey();
      if (key) {
        if ((key == '*') && exit_ptr) {
          beep_key();
          (*exit_ptr)();
        } else {
          if ((key == '#') || !exit_ptr) {
            beep_key();
            state = STATE_START;
          } else {
            beep_error();
          }
        }
      }
  }
}
