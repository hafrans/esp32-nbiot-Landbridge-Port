/*
   modified on 2019/10/10
   this is a new version, which imporve the loop logic and structure.
*/
#include <arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>
#include <HTTPUpdate.h>

#include <ArduinoJson.h>

#include "Timenew.h"
//beep control and led control module
#include "miscPINs.h"
//GPS module
#include "ublox7020.h"
// NB BC26 module
#include "NBIoTBC26.h"

extern "C" {
#include "miscCommon.h"
}

/*================================================================================*/
//MAIN_PART_BEGIN
/*================================================================================*/
//DON'T CHANGE THE MAIN PART CODE

#define FORMAT_SPIFFS_IF_FAILED true
#define CONST_APP_GPSWARN_VERSION 1900000

extern LVTime gTime;

#define CONST_IMEI_LENGTH 20
char IMEI[CONST_IMEI_LENGTH + 2];//=devID device ID

// function definition
void setup();
void loop();
void check_sys_data();
void vGenerate_IMEI();
short wifi_upgrade_software(char *ssid, char *pass, char *url);
void timer_init();
void disable_interrupts();
void enable_interrupts();
void read_GPS_send_data();
void check_system_cmd();
void vAction_before_restart();
void vApplication_setup_call_before_main();
void vApplication_setup_call_after_main();
void vApplication_connected_loop_call();
void func_100ms_loop();
void func_second_loop();
void func_minute_loop();
void func_hour_loop();
void func_day_loop();
void func_INIT_STATE();
void func_CONNECTING_STATE();
void func_CONNECTED_STATE();
void func_WARNING_STATE();
void func_FAST_WARNING_STATE();
void func_ERROR_STATE();
void func_UPGRADE_STATE();
void vApplication_main_loop_call();
bool bLoad_application_data();
bool bSave_application_data();
void external_loop_process();


//global var
bool bRequestSystemReboot = false;
#define CONST_REBOOT_INTERVAL_LEN (1000*30) //设备重启必须在开机30秒钟以后,以保证能够处理基本的一些检测业务

void setup()
{
  SERIAL_DEBUG_BEGIN    //Serial begin 115200
  Serial.begin(CONST_UBLOX_SIO_BAUD_DATA);
  DBGPRINTF("\n-> HELLO NBIOT+GPS+LED+WARNING Device Ver:[%d]",CONST_APP_GPSWARN_VERSION);
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
  {
    DBGPRINTLN("Failed to open config file for writing, then format ... ...");
    //SPIFFS.format();
  }

  //turn radio off
  //WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_AP_STA);
  btStop();

  bLoad_time_to_file();

  DBGPRINTF("\n-> gTime.timeString [%s]", gTime.timeString);
  randomSeed(micros());
  vGenerate_IMEI();
  vApplication_setup_call_before_main();
  set_time_zone(8 * 2); //默认设备时区是东八区
  check_sys_data();
  DBGPRINTLN("+--------------------------------------------+");
  DBGPRINTLN("+HELLO, THIS FIRMWARE WAS COMPILED BY HAFRANS+");
  DBGPRINTLN("+           Date: 10/22/2019 Build 2         +");
  DBGPRINTLN("+--------------------------------------------+");
  vApplication_setup_call_after_main();
}

void loop()
{
  vApplication_main_loop_call();
  if ((bRequestSystemReboot == true ) && (millis() > CONST_REBOOT_INTERVAL_LEN)) {
    //系统restart之前必须保留的数据
    vAction_before_restart();
    delay(random(200));
    ESP.restart();
  }
}

/*================================================================================*/
//MAIN_PART_END
/*================================================================================*/

/*====================================================================================*/
//APPLICATION_PART_BEGIN Ver=20161116
/*====================================================================================*/

/*--------------------------------------------------------------------------------*/
//APPLICATION_DATA_PART_BEGIN
/*--------------------------------------------------------------------------------*/
// 不要修改下面的CONST_DEVICE_VERSION的值,一类设备对应于一个值,修改这个设备的值,会造成软件升级失败
#define CONST_DEVICE_VERSION "50_6_20180310"
/* 软件版本解释：
   LVSW_GATE_A_S01_V1.0.1_20170301
   软件_设备类型_子类型_子参数_版本_日期
   例如： GATE==网关,探针类别; 子类型=A; 子参数=S01(网关扫描参数 CONST_HOMEAP_SPLIT =1),
   硬件版本解释：
   LVHW_GATE_4M1M_V1.0.0_20161116
   软件_设备类型_内存选择参数_版本_日期
*/

#define CONST_DEVICE_CHIP "ESP32"
#define CONST_DEVICE_MEMORY_MAP "4M1M"
#define CONST_DEVICE_SWVN "LVSW_NBGPS_%s_V0.3.0_20191012"
#define CONST_DEVICE_HWVN "LVHW_NBGPS_%s_%s_V1.1.0_20190910"
#define CONST_DEVICE_MAID "50"
#define CONST_DEVICE_PID "9"
#define CONST_DEVICE_CAT "npGp"

