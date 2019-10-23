/*
   ublox7020.h

   Library for ublox 7020
    ---

    Copyright (C) 2019/08/11  Steven Lian (steven.lian@gmail.com)

*/

#include "ublox7020.h"


//static SoftwareSerial serialDeviceGPS(CONST_UBLOX_RX_DEVICE_PORT, CONST_UBLOX_TX_DEVICE_PORT,false,256);

#define RECEIVE_BUFFER_COUNT 16
static ListArray recvList(RECEIVE_BUFFER_COUNT, CONST_APP_GPS_UBLOX_SIO_MAX_LENGTH + 2);

ublox::ublox()
{
}


bool ublox::begin()
{
  bool ret = true;
  APP_UBLOX_DEVICE_SERIAL.begin(CONST_UBLOX_SIO_BAUD_DATA);

  short nTimes = 5;
  while (nTimes > 0) {
    delay(1);
    deviceReady = is_device_exist();
    if (deviceReady) {
      DBGPRINTLN("\n ublox is ready!!!");
      break;
    }
    delay(1);
    nTimes--;
  }
  return ret;
}


bool ublox::is_state_change(short currVal, short preVal, short threshold)
{
  int32_t diff;
  diff = currVal - preVal;
  if (diff < 0) {
    diff = -diff;
  }
  if (diff >= threshold )  {
    bStatChangeFlag = true;
    DBGPRINTF("\n-> Stat changed: currVal [%d],preVal [%d],diff [%d],threshold [%d]", currVal, preVal, diff, threshold);
  } else {
    bStatChangeFlag = false;
  }
  return bStatChangeFlag;
}


void ublox::handle_device_data()
{
  uint16_t cmd;
  char buff[1000];
  DBGPRINT("\n-> handle_device_data ");
  recvList.rpop(recvData);
  nmea_type = getNmeaType((char *) recvData);
  switch (nmea_type)
  {
    case NMEA_TYPE_RMC:
    case NMEA_TYPE_GGA:
      parse((char *) recvData);
      if (latitude.value > 0)
        gpsReady = true;
      else
        gpsReady = false;
      break;
    default:
      break;
  }
  DBGPRINTF("\n-> current GPS latitude:[%d][%d]", latitude.value, latitude.scale);
  DBGPRINTF("\n-> current GPS longitude:[%d][%d]", longitude.value, longitude.scale);
  DBGPRINTF("\n-> current GPS speed:[%d][%d]", speed.value, speed.scale);
  //DBGPRINTF("\n %d [%04X] [%s]",sendStru.len,cmd,buff);

}



void ublox::loop()
{
  available();
  //if (recvList.len() && ulGet_interval(ulRecvTimeOutTick) > CONST_UBLOX_SIO_BUSY_TIME_IN_MS)
  if (recvList.len() > 0)
  {
    handle_device_data();
  }
}


short ublox::available()
{
  short ret = -1;
  if (APP_UBLOX_DEVICE_SERIAL.available()) {
    uint8_t chR;
    chR = APP_UBLOX_DEVICE_SERIAL.read();
    ulRecvTimeOutTick = ulReset_interval();
    //DBGPRINTF(" %02X", chR);
    //DBGPRINTF("%c", chR);
    if (chR == CONST_APP_GPS_UBLOX_SIO_BEGIN) {
      nRecvLen = 0;
    }
    au8recvBuff[nRecvLen++] = chR;

    if (nRecvLen >= CONST_APP_GPS_UBLOX_SIO_MAX_LENGTH) { //防止缓冲区溢出
      nRecvLen = 0;
    }
	
    //判断帧尾
    if (chR == CONST_APP_GPS_UBLOX_SIO_END)
    {
      au8recvBuff[nRecvLen++] = 0;
      ret = nRecvLen;
      recvList.lpush(au8recvBuff);
      //DBGPRINTF("\n-> nRecvLen:[%d][%d] [%s]", nRecvLen, recvList.len(), au8recvBuff);
    }

    //得到完整的帧尾
  }
  return ret;
}


void ublox::clean_SIO_data()
{
  /*
  while (APP_UBLOX_DEVICE_SERIAL.available()) {
    APP_UBLOX_DEVICE_SERIAL.flush();
    ; // wait for serial port to connect. Needed for Leonardo only
  };
  */
  APP_UBLOX_DEVICE_SERIAL.flush();
  nRecvLen = 0;
}

bool ublox::devReady()
{
  return deviceReady;
}



bool ublox::is_device_exist()
{
  bool ret = false;
  DBGPRINT("\n is_device_exist : ");
  //clean_SIO_data();
  clean_SIO_data();
  ret = true;
  return ret;
}


void ublox::debug_print_info()
{
  DBGPRINT("\n--- debug_print_info begin ----");
  DBGPRINT("\n--- debug_print_info end ----");

}


