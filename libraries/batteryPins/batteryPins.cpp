//Modified by Steven Lian on 2019/09/22
/*
    battery pins, to control battery LED and button 

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

#include "batteryPins.h"

bool LED_BUTTON::begin(unsigned short *dataPtr, bool *enablerPtr, uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t button)
{
  dispValPtr = dataPtr;
  enableValPtr = enablerPtr;

  ledPin[0] = pin0;
  ledPin[1] = pin1;
  ledPin[2] = pin2;
  ledPin[3] = pin3;
  ledPin[4] = pin4;
  buttonPin = button;

  for (short i = 0; i < CONST_APP_BATT_MAX_LED_NUM; i++)
    pinMode(ledPin[i], OUTPUT);
}


void LED_BUTTON::self_delay(int msTicks)
{
  delay(msTicks);
}


void LED_BUTTON::self_test()
{
  //DBGPRINTF("\n-> self_test %s", "");
  /*
    for (short i = 0; i < CONST_APP_BATT_MAX_LED_NUM; i++)
    {
    DBGPRINTF("\n-> ledPin[%d] %s", i, "ON");
    digitalWrite(ledPin[i], CONST_APP_BATT_LED_ON);
    self_delay(CONST_APP_BATT_LED_SELF_TEST_IN_MS);
    DBGPRINTF("\n-> ledPin[%d] %s", i, "OFF");
    digitalWrite(ledPin[i], CONST_APP_BATT_LED_OFF);
    self_delay(CONST_APP_BATT_LED_SELF_TEST_IN_MS);
    }
  */
  for (short k = 0; k < 1; k++)
  {
    //DBGPRINTF("\n-> 5 led on %s", "");
    for (short i = 0; i < CONST_APP_BATT_MAX_LED_NUM; i++)
    {
      //DBGPRINTF("\n-> ledPin[%d] %s", i, "ON");
      digitalWrite(ledPin[i], CONST_APP_BATT_LED_ON);
    }
    self_delay(CONST_APP_BATT_LED_SELF_TEST_IN_MS);
    //DBGPRINTF("\n-> 5 led off %s", "");
    for (short i = 0; i < CONST_APP_BATT_MAX_LED_NUM; i++)
    {
      //DBGPRINTF("\n-> ledPin[%d] %s", i, "OFF");
      digitalWrite(ledPin[i], CONST_APP_BATT_LED_OFF);
    }
    self_delay(CONST_APP_BATT_LED_SELF_TEST_IN_MS);
  }
}


void LED_BUTTON::led_display(short val)
{
  uint8_t pin;
  //DBGPRINTF("\n-> led_display %d", val);
  if (val > 0)
  {
    pin = 0;
    //DBGPRINTF("\n-> ledPin[%d][%d] %s", pin, ledPin[pin], "ON");
    digitalWrite(ledPin[pin], CONST_APP_BATT_LED_ON);
  }
  if (val > 1)
  {
    pin = 1;
    //DBGPRINTF("\n-> ledPin[%d][%d] %s", pin, ledPin[pin], "ON");
    digitalWrite(ledPin[pin], CONST_APP_BATT_LED_ON);
  }
  if (val > 2)
  {
    pin = 2;
    //DBGPRINTF("\n-> ledPin[%d][%d] %s", pin, ledPin[pin], "ON");
    digitalWrite(ledPin[pin], CONST_APP_BATT_LED_ON);
  }
  if (val > 3)
  {
    pin = 3;
    //DBGPRINTF("\n-> ledPin[%d][%d] %s", pin, ledPin[pin], "ON");
    digitalWrite(ledPin[pin], CONST_APP_BATT_LED_ON);
  }
  if (val > 4)
  {
    pin = 4;
    //DBGPRINTF("\n-> ledPin[%d][%d] %s", pin, ledPin[pin], "ON");
    digitalWrite(ledPin[pin], CONST_APP_BATT_LED_ON);
  }
}


void LED_BUTTON::led_off()
{
  //DBGPRINTF("\n-> led_off %s", "");
  for (short i = 0; i < CONST_APP_BATT_MAX_LED_NUM; i++)
  {
    //DBGPRINTF("\n-> ledPin[%d] %s", i, "OFF");
    digitalWrite(ledPin[i], CONST_APP_BATT_LED_OFF);
  }
}


bool LED_BUTTON::button_detection()
{
  bool ret = false;
  //DBGPRINTF("\n-> button_detection %s", "");
  unsigned short detectVal = 0;
  short detectCount = 0;
  //detectVal = digitalRead(buttonPin);
  detectVal = analogRead(buttonPin);
  //DBGPRINTF("\n-> detectVal[%d]", detectVal);
  if (detectVal > CONST_APP_BATT_BUTTON_VAL_THRESHOLD)
  {
    detectCount = 1;
    for (short i = 0; i < CONST_APP_BATT_BUTTON_DETECT_NUM; i++)
    {
      self_delay(CONST_APP_BATT_BUTTON_DETECT_DELAY);
      //detectVal += digitalRead(buttonPin);
      detectVal = analogRead(buttonPin);
      if (detectVal > CONST_APP_BATT_BUTTON_VAL_THRESHOLD)
      {
        detectCount++;
        //DBGPRINTF("\n-> ledPin[%d] detectVal[%d] detctCount[%d]", i, detectVal, detectCount);
      }
    }
    if (detectCount >= CONST_APP_BATT_BUTTON_DETECT_NUM)
    {
      //DBGPRINTF("\n-> detectVal[%d] %s", detectVal, "button_detection");
      ret = true;
    }
  }
  return ret;
}


void LED_BUTTON::loop()
{
  if (button_detection())
  {
    led_display(*dispValPtr);
    if (enableValPtr != NULL)
	  *enableValPtr = true;
	ulTickDispTime = ulReset_interval();
    //self_delay(CONST_APP_BATT_DISP_TIME_IN_MS);
    //led_off();
  }
  else {
    if (ulGet_interval(ulTickDispTime) > CONST_APP_BATT_DISP_TIME_IN_MS)
    {
      ulTickDispTime = ulReset_interval();
	  if (enableValPtr != NULL)
  	    *enableValPtr = false;
      led_off();
    }
  }
}
