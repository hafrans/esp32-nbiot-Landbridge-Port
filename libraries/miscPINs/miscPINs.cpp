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

#include "miscPINs.h"


miscPINs::miscPINs()
{
  begin(CONST_PINS_DEFAULT_MODE);
} 

void  miscPINs::begin()
{
  begin(CONST_PINS_DEFAULT_MODE);
}

void  miscPINs::begin(short mode)
{
	
  _WorkMode = mode;
  //LED PIN MODE
  pinMode(CONST_PINS_COLOR_RED_PIN, OUTPUT);
  pinMode(CONST_PINS_COLOR_GREEN_PIN, OUTPUT);
  pinMode(CONST_PINS_COLOR_BLUE_PIN, OUTPUT);
  pinMode(CONST_PINS_BEEP_PIN, OUTPUT);
  ul100msLoopTick = 0;

  //CONST_PINS_STATUS_SELFDEF
  asLedDisplay[CONST_PINS_STATUS_SELFDEF].loopFlag = CONST_PINS_LOOP_ALWAYS;
  asLedDisplay[CONST_PINS_STATUS_SELFDEF].onColor = CONST_PINS_COLOR_GREEN_MASK;
  asLedDisplay[CONST_PINS_STATUS_SELFDEF].blinkCount = 4;
  asLedDisplay[CONST_PINS_STATUS_SELFDEF].blinkLoop[0] = 3;
  asLedDisplay[CONST_PINS_STATUS_SELFDEF].blinkLoop[1] = 6;
  asLedDisplay[CONST_PINS_STATUS_SELFDEF].blinkLoop[2] = 9;
  asLedDisplay[CONST_PINS_STATUS_SELFDEF].blinkLoop[3] = 19;

  //CONST_PINS_STATUS_SMARTCONFIG
  asLedDisplay[CONST_PINS_STATUS_SMARTCONFIG].loopFlag = CONST_PINS_LOOP_ALWAYS;
  asLedDisplay[CONST_PINS_STATUS_SMARTCONFIG].onColor = CONST_PINS_COLOR_GREEN_MASK;
  asLedDisplay[CONST_PINS_STATUS_SMARTCONFIG].blinkCount = 4;
  asLedDisplay[CONST_PINS_STATUS_SMARTCONFIG].blinkLoop[0] = 2;
  asLedDisplay[CONST_PINS_STATUS_SMARTCONFIG].blinkLoop[1] = 4;
  asLedDisplay[CONST_PINS_STATUS_SMARTCONFIG].blinkLoop[2] = 6;
  asLedDisplay[CONST_PINS_STATUS_SMARTCONFIG].blinkLoop[3] = 18;

  //CONST_PINS_STATUS_CONNECTING
  asLedDisplay[CONST_PINS_STATUS_CONNECTING].loopFlag = CONST_PINS_LOOP_ALWAYS;
  asLedDisplay[CONST_PINS_STATUS_CONNECTING].onColor = CONST_PINS_COLOR_GREEN_MASK;
  asLedDisplay[CONST_PINS_STATUS_CONNECTING].blinkCount = 4;
  asLedDisplay[CONST_PINS_STATUS_CONNECTING].blinkLoop[0] = 4;
  asLedDisplay[CONST_PINS_STATUS_CONNECTING].blinkLoop[1] = 8;
  asLedDisplay[CONST_PINS_STATUS_CONNECTING].blinkLoop[2] = 12;
  asLedDisplay[CONST_PINS_STATUS_CONNECTING].blinkLoop[3] = 16;

  //CONST_PINS_STATUS_FAST_FLASH
  asLedDisplay[CONST_PINS_STATUS_FAST_FLASH].loopFlag = CONST_PINS_LOOP_ALWAYS;
  asLedDisplay[CONST_PINS_STATUS_FAST_FLASH].onColor = CONST_PINS_COLOR_GREEN_MASK;
  asLedDisplay[CONST_PINS_STATUS_FAST_FLASH].blinkCount = 4;
  asLedDisplay[CONST_PINS_STATUS_FAST_FLASH].blinkLoop[0] = 2;
  asLedDisplay[CONST_PINS_STATUS_FAST_FLASH].blinkLoop[1] = 4;
  asLedDisplay[CONST_PINS_STATUS_FAST_FLASH].blinkLoop[2] = 6;
  asLedDisplay[CONST_PINS_STATUS_FAST_FLASH].blinkLoop[3] = 8;

  //CONST_PINS_STATUS_MID_FLASH
  asLedDisplay[CONST_PINS_STATUS_MID_FLASH].loopFlag = CONST_PINS_LOOP_ALWAYS;
  asLedDisplay[CONST_PINS_STATUS_MID_FLASH].onColor = CONST_PINS_COLOR_GREEN_MASK;
  asLedDisplay[CONST_PINS_STATUS_MID_FLASH].blinkCount = 4;
  asLedDisplay[CONST_PINS_STATUS_MID_FLASH].blinkLoop[0] = 3;
  asLedDisplay[CONST_PINS_STATUS_MID_FLASH].blinkLoop[1] = 6;
  asLedDisplay[CONST_PINS_STATUS_MID_FLASH].blinkLoop[2] = 9;
  asLedDisplay[CONST_PINS_STATUS_MID_FLASH].blinkLoop[3] = 12;

  //CONST_PINS_STATUS_SLOW_FLASH
  asLedDisplay[CONST_PINS_STATUS_SLOW_FLASH].loopFlag = CONST_PINS_LOOP_ALWAYS;
  asLedDisplay[CONST_PINS_STATUS_SLOW_FLASH].onColor = CONST_PINS_COLOR_GREEN_MASK;
  asLedDisplay[CONST_PINS_STATUS_SLOW_FLASH].blinkCount = 4;
  asLedDisplay[CONST_PINS_STATUS_SLOW_FLASH].blinkLoop[0] = 6;
  asLedDisplay[CONST_PINS_STATUS_SLOW_FLASH].blinkLoop[1] = 12;
  asLedDisplay[CONST_PINS_STATUS_SLOW_FLASH].blinkLoop[2] = 18;
  asLedDisplay[CONST_PINS_STATUS_SLOW_FLASH].blinkLoop[3] = 24;

    //CONST_PINS_STATUS_VERY_SLOW_FLASH
  asLedDisplay[CONST_PINS_STATUS_VERY_SLOW_FLASH].loopFlag = CONST_PINS_LOOP_ALWAYS;
  asLedDisplay[CONST_PINS_STATUS_VERY_SLOW_FLASH].onColor = CONST_PINS_COLOR_GREEN_MASK;
  asLedDisplay[CONST_PINS_STATUS_VERY_SLOW_FLASH].blinkCount = 4;
  asLedDisplay[CONST_PINS_STATUS_VERY_SLOW_FLASH].blinkLoop[0] = 10;
  asLedDisplay[CONST_PINS_STATUS_VERY_SLOW_FLASH].blinkLoop[1] = 20;
  asLedDisplay[CONST_PINS_STATUS_VERY_SLOW_FLASH].blinkLoop[2] = 30;
  asLedDisplay[CONST_PINS_STATUS_VERY_SLOW_FLASH].blinkLoop[3] = 40;

  //CONST_PINS_STATUS_BEEP_ONLY
  asLedDisplay[CONST_PINS_STATUS_BEEP_ONLY].loopFlag = CONST_PINS_LOOP_ALWAYS;
  asLedDisplay[CONST_PINS_STATUS_BEEP_ONLY].onColor = CONST_PINS_BEEP_MASK;
  asLedDisplay[CONST_PINS_STATUS_BEEP_ONLY].blinkCount = 2;
  asLedDisplay[CONST_PINS_STATUS_BEEP_ONLY].blinkLoop[0] = 10;
  asLedDisplay[CONST_PINS_STATUS_BEEP_ONLY].blinkLoop[1] = 30;

  //CONST_PINS_STATUS_RED_BEEP
  asLedDisplay[CONST_PINS_STATUS_RED_BEEP].loopFlag = CONST_PINS_LOOP_ALWAYS;
  asLedDisplay[CONST_PINS_STATUS_RED_BEEP].onColor = CONST_PINS_COLOR_RED_MASK|CONST_PINS_BEEP_MASK;
  asLedDisplay[CONST_PINS_STATUS_RED_BEEP].blinkCount = 4;
  asLedDisplay[CONST_PINS_STATUS_RED_BEEP].blinkLoop[0] = 3;
  asLedDisplay[CONST_PINS_STATUS_RED_BEEP].blinkLoop[1] = 10;
  asLedDisplay[CONST_PINS_STATUS_RED_BEEP].blinkLoop[2] = 13;
  asLedDisplay[CONST_PINS_STATUS_RED_BEEP].blinkLoop[3] = 20;
  
  //CONST_PINS_STATUS_RED_ONLY
  asLedDisplay[CONST_PINS_STATUS_RED_ONLY].loopFlag = CONST_PINS_LOOP_ALWAYS;
  asLedDisplay[CONST_PINS_STATUS_RED_ONLY].onColor = CONST_PINS_COLOR_RED_MASK;
  asLedDisplay[CONST_PINS_STATUS_RED_ONLY].blinkCount = 4;
  asLedDisplay[CONST_PINS_STATUS_RED_ONLY].blinkLoop[0] = 3;
  asLedDisplay[CONST_PINS_STATUS_RED_ONLY].blinkLoop[1] = 6;
  asLedDisplay[CONST_PINS_STATUS_RED_ONLY].blinkLoop[2] = 9;
  asLedDisplay[CONST_PINS_STATUS_RED_ONLY].blinkLoop[3] = 12;

  //CONST_PINS_STATUS_GREEN_ONLY
  asLedDisplay[CONST_PINS_STATUS_GREEN_ONLY].loopFlag = CONST_PINS_LOOP_ALWAYS;
  asLedDisplay[CONST_PINS_STATUS_GREEN_ONLY].onColor = CONST_PINS_COLOR_GREEN_MASK;
  asLedDisplay[CONST_PINS_STATUS_GREEN_ONLY].blinkCount = 4;
  asLedDisplay[CONST_PINS_STATUS_GREEN_ONLY].blinkLoop[0] = 2;
  asLedDisplay[CONST_PINS_STATUS_GREEN_ONLY].blinkLoop[1] = 12;
  asLedDisplay[CONST_PINS_STATUS_GREEN_ONLY].blinkLoop[2] = 14;
  asLedDisplay[CONST_PINS_STATUS_GREEN_ONLY].blinkLoop[3] = 24;

  //CONST_PINS_STATUS_BLUE_ONLY
  asLedDisplay[CONST_PINS_STATUS_BLUE_ONLY].loopFlag = CONST_PINS_LOOP_ALWAYS;
  asLedDisplay[CONST_PINS_STATUS_BLUE_ONLY].onColor = CONST_PINS_COLOR_BLUE_MASK;
  asLedDisplay[CONST_PINS_STATUS_BLUE_ONLY].blinkCount = 4;
  asLedDisplay[CONST_PINS_STATUS_BLUE_ONLY].blinkLoop[0] = 6;
  asLedDisplay[CONST_PINS_STATUS_BLUE_ONLY].blinkLoop[1] = 12;
  asLedDisplay[CONST_PINS_STATUS_BLUE_ONLY].blinkLoop[2] = 18;
  asLedDisplay[CONST_PINS_STATUS_BLUE_ONLY].blinkLoop[3] = 24;

  //CONST_PINS_STATUS_FAST_RED_BEEP
  asLedDisplay[CONST_PINS_STATUS_FAST_RED_BEEP].loopFlag = CONST_PINS_LOOP_ALWAYS;
  asLedDisplay[CONST_PINS_STATUS_FAST_RED_BEEP].onColor = CONST_PINS_COLOR_RED_MASK|CONST_PINS_BEEP_MASK;;
  asLedDisplay[CONST_PINS_STATUS_FAST_RED_BEEP].blinkCount = 4;
  asLedDisplay[CONST_PINS_STATUS_FAST_RED_BEEP].blinkLoop[0] = 3;
  asLedDisplay[CONST_PINS_STATUS_FAST_RED_BEEP].blinkLoop[1] = 6;
  asLedDisplay[CONST_PINS_STATUS_FAST_RED_BEEP].blinkLoop[2] = 9;
  asLedDisplay[CONST_PINS_STATUS_FAST_RED_BEEP].blinkLoop[3] = 12;

  //CONST_PINS_STATUS_YELLOW_ONLY
  asLedDisplay[CONST_PINS_STATUS_YELLOW_ONLY].loopFlag = CONST_PINS_LOOP_ALWAYS;
  asLedDisplay[CONST_PINS_STATUS_YELLOW_ONLY].onColor = CONST_PINS_COLOR_RED_MASK|CONST_PINS_COLOR_GREEN_MASK;;
  asLedDisplay[CONST_PINS_STATUS_YELLOW_ONLY].blinkCount = 4;
  asLedDisplay[CONST_PINS_STATUS_YELLOW_ONLY].blinkLoop[0] = 2;
  asLedDisplay[CONST_PINS_STATUS_YELLOW_ONLY].blinkLoop[1] = 4;
  asLedDisplay[CONST_PINS_STATUS_YELLOW_ONLY].blinkLoop[2] = 6;
  asLedDisplay[CONST_PINS_STATUS_YELLOW_ONLY].blinkLoop[3] = 18;



}