#define CONST_IMEI_PREFIX_1 "83190"
#define CONST_IMEI_PREFIX_2 "811"

// application data save cycle by minutes,default = 60 mins
#define CONST_APPLICATION_SAVE_CYCLE (1000*60*60)
unsigned long glApplicationSaveTick;

#define CONST_APPLICATION_STATUS_CHANGE_INTERVAL  (1000*60*2)
#define CONST_APP_STATUS_CHANGE_INTERVAL_MIN_SECOND  (2)
unsigned long glApplicationStatusChangeTick;
unsigned long glAppStatusChangeInterval;

unsigned long gulApplicationTicks;
#define CONST_READ_INTERVAL 5*1000

#define CONST_BIG_BUFF_LEN 1000

char bigBuff[CONST_BIG_BUFF_LEN];

//define status values
#define CONST_GPS_STATUS_INIT_STATE 1
#define CONST_GPS_STATUS_CONNECTING_STATE 2
#define CONST_GPS_STATUS_CONNECTED_STATE 3
#define CONST_GPS_STATUS_WARNING_STATE 4
#define CONST_GPS_STATUS_FAST_WARNING_STATE 5
#define CONST_GPS_STATUS_SLOW_STATE 6
#define CONST_GPS_STATUS_ERROR_STATE 7
#define CONST_GPS_STATUS_UPGRADE_STATE 8

uint8_t gchMainState = CONST_GPS_STATUS_INIT_STATE; //系统状态标志

unsigned gl100msLoopTick; //100ms 循环计数器
unsigned ulTimeSecondTick; //1000ms循环计数器
short preMinute;
short preHour;
short preDay;


// wifi upgrade begin
//modified on 2019/9/18
#define WIFI_UPGRADE_LED_PIN 4
#define WIFI_UPGRADE_TRY_TIMES 5

static WiFiMulti WiFiMulti;

short wifi_upgrade_software(char *ssid, char *pass, char *url)
{
  short tryTimes = WIFI_UPGRADE_TRY_TIMES;
  DBGPRINT("\n-> wifi_upgrade_software");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, pass);

  while (tryTimes > 0)
  {
    tryTimes--;
    // wait for WiFi connection
    if ((WiFiMulti.run() == WL_CONNECTED))
    {
      DBGPRINTF("\n-> wifi is connected, url[%s]", url);
      WiFiClient client;

      // The line below is optional. It can be used to blink the LED on the board during flashing
      // The LED will be on during download of one buffer of data from the network. The LED will
      // be off during writing that buffer to flash
      // On a good connection the LED should flash regularly. On a bad connection the LED will be
      // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
      // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
      //httpUpdate.setLedPin(WIFI_UPGRADE_LED_PIN, LOW);

      t_httpUpdate_return ret = httpUpdate.update(client, url);
      // Or:
      //t_httpUpdate_return ret = httpUpdate.update(client, "server", 80, "file.bin");

      switch (ret) {
        case HTTP_UPDATE_FAILED:
          DBGPRINTF("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
          break;

        case HTTP_UPDATE_NO_UPDATES:
          DBGPRINT("HTTP_UPDATE_NO_UPDATES");
          break;

        case HTTP_UPDATE_OK:
          DBGPRINT("HTTP_UPDATE_OK");
          break;
      }
    }
    delay(1000);
    DBGPRINTF("\n-> tryTimes[%d]", tryTimes);
  }
  //DBGPRINTF("\n-> bRequestSystemReboot[%d]", bRequestSystemReboot);
  //bRequestSystemReboot = true;
}

// wifi upgrade end

// wifi scan network begin

#define WIFI_DEFAULT_TARGET_SSID "avoid_subject_wifi_no_internet"
#define WIFI_DEFAULT_RSSI_THRESHOLD -75

// scan network to find given ssid
bool wifi_scan_network(char *ssid, short rssiThreshold)
{
  bool ret = false;
  short scanedNum;
  short rssi = -1000;

  DBGPRINT("\n-> wifi_scan_network");

  scanedNum = WiFi.scanNetworks();

  for (short k = 0; k < scanedNum; k++)
  {
    if (!strcmp((const char *) WiFi.SSID(k).c_str(), (const char *) ssid))
    {
      // SSID 名字匹配
      rssi = WiFi.RSSI(k);
      if (rssi > rssiThreshold)
      {
        ret = true;
        break;
      }
    }
  }
  if (ret)
    DBGPRINTF(" found,[%d]", rssi);
  else {
    DBGPRINT(" not found");
  }
  return ret;
}

// wifi scan network end

//其他数据和代码

ublox gsUblox; //GPS 驱动

NBIOT gsNB; //NBIOT 驱动


