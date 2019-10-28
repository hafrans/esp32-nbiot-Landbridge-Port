//Modified by Steven Lian on 2019/09/17
/*
	miscPins,  to control LED and beep 
	
    Copyright (C) 2019  Steven Lian (steven.lian@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

*/

#pragma once
#ifndef _MISCPINS_H
#define _MISCPINS_H

#include <Arduino.h>

//
extern "C" {
#include "miscCommon.h"
}


//LED global data
#define CONST_PINS_CHANGE_COUNT 8

//final
//#define CONST_PINS_COLOR_RED_PIN 25 //ESP32
//#define CONST_PINS_COLOR_GREEN_PIN 26 //ESP32
//#define CONST_PINS_COLOR_BLUE_PIN 27 //ESP32
//#define CONST_PINS_BEEP_PIN 32 //ESP32

//temp
#define CONST_PINS_COLOR_RED_PIN 25 //ESP32
#define CONST_PINS_COLOR_GREEN_PIN 26 //ESP32
#define CONST_PINS_COLOR_BLUE_PIN 27 //ESP32
// 新旧板子定义 temp为旧板子
#ifdef NEW
  #define CONST_PINS_BEEP_PIN 27 //ESP32 temp solution
#else
  #define CONST_PINS_BEEP_PIN 32 //ESP32 solution
#endif

#define CONST_PINS_COLOR_BLINK_DELAY 100
#define CONST_PINS_COLOR_50MS_DELAY 50
#define CONST_PINS_POWERON_LOOP_COUNT 10

#define CONST_PINS_COLOR_RED_MASK 4
#define CONST_PINS_COLOR_GREEN_MASK 2
#define CONST_PINS_COLOR_BLUE_MASK 1
#define CONST_PINS_BEEP_MASK 8

#define CONST_PINS_LOOP_ALWAYS 255

#define CONST_PINS_STATUS_ALWAYSOFF 254
#define CONST_PINS_STATUS_ALWAYSON 255

#define CONST_PINS_STRUCT_LEN 14 // LED status count 

#define CONST_PINS_STATUS_SELFDEF 0
#define CONST_PINS_STATUS_SMARTCONFIG 1
#define CONST_PINS_STATUS_CONNECTING 2
#define CONST_PINS_STATUS_FAST_FLASH 3
#define CONST_PINS_STATUS_MID_FLASH 4
#define CONST_PINS_STATUS_SLOW_FLASH 5
#define CONST_PINS_STATUS_VERY_SLOW_FLASH 6
#define CONST_PINS_STATUS_BEEP_ONLY 7
#define CONST_PINS_STATUS_RED_BEEP 8
#define CONST_PINS_STATUS_RED_ONLY 9
#define CONST_PINS_STATUS_GREEN_ONLY 10
#define CONST_PINS_STATUS_BLUE_ONLY 11
#define CONST_PINS_STATUS_FAST_RED_BEEP 12
#define CONST_PINS_STATUS_YELLOW_ONLY 13
#define CONST_PINS_STATUS_RED_GREEN 14

#define CONST_PINS_DEFAULT_MODE 0xF
#define CONST_PINS_MODE_RED 0x1
#define CONST_PINS_MODE_GREEN 0x2
#define CONST_PINS_MODE_BLUE 0x4
#define CONST_PINS_MODE_BEEP 0x8

#define CONST_PINS_ON HIGH
#define CONST_PINS_OFF LOW

#define CONST_PINS_TEST_DELAY_TIME 500


typedef struct {
  uint8_t loopFlag;
  uint8_t onColor;
  uint8_t blinkCount;
  uint8_t blinkLoop[CONST_PINS_CHANGE_COUNT];
} stLedDispStruct;

class miscPINs
{
  public:
    //LED control
    uint8_t gu8LEDStatus = 0; // = 0;
	//funcs
    miscPINs();
    void begin();
	void begin(short mode);
    void loop();
    void int_loop();//interrupt loop
    void change_PINS_status(uint8_t u8Status);
    void LED_ON(uint8_t onColor);
    void LED_ON();
    void LED_OFF();
    void LED_OFF(uint8_t onColor);
    void LED_Blink();
    void LED_poweron();
    void display_PINS_color(void);
    uint8_t get_PINS_status();
    void LED_ON(uint8_t onColor,short loopCount,short delayTime);
    void LED_OFF(uint8_t onColor,short loopCount,short delayTime);
	
	void beep_self_test();

  private:
	short _WorkMode;
    unsigned long ul100msLoopTick = 0;
    //uint8_t gu8LEDPreStatus = 0;
    uint8_t gu8LEDLoopCount = 0; // = 0;
    uint8_t gu8LEDDispCount = 0; // = 0;

    stLedDispStruct asLedDisplay[CONST_PINS_STRUCT_LEN];
};

#endif