void  miscPINs::beep_self_test(void)
{
  digitalWrite(CONST_PINS_COLOR_RED_PIN, CONST_PINS_ON);
  delay(CONST_PINS_TEST_DELAY_TIME);
  digitalWrite(CONST_PINS_COLOR_RED_PIN, CONST_PINS_OFF);
  delay(CONST_PINS_TEST_DELAY_TIME);
  digitalWrite(CONST_PINS_COLOR_GREEN_PIN, CONST_PINS_ON);
  delay(CONST_PINS_TEST_DELAY_TIME);
  digitalWrite(CONST_PINS_COLOR_GREEN_PIN, CONST_PINS_OFF);
  delay(CONST_PINS_TEST_DELAY_TIME);
  digitalWrite(CONST_PINS_COLOR_BLUE_PIN, CONST_PINS_ON);
  delay(CONST_PINS_TEST_DELAY_TIME);
  digitalWrite(CONST_PINS_COLOR_BLUE_PIN, CONST_PINS_OFF);
  delay(CONST_PINS_TEST_DELAY_TIME);
  digitalWrite(CONST_PINS_BEEP_PIN, CONST_PINS_ON);
  delay(CONST_PINS_TEST_DELAY_TIME);
  digitalWrite(CONST_PINS_BEEP_PIN, CONST_PINS_OFF);
  delay(CONST_PINS_TEST_DELAY_TIME);
}