#define CONST_APP_BC26_SERVER_ADDR "120.132.55.236" //120.132.55.236;106.75.105.138
#define CONST_APP_BC26_SERVER_PORT 10001
#define CONST_APP_BC26_LOCAL_PORT 10001
#define CONST_APP_BC26_SERVER_TCP_PORT 10000
#define CONST_APP_BC26_LOCAL_TCP_PORT 10000

#define CONST_APP_BC26_BUPT_SERVER_ADDR "39.107.58.68" //120.132.55.236;106.75.105.138
#define CONST_APP_BC26_BUPT_SERVER_PORT 3000
#define CONST_APP_BC26_BUPT_LOCAL_PORT 3000


//应用数据结构体
#define CONST_APP_DEVICE_ID_LEN 18
#define CONST_APP_DEVICE_INFO_LEN 20
#define CONST_APP_LAT_LEN 16

#define CONST_APP_DEVICE_INFO_NORMAL_VAL 0
#define CONST_APP_DEVICE_INFO_WARNING_VAL 1
#define CONST_APP_DEVICE_INFO_ERROR_VAL 2
#define CONST_APP_DEVICE_INFO_FAST_WARNING_VAL 3
#define CONST_APP_DEVICE_INFO_SELF_WARNING_VAL 4

#define CONST_APP_DEVICE_INFO_NORMAL_MSG "Normal"
#define CONST_APP_DEVICE_INFO_WARNING_MSG "Warning"
#define CONST_APP_DEVICE_INFO_ERROR_MSG "Error"
#define CONST_APP_DEVICE_INFO_FAST_WARNING_MSG "Warning3"
#define CONST_APP_DEVICE_INFO_SELF_WARNING_MSG "SelfWarning"

#define CONST_APP_DEVICE_MAX_INTERVAL 600 //最大的通信间隔
#define CONST_APP_DEVICE_SLOW_INTERVAL 60 //无风险时候的通信间隔
#define CONST_APP_DEVICE_DEFAULT_INTERVAL 15 //正常的通信间隔

#define CONST_APP_GPS_DEFAULT_IGNORE_TIMES 3

#define CONST_APP_GPSWARN_FILE_NAME "/nbGPSwarn.ini"

//{"SN","12","DeviceMac":"123456789012345","DeviceInfo":"Normal","GPSN":"9712.3456","GPSE":"12345.6789","WarnSta":"1"}
class GPSWARN
{
  public:
    //data
    long gps_n_int = 0; //lat int
    long gps_n_dec = 0; //lat dec
    long gps_e_int = 0; //lon int
    long gps_e_dec = 0; //lon dec

    unsigned long ulNBConnInterval = CONST_APP_DEVICE_DEFAULT_INTERVAL; //和服务器通信间隔单位秒
    unsigned long ulNBSysConnInterval = CONST_APP_DEVICE_DEFAULT_INTERVAL; //和服务器通信间隔单位秒
    unsigned long ulNBSlowConnInterval = CONST_APP_DEVICE_SLOW_INTERVAL; //和服务器无风险时候通信间隔单位秒
    unsigned long ulNBConnTicks = 0; //NB通信延时计数器
    char *buffPtr;

    short errorCount = 0;

    short warnSta;//告警工作状态.
    short warnCMD;//服务器下发命令

    short preWarnSta;//前一次告警工作状态.
    short preWarnCMD;//前一次服务器下发命令

    bool avoidNetworkDetectedFlag; //需要规避的wifi检测情况

    unsigned char SN;
    char deviceMac[CONST_APP_DEVICE_ID_LEN];
    char deviceInfo[CONST_APP_DEVICE_INFO_LEN];
    char GPSN[CONST_APP_LAT_LEN];
    char GPSE[CONST_APP_LAT_LEN];

    uint8_t alarmLoopCount;//告警循环次数
    uint8_t alarmLoopMode;//告警循环模式, 单位100ms.
    uint8_t _ignoreTimes;
    uint8_t noGPSCount;

    //func
    GPSWARN(char *ptr);
    short decode_ServerData(char *spData);
    void print_info();

    bool bLoad_config();
    bool bSave_config();

  private:
    //data
};


GPSWARN::GPSWARN(char *ptr)
{
  buffPtr = ptr;
  warnSta = CONST_APP_DEVICE_INFO_NORMAL_VAL;
  warnCMD = CONST_APP_DEVICE_INFO_NORMAL_VAL;
  alarmLoopCount = 5;//5次循环
  alarmLoopMode = 5;// 10* 100ms = 1秒
  _ignoreTimes =  CONST_APP_GPS_DEFAULT_IGNORE_TIMES;
  noGPSCount = 0;
  avoidNetworkDetectedFlag = false;

}

//new one, based on AuduinoJson
#define CONST_GPS_WARN_JSON_LEN 1000
StaticJsonDocument <CONST_GPS_WARN_JSON_LEN> jsonDoc;

