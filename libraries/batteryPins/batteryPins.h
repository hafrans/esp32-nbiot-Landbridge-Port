//Modified by Steven Lian on 2019/09/22
/*
	battery pins,  to control battery LED and button 
	
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
#ifndef _BATTERY_PINS_H
#define _BATTERY_PINS_H

#include <arduino.h>

extern "C" {
#include "miscCommon.h"
}

#define CONST_APP_BATT_MAX_LED_NUM 5

#define CONST_APP_BATT_LED0_PIN 4
#define CONST_APP_BATT_LED1_PIN 32 // 25
#define CONST_APP_BATT_LED2_PIN 27 // 26
#define CONST_APP_BATT_LED3_PIN 26 // 27
#define CONST_APP_BATT_LED4_PIN 25 // 32
#define CONST_APP_BATT_BUTTON_PIN 34
#define CONST_APP_BATT_DISP_TIME_IN_MS 4000 //显示时间单位毫秒

#define CONST_APP_BATT_BUTTON_DETECT_NUM 4
#define CONST_APP_BATT_BUTTON_DETECT_DELAY 30
#define CONST_APP_BATT_BUTTON_VAL_THRESHOLD 3000

#define CONST_APP_BATT_LED_SELF_TEST_IN_MS 500 //自测delay 时间

#define CONST_APP_BATT_LED_ON HIGH
#define CONST_APP_BATT_LED_OFF LOW


class LED_BUTTON
{
  public:
    //data
    //func
    bool begin(unsigned short *dataPtr, bool *enablerPtr = NULL, uint8_t pin0 = CONST_APP_BATT_LED0_PIN, uint8_t pin1 = CONST_APP_BATT_LED1_PIN,\
  	  uint8_t pin2 = CONST_APP_BATT_LED2_PIN, uint8_t pin3 = CONST_APP_BATT_LED3_PIN, uint8_t pin4 = CONST_APP_BATT_LED4_PIN, uint8_t button = CONST_APP_BATT_BUTTON_PIN);

    void self_test();
    void led_display(short val);
    void led_off();
    void self_delay(int);
    bool button_detection();
    void loop();

  private:
    // data
    unsigned long ulTickDispTime;

    unsigned short *dispValPtr;
	bool *enableValPtr; //conterol other led's based on button press

    uint8_t ledPin[CONST_APP_BATT_MAX_LED_NUM];
    uint8_t buttonPin;

    uint8_t buttonPinStatus;

    //func
};


#endif