void  miscPINs::loop(void)
{
  if (ulGet_interval(ul100msLoopTick) >= 100)
  {
    //DBGPRINTLN("display_PINS_color");
    display_PINS_color();
    ul100msLoopTick = ulReset_interval();
  }
}

void  miscPINs::int_loop(void)
{
    //DBGPRINTLN("display_PINS_color");
    display_PINS_color();
}


// to setup or change the LED display status, the status include:CONST_PINS_STATUS_ALWAYSON,CONST_PINS_STATUS_SMARTCONFIG,etc.
void  miscPINs::change_PINS_status(uint8_t u8Status)
{
  if (gu8LEDStatus != u8Status)
  {
    gu8LEDStatus = u8Status;
    gu8LEDLoopCount = 0;
    gu8LEDDispCount = 0;
  }
}

uint8_t  miscPINs::get_PINS_status()
{
  return gu8LEDStatus;
}


void  miscPINs::LED_ON(uint8_t onColor)
{
  if (onColor & CONST_PINS_COLOR_RED_MASK)
    digitalWrite(CONST_PINS_COLOR_RED_PIN, CONST_PINS_ON);
  if (onColor & CONST_PINS_COLOR_GREEN_MASK)
    digitalWrite(CONST_PINS_COLOR_GREEN_PIN, CONST_PINS_ON);
  if (onColor & CONST_PINS_COLOR_BLUE_MASK)
    digitalWrite(CONST_PINS_COLOR_BLUE_PIN, CONST_PINS_ON);
  if (onColor & CONST_PINS_BEEP_MASK)
    digitalWrite(CONST_PINS_BEEP_PIN, CONST_PINS_ON);
}