short GPSWARN::decode_ServerData(char *spData)
{
  short ret = -1;
  short nConn, nWarn;
  unsigned long ulTemp;
  const char *spConn, *spWarn, *spSlow;
  const char *spActionID, *spSSID, *spPASS, *spURL;

  DBGPRINTF("\n-> spData [%s]", spData);
  DeserializationError error = deserializeJson(jsonDoc, spData);

  if (!error)
  {
    // ok Fetch values.
    spActionID = jsonDoc["ActionID"];
    if (spActionID == NULL)
    {
      spConn = jsonDoc["ConnInt"];
      spWarn = jsonDoc["WarnCtl"];
      spSlow = jsonDoc["SlowInt"];
      if (spConn == NULL)
      {
        spConn = jsonDoc["IntervalTime"];
      }

      if ((spConn != NULL) && (spWarn != NULL))
      {

        ulTemp = atol(spConn);
        DBGPRINTF("\n-> spConn[%s]  ulTemp[%d]", spConn, ulTemp);
        if (ulTemp > 0)
        {
          ulNBSysConnInterval = ulTemp;
          if (ulNBSysConnInterval > CONST_APP_DEVICE_MAX_INTERVAL)
            ulNBSysConnInterval = CONST_APP_DEVICE_MAX_INTERVAL;
        }
      }

      if ((spSlow != NULL) && (spWarn != NULL))
      {

        ulTemp = atol(spSlow);
        DBGPRINTF("\n-> spSlow[%s]  ulTemp[%d]", spConn, ulTemp);
        if (ulTemp > 0)
        {
          ulNBSlowConnInterval = ulTemp;
          if (ulNBSlowConnInterval > CONST_APP_DEVICE_MAX_INTERVAL)
            ulNBSlowConnInterval = CONST_APP_DEVICE_MAX_INTERVAL;
        }
      }

      ulTemp = atol(spWarn);
      DBGPRINTF("\n-> spWarn[%s]  ulTemp[%d]", spWarn, ulTemp);
      if (ulTemp >= 0)
      {
        if ((ulTemp != CONST_APP_DEVICE_INFO_ERROR_VAL ) && (ulTemp != CONST_APP_DEVICE_INFO_SELF_WARNING_VAL ))
          warnCMD = ulTemp;
      }
    }
    else
    {
      //upgrade command
      DBGPRINTF("\n-> upgarde command [%s]", spActionID);
      ulTemp = atol(spActionID);
      if (ulTemp == 901)
      {
        //software upgrade command
        spSSID = jsonDoc["SSID"];
        spPASS = jsonDoc["PASS"];
        spURL = jsonDoc["URL"];
        DBGPRINTF("\n-> ssid[%s],pass[%s],url[%s]", spSSID, spPASS, spURL);
        if ((spSSID != NULL) && (spPASS != NULL) && (spURL != NULL))
        {
          wifi_upgrade_software((char *)spSSID, (char *)spPASS, (char *)spURL);
        }
      }
    }
  }
}

void GPSWARN::print_info()
{
  DBGPRINTF("\n-> ==== GPSWARN print_info begin ==== %s", "");
  DBGPRINTF("\n-> warnSta[%d], warnCMD[%d], connInt[%d]", warnSta, warnCMD, ulNBConnInterval);
  DBGPRINTF("\n-> gps_n_int[%d], gps_n_dec[%d], gps_e_int[%d],gps_e_dec[%d] ", gps_n_int, gps_n_dec, gps_e_int, gps_e_dec);
  DBGPRINTF("\n-> ==== GPSWARN print_info end ==== %s", "");
}

/* load application data, return True==Success*/
bool GPSWARN::bLoad_config()
{
  disable_interrupts();
  bool bRet = false;
  int nLen;
  char *spTemp;
  char *bigBuffPtr = (char *) buffPtr;

  File configFile = SPIFFS.open(CONST_APP_GPSWARN_FILE_NAME, "r");
  if (!configFile)
  {
    DBGPRINTF("\n->Failed to open config file :%s", CONST_APP_GPSWARN_FILE_NAME);
    enable_interrupts();
    return bRet;
  }

  // while (True)
  while (configFile.available())
  {
    nLen = configFile.readBytesUntil('\n', bigBuffPtr, CONST_AT_CMD_BUFF_SIZE);
    if (nLen <= 0)
      break;
    bigBuffPtr[nLen - 1] = '\0'; //trim
    spTemp = strchr((char *)bigBuffPtr, ':');
    if (spTemp == NULL)
      break;//not found;
    spTemp++;

    if (memcmp(bigBuffPtr, "DEVID", 5) == 0) {
      strncpy(deviceMac, spTemp, CONST_APP_DEVICE_ID_LEN);
    }
    if (memcmp(bigBuffPtr, "WSTA", 4) == 0) {
      warnSta = atoi(spTemp);
    }
    if (memcmp(bigBuffPtr, "LATI", 4) == 0) {
      gps_n_int = atol(spTemp);
    }
    if (memcmp(bigBuffPtr, "LATD", 4) == 0) {
      gps_n_dec = atol(spTemp);
    }
    if (memcmp(bigBuffPtr, "LONI", 4) == 0) {
      gps_e_int = atol(spTemp);
    }
    if (memcmp(bigBuffPtr, "LOND", 4) == 0) {
      gps_e_dec = atol(spTemp);
    }
  }

  // Real world application would store these values in some variables for later use
  DBGPRINTF("\n->Loaded data: warnSta[%d],deviceMac[%s] LAT[%d.%05d],LON[%d.%05d]", warnSta, deviceMac, gps_n_int, gps_n_dec, gps_e_int, gps_e_dec);
  bRet = true;

  configFile.close();
  DBGPRINTLN("\n->Application Config ok");
  enable_interrupts();
  return bRet;
}


