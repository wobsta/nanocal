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

#define setup() setup_calc()
#define loop() loop_calc()
#include "calc/calc.ino"
#undef setup
#undef loop

#define setup() setup_qwirkle()
#define loop() loop_qwirkle()
#define state state_qwirkle
#include "qwirkle/qwirkle.ino"
#undef setup
#undef loop
#undef state

#define setup() setup_dice()
#define loop() loop_dice()
#define last_millis last_millis_dice
#include "dice/dice.ino"
#undef setup
#undef loop
#undef last_millis

#define setup() setup_practice()
#define loop() loop_practice()
#define state state_practice
#define pos pos_practice
#define number1 number1_practice
#define number2 number2_practice
#define result result_practice
#define last_millis last_millis_practice
#define step step_practice
#define again_msg again_msg_practice
#undef STATE_ERROR
#include "practice/practice.h"
#define state state_practice
#undef setup
#undef loop
#undef state
#undef pos
#undef number1
#undef number2
#undef result
#undef last_millis
#undef step
#undef again_msg

#define setup(use_exit_ptr) setup_settings(use_exit_ptr)
#define loop() loop_settings()
#define menu menu_settings
#define menu_scroll menu_scroll_settings
#define exit_ptr exit_ptr_settings
#define state state_settings
#define pos pos_settings
#define last_millis last_millis_settings
#include "settings/settings.ino"
#undef setup
#undef setup_with_exit
#undef loop
#undef menu
#undef menu_scroll
#undef exit_ptr
#undef state
#undef pos
#undef last_millis

const char* menu = "1:" NAME_CALC
                  " 2:" NAME_QWIRKLE
                  " 3:" NAME_DICE
                  " 4:" NAME_FOUR
                  " 5:" NAME_ADD
                  " 6:" NAME_SUB
                  " 7:" NAME_101
                  " 8:" NAME_MUL
                  " 9:" NAME_DIV
                  " 0:" NAME_SETTINGS
                  " ";

#define RESET_TIME 1500

byte mode, pos;
char menu_scroll[17];
unsigned long last_millis, reset_start;

#define MODE_CHOICE 255
#define MODE_SETTINGS 0
#define MODE_CALC 1
#define MODE_QWIRKLE 2
#define MODE_DICE 3
#define MODE_FOUR 4
#define MODE_ADD 5
#define MODE_SUB 6
#define MODE_101 7
#define MODE_MUL 8
#define MODE_DIV 9

void setup_choice()
{
  last_millis = millis() + 1000;

  strncpy(menu_scroll, menu, 16);
  menu_scroll[16] = 0;

  lcd.noCursor();
  lcd.noBlink();
  lcd.setCursor(0, 0);
  for(pos=0; pos<16; pos++) {
    mode = EEPROM.read(pos+4);
    if (mode > 127) {
      lcd.write(mode-125);
    } else {
      lcd.print((char)mode);
    }
  }
  lcd.setCursor(0, 1);
  lcd.print(menu_scroll);

  mode = MODE_CHOICE;
  pos = 0;
}

void loop_choice()
{
  if (millis() > last_millis + 500)
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
  key = keypad.getKey();
  if (key >= '0' && key <= '9') {
    beep_key();
    lcd.clear();
    mode = (int)key-48;
    switch (mode) {
      case MODE_CALC:
        setup_calc();
        break;
      case MODE_QWIRKLE:
        setup_qwirkle();
        break;
      case MODE_DICE:
        setup_dice();
        break;
      case MODE_FOUR:
        setup_practice('4', setup_choice);
        break;
      case MODE_ADD:
        setup_practice('+', setup_choice);
        break;
      case MODE_SUB:
        setup_practice('-', setup_choice);
        break;
      case MODE_101:
        setup_practice('1', setup_choice);
        break;
      case MODE_MUL:
        setup_practice('*', setup_choice);
        break;
      case MODE_DIV:
        setup_practice('/', setup_choice);
        break;
      case MODE_SETTINGS:
        setup_settings(setup_choice);
        break;
    }
  } else {
    if (key)
      beep_error();
  }
}

void setup()
{
  setup_nanocal();
  setup_choice();
  keypad.getKey();
}

void loop()
{
  // similar to scanKeys
  pinMode(5, OUTPUT); // column
  pinMode(4, OUTPUT); // other columns
  pinMode(3, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(5, LOW);
  digitalWrite(4, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(2, HIGH);
  pinMode(6, INPUT_PULLUP); // row
  if (!digitalRead(6)) {
    if (reset_start) {
      if (millis() - reset_start > RESET_TIME)
      {
        setup_choice();
      }
    } else {
      reset_start = millis();
    }
  } else {
    reset_start = 0;
  }
  digitalWrite(5, HIGH);
  pinMode(5, INPUT);

  switch (mode) {
    case MODE_CHOICE:
      loop_choice();
      break;
    case MODE_CALC:
      loop_calc();
      break;
    case MODE_QWIRKLE:
      loop_qwirkle();
      break;
    case MODE_DICE:
      loop_dice();
      break;
    case MODE_FOUR:
      loop_practice('4');
      break;
    case MODE_ADD:
      loop_practice('+');
      break;
    case MODE_SUB:
      loop_practice('-');
      break;
    case MODE_101:
      loop_practice('1');
      break;
    case MODE_MUL:
      loop_practice('*');
      break;
    case MODE_DIV:
      loop_practice('/');
      break;
    case MODE_SETTINGS:
      loop_settings();
      break;
  }
}