void  miscPINs::LED_OFF(uint8_t onColor)
{
  if (onColor & CONST_PINS_COLOR_RED_MASK)
    digitalWrite(CONST_PINS_COLOR_RED_PIN, CONST_PINS_OFF);
  if (onColor & CONST_PINS_COLOR_GREEN_MASK)
    digitalWrite(CONST_PINS_COLOR_GREEN_PIN, CONST_PINS_OFF);
  if (onColor & CONST_PINS_COLOR_BLUE_MASK)
    digitalWrite(CONST_PINS_COLOR_BLUE_PIN, CONST_PINS_OFF);
  if (onColor & CONST_PINS_BEEP_MASK)
    digitalWrite(CONST_PINS_BEEP_PIN, CONST_PINS_OFF);
}


void  miscPINs::LED_ON()
{
  if (_WorkMode & CONST_PINS_COLOR_RED_MASK)
    digitalWrite(CONST_PINS_COLOR_RED_PIN, CONST_PINS_ON);
  if (_WorkMode & CONST_PINS_COLOR_GREEN_MASK)
    digitalWrite(CONST_PINS_COLOR_GREEN_PIN, CONST_PINS_ON);
  if (_WorkMode & CONST_PINS_COLOR_BLUE_MASK)
    digitalWrite(CONST_PINS_COLOR_BLUE_PIN, CONST_PINS_ON);
  if (_WorkMode & CONST_PINS_BEEP_MASK)
    digitalWrite(CONST_PINS_BEEP_PIN, CONST_PINS_ON);
}