/* save application data, return True = success */
bool GPSWARN::bSave_config()
{
  disable_interrupts();
  char *bigBuffPtr = (char *) buffPtr;

  DBGPRINTLN("\n->--save modem config data--");
  File configFile = SPIFFS.open(CONST_APP_GPSWARN_FILE_NAME, "w");
  if (!configFile)
  {
    DBGPRINTLN("\n->Failed to open config file for writing, then format ... ...");
    SPIFFS.format();
    configFile = SPIFFS.open(CONST_APP_GPSWARN_FILE_NAME, "w");
    if (!configFile)
    {
      DBGPRINTF("\n->Failed to open config file :%s", CONST_APP_GPSWARN_FILE_NAME);
      enable_interrupts();
      return false;
    }
  }

  configFile.print("DEVID:");
  configFile.println(deviceMac);

  configFile.print("WSTA:");
  configFile.println(warnSta);

  configFile.print("LATI:");
  configFile.println(gps_n_int);

  configFile.print("LATD:");
  configFile.println(gps_n_dec);

  configFile.print("LONI:");
  configFile.println(gps_e_int);

  configFile.print("LOND:");
  configFile.println(gps_e_dec);

  configFile.close();
  DBGPRINTLN("\n-> -- end");
  enable_interrupts();
  return true;
}


GPSWARN gpsWarn(bigBuff);


//pins control beep etc
miscPINs beepLed;

//时间中断
hw_timer_t * timer100 = NULL;

void IRAM_ATTR onTimer100()
{
  //beepLed.int_loop();
}

void timer_init()
{
  //timer100 = timerBegin(0, 80, true);
  //timerAttachInterrupt(timer100, &onTimer100, true);
  //timerAlarmWrite(timer100, 100000, true);//100ms
  //timerAlarmEnable(timer100);
}

void disable_interrupts()
{

  //timerAlarmDisable(timer100);
}


void enable_interrupts()
{
  //APP_SERIAL.begin(CONST_AT_SIO_BAUD_DATA); //temp solution
  //APP_UBLOX_DEVICE_SERIAL.begin(CONST_UBLOX_SIO_BAUD_DATA); //temp solution
  //timerAlarmEnable(timer100);
}

