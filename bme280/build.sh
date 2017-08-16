#!/bin/bash

ARDUINO_DIR=/home/andreas/arduino-1.8.3
BUILD_PATH=./build
CACHE=./cache

mkdir -p ${BUILD_PATH}
mkdir -p ${CACHE}

${ARDUINO_DIR}/arduino-builder -dump-prefs -logger=machine -hardware /${ARDUINO_DIR}/hardware -hardware /home/andreas/.arduino15/packages -tools /${ARDUINO_DIR}/tools-builder -tools /${ARDUINO_DIR}/hardware/tools/avr -tools /home/andreas/.arduino15/packages -built-in-libraries /${ARDUINO_DIR}/libraries -libraries /home/andreas/Arduino/libraries -fqbn=SparkFun:avr:promicro:cpu=8MHzatmega32U4 -ide-version=10803 -build-path ${BUILD_PATH} -warnings=all -build-cache ${CACHE} -prefs=build.warn_data_percentage=75 -prefs=runtime.tools.arduinoOTA.path=/${ARDUINO_DIR}/hardware/tools/avr -prefs=runtime.tools.avr-gcc.path=/${ARDUINO_DIR}/hardware/tools/avr -prefs=runtime.tools.avrdude.path=/${ARDUINO_DIR}/hardware/tools/avr -verbose /home/andreas/Arduino/bme280test/bme280test.ino

/${ARDUINO_DIR}/arduino-builder -compile -logger=machine -hardware /${ARDUINO_DIR}/hardware -hardware /home/andreas/.arduino15/packages -tools /${ARDUINO_DIR}/tools-builder -tools /${ARDUINO_DIR}/hardware/tools/avr -tools /home/andreas/.arduino15/packages -built-in-libraries /${ARDUINO_DIR}/libraries -libraries /home/andreas/Arduino/libraries -fqbn=SparkFun:avr:promicro:cpu=8MHzatmega32U4 -ide-version=10803 -build-path ${BUILD_PATH} -warnings=all -build-cache ${CACHE} -prefs=build.warn_data_percentage=75 -prefs=runtime.tools.arduinoOTA.path=/${ARDUINO_DIR}/hardware/tools/avr -prefs=runtime.tools.avr-gcc.path=/${ARDUINO_DIR}/hardware/tools/avr -prefs=runtime.tools.avrdude.path=/${ARDUINO_DIR}/hardware/tools/avr -verbose /home/andreas/Arduino/bme280test/bme280test.ino