void  miscPINs::LED_OFF()
{
    digitalWrite(CONST_PINS_COLOR_RED_PIN, CONST_PINS_OFF);
    digitalWrite(CONST_PINS_COLOR_GREEN_PIN, CONST_PINS_OFF);
    digitalWrite(CONST_PINS_COLOR_BLUE_PIN, CONST_PINS_OFF);
    digitalWrite(CONST_PINS_BEEP_PIN, CONST_PINS_OFF);
}


void  miscPINs::LED_ON(uint8_t onColor,short loopCount,short delayTime)
{
  if ((loopCount < 0)||(delayTime <0))
	  return;
  //DBGPRINTF("\n onColor[%x]",onColor);
  while(loopCount > 0)
  {
	loopCount--;
    if (onColor & CONST_PINS_COLOR_RED_MASK)
      digitalWrite(CONST_PINS_COLOR_RED_PIN, CONST_PINS_ON);
    if (onColor & CONST_PINS_COLOR_GREEN_MASK)
      digitalWrite(CONST_PINS_COLOR_GREEN_PIN, CONST_PINS_ON);
    if (onColor & CONST_PINS_COLOR_BLUE_MASK)
      digitalWrite(CONST_PINS_COLOR_BLUE_PIN, CONST_PINS_ON);
    if (onColor & CONST_PINS_BEEP_MASK)
      digitalWrite(CONST_PINS_BEEP_PIN, CONST_PINS_ON);
    delay(delayTime);
	LED_OFF();
    delay(delayTime);
  }
}


void  miscPINs::LED_OFF(uint8_t onColor,short loopCount,short delayTime)
{
  if ((loopCount < 0)||(delayTime <0))
	  return;
  while(loopCount > 0)
  {
	loopCount--;
    if (onColor & CONST_PINS_COLOR_RED_MASK)
      digitalWrite(CONST_PINS_COLOR_RED_PIN, CONST_PINS_OFF);
    if (onColor & CONST_PINS_COLOR_GREEN_MASK)
      digitalWrite(CONST_PINS_COLOR_GREEN_PIN, CONST_PINS_OFF);
    if (onColor & CONST_PINS_COLOR_BLUE_MASK)
      digitalWrite(CONST_PINS_COLOR_BLUE_PIN, CONST_PINS_OFF);
    if (onColor & CONST_PINS_BEEP_MASK)
      digitalWrite(CONST_PINS_BEEP_PIN, CONST_PINS_OFF);
    delay(delayTime);
  }
}


