/*
   NBIoTBC26.h

   Library to NBIoTBC26 AT command interface
   To follow the spec of CMCC/CU/CT
    ---

    Copyright (C) 2019  Steven Lian (steven.lian@gmail.com)
    modified on 2019/10/08

*/

#pragma once
#ifndef _NBIoTBC26_H
#define _NBIoTBC26_H


#include <Arduino.h>

//WIT_MOTION CLASS BEGIN
#include <FS.h> //for esp8266
#include <SPIFFS.h> //for ESP32
//#include <ESPSoftwareSerial.h> //for ESP8266
#include <SoftwareSerial.h> //for ESP32
#include "ListArray.h"
#include "Timenew.h"

extern "C" {
#include "simpleEncode.h"
#include "miscCommon.h"
}

#include "NBIoTBC26_def.h"

#define CONST_APP_NBIOT_BC26_VERSION 190521

// BC26的串口电平是5V,ESP8266串口电平是3.3V 如果没有电压转换，容易造成误码
#define CONST_AT_SIO_BAUD_DATA 9600 //软串口速度
#define CONST_AT_SIO_BUSY_TIME_IN_MS 10

#define CONST_AT_BC26_ECHO_TIME_OUT 1000

// header defining the interface of the source.
#define APP_SERIAL Serial2 //for ESP32 IO16 = RXD2 IO17 = TXD2
//#define APP_SERIAL Serial //for ESP8266
//#define APP_SERIAL serialSIM800 //for ESP8266

//#define CONST_APP_RX_PORT 5         // GPIO 5 D1 ESP8266
//#define CONST_APP_TX_PORT 14         // GPIO 14 D5

//#define CONST_APP_RX_PORT 14         // GPIO 14 D5 ESP8266
//#define CONST_APP_TX_PORT 5         // GPIO 5 D1

//#define CONST_APP_RX_PORT 18         // GPIO 18 ESP32
//#define CONST_APP_TX_PORT 19         // GPIO 19 
#define CONST_APP_RX_PORT 19         // GPIO 18 ESP32
#define CONST_APP_TX_PORT 18         // GPIO 19 
#define CONST_APP_PEN_PORT 5         // GPIO 5
#define CONST_APP_EINT_PORT 33         // GPIO 33

#define CONST_AT_PEN_ENABLE HIGH // 拉高PEN引脚至少500ms使模块开机
#define CONST_AT_PEN_DISABLE LOW 
#define CONST_AT_PEN_TIME_IN_MS 600


#define CONST_APP_NBIOT_RECV_FILE_NAME "/NBIoTBC26.ini"


//ClASS BEGIN

class NBIOT
{
  public:
    //data
    bool bSendBusyFlag = false;

    bool bStatChangeFlag; //状态变换指示器,这个是说本机状态改变
    bool bNeedReset; // 需要reset BC26
    short dataCount; //内部数据缓存计数.
    short errorCount; //错误计数器
	short responseErrorCount; //回送计数器

    uint8_t _workMode;
    uint8_t saveWorkMode;
    uint8_t _codingType;

    uint8_t au8recvBuff[CONST_AT_CMD_BUFF_SIZE*2];
    uint8_t au8sendBuff[CONST_AT_CMD_BUFF_SIZE*2];
    short sendDataLen;

    stATQueryCMD sendStru; //发送结构体描述
    stATQueryCMD recvStru; //接收结构体描述

    stDataStruct sendData; //发送数据结构体
    stDataStruct recvData; //接收数据结构体

    short recvDataCount;//接收数据个数.

    short nSIOErrCount; //串口通信故障计数

    //uint8_t bigBuff[CONST_APP_METER_RTU_201_RECV_BIG_BUFFLEN + 2];

    short nRecvLen;
    unsigned long ulRecvTimeOutTick;
    // application data save cycle by minutes,default = 60 mins
    unsigned long ulSaveTick;

    char IMSI[CONST_IMSI_NUMBER_LENGTH];
    char IMEI[CONST_IMEI_NUMBER_LENGTH];
    char saveIMEI[CONST_IMEI_NUMBER_LENGTH];
    short funcStatus;

    //uint32_t _devID;
    char _host[CONST_HOST_ADDRESS_LENGTH];
    char _APN[CONST_APN_LENGTH];
    char _localIP[CONST_IP_NUMBER_LENGTH];
    uint16_t _rPort;//remote port
    uint16_t _lPort;//local port
    uint16_t _band; //default band
    short socketNo;
	
