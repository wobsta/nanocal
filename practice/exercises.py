# This file is part of nanocal.
#
# Copyright (C) 2022-2024 André Wobst <project.nanocal@wobsta.de>
#
# nanocal is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# nanocal is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with nanocal.  If not, see <http://www.gnu.org/licenses/>.

from random import seed, randint, shuffle

seed()

f = open('exercises.h', 'w')
f.write('// this is an auto-generated file by exercises.py\n\n')

f.write('const byte DATA_ADD[] PROGMEM = {\n')
# 10+10=20 bits per task, 25 bytes per 10 tasks

for i in range(256):

    tasks = []

    # two tasks without carry flags,
    # three with the carry flag at the end and the front,
    # and two with both carry flags
    carries = [2, 3, 3, 2]
    # note the inverse order: [10, 0, 0, 0] means no carries at all
    # whereas [0, 0, 0, 10] would mean carries all the time
    # we should have: sum(carries) == 10

    while len(tasks) < 10:
        a = randint(100, 999)
        b = randint(100, 999)
        if a < b: a, b = b, a
        if (a, b) not in tasks:
            if a+b < 1000:
                carry1 = a%10 + b%10 >= 10
                carry2 = a//100 + b//100 != (a+b)//100
                c = carry2*2+carry1
                if carries[c]:
                    tasks.append((a, b))
                    carries[c] -= 1

    bits = []
    for a, b in tasks:
        if randint(0, 1):
            a, b = b, a

        assert a & 0b1111111111 == a
        assert b & 0b1111111111 == b
        bits.append(bool(a&0b1000000000))
        bits.append(bool(a&0b0100000000))
        bits.append(bool(a&0b0010000000))
        bits.append(bool(a&0b0001000000))
        bits.append(bool(a&0b0000100000))
        bits.append(bool(a&0b0000010000))
        bits.append(bool(a&0b0000001000))
        bits.append(bool(a&0b0000000100))
        bits.append(bool(a&0b0000000010))
        bits.append(bool(a&0b0000000001))
        bits.append(bool(b&0b1000000000))
        bits.append(bool(b&0b0100000000))
        bits.append(bool(b&0b0010000000))
        bits.append(bool(b&0b0001000000))
        bits.append(bool(b&0b0000100000))
        bits.append(bool(b&0b0000010000))
        bits.append(bool(b&0b0000001000))
        bits.append(bool(b&0b0000000100))
        bits.append(bool(b&0b0000000010))
        bits.append(bool(b&0b0000000001))

    if i:
        f.write(',\n')
    byte = 0
    for j, bit in enumerate(bits):
        byte = byte*2
        if bit: byte += 1
        if not ((j+1)%8):
            if j != 7:
                f.write(', ')
            f.write(f'{byte:3}')
            byte = 0

f.write('};\n\n')


f.write('const byte DATA_101[] PROGMEM = {\n')
# 3+3=6 bits per task, 7.5 bytes per 10 tasks

prevbits = []

for i in range(256):

    tasks = [(6, 7),
             (6, 9),
             (7, 8),
             (2, randint(5, 9)),
             (5, randint(3, 9))]

    number = {1: 3, 2: 4, 3: 6, 4: 7, 5: 8, 6: 9}

    while len(tasks) < 10:
        a = number[randint(1, 6)]
        b = number[randint(1, 6)]
        if a > b: a, b = b, a
        if (a, b) not in tasks and 10 <= a*b < 100:
            tasks.append((a, b))

    shuffle(tasks)

    bits = prevbits
    for a, b in tasks:
        if randint(0, 1):
            a, b = b, a
        if not i:
            print(a, b)

        assert 1 < a < 10
        assert 1 < b < 10
        a -= 2
        b -= 2
        assert a & 0b111 == a
        assert b & 0b111 == b
        bits.append(bool(a&0b100))
        bits.append(bool(a&0b010))
        bits.append(bool(a&0b001))
        bits.append(bool(b&0b100))
        bits.append(bool(b&0b010))
        bits.append(bool(b&0b001))

    if not i%2:
        # keep the last four bits for the next round
        prevbits = bits[-4:]
        bits = bits[:-4]
    else:
        prevbits = []

    if not i: print(bits[:6])
    if not i: print(bits[:8])

    assert not len(bits)%8

    if i:
        f.write(',\n')
    byte = 0
    for j, bit in enumerate(bits):
        byte = byte*2
        if bit: byte += 1
        if not ((j+1)%8):
            if j != 7:
                f.write(', ')
            f.write(f'{byte:3}')
            byte = 0

assert not prevbits

f.write('};\n\n')


f.write('const byte DATA_MUL[] PROGMEM = {\n')
# 3+7=10 bits per task, 12.5 bytes per 10 tasks

prevbits = []

for i in range(256):

    tasks = []

    while len(tasks) < 10:
        # one (and only one) first operand with tailing zero
        if len(tasks):
            a = randint(10, 99)
            if not a%10:
                continue
        else:
            a = randint(1, 9)*10
        b = len(tasks) # second operand is b+2, i.e. 2 to 9
        if b == 8:
            b = 5 # have a second 7
        elif b == 9:
            b = 7 # and a second 9
        if (a, b) not in tasks and 100 <= a*(b+2) < 1000:
            tasks.append((a, b))

    shuffle(tasks)

    bits = prevbits
    for a, b in tasks:

        assert a & 0b1111111 == a
        assert b & 0b111 == b
        bits.append(bool(a&0b1000000))
        bits.append(bool(a&0b0100000))
        bits.append(bool(a&0b0010000))
        bits.append(bool(a&0b0001000))
        bits.append(bool(a&0b0000100))
        bits.append(bool(a&0b0000010))
        bits.append(bool(a&0b0000001))
        bits.append(bool(b&0b100))
        bits.append(bool(b&0b010))
        bits.append(bool(b&0b001))

    if not i%2:
        # keep the last four bits for the next round
        prevbits = bits[-4:]
        bits = bits[:-4]
    else:
        prevbits = []

    assert not len(bits)%8

    if i:
        f.write(',\n')
    byte = 0
    for j, bit in enumerate(bits):
        byte = byte*2
        if bit: byte += 1
        if not ((j+1)%8):
            if j != 7:
                f.write(', ')
            f.write(f'{byte:3}')
            byte = 0

assert not prevbits

f.write('};\n\n')