//读取当前GPS数据, 并负责发送数据
void read_GPS_send_data()
{

  //read GPS Data
  if ((ulGet_interval(gpsWarn.ulNBConnTicks) > (gpsWarn.ulNBConnInterval * 1000)))
  {
    DBGPRINTF("\n-> read_GPS_send_data %d, %d", gpsWarn.ulNBConnTicks, gpsWarn.ulNBConnInterval);
    gpsWarn.ulNBConnTicks = ulReset_interval();

    if (gsUblox.latitude.scale > 0)
    {
      //有效的GPS数据
      gpsWarn.gps_n_int = gsUblox.latitude.value / gsUblox.latitude.scale;
      gpsWarn.gps_n_dec = gsUblox.latitude.value % gsUblox.latitude.scale;
      gpsWarn.noGPSCount = 0;
    }
    else
    {
      gpsWarn.noGPSCount++;
    }

    if ((gsUblox.longitude.scale > 0) || (gsUblox.latitude.scale > 0))
    {
      //有效的GPS数据
      gpsWarn.gps_e_int = gsUblox.longitude.value / gsUblox.longitude.scale;
      gpsWarn.gps_e_dec = gsUblox.longitude.value % gsUblox.longitude.scale;
      gpsWarn.noGPSCount = 0;
    }
    else {
      gpsWarn.noGPSCount++;
    }

    if (gpsWarn.noGPSCount > gpsWarn._ignoreTimes)
    {
      //GPS设备没有有效数据,而且多次出现错误
      gpsWarn.warnSta = CONST_APP_DEVICE_INFO_ERROR_VAL;
    }
    else
    {
      //modified 20190917 begin
      gpsWarn.warnSta = gpsWarn.warnCMD;
      //gpsWarn.bSave_config();
      if ((gpsWarn.gps_n_int > 0) && (gpsWarn.gps_n_dec > 0)) //temp solution
        sprintf(gpsWarn.GPSN, "%d.%05d", gpsWarn.gps_n_int, gpsWarn.gps_n_dec );
      if ((gpsWarn.gps_e_int > 0) && (gpsWarn.gps_e_dec > 0)) //temp solution
        sprintf(gpsWarn.GPSE, "%d.%05d", gpsWarn.gps_e_int, gpsWarn.gps_e_dec );
      //modified 20190917 end
    }

    if (!gsUblox.devReady()) //GPS设备不存在
    {
      gpsWarn.warnSta = CONST_APP_DEVICE_INFO_ERROR_VAL;
    }
    else
    {
      gpsWarn.warnSta = gpsWarn.warnCMD;
    }

    switch (gpsWarn.warnSta)
    {
      case CONST_APP_DEVICE_INFO_NORMAL_VAL://Normal
        sprintf(gpsWarn.deviceInfo, "%s", CONST_APP_DEVICE_INFO_NORMAL_MSG);
        gchMainState = CONST_GPS_STATUS_CONNECTED_STATE;
        break;
      case CONST_APP_DEVICE_INFO_SELF_WARNING_VAL://self warning
        sprintf(gpsWarn.deviceInfo, "%s", CONST_APP_DEVICE_INFO_SELF_WARNING_MSG);
        gchMainState = CONST_GPS_STATUS_WARNING_STATE;
        break;
      case CONST_APP_DEVICE_INFO_WARNING_VAL://warning
        sprintf(gpsWarn.deviceInfo, "%s", CONST_APP_DEVICE_INFO_WARNING_MSG);
        gchMainState = CONST_GPS_STATUS_WARNING_STATE;
        break;
      case CONST_APP_DEVICE_INFO_FAST_WARNING_VAL://warning
        sprintf(gpsWarn.deviceInfo, "%s", CONST_APP_DEVICE_INFO_FAST_WARNING_MSG);
        gchMainState = CONST_GPS_STATUS_FAST_WARNING_STATE;
        break;
      case CONST_APP_DEVICE_INFO_ERROR_VAL://error
      default:
        sprintf(gpsWarn.deviceInfo, "%s", CONST_APP_DEVICE_INFO_ERROR_MSG);
        gchMainState = CONST_GPS_STATUS_ERROR_STATE;
        break;

    }

    sprintf(bigBuff, "{\"SN\":\"%d\",\"DeviceMac\":\"%s\",\"DeviceInfo\":\"%s\",\"RSSI\":\"%d\",\"GPSN\":\"%s\",\"GPSE\":\"%s\",\"WarnSta\":\"%d\",\"ConnInt\":\"%d\",\"Ver\":\"%d\"}", \
            gpsWarn.SN, gpsWarn.deviceMac, gpsWarn.deviceInfo, gsNB.nbRssi, gpsWarn.GPSN, gpsWarn.GPSE, gpsWarn.warnSta, gpsWarn.ulNBConnInterval, CONST_APP_GPSWARN_VERSION);
    DBGPRINTF("\n-> gsNB.send_data:[%s]", bigBuff);
    gsNB.send_data(strlen(bigBuff), (uint8_t *) bigBuff);
    if (gsNB.responseErrorCount > CONST_AT_RESPONSE_ERROR_RESET)
      gchMainState = CONST_GPS_STATUS_CONNECTING_STATE;
    gpsWarn.errorCount++;
    gpsWarn.SN++;
    gpsWarn.print_info();

    //
  }
}


void check_system_cmd()
{
  //to determine if there is a system cmd available
  if (gsNB.dataCount > 0)
  {
    DBGPRINTF("\n-> server response is available %d", gsNB.dataCount);
    gsNB.pop_data((unsigned char *)gsNB.recvData.data);
    DBGPRINTF("\n-> pop_data [%s]", gsNB.recvData.data);
    gpsWarn.decode_ServerData(gsNB.recvData.data);

    switch (gpsWarn.warnCMD)
    {
      case CONST_APP_DEVICE_INFO_WARNING_VAL:
      case CONST_APP_DEVICE_INFO_FAST_WARNING_VAL:
        gpsWarn.preWarnSta = gpsWarn.warnSta;
        gpsWarn.warnSta = gpsWarn.warnCMD;
        break;

      case CONST_APP_DEVICE_INFO_NORMAL_VAL:
      default:
        DBGPRINTF("\n-> will turn beep off %d", gpsWarn.warnCMD);
        gpsWarn.preWarnSta = gpsWarn.warnSta;
        gpsWarn.warnSta = CONST_APP_DEVICE_INFO_NORMAL_VAL;
        break;
    }
    gpsWarn.errorCount = 0;
  }


  if ((gsNB.errorCount > CONST_AT_ERROR_RESET))
  {
    gpsWarn.warnSta = CONST_APP_DEVICE_INFO_ERROR_VAL;
    disable_interrupts();
    bSave_time_to_file();
    enable_interrupts();
    //
  }
}