	short nbRssi; //信号强度

    uint8_t SN;

    //function
    NBIOT();
    bool begin(char workMode);
    bool reInitModem();
	
    bool begin(char workMode, char *host,char *apn,unsigned short rPort, unsigned short lPort,void (* cbP) () = NULL);
    bool begin(char workMode, char *host,char *apn,unsigned short rPort, unsigned short lPort,bool reInitFlag = false,void (* cbP) () = NULL);

    bool resetModem();
    bool init_UDPTCP();
    bool init_Coap();
    bool init_CT();

    void loop();
    void func_100ms_loop();
    void func_second_loop();
    void func_minute_loop();
    void func_hour_loop();
    void func_day_loop();
    bool devReady();
    bool modemReady();
    void determine_device_status();
    void get_modem_time();
    void recv_data_check();
    short recv_data_read(short nLen, uint8_t *buff);
    short available(); //检测是否有数据
    short read_with_wait(short microSeconds);
    short read_with_wait(short microSeconds,short LFCount);
    short pop_data(uint8_t *buff);

    void send_CMD(short nLen, char *dataPtr);//发送给device的命令到给device
    short send_data(short nLen, uint8_t *dataPtr);
    //short send_data_udp(short nLen, uint8_t *dataPtr);
    //short send_data_udp_old(short nLen, uint8_t *dataPtr);
    //short send_data_tcp(short nLen, uint8_t *dataPtr);
    short send_data_tcpudp(short socketType,short nLen, uint8_t *dataPtr);
    short send_data_tcpudp_old(short socketType,short nLen, uint8_t *dataPtr);
    short send_data_Coap(short nLen, uint8_t *dataPtr);
    short send_data_CT(short nLen, uint8_t *dataPtr);
    void trans_data_HEX(short nLen, uint8_t *fromBuff, uint8_t *toBuff);
    void trans_HEX_data(short nLen, uint8_t *fromBuff, uint8_t *toBuff);
    short calc_msg_mode(short nLen);
    short encode_data(char *targetData,char *srcData,short nLen);
    short decode_data(uint8_t localCodingType, char *targetData,char *srcData,short nLen);
    short send_data_to_modem();


    void debug_print_info();

    bool bLoad_config();
    bool bSave_config();
    
    //功能调用
    bool is_device_exist();
    bool is_device_attached();
    bool is_device_singalOK();
    bool is_conn_actived();
    
    void init_modem();

    //test only

  private:
    //data
    bool deviceReady;
    bool attachedFlag;
    bool modemExist;
    short lastCMD;
    
    unsigned long statusCheckTicks;
    unsigned long dataCheckTicks;
    unsigned long gl100msLoopTick;
    unsigned long ulTimeSecondTick;

    uint8_t preDay;
    uint8_t preHour;
    uint8_t preMinute;
	
    void (* cbFunc) (void);
	
    //func
    bool gen_AT_CMD(short cmd);
    bool gen_AT_CMD(short cmd,short val,char *spStr);
    char *find_str(char *spData,char *keys,char tailChar);

    short decode_AT_CMD(short cmd,char *spData);
    short decode_others(char *spData);
    short decode_CFUN(char *spData);
    short decode_CGSN(char *spData);
    short decode_CIMI(char *spData);
    short decode_CESQ(char *spData);
    short decode_CEREG(char *spData);
    short decode_CGATT(char *spData);
    short decode_IP(char *spData);
    short decode_CGPADDR(char *spData);
    short decode_CCLK(char *spData);
    short decode_QSONMI(char *spData);
    short decode_QSOC(char *spData);
    short decode_QIOPEN(char *spData);
    short decode_QISENDEX(char *spData);
    short decode_QIRD(char *spData);
    short decode_QSOSEND(char *spData);
    short decode_QSORF(short nLen, char *IPAddr, char *spData, char *buff);
    short decode_OKERROR(char *spData);
    
    //基础函数  
    short read_IMEI();
    short read_IMSI();
    short read_IP();
    
    short socket_create();
    short socket_close();
    short socket_send_data(char *spData);
    
    void clean_SIO_data();
    bool is_state_change(uint32_t currVal,uint32_t preVal,uint32_t threshold);
	//带回调的delay函数
	void delay_cb(unsigned long);
};

//class end

extern LVTime gTime;
extern LVTime gPreTime;
extern void external_loop_process();
extern void enable_interrupts();
extern void disable_interrupts();

#endif // _NBIoTBC26_H

