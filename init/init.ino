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

#define GREETING "Hallo nanocal.de"
// #define GREETING "Papas Rechner"
// #define GREETING "Julians Rechner"
// #define GREETING "Jasmins Rechner"

void setup()
{
  setup_nanocal();

  EEPROM.update(0, 70);
  EEPROM.update(1, 5);
  EEPROM.update(2, 60);
  EEPROM.update(3, 15);

  for(unsigned int i=0; i<16; i++)
    if (i < strlen(GREETING))
      EEPROM.update(i+4, (byte)GREETING[i]);
    else
      EEPROM.update(i+4, (byte)' ');

  lcd.print("eeprom defaults");
  lcd.setCursor(0, 1);
  lcd.print("written, enjoy!");
}

void loop()
{
}