/*--------------------------------------------------------------------------------*/
//APPLICATION_DATA_PART_END
/*--------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------*/
//APPLICATION_FUCTION_PART_BEGIN
/*--------------------------------------------------------------------------------*/

void check_sys_data()
{

}


void vGenerate_IMEI()
{
  uint8_t nT1;
  uint64_t tempID;
  char *spStart;
  char tempBuff[100];
  //sprintf(LVDATA.IMEI, "%s%010d", CONST_IMEI_PREFIX, ESP.getChipId());
  //sprintf(LVDATA.IMEI, "%s%010d%s%02d", CONST_IMEI_PREFIX_1, ESP.getChipId(), CONST_IMEI_PREFIX_2, nT1);
  //ESP32 的特殊处理
  tempID = ESP.getEfuseMac();
  spStart = tempBuff;
  sprintf(spStart, "%d", (uint32_t) tempID);
  if ((*spStart) == '-')
  {
    spStart++;
  }
  nT1 = strlen(spStart);
  if (nT1 > 10)
  {
    spStart += (nT1 - 10);
    nT1 = 10;
  }
  DBGPRINTF("[%s]", spStart);
  memset(IMEI, '0', CONST_IMEI_LENGTH);
  memcpy(IMEI, CONST_IMEI_PREFIX_1, 5);
  memcpy(IMEI + 5 + 10 - nT1, spStart, nT1);
  nT1 = random(99);
  sprintf(IMEI + 15, "%s%02d", CONST_IMEI_PREFIX_2, nT1); //The chip ID is essentially its MAC address(length: 6 bytes).
  DBGPRINTF("\n IMEI [%s]", IMEI);
}


void vAction_before_restart()
{
  //系统restart之前必须保留的数据
  disable_interrupts();
  bSave_time_to_file();
  enable_interrupts();
  gchMainState = CONST_GPS_STATUS_INIT_STATE;
}


// application init data is used in setup
void vApplication_setup_call_before_main()
{
  DBGPRINTLN("\n-> == vApplication_setup_call_before_main()==");

  timer_init();

  bLoad_application_data();
  beepLed.begin();
  delay(5);
  beepLed.beep_self_test();
  gsUblox.begin();
  delay(5);
  gsNB.init_modem();
  delay(5);
  gpsWarn.bLoad_config();
  delay(5);
  gpsWarn.ulNBConnTicks = ulReset_interval();
  delay(5);

};

void vApplication_setup_call_after_main()
{
  DBGPRINTLN("\n-> == vApplication_setup_call_after_main()==");
  //必须在系统时间load之后load历史数据
}


void vApplication_connected_loop_call()
{
  //irda receive data handle

  //send request

  //other application read action
  if ((ulGet_interval(gulApplicationTicks) > CONST_READ_INTERVAL))
  {
    gulApplicationTicks = ulReset_interval();
  }

}


void func_100ms_loop()
{
  beepLed.loop();
}


void func_second_loop()
{
  //beepLed.loop();
  DBGPRINTF("\n-> main status:[%d] LED[%d]", gchMainState, beepLed.gu8LEDStatus);
  if (wifi_scan_network(WIFI_DEFAULT_TARGET_SSID, WIFI_DEFAULT_RSSI_THRESHOLD))
  {
    gpsWarn.ulNBConnInterval = gpsWarn.ulNBSysConnInterval;
    gpsWarn.warnSta = CONST_APP_DEVICE_INFO_SELF_WARNING_VAL;
  }
  else
  {
    gpsWarn.ulNBConnInterval = gpsWarn.ulNBSlowConnInterval;
    gpsWarn.warnSta = gpsWarn.warnCMD;
  }
}


void func_minute_loop()
{

}

void func_hour_loop()
{

}

void func_day_loop()
{

}


//初始化状态
void func_INIT_STATE()
{
  //DBGPRINTF("\n-> func_INIT_STATE %d", gchMainState);
  beepLed.change_PINS_status(CONST_PINS_STATUS_SMARTCONFIG);
  gsNB.begin(CONST_AT_COMM_MODE_UDP, CONST_APP_BC26_BUPT_SERVER_ADDR, "", CONST_APP_BC26_BUPT_SERVER_PORT, CONST_APP_BC26_BUPT_LOCAL_PORT, external_loop_process);
  //gsNB.begin(CONST_AT_COMM_MODE_UDP, CONST_APP_BC26_SERVER_ADDR,"",CONST_APP_BC26_SERVER_PORT, CONST_APP_BC26_LOCAL_PORT,external_loop_process);
  gchMainState = CONST_GPS_STATUS_CONNECTING_STATE;

}