void  miscPINs::LED_Blink()
{
  LED_ON();
  delay(CONST_PINS_COLOR_BLINK_DELAY);
  LED_OFF();
}

void  miscPINs::LED_poweron()
{
  for(int i=0;i<CONST_PINS_POWERON_LOOP_COUNT;i++)
  {
    LED_ON();
    delay(CONST_PINS_COLOR_50MS_DELAY);
    LED_OFF();
    delay(CONST_PINS_COLOR_BLINK_DELAY);
  }
}


void  miscPINs::display_PINS_color(void)
{
  //DBGPRINTF("\ndisplay_PINS_color:[%d]",gu8LEDStatus);
  if (gu8LEDStatus == CONST_PINS_STATUS_ALWAYSOFF)
  {
    //DBGPRINTLN("DISPLAY ALWAYS OFF");
    LED_OFF();
    //DBGPRINT("_");

  }
  else if  (gu8LEDStatus == CONST_PINS_STATUS_ALWAYSON)
  {
    //DBGPRINTLN("DISPLAY ALWAYS ON");
    LED_ON();
    //DBGPRINT("T");
  }
  else
  {
    if (gu8LEDLoopCount < asLedDisplay[gu8LEDStatus].blinkLoop[gu8LEDDispCount])
      //if (gu8LEDLoopCount == asLedDisplay[gu8LEDStatus].blinkLoop[gu8LEDDispCount])
    {
      if ((gu8LEDDispCount % 2) == 0)
      {
        //LED_ON();
        LED_ON(asLedDisplay[gu8LEDStatus].onColor);
        //digitalWrite(CONST_PINS_COLOR_RED_PIN, asLedDisplay[gu8LEDStatus].onColor & CONST_PINS_COLOR_RED_MASK );
        //digitalWrite(CONST_PINS_COLOR_GREEN_PIN, asLedDisplay[gu8LEDStatus].onColor & CONST_PINS_COLOR_GREEN_MASK);
        //digitalWrite(CONST_PINS_COLOR_BLUE_PIN, asLedDisplay[gu8LEDStatus].onColor & CONST_PINS_COLOR_BLUE_MASK);
        //DBGPRINT("T");
      }
      else
      {
        //DBGPRINTLN("DISPLAY OFF");
        LED_OFF();
        //digitalWrite(CONST_PINS_COLOR_RED_PIN, LOW);
        //digitalWrite(CONST_PINS_COLOR_GREEN_PIN, LOW);
        //digitalWrite(CONST_PINS_COLOR_BLUE_PIN, LOW);
        //DBGPRINT("_");
      }
      //DBGPRINT(gu8LEDDispCount);

    }
    else {
      gu8LEDDispCount++;
      if (gu8LEDDispCount >= asLedDisplay[gu8LEDStatus].blinkCount)
      {
        gu8LEDDispCount = 0;
        if (asLedDisplay[gu8LEDStatus].loopFlag != CONST_PINS_LOOP_ALWAYS)
        {
          gu8LEDStatus = CONST_PINS_STATUS_ALWAYSOFF;
          //DBGPRINTF("!!!!!1(asLedDisplay[gu8LEDStatus].loopFlag [%d] != CONST_PINS_LOOP_ALWAYS)[%d]",asLedDisplay[gu8LEDStatus].loopFlag,CONST_PINS_LOOP_ALWAYS);
        }
      }
    }

    gu8LEDLoopCount++;
    if (gu8LEDLoopCount >= asLedDisplay[gu8LEDStatus].blinkLoop[asLedDisplay[gu8LEDStatus].blinkCount - 1])
    {
      gu8LEDLoopCount = 0;
      gu8LEDDispCount = 0;
      if (asLedDisplay[gu8LEDStatus].loopFlag != CONST_PINS_LOOP_ALWAYS)
      {
        gu8LEDStatus = CONST_PINS_STATUS_ALWAYSOFF;
        //DBGPRINTF("!!!!!2(asLedDisplay[gu8LEDStatus].loopFlag != CONST_PINS_LOOP_ALWAYS)[%d]",asLedDisplay[gu8LEDStatus].loopFlag,CONST_PINS_LOOP_ALWAYS);
      }

    }
  }
  //delay(0);
}

