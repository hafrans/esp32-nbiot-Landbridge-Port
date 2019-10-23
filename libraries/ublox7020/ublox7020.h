/*
   ublox7020.h

   Library for ublox 7020 
    ---

    Copyright (C) 2019/08/11  Steven Lian (steven.lian@gmail.com)    

*/

#pragma once
#ifndef _GPS_UBLOX_H
#define _GPS_UBLOX_H

#include <Arduino.h>

//#include <ESPSoftwareSerial.h> //ESP8266
//#include <SoftwareSerial.h> //ESP32
#include "ListArray.h"
#include "nmea.h"

extern "C" {
#include "miscCommon.h"
}

#define CONST_APP_GPS_UBLOX_VERSION 190601

#define CONST_UBLOX_SIO_BAUD_DATA 9600 //软串口速度
#define CONST_UBLOX_SIO_BUSY_TIME_IN_MS 100 

// header defining the interface of the source.
//#define APP_UBLOX_DEVICE_SERIAL Serial //for ESP8266
#define APP_UBLOX_DEVICE_SERIAL Serial //for ESP32
//#define APP_UBLOX_DEVICE_SERIAL Serial2 //for ESP32 IO16 = RXD2 IO17 = TXD2
//#define APP_UBLOX_DEVICE_SERIAL serialDeviceGPS


//#define CONST_UBLOX_RX_DEVICE_PORT 4         // GPIO 4 D2 //ESP8266
//#define CONST_UBLOX_TX_DEVICE_PORT 2         // GPIO 2 D4 //ESP8266

#define CONST_APP_GPS_UBLOX_RECV_FILE_NAME "/ublox7020.ini"
#define CONST_APP_GPS_UBLOX_RECV_FILE_SIZE 1024


#define CONST_APP_GPS_UBLOX_SIO_MAX_LENGTH 240
#define CONST_APP_GPS_UBLOX_SIO_BEGIN '$'
#define CONST_APP_GPS_UBLOX_SIO_END '\n'

//ClASS BEGIN

class ublox : public NMEA
{
  public:
    //data
    bool bSendBusyFlag = false;

    bool bStatChangeFlag; //状态变换指示器,这个是说本机状态改变
	
    uint8_t au8recvBuff[CONST_APP_GPS_UBLOX_SIO_MAX_LENGTH+2];
    uint8_t au8sendBuff[CONST_APP_GPS_UBLOX_SIO_MAX_LENGTH+2];
    
    short sendDataLen;

    short nSIOErrCount; //串口通信故障计数

    uint8_t recvData[CONST_APP_GPS_UBLOX_SIO_MAX_LENGTH+2];
	
    unsigned long ulRecvTimeOutTick;
	short nRecvLen;
  	short tailCount;
  	bool finishFlag;
	

    //device current status

    //function
    
    ublox();
    bool begin();
    void loop();
    bool devReady();
    short available(); //检测是否有数据
	void handle_display_data();
	
  	void handle_device_data();
    
    void debug_print_info();

    //功能调用

    //test only

  private:
    //data
    bool deviceReady;
    bool gpsReady;
    unsigned long dataCheckTicks;

    //func
    bool is_device_exist();
    
    bool is_state_change(short currVal,short preVal,short threshold);
	//uint8_t calc_checkSum(stUniconLockerData *dataPtr);

    void clean_SIO_data();
};

//class end


#endif // _GPS_UBLOX_H

