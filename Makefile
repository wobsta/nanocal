# ARDUINO_DIR = /usr/share/arduino
ARDUINO_PORT = /dev/ttyUSB1

BOARD_TAG = nano
BOARD_SUB = atmega328old
BOOTLOADER_FILE = ATmegaBOOT_168_atmega328.hex

ARDUINO_LIBS = EEPROM Wire LiquidCrystal_I2C Keypad
AVRDUDE_CONF = /etc/avrdude.conf

include /usr/share/arduino/Arduino.mk

.PHONY: cleanall
cleanall:
	-rm -r build* */build* */*/build*