//在连接服务器状态
void func_CONNECTING_STATE()
{
  //DBGPRINTF("\n-> func_CONNECTING_STATE %d", gchMainState);
  beepLed.change_PINS_status(CONST_PINS_STATUS_SMARTCONFIG);
  if (gsNB.devReady())
  {
    gchMainState = CONST_GPS_STATUS_CONNECTED_STATE;
    sprintf(gpsWarn.deviceMac, "%s", gsNB.IMEI);
  }
  else {
    gsNB.reInitModem();
  }
}


//已经连接状态
void func_CONNECTED_STATE()
{
  //DBGPRINTF("\n-> func_CONNECTING_STATE %d", gchMainState);
  beepLed.change_PINS_status(CONST_PINS_STATUS_GREEN_ONLY);

  //NB 数据读取自循环
  gsNB.loop();

  //读当前GPS数据并上报
  read_GPS_send_data();

  //监测是否有服务器命令
  check_system_cmd();
}

//告警状态
void func_WARNING_STATE()
{
  //DBGPRINTF("\n-> func_WARNING_STATE %d", gchMainState);
  beepLed.change_PINS_status(CONST_PINS_STATUS_RED_BEEP);

  //NB 数据读取自循环
  gsNB.loop();

  //读当前GPS数据并上报
  read_GPS_send_data();

  //监测是否有服务器命令
  check_system_cmd();
}

//快速告警状态
void func_FAST_WARNING_STATE()
{
  //DBGPRINTF("\n-> func_FAST_WARNING_STATE %d", gchMainState);
  beepLed.change_PINS_status(CONST_PINS_STATUS_FAST_RED_BEEP);

  //NB 数据读取自循环
  gsNB.loop();

  //读当前GPS数据并上报
  read_GPS_send_data();

  //监测是否有服务器命令
  check_system_cmd();

}

//错误状态
void func_ERROR_STATE()
{
  //DBGPRINTF("\n-> func_ERROR_STATE %d", gchMainState);
  beepLed.change_PINS_status(CONST_PINS_STATUS_YELLOW_ONLY);

  //NB 数据读取自循环
  gsNB.loop();

  //读当前GPS数据并上报
  read_GPS_send_data();

  //监测是否有服务器命令
  check_system_cmd();
}

//升级状态
void func_UPGRADE_STATE()
{
  //DBGPRINTF("\n-> func_UPGRADE_STATE %d", gchMainState);
  beepLed.change_PINS_status(CONST_PINS_STATUS_VERY_SLOW_FLASH);

}

//应用主循环
void vApplication_main_loop_call()
{

  // a common 100 us loop function, for led, sync time, ...
  if (ulGet_interval(gl100msLoopTick) >= 100)
  {
    func_100ms_loop();
    // sync time
    if (ulGet_interval(ulTimeSecondTick) >= 1000)
    {
      func_second_loop();
      //minute change
      if (gTime.minute != preMinute)
      {
        preMinute = gTime.minute;
        func_minute_loop();

        // hour change
        if (gTime.hour != preHour)
        {
          preHour = gTime.hour;
          func_hour_loop();
          // day change
          if (gTime.day != preDay)
          {
            preDay = gTime.day;
            func_day_loop();
          }
        }
      }
      //vReset_interval(ulTimeSecondTick);
      ulTimeSecondTick = ulReset_interval();
    }
    //vReset_interval(gl100msLoopTick);
    gl100msLoopTick = ulReset_interval();

  }

  switch (gchMainState)
  {
    case CONST_GPS_STATUS_INIT_STATE:
      func_INIT_STATE();
      break;

    case CONST_GPS_STATUS_CONNECTING_STATE:
      func_CONNECTING_STATE();
      break;

    case CONST_GPS_STATUS_CONNECTED_STATE:
      func_CONNECTED_STATE();
      break;

    case CONST_GPS_STATUS_WARNING_STATE:
      func_WARNING_STATE();
      break;

    case CONST_GPS_STATUS_FAST_WARNING_STATE:
      func_FAST_WARNING_STATE();
      break;

    case CONST_GPS_STATUS_ERROR_STATE:
      func_ERROR_STATE();
      break;

    case CONST_GPS_STATUS_UPGRADE_STATE:
      func_UPGRADE_STATE();
      break;

    default:        //return to controlled state
      gchMainState = CONST_GPS_STATUS_INIT_STATE;
      break;
  }

  //GPS 自循环
  gsUblox.loop();

  delay(1);
}


//调整glAppStatusChangeInterval的值


/* load application data, return True==Success*/
bool bLoad_application_data()
{
  bool ret = false;
  ret = true;
  return ret;
}

/* save application data, return True = success */
bool bSave_application_data()
{
  bool ret = false;
  ret = true;
  return ret;

}


void external_loop_process()
{
  beepLed.loop();

}
/*--------------------------------------------------------------------------------*/
//APPLICATION_PART_END
/*--------------------------------------------------------------------------------*/
