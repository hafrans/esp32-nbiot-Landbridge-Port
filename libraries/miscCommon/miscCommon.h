/*
    miscCommon.h

   Library implementing of some common function in ESP8266.

    ---

    Copyright (C) 2017  Steven Lian (steven.lian@gmail.com)

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

// header defining the interface of the source.
// include Arduino basic header.
#include <Arduino.h>

#ifndef _MISCCOMMON_H
#define _MISCCOMMON_H

//modified on 2017/4/5

/*====================================================================================*/
//DEBUG_PART_BEGIN Ver=20161104
/*====================================================================================*/
//#define DEBUG_SIO Serial1
#define DEBUG_SIO Serial


#if defined(DEBUG_SIO)
  #define SERIAL_DEBUG_BEGIN DEBUG_SIO.begin(115200);
  #define DBGPRINT(__VA_ARGS__) DEBUG_SIO.print(__VA_ARGS__)
  #define DBGPRINTLN(__VA_ARGS__) DEBUG_SIO.println(__VA_ARGS__)
  #define DBGPRINTF(fmt,...) DEBUG_SIO.printf(fmt,__VA_ARGS__)
  
#else
  #define SERIAL_DEBUG_BEGIN
  #define DBGPRINT(__VA_ARGS__)
  #define DBGPRINTLN(__VA_ARGS__)
  #define DBGPRINTF(fmt,...)
#endif


/*
#define DBGPRINT(__VA_ARGS__)
#define DBGPRINTLN(__VA_ARGS__)
#define DBGPRINTF(fmt,...)
*/

/*====================================================================================*/
//DEBUG_PART_END
/*====================================================================================*/

#define CONST_SYS_RESTART_REASON_NORMAL_BOOT 0    
#define CONST_SYS_RESTART_REASON_REGULAR_REBOOT 1    
#define CONST_SYS_RESTART_REASON_NOPOST_TIME_OUT 2    
#define CONST_SYS_RESTART_REASON_SW_UPGRADE 3    
#define CONST_SYS_RESTART_REASON_NETWORK_ERROR 4    
#define CONST_SYS_RESTART_REASON_WIFI_FAIL 5    
#define CONST_SYS_RESTART_REASON_FACTORY_TEST_TIME_OUT 6    
#define CONST_SYS_RESTART_REASON_LORA_CONFIG_DONE_REBOOT 7
#define CONST_SYS_RESTART_REASON_LORA_NO_SYNC_DATA 8
#define CONST_SYS_RESTART_REASON_LORA_ADDR_CLEAN_UP 9
#define CONST_SYS_RESTART_REASON_LORA_CONFIG_TIME_OUT_REBOOT 10
#define CONST_SYS_RESTART_REASON_LORA_FREQ_CHANGE_REBOOT 11
#define CONST_SYS_RESTART_REASON_LORA_DEVICE_IS_NOT_EXIST 12
#define CONST_SYS_RESTART_REASON_LORA_CLIENT_NO_HOST_TIME_OUT_REBOOT 13
#define CONST_SYS_RESTART_REASON_LORA_ADDR_CONFLICT_REBOOT 14


/////////////////////////

#define MAX(x, y)     (((x) > (y)) ? (x) : (y))
#define MIN(x, y)     (((x) < (y)) ? (x) : (y))


unsigned long ulReset_interval();
unsigned long ulAdd_interval(unsigned long addTick);
unsigned long ulGet_interval(unsigned long checkTick);

unsigned long ulReset_microseconds();
unsigned long ulAdd_microseconds(unsigned long addTick);
unsigned long ulGet_microseconds(unsigned long checkTick);

unsigned short nReset_seconds();
unsigned short nGet_seconds(unsigned short checkSeconds);
unsigned short nAdd_seconds(unsigned short addSeconds);

void second_loop();

/*
   conver a hex data to ascii format
   ex: 0x1--> 0x31 ('1')
*/
uint8_t hex(int nHex); //   ex: 0x1--> 0x31 ('1')

int int16(char *hex);
void convert_to_ip_format(char *ptr, uint8_t *ip, short nLen);
void convert_to_dec_format(char *ptr, uint8_t *data, short nLen);
void convert_to_hex_format(char *ptr, uint8_t *data, short nLen);


//return the pointer to trimed string;
char * strTrim(char *str);

// string format, convert, ...
uint8_t ascii(int hex);
uint8_t ascii_hex(uint8_t cc);
//char isprint (unsigned char c);
char * strTrim(char *str); // trim function
int strchrPos(char achData[], char chData); // enhance of strchr, return the position instead of char * pointer
int nGet_json_pos(char achData[], char achKey[], char **spPos);// split a string(achData) by key (achKey)


//把一个字符串ptr,按照chSplit字符切割，并且返回chSplit下一个位置地址
char *get_str_split_with_len(char *ptr, char **spData, char chSplit,short nMaxLen);
char *get_char_split(char *ptr, char *spData, char chSplit);
char *get_short_split(char *ptr, short *nData, char chSplit);
char *get_int_split(char *ptr, int *nData, char chSplit);
char *get_long_split(char *ptr, long *nData, char chSplit);


//void print_hex_data(char *title, uint8_t *ptr, short nLen);

#endif // _MISCCOMMON_H


