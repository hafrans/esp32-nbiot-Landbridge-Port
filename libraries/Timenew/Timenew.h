#include "TimeLib.h"
#include <FS.h>
#include <SPIFFS.h>

//modified by Steven Lian BEGIN
#pragma once
#ifndef _TIME_NEW_H
#define _TIME_NEW_H

#define CONST_TIME_REBOOT_ADJUST_SECOND 1
#define CONST_TIME_RECORD_FILE_BUF_LEN 24
#define CONST_TIME_RECORD_YEAR_POS 0
#define CONST_TIME_RECORD_MONTH_POS 4
#define CONST_TIME_RECORD_DAY_POS 6
#define CONST_TIME_RECORD_HOUR_POS 8
#define CONST_TIME_RECORD_MINUTE_POS 10
#define CONST_TIME_RECORD_SECOND_POS 12

#define CONST_TIME_RECORD_FILE_NAME "/datetimerecord.ini"

//the time struct, it is the timer system in this project.
typedef struct {
  short year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t wday;
  int8_t timezone2; //2 x timezone , consider india GMT +7.5= 15
  char timeString[20];
  bool synced;
  //keep preInformation
} LVTime;

void setTime(int hr,int min,int sec,int dy, int mnth, int yr);

void vGet_time();
bool bSave_time_to_file(); //在设备重启之前用
bool bLoad_time_to_file(); //在设备重启之后用
void set_time_zone(short tZone2); //tZone=time zone *2 


//extern gTime,gPreTime;
#endif
//modified by Steven Lian END