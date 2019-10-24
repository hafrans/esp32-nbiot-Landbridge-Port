/*
   NBIoTBC26.CPP

   Library to NBIoTBC26 AT command interface
   To follow the spec of China Unicom/CHina Telecom
    ---

    Copyright (C) 2019  Steven Lian (steven.lian@gmail.com)    
    modified on 2019/10/08

*/


#include "NBIoTBC26.h"

//static SoftwareSerial serialSIM800(CONST_APP_RX_PORT, CONST_APP_TX_PORT); //for ESP8266
//static SoftwareSerial serialSIM800; //for ESP32

//HardwareSerial Serial1(1); //for ESP32

static char AT_CMD_STRING[CONST_AT_CMD_MAX_NUM][CONST_AT_CMD_MAX_LEN]=
{
    "ATI",   //Display Product Identification Information 
    "ATE",   //Set Command Echo Mode 
    "AT&W",   //Store Current Parameters to User Defined Profile 
    "IPR",   //Set TE-TA Fixed Local Rate 
    "CGMI",   //Request Manufacturer Identification 
    "CGMM",   //Request Model Identification 
    "CGMR",   //Request Manufacturer Revision 
    "CSQ",   //Signal Quality Report 
    "CESQ",   //Extended Signal Quality 
    "CGSN",   //Request Product Serial Number 
    "CPIN",   //Enter PIN 
    "CEREG",   //EPS Network Registration Status 
    "CSCON",   //Signalling Connection Status 
    "COPS",   //Operator Selection 
    "CGATT",   //PS Attach or Detach 
    "CGDCONT",   //Define a PDP Context 
    "CGACT",   //PDP Context Activation/Deactivation
    "CGPADDR",   //Show PDP Addresses 
    "CIMI",   //Request International Mobile Subscriber Identity 
    "CFUN",   //Set UE Functionality 
    "CMEE",   //Report Mobile Termination Error 
    "CCLK",   //Return Current Date and Time 
    "CBC",   //Query Power Supply Voltage 
    "CPSMS",   //Power Saving Mode Setting 
    "CEDRXS",   //eDRX Setting 
    "CEDRXRDP",   //eDRX Read Dynamic Parameters 
    "CTZR",   //Time Zone Reporting 
    "CIPCA",   //Initial PDP Context Activation 
    "CCIOTOPT",   //CloT Optimization Configuration 
    "CGAPNRC",   //APN Rate Control 
    "CEER",   //Extended Error Report 
    "QGACT",   //Activate/Deactivate a PDN Context 
    "QBAND",   //Get and Set Mobile Operation Band 
    "QRST",   //Automatically Reset 
    "QSPCHSC",   //Set Scrambling Algorithm 
    "QLOCKF",   //Lock NB-IoT Frequency
    "QCSEARFCN",   //Clear NB-IoT Stored EARFCN List 
    "QCGDEFCONT",   //Set Default PSD Connection Settings 
    "QNBIOTRAI",   //NB-IoT Release Assistance Indication 
    "QNBIOTEVENT",   //Enable/Disable NB-IoT Related Event Report 
    "QATWAKEUP",   //Enable/Disable Deep Sleep Wakeup Indication 
    "QENG",   //Engineering Mode 
    "QCCID",   //USIM Card Identification 
    "QPOWD",   //Power off the Module 
    "QSCLK",   //Configure Sleep Mode 
    "QRELLOCK",  //Release Sleep Lock of AT Commands 
    //
    "QIOPEN",    //Open a Socket Service 
    "QICLOSE",    //Close a Socket Service 
    "QISTATE",    //Query Socket Service Status 
    "QISEND",    //Send Text String Data 
    "QIRD",    //Retrieve the Received TCP/IP Data 
    "QISENDEX",    //Send Hex String Data 
    "QISWTMD",    //Switch Data Access Modes 
    "QPING",    //Ping a Remote Server 
    "QNTP",    //Synchronize Local Time with NTP Server 
    "QIDNSGIP",    //Get IP Address by Domain Name 
    "QICFG",    //Configure Optional Parameters 
    //
    "QSOC",   //create a socket, 1 IPV4, 2, IPV6;1 TCP, 2 UDP; 1 IP, 2 ICMP;    
    "QSOCON", //Connect to server
    "QSOSEND", //Send data command
    "QSORF", //retrive data command
    "QSODIS", //DisconnectSocket
    "QSOCL", //Close the socket
    //
    "QIGETERROR"   //Query the Last Error Code 
};


// 针对中国电信华为平台的profile定义,对应的string 类型长度，
// 由于采用base64编码，实际可以传输长度是 (消息长度-OVERHEAD)/4*3,
// 例如：mode 0,空中消息长度是 20，实际数据是 (20-4)/4*3 = 12 bytes.
// 另外增加两个字节，分别表示序号SN，和codingType.
// 以下是为了方便计算，设置的查表法数据
static uint8_t MODE_NUM_LIST[CONST_MODE_NUM_COUNT] =
{
  CONST_CT_MSG_MODE_NUM_0,
  CONST_CT_MSG_MODE_NUM_1,
  CONST_CT_MSG_MODE_NUM_2,
  CONST_CT_MSG_MODE_NUM_3,
  CONST_CT_MSG_MODE_NUM_4,
  CONST_CT_MSG_MODE_NUM_5,
  CONST_CT_MSG_MODE_NUM_6
};


static uint8_t MSG_LEN_LIST[CONST_MODE_NUM_COUNT] = 
{
  CONST_CT_MSG_MODE_LEN_0,
  CONST_CT_MSG_MODE_LEN_1,
  CONST_CT_MSG_MODE_LEN_2,
  CONST_CT_MSG_MODE_LEN_3,
  CONST_CT_MSG_MODE_LEN_4,
  CONST_CT_MSG_MODE_LEN_5,
  CONST_CT_MSG_MODE_LEN_6
};


//接收数据队列
static ListArray recvDataList(CONST_AT_DATA_LIST_COUNT,sizeof(stDataStruct));


//func begin
NBIOT::NBIOT()
{

}


//默认初始化进程,只选工作模式UDP,TCP,CT,COAP等
bool NBIOT::begin(char workMode)
{
  bool ret = true;
  switch(workMode)
  {
    case CONST_AT_COMM_MODE_COAP:
      begin(workMode,CONST_AT_DEFAULT_COAP_ADDR,CONST_AT_DEFAULT_COAP_APN,CONST_AT_DEFAULT_COAP_PORT,CONST_AT_DEFAULT_COAP_PORT,cbFunc);
      break;
    case CONST_AT_COMM_MODE_UDP:
      begin(workMode,CONST_AT_DEFAULT_UDP_ADDR,CONST_AT_DEFAULT_UDP_APN,CONST_AT_DEFAULT_UDP_PORT,CONST_AT_DEFAULT_UDP_PORT,cbFunc);
      break;
    case CONST_AT_COMM_MODE_TCP:
      begin(workMode,CONST_AT_DEFAULT_UDP_ADDR,CONST_AT_DEFAULT_TCP_APN,CONST_AT_DEFAULT_TCP_PORT,CONST_AT_DEFAULT_TCP_PORT,cbFunc);
      break;
    case CONST_AT_COMM_MODE_CT:
      begin(workMode,CONST_AT_DEFAULT_CT_ADDR,CONST_AT_DEFAULT_CT_APN,CONST_AT_DEFAULT_CT_PORT,CONST_AT_DEFAULT_CT_PORT,cbFunc);
    default:
      break;
  }
  return ret;
}


void NBIOT::init_modem()
{
  pinMode(CONST_APP_PEN_PORT,OUTPUT);
  pinMode(CONST_APP_EINT_PORT,OUTPUT);

  delay_cb(CONST_AT_PEN_TIME_IN_MS);
  digitalWrite(CONST_APP_EINT_PORT, HIGH);
  delay_cb(CONST_AT_PEN_TIME_IN_MS);
  digitalWrite(CONST_APP_EINT_PORT, LOW);
  delay_cb(CONST_AT_PEN_TIME_IN_MS);
  digitalWrite(CONST_APP_EINT_PORT, HIGH);
  //
  delay_cb(CONST_AT_PEN_TIME_IN_MS);
  digitalWrite(CONST_APP_PEN_PORT, LOW);
  delay_cb(CONST_AT_PEN_TIME_IN_MS);
  digitalWrite(CONST_APP_PEN_PORT, HIGH);
  delay_cb(CONST_AT_PEN_TIME_IN_MS);
  digitalWrite(CONST_APP_PEN_PORT, LOW);
  delay_cb(CONST_AT_PEN_TIME_IN_MS);
  digitalWrite(CONST_APP_PEN_PORT, HIGH);
  delay_cb(CONST_AT_PEN_TIME_IN_MS);
}


//第一次启动的初始化进程入口
bool NBIOT::begin(char workMode, char *host,char *apn,unsigned short rPort, unsigned short lPort,void (* cbP) ())
{
  init_modem();
  resetModem();
  return begin(workMode, host,apn,rPort,lPort,false,cbP);
}


//重启初始化进程,保留原参数
bool NBIOT::reInitModem()
{
  init_modem();
  resetModem();
  return begin(_workMode, _host,_APN,_rPort,_lPort, true);
}


//带回调函数的delay
void NBIOT::delay_cb(unsigned long microSeconds)
{
  if (microSeconds < 10)
  {
	delay(microSeconds);
  }
  else
  {
    unsigned long tempTicks;
	tempTicks = ulReset_interval();
	//DBGPRINTF("\n->delay_cb begin[%d]",microSeconds);
	while (ulGet_interval(tempTicks) < (microSeconds)) 
	{
	  if (cbFunc != NULL)
	  {
		(* cbFunc) ();
      }
    }
  }
  //DBGPRINT("\n->delay_cb end");
}


//真正的初始化进程入口
bool NBIOT::begin(char workMode, char *host,char *apn,unsigned short rPort, unsigned short lPort,bool reInitFlag, void (* cbP) ())
{
  bool ret = false;
  short tryTimes = CONST_DEFAULT_TRY_TIMES;
  DBGPRINTF("\n-> ******* NBIOT begin  ****** %s","");

  bNeedReset = false;
  
  errorCount = 0;
  responseErrorCount = 0;
  
  _codingType = CONST_CT_DEFAULT_CODING_TYPE;
  statusCheckTicks = ulReset_interval();
  dataCheckTicks = ulReset_interval();
  socketNo = -1;
  //系统参数
  if(!reInitFlag)
  {
    _workMode = workMode;
    strcpy(_host,host);
    strcpy(_APN,apn);
    _rPort = rPort;
    _lPort = lPort;
    gTime.year = 2019;
    gTime.month = 8;
    gTime.day = 10;
    gTime.hour = 12;
    gTime.minute = 11;
    gTime.second = 10;
    gTime.wday = 0;
    gTime.timezone2 = 8 * 2;
    gTime.synced = false;
    cbFunc = cbP;
  }
    
  DBGPRINTF("\n-> workMode[%d],host[%s],apn[%s],rPort[%d],lPort[%d]",_workMode,_host,_APN,_rPort,_lPort);
  
  deviceReady = false;
  attachedFlag = false;
  modemExist = false;

  
  //APP_SERIAL.begin(CONST_AT_SIO_BAUD_DATA,CONST_APP_RX_PORT,CONST_APP_TX_PORT); //for ESP32
  APP_SERIAL.begin(CONST_AT_SIO_BAUD_DATA); //for ESP32 hardware Seraial

  resetModem();//重启modem

  // 检测modem是否存在
  while(tryTimes >= 0)
  {
    if (is_device_exist())
	{
	  ret = true;
      break;
	}
    else
    {
      resetModem();
      DBGPRINTF("\n-> could not find modem!!!, reset[%s]","");
    }
	tryTimes--;
  }
  
  if (ret)
  {
	DBGPRINTF("\n-> Modem is ready %s","");
	//读取保存数据
	bLoad_config();
	  
	//不同模式的初始化过程
	switch(_workMode)
	{
	  case CONST_AT_COMM_MODE_COAP:
		ret = init_Coap();
		break;
	  case CONST_AT_COMM_MODE_UDP:
	  case CONST_AT_COMM_MODE_TCP:
		ret = init_UDPTCP();
		break;
	  case CONST_AT_COMM_MODE_CT:
		ret = init_CT();
	  default:
		break;
	}
	//设置错误消息模式
	//gen_AT_CMD(CMEE,2,""); //文本方式
	gen_AT_CMD(CMEE,1,"");//数字方式
	send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
	read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT,2);
	clean_SIO_data();
	DBGPRINTF("\n-> ******* begin() end  ****** %s","");
  }
  return ret;
}


//reset modem 
bool NBIOT::resetModem()
{
  bool ret = true;
  DBGPRINTF("\n-> **resetModem %s**","");
  gen_AT_CMD(ATI);//唤醒modem
  send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
  gen_AT_CMD(ATI);//唤醒modem
  clean_SIO_data();
  send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
  gen_AT_CMD(QRST);//数字方式
  send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
  read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT,2);
  clean_SIO_data();
  return ret;
  
}


bool NBIOT::init_Coap()
{
  
}


bool NBIOT::init_UDPTCP()
{
  bool ret = false;
  short tryTimes = CONST_DEFAULT_TRY_TIMES*2;
  clean_SIO_data();
  DBGPRINT("\n-> init_UDPTCP");
  _band = CONST_AT_DEFAULT_CU_BAND; //CMCC CU have same band
  // 读取设备IMEI
  read_IMEI();
  //debug
  //saveWorkMode = _workMode+1;
  if (strcmp(IMEI,saveIMEI) || (_workMode != saveWorkMode))
  {
    //没有配置，需要关闭modem进行配置
    DBGPRINTF("\n-> IMEI:[%s][%s]",IMEI,saveIMEI);
    DBGPRINTF("\n-> workMode:[%d][%d]",_workMode,saveWorkMode);
    
    bSave_config();
  }
  
  while (tryTimes >= 0)
  {
    if(is_device_singalOK())
	{
	  ret = true;
      break;
	}
    delay_cb(CONST_DEFAULT_TRY_DELAY * 10);
    DBGPRINTF("\n-> cannot find NB-IoT network !!!%s","");
    tryTimes --;
    //resetModem();
    //DBGPRINTF("\n-> reset modem !!!%s","");    
  }
  
  if (ret)
  {
    //获取终端IP
    read_IP();
    //get_modem_time();
  }
  return ret;   
}


bool NBIOT::init_CT()
{
  bool ret = false;
  short tryTimes;
  
  DBGPRINT("\n-> init_CT");
  _band = CONST_AT_DEFAULT_CT_BAND;
  // 读取设备IMEI
  gen_AT_CMD(CGSN);
  send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
  read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT,4);
  decode_AT_CMD(recvStru.cmd,recvStru.cmdString);
  DBGPRINTF("\n-> **** IMEI:[%s] ****",IMEI);
  //debug
  //saveWorkMode = _workMode+1;
  if (strcmp(IMEI,saveIMEI) || (_workMode != saveWorkMode))
  {
    //没有配置，需要关闭modem进行配置
    DBGPRINTF("\n-> IMEI:[%s][%s]",IMEI,saveIMEI);
    DBGPRINTF("\n-> workMode:[%d][%d]",_workMode,_workMode);
    //关机
    gen_AT_CMD(CFUN,0,"");
    send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
    read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT,2);
    ret = decode_AT_CMD(recvStru.cmd,recvStru.cmdString);

    //设置IMEI
    
    //设置平台IP和端口

    bSave_config();

    //开机，开机需要时间
    tryTimes = CONST_DEFAULT_TRY_TIMES;
    while(tryTimes > 0)
    {
	  short nT1;
      gen_AT_CMD(CFUN,1,"");
      send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
      read_with_wait(2000);
      nT1 = decode_AT_CMD(recvStru.cmd,recvStru.cmdString);
      if (nT1 == 0)
      { 
        ret = true;
        break;
      }
      delay_cb(CONST_DEFAULT_TRY_DELAY);
      tryTimes--;
    }
	
	if (ret)
	{
      short nT1;
	  //设置频段

      //设置核心网
	  gen_AT_CMD(CGDCONT);
	  send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
	  read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT,2);
	  nT1 = decode_AT_CMD(recvStru.cmd,recvStru.cmdString);
	
	  //入网
	  gen_AT_CMD(CGATT);
      send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
      read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT,2);
	  nT1 = decode_AT_CMD(recvStru.cmd,recvStru.cmdString);
    }
  }    

  if (ret)
  {
    // 上网过程

    //获取终端IP
    tryTimes = CONST_DEFAULT_TRY_TIMES;
    while(tryTimes > 0)
    {
	  short nT1;
      gen_AT_CMD(CGPADDR);
      send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
      read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT,4);
      nT1 = decode_AT_CMD(recvStru.cmd,recvStru.cmdString);
      if (nT1 == 0)
      {
        deviceReady = true;
        break;
      }
      delay_cb(CONST_DEFAULT_TRY_DELAY);
      tryTimes--;
    }
  }	
  return ret;
}


void NBIOT::get_modem_time()
{
  //get date time
  clean_SIO_data();
  gen_AT_CMD(CCLK);
  send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
  read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT,4);
  DBGPRINTF("\n->**** CCLK:[%s] ****",recvStru.cmdString);
  decode_AT_CMD(recvStru.cmd,recvStru.cmdString);
  //find_datetime(recvStru.cmdString);  
}


void NBIOT::determine_device_status()
{
  unsigned long ulTicks;
  if (deviceReady)
    ulTicks = CONST_AT_LONG_STATUS_CHECK_TICK_LEN;
  else
    ulTicks = CONST_AT_DEFAULT_STATUS_CHECK_TICK_LEN;
    
  if ((ulGet_interval(statusCheckTicks) > ulTicks))
  {
    read_IP();
    statusCheckTicks = ulReset_interval();
    DBGPRINTF("\n-> deviceReady:[%d]",devReady());
  }
}


void NBIOT::recv_data_check()
{
  unsigned long ulTicks;
  ulTicks = CONST_AT_DEFAULT_DATA_CHECK_TICK_LEN;
    
  dataCheckTicks = ulReset_interval();
  while ((ulGet_interval(dataCheckTicks) < ulTicks))
  {
    short bufferedData = 0;
    short dataLen = 0;
    DBGPRINT("\n-> check data : ");
    gen_AT_CMD(QIRD);
    send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
    read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT,4);
    bufferedData = decode_AT_CMD(recvStru.cmd,recvStru.cmdString);
    if (bufferedData > 0)
    {
      DBGPRINT("\n-> read data : ");
      //gen_AT_CMD(NMGR);
      send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
      read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT,4);
      dataLen = decode_AT_CMD(recvStru.cmd,recvStru.cmdString);
      break;
    }
    delay_cb(100);
    //DBGPRINTF("\n-> deviceReady:[%d]",devReady());
  }
}


short NBIOT::pop_data(uint8_t *buff)
{
  short ret = 0;
  recvDataList.rpop(buff);
  dataCount = recvDataList.len();
  ret = dataCount;
  return ret;  
}


//0,123.206.108.227,9099,3,123456,0
short NBIOT::recv_data_read(short dataLen, uint8_t *buff)
{
    short ret;
    DBGPRINT("\n-> read data : ");
    //gen_AT_CMD(QSORF,CONST_UDP_MSG_MODE_LEN,"");
    clean_SIO_data();
    gen_AT_CMD(QSORF,dataLen,"");
    send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
    read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT,4);
    //借用 au8recvBuff
    ret = decode_QSORF(dataLen,_host,recvStru.cmdString,(char *) buff);
    DBGPRINTF("\n-> recv_data_read:[%d][%s]",ret,buff);
    return ret;
}


void NBIOT::loop()
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
  
  determine_device_status();
  
  /*
  if (deviceReady)
    recv_data_check();
  */
  
  if (available() > 0)
  {
    //有数据
    short ret;
    ret = decode_AT_CMD(recvStru.cmd,recvStru.cmdString);
  }
  delay_cb(0);
}


void NBIOT::func_100ms_loop()
{
  
}


void NBIOT::func_second_loop()
{
  // current time and preTime
  vGet_time();
  debug_print_info();  
}


void NBIOT::func_minute_loop()
{
  get_modem_time();
}


void NBIOT::func_hour_loop()
{
  
}


void NBIOT::func_day_loop()
{
  
}


void NBIOT::debug_print_info()
{
  DBGPRINT("\n-> DATE:");
  DBGPRINT(gPreTime.timeString);

}


void NBIOT::clean_SIO_data()
{
  /*
  while (APP_SERIAL.available()) {
    APP_SERIAL.read();
    external_loop_process();
    ; // wait for serial port to connect. Needed for Leonardo only
  };
  */
  APP_SERIAL.flush();
  nRecvLen = 0;
}


bool NBIOT::devReady()
{
  return deviceReady;
}


bool NBIOT::modemReady()
{
  return modemExist;
}


bool NBIOT::gen_AT_CMD(short cmd)
{
  gen_AT_CMD(cmd,0,"");
}


bool NBIOT::gen_AT_CMD(short cmd,short val,char *spStr)
{
  bool ret = true;
  DBGPRINTF("\n-> gen_AT_CMD:[%d][%s]",cmd,AT_CMD_STRING[cmd]);
  //保留最后一条命令
  lastCMD = cmd;

  if ((cmd < CONST_AT_CMD_MAX_NUM) && (cmd >= 0)) {
    sendStru.cmd = cmd;
    switch (cmd)
    {
      case ATI:
        strcpy(sendStru.cmdString,"ATI");
        break;
        
      case ATE:
        sprintf(sendStru.cmdString,"ATE%d",val);
        break;
        
      case ATW:
        strcpy(sendStru.cmdString,"AT&W");
        break;
        
      case IPR:
        sprintf(sendStru.cmdString,"AT+%s=%d",AT_CMD_STRING[cmd],val);
        break;
        
      case CGMI:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
        
      case CGMM:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
        
      case CGMR:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
        
      case CSQ:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
        
      case CESQ:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
        
      case CGSN:
        sprintf(sendStru.cmdString,"AT+%s=1",AT_CMD_STRING[cmd]);
        break;
        
      case CPIN:
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
        
      case CEREG:
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
        
      case CSCON:
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
        
      case COPS:
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
        
      case CGATT:
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
        
      case CGDCONT:
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
        
      case CGACT:
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
        
      case CGPADDR:
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
        
      case CIMI:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
        
      case CFUN:
        sprintf(sendStru.cmdString,"AT+%s=?",AT_CMD_STRING[cmd]);
        break;
        
      case CMEE:
        sprintf(sendStru.cmdString,"AT+%s=%d",AT_CMD_STRING[cmd],val);
        break;
        
      case CCLK:
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
        
      case CBC:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
        
      case CPSMS:
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
        
      case CEDRXS:
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
        
      case CEDRXRDP:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
        
      case CTZR:
        sprintf(sendStru.cmdString,"AT+%s=%d",AT_CMD_STRING[cmd],val);
        break;
        
      case CIPCA:
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
        
      case CCIOTOPT:
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
        
      case CGAPNRC:
        sprintf(sendStru.cmdString,"AT+%s=?",AT_CMD_STRING[cmd]);
        break;
        
      case CEER:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
        
      case QGACT:
      //need more parameter
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
        
      case QBAND:
        sprintf(sendStru.cmdString,"AT+%s=%d",AT_CMD_STRING[cmd],val);
        break;
        
      case QRST:
        sprintf(sendStru.cmdString,"AT+%s=1",AT_CMD_STRING[cmd]);
        break;
        
      case QSPCHSC:
        sprintf(sendStru.cmdString,"AT+%s=%d",AT_CMD_STRING[cmd],val);
        break;
        
      case QLOCKF:
      //need more parameter
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
        
      case QCSEARFCN:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
        
      case QCGDEFCONT:
      //need more parameter
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
        
      case QNBIOTRAI:
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
        
      case QNBIOTEVENT:
      //need more parameter
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
        
      case QATWAKEUP:
        sprintf(sendStru.cmdString,"AT+%s=%d",AT_CMD_STRING[cmd],val);
        break;
        
      case QENG:
        sprintf(sendStru.cmdString,"AT+%s=%d",AT_CMD_STRING[cmd],val);
        break;
        
      case QCCID:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
        
      case QPOWD:
        sprintf(sendStru.cmdString,"AT+%s=0",AT_CMD_STRING[cmd]);
        break;
        
      case QSCLK:
        sprintf(sendStru.cmdString,"AT+%s=%d",AT_CMD_STRING[cmd],val);
        break;
        
      case QRELLOCK:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
      // base on BC26_TCP/IP_AT_Commands_Manual_V1.0
      case QIOPEN:
        sprintf(sendStru.cmdString,"AT+%s=1,%d,\"%s\",%s,%d,%d,0,0",AT_CMD_STRING[cmd],val,spStr,_host,_rPort,_lPort);
        break;
      
      case QICLOSE:
        sprintf(sendStru.cmdString,"AT+%s=%d",AT_CMD_STRING[cmd],socketNo);
        break;
      
      case QISTATE:
        sprintf(sendStru.cmdString,"AT+%s?",AT_CMD_STRING[cmd]);
        break;
      
      case QISEND:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
      
      case QIRD:
        sprintf(sendStru.cmdString,"AT+%s=%d,%d",AT_CMD_STRING[cmd],socketNo,512);
        break;
      
      case QISENDEX:
        sprintf(sendStru.cmdString,"AT+%s=%d,%d,%s",AT_CMD_STRING[cmd],socketNo,val,spStr);
        break;
      
      case QISWTMD:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
      
      case QPING:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
      
      case QNTP:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
      
      case QIDNSGIP:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
      
      case QICFG:
        sprintf(sendStru.cmdString,"AT+%s",AT_CMD_STRING[cmd]);
        break;
      
      // base on internet manual
      case QSOC:
        sprintf(sendStru.cmdString,"AT+%s=1,%d,1",AT_CMD_STRING[cmd],val);
        break;
        
      case QSOCON:
        sprintf(sendStru.cmdString,"AT+%s=%d,%d,\"%s\"",AT_CMD_STRING[cmd],socketNo,_rPort,_host);
        break;
        
      case QSOSEND:
        sprintf(sendStru.cmdString,"AT+%s=%d,%d,%s",AT_CMD_STRING[cmd],socketNo,val,spStr);
        break;
        
      case QSORF:
        sprintf(sendStru.cmdString,"AT+%s=0,%d",AT_CMD_STRING[cmd],val);
        break;
        
      case QSODIS:
        sprintf(sendStru.cmdString,"AT+%s=%d",AT_CMD_STRING[cmd],socketNo);
        break;
        
      case QSOCL:
        sprintf(sendStru.cmdString,"AT+%s=%d",AT_CMD_STRING[cmd],socketNo);
        break;
        
      default:
        break;
    }
    //DBGPRINTF("\n-> %d, %s",sendStru.cmd,sendStru.cmdString);
  }
  return true;
}


char * NBIOT::find_str(char *spData,char *keys,char tailChar)
{
  char *ret = NULL;
  char *spStart,*spEnd;
  spStart = strstr(spData,keys);
  //DBGPRINTF("\n find_str:[%s][%X]",keys,spStart);
  if (spStart != NULL)
  {
    //DBGPRINTF("\n spStart[%s]",spStart);
    spEnd = strchr(spStart,tailChar);
    if (spEnd != NULL)
    {
      *spEnd = '\0';
      ret = spStart + strlen(keys);
      //DBGPRINTF("\n spStart:[%s]",spStart);
    }
    //DBGPRINTF("\n ret:[%s]",ret);
  }
  else{
    //不这样写会在ESP32上面死机
    //DBGPRINT("\n ret:[NULL]");    
  }
  return ret;
}


// general searching
short NBIOT::decode_others(char *spData)
{
  short ret = -1;
  char *spStr;

  spStr = strstr(spData,"+IP:");
  if (spStr!=NULL)
    ret = decode_IP(spData);

  spStr = strstr(spData,"+QSONMI=");
  if (spStr!=NULL)
    ret = decode_QSONMI(spData);

  spStr = strstr(spData,"+QSOC=");
  if (spStr!=NULL)
    ret = decode_QSOC(spData);

  spStr = strstr(spData,"+CCLK:");
  if (spStr!=NULL)
    ret = decode_CCLK(spData);


  return ret;
}


// \r\n+CFUN: 1\r\n\r\nOK\r\n
short NBIOT::decode_CFUN(char *spData)
{
  short ret = -1;
  char *spStr;
  spStr = find_str(spData,"+CFUN:",0xD);
  if (spStr != NULL)
  {
    funcStatus = atoi(spStr);
    ret = 0;
  }
  else
  {
    spStr = find_str(spData,"ERROR",0xD);
    if (spStr != NULL)
    {
      DBGPRINTF("\n-> spStr:[%s]",spStr);
      spStr = strchr(spStr,':');
      if (spStr != NULL)
      {
        ret = atoi(spStr);
        DBGPRINTF("\n-> spStr:[%s] [%d]",spStr,ret);
      }
    }
  }
  return ret;
}


// \r\n+CGSN: 866971030221225\r\n\r\nOK\r\n
short NBIOT::decode_CGSN(char *spData)
{
  short ret = -1;
  char *spStr;
  spStr = find_str(spData,"+CGSN:",0xD);
  if (spStr != NULL)
  {
    strcpy(IMEI,strTrim(spStr));
    ret = 0;
  }
  else
  {
    spStr = find_str(spData,"ERROR",0xD);
    if (spStr != NULL)
    {
      DBGPRINTF("\n-> spStr:[%s]",spStr);
      spStr = strchr(spStr,':');
      if (spStr != NULL)
      {
        ret = atoi(spStr);
        DBGPRINTF("\n-> spStr:[%s] [%d]",spStr,ret);
      }
    }
  }
  return ret;
}



// \r\n460045500200726\r\n\r\nOK\r\n
short NBIOT::decode_CIMI(char *spData)
{
  short ret = -1;
  char *spStr;
  spStr = find_str(spData,"\n",0xD);
  if (spStr != NULL)
  {
    if (strlen(spStr)> 10)
    {
        strcpy(IMSI,strTrim(spStr));
        ret = 0;
    }
  }
  else
  {
    spStr = find_str(spData,"ERROR",0xD);
    if (spStr != NULL)
    {
      DBGPRINTF("\n-> spStr:[%s]",spStr);
      spStr = strchr(spStr,':');
      if (spStr != NULL)
      {
        ret = atoi(spStr);
        DBGPRINTF("\n-> spStr:[%s] [%d]",spStr,ret);
      }
    }
  }
  return ret;
}


// \r\n+CESQ: 51,0,255,255,28,76\r\n\r\nOK\r\n
// return 0: signal is ok, !=0 bad
short NBIOT::decode_CESQ(char *spData)
{
  short ret = -1;
  char *spStr;
  spStr = find_str(spData,"+CESQ:",',');
  if (spStr != NULL)
  {
    short rssi;
    rssi = atoi(strTrim(spStr));
    if ((rssi > 5) &&(rssi < 99))
        ret = 0;
    nbRssi = rssi;
  }
  else
  {
    spStr = find_str(spData,"ERROR",0xD);
    if (spStr != NULL)
    {
      DBGPRINTF("\n-> spStr:[%s]",spStr);
      spStr = strchr(spStr,':');
      if (spStr != NULL)
      {
        ret = atoi(spStr);
        DBGPRINTF("\n-> spStr:[%s] [%d]",spStr,ret);
      }
    }
  }
  return ret;
}


// \r\n+CEREG: 0,1\r\n\r\nOK\r\n
// return 0: registered, !=0 bad
short NBIOT::decode_CEREG(char *spData)
{
  short ret = -1;
  char *spStr;
  spStr = strstr(spData,"+CEREG:");
  if (spStr!=NULL)
  {
      spStr = find_str(spStr,",",'\r');
      if (spStr != NULL)
      {
          short status;
          status = atoi(strTrim(spStr));
          if ((status == 1) || (status == 5))
            ret = 0;
      }
  }
  else
  {
    spStr = find_str(spData,"ERROR",0xD);
    if (spStr != NULL)
    {
      DBGPRINTF("\n-> spStr:[%s]",spStr);
      spStr = strchr(spStr,':');
      if (spStr != NULL)
      {
        ret = atoi(spStr);
        DBGPRINTF("\n-> spStr:[%s] [%d]",spStr,ret);
      }
    }
  }
  return ret;
}



// \r\n+CGATT: 1\r\n\r\nOK\r\n
// return 0: attached, !=0 bad
short NBIOT::decode_CGATT(char *spData)
{
  short ret = -1;
  char *spStr;
  spStr = find_str(spData,"+CGATT:",'\r');
  if (spStr != NULL)
  {
    short attached;
    attached = atoi(strTrim(spStr));
    if (attached ==1)
        ret = 0;
  }
  else
  {
    spStr = find_str(spData,"ERROR",0xD);
    if (spStr != NULL)
    {
      DBGPRINTF("\n-> spStr:[%s]",spStr);
      spStr = strchr(spStr,':');
      if (spStr != NULL)
      {
        ret = atoi(spStr);
        DBGPRINTF("\n-> spStr:[%s] [%d]",spStr,ret);
      }
    }
  }
  return ret;
}


// \r\n+IP: 100.110.110.136\r\n
short NBIOT::decode_IP(char *spData)
{
  short ret = -1;
  char *spStr;
  spStr = find_str(spData,"+IP:",0xD);
  if (spStr != NULL)
  {
    strcpy(_localIP,strTrim(spStr));
    ret = 0;
  }
  else
  {
    spStr = find_str(spData,"ERROR",0xD);
    if (spStr != NULL)
    {
      DBGPRINTF("\n-> spStr:[%s]",spStr);
      spStr = strchr(spStr,':');
      if (spStr != NULL)
      {
        ret = atoi(spStr);
        DBGPRINTF("\n-> spStr:[%s] [%d]",spStr,ret);
      }
    }
  }
  return ret;
}


// \r\n+CGPADDR:0,10.174.86.218\r\n\r\nOK\r\n
// \r\n+CGPADDR:0\r\n+CGPADDR:1\r\n\r\n
// \r\n+CGPADDR:0\r\n+CGPADDR:1,10.6.72.6\r\n\r\n
// \r\n+CGPADDR: 1,100.96.58.2\r\n\r\nOK\r\n
// +CGPADDR: 1,10.147.160.207,0.0.0.0.0.0.0.0.0.1.0.1.154.244.252.59[0D][0A][0D][0A]OK[0D][0A]
short NBIOT::decode_CGPADDR(char *spData)
{
  bool ret = -1;
  char *spStr;
  char *spStart,*spEnd;
  spStart = spData;
  spEnd = strchr(spStart,0xA);
  while (spEnd != NULL)
  {
    spEnd = strchr(spEnd+1,0xA);
    spStr = find_str(spStart,"+CGPADDR:",0xD);
    if (spStr != NULL)
    {
      spStart = strchr(spStr,',');
      if (spStart != NULL)
      {
        char *spT;
		ret = 0;
		spT = strchr(spStart+1,',');
		if (spT != NULL)
		{
			*spT = 0;
		}
        strcpy(_localIP,spStart+1);
        //printf("\nIP:[%s]",_localIP);
        break;
      }
    }
    else
    {
      spStr = find_str(spData,"ERROR",0xD);
      if (spStr != NULL)
      {
        spStr = strchr(spStr,':');
        if (spStr != NULL)
        {
          ret = atoi(spStr);
        }
      }
      break;
    }
    //新的起点
    spStart = spEnd;
    spEnd = strchr(spStart,0xA);
  }

  return ret;
}


// \r\n+CCLK: 2019/5/17,6:51:12GMT+8\r\n\r\nOK\r\n
// \r\n+CCLK: 2019/5/17,6:51:53GMT+8\r\n\r\nOK\r\n
short NBIOT::decode_CCLK(char *spData)
{
  bool ret = -1;
  char *spStr;
  char *spStart,*spEnd;
  spStr = find_str(spData,"+CCLK:",0xD);
  if (spStr != NULL)
  {
    short hr, min, sec, dy, mnth, yr, tzone2;
    spStart = spStr;
    //year
    spEnd = strchr(spStart,'/');
    if (spEnd == NULL) return ret;
    *spEnd = 0;
    yr = atoi(spStart);
    DBGPRINTF("\n-> year:[%d]",yr);
    spStart = spEnd+1;
    //month
    spEnd = strchr(spStart,'/');
    if (spEnd == NULL) return ret;
    *spEnd = 0;
    mnth = atoi(spStart);
    DBGPRINTF(" month:[%d]",mnth);
    spStart = spEnd+1;
    //day
    spEnd = strchr(spStart,',');
    if (spEnd == NULL) return ret;
    *spEnd = 0;
    dy = atoi(spStart);
    DBGPRINTF(" day:[%d]",dy);
    spStart = spEnd+1;
    //hour
    spEnd = strchr(spStart,':');
    if (spEnd == NULL) return ret;
    *spEnd = 0;
    hr = atoi(spStart);
    DBGPRINTF(" hour:[%d]",hr);
    spStart = spEnd+1;
    //minute
    spEnd = strchr(spStart,':');
    if (spEnd == NULL) return ret;
    *spEnd = 0;
    min = atoi(spStart);
    DBGPRINTF(" minute:[%d]",min);
    spStart = spEnd+1;
    //second
    spEnd = strchr(spStart,'G');
    if (spEnd == NULL) return ret;
    *spEnd = 0;
    sec = atoi(spStart);
    DBGPRINTF(" second:[%d]",sec);
    spStart = spEnd+3;
    //zone
    tzone2 = atoi(spStart)*2;
    DBGPRINTF(" tzone:[%d]",tzone2);

    //set time
    if (gTime.day != dy)
    {

    }
    setTime(hr,min,sec,dy,mnth,yr);
    set_time_zone(tzone2);//time zone * 2, indian = GMT 7.5
    vGet_time();
    gTime.synced = true;
    DBGPRINTF("\n-> %d-%d-%d %d:%d:%d GMT:%+d",gTime.year,gTime.month,gTime.day,gTime.hour,gTime.minute,gTime.second,gTime.timezone2/2);

    ret = 0;
  }

  return ret;
}


short NBIOT::decode_OKERROR(char *spData)
{
  short ret = -1;
  char *spStr;
  DBGPRINT("\n-> decode_OKERROR");
  spStr = find_str(spData,"OK",0xD);
  if (spStr != NULL)
  {
    //DBGPRINT("\n OK");
    ret = 0;
  }
  else
  {
    DBGPRINT("\n-> ERROR0xD");
    spStr = find_str(spData,"ERROR",0xD);  
    if (spStr != NULL)
    {
      //DBGPRINTF("\n spStr:[%s]",spStr);
    }
    else
    {
      DBGPRINT("\n-> ERROR:");
      spStr = find_str(spData,"ERROR",':');  
      if (spStr != NULL)
      {
        //DBGPRINTF("\n spStr:[%s]",spStr);   
        spStr = strchr(spStr,':');
        if (spStr != NULL)
        {
          ret = atoi(spStr);
          //DBGPRINTF("\n spStr:[%s]",spStr);   
        }
      }
    }
  }
  DBGPRINTF("\n-> ret:[%d]",ret);   
  return ret;
}


// \r\n+CEREG: 0,1\r\n\r\nOK\r\n
// return 0: no data, >0 data length
short NBIOT::decode_QSONMI(char *spData)
{
  short ret = -1;
  char *spStr;
  spStr = strstr(spData,"+CEREG:");
  if (spStr!=NULL)
  {
      spStr = find_str(spStr,",",'\r');
      if (spStr != NULL)
      {
          short status;
          status = atoi(strTrim(spStr));
          if ((status == 1) || (status == 5))
            ret = 0;
      }
  }
  else
  {
    spStr = find_str(spData,"ERROR",0xD);
    if (spStr != NULL)
    {
      DBGPRINTF("\n-> spStr:[%s]",spStr);
      spStr = strchr(spStr,':');
      if (spStr != NULL)
      {
        ret = atoi(spStr);
        DBGPRINTF("\n-> spStr:[%s] [%d]",spStr,ret);
      }
    }
  }
  return ret;
}


//\r\n+QSOC=1\r\n\r\nOK\r\n
// return -1: error, otherwise , socketNo
//\r\nQSOC: OK\r\nAT+CMEE=2\r\n\r\nOK\r\nAT+QSOC=1,2,1\r\n\r\n+QSOC=0\r\n\r\nOK\r\n
short NBIOT::decode_QSOC(char *spData)
{
  short ret = -1;
  char *spStart;
  char *spStr;
  spStart = spData;
  while (1)
  {
    spStr = strstr(spStart, "+QSOC=");
    if (spStr != NULL)
    {
      short nSID;
      char *spTemp;
      spTemp = strchr(spStr, ',');
      if (spTemp == NULL)
      {
        spStr = find_str(spStart, "+QSOC=", '\r');
        nSID = atoi(strTrim(spStr));
        if (nSID >= 0)
        {
          socketNo = nSID;
          ret = socketNo;
          break;
        }
      }
      else
      {
          spStart = spStr+1;
      }
    }
    else
    {
      spStr = find_str(spData, "ERROR", 0xD);
      if (spStr != NULL)
      {
        DBGPRINTF("\n-> spStr:[%s]", spStr);
        spStr = strchr(spStr, ':');
        if (spStr != NULL)
        {
          ret = atoi(spStr);
          DBGPRINTF("\n-> spStr:[%s] [%d]", spStr, ret);
        }
      }
      break;
    }
  }
  return ret;
}


//[0D][0A]ERROR[0D][0A]
//[0D][0A]OK[0D][0A]
// return 0:ok, -1:error
short NBIOT::decode_QIOPEN(char *spData)
{
  short ret = -1;
  char *spStr;
  spStr = strstr(spData, "OK");
  if (spStr != NULL)
  {
    ret = 0;
  }
  else
  {
    spStr = find_str(spData, "ERROR", 0xD);
    if (spStr != NULL)
    {
      DBGPRINTF("\n-> spStr:[%s]", spStr);
    }
  }
  return ret;
}


//[0D][0A]OK[0D][0A][0D][0A]SEND OK[0D][0A]
//[0D][0A]ERROR[0D][0A]
// return 0:ok, -1:error
short NBIOT::decode_QISENDEX(char *spData)
{
  short ret = -1;
  char *spStr;
  spStr = strstr(spData, "SEND OK");
  if (spStr != NULL)
  {
    ret = 0;
  }
  else
  {
    spStr = find_str(spData, "ERROR", 0xD);
    if (spStr != NULL)
    {
      DBGPRINTF("\n-> spStr:[%s]", spStr);
    }
  }
  return ret;
}


//[0D][0A]+QIRD:0[0D][0A]
//[0D][0A]+QIRD:120,0,12312332423[0D][0A]
// return >0:have data, =0: no data, <0:error
short NBIOT::decode_QIRD(char *spData)
{
  short ret = -1;
  char *spStr;
  spStr = strstr(spData, "+QIRD:0\r");
  if (spStr != NULL)
  {
    ret = 0;
  }
  spStr = strstr(spData, "+QIRD:");
  if (spStr != NULL)
  {
    //have data
    short nLen;
    short nRemained;
    char *spStart, *spEnd;
    spStart = spStr + 6;
    spEnd = strchr(spStart, ',');
    if (spEnd != NULL)
    {
      spStr = spEnd + 1;
      *spEnd = 0;
      nLen = atoi(spStart);
      spStart = spStr;
      spEnd = strchr(spStart, ',');
      if (spEnd != NULL)
      {
        spStr = spEnd + 1;
        *spEnd = 0;
        nRemained = atoi(spStart);
        spStart = spStr;
        spEnd = strchr(spStart, '\r');
        if (spEnd != NULL)
        {
          *spEnd = 0;
          //借用sendbuff
          strncpy((char *) au8sendBuff, spStart, sizeof(au8sendBuff) - 1);
          
          ret = nLen;
        }
      }
    }
  }
  return ret;
}


//\r\nOK\r\n
//\r\nOK\r\n\r\n+QSONMI=0,50\r\n
// return >0:have data, =0: no data, <0:error
short NBIOT::decode_QSOSEND(char *spData)
{
  short ret = -1;
  char *spStr;
  //check if there is response data
  spStr = strstr(spData, "OK");
  if (spStr != NULL)
  {
    ret = 0;
    DBGPRINTF("\n-> decode_QSOSEND spStr:[%s]", spStr);
    //DBGPRINTF("\n-> spStr:[%d]", strlen(spStr));
  }

  spStr = strstr(spData, "+QSONMI=");
  if (spStr != NULL)
  {
    //have data
    short sid;
    short nLen;
    char *spStart, *spEnd;
    spStart = spStr + 8;
    spEnd = strchr(spStart, ',');
    if (spEnd != NULL)
    {
      spStr = spEnd + 1;
      *spEnd = 0;
      sid = atoi(spStart);
      spStart = spStr;
      spEnd = strchr(spStart, '\r');
      if (spEnd != NULL)
      {
        spStr = spEnd + 1;
        *spEnd = 0;
        nLen = atoi(spStart);
        ret = nLen;
      }
    }
  }

  spStr = strstr(spData, "ERROR");
  if (spStr != NULL)
  {
    DBGPRINTF("\n-> spStr:[%s]", spStr);
    ret = -1;
  }
  spStr = strstr(spData, "error");
  if (spStr != NULL)
  {
    //DBGPRINTF("\n-> spStr:[%s]", spStr);
    DBGPRINTF("\n-> spStr:[%d]", strlen(spStr));
    ret = -1;
  }
  
  return ret;
}


//\r\n0,120.132.55.236,10001,50,7b22436f6e6e496e74223a2232303030222c22534e223a2230222c225761726e537461223a2231227d0d0a0d0a0d0a0d0a0d,0\r\n\r\nOK\r\n
// return >0:have data, =0: no data, <0:error
short NBIOT::decode_QSORF(short nLen, char *IPAddr, char *spData, char *buff)
{
  short ret = -1;
  short nTLen;
  char *spStr;
  char *spStart, *spEnd;
  nTLen = strlen(IPAddr);
  spStr = strstr(spData, IPAddr);
  DBGPRINTF("\n-> decode_QSORF,spStr[%s]",spStr);
  if (spStr != NULL)
  {
    short tryTimes = 3;
    spStart = spStr;
    spEnd = strchr(spStart, '\r');
    DBGPRINTF("\n-> decode_QSORF,spEnd[%s]",spEnd);
    while (tryTimes >0)
    {
      tryTimes--;
      spStr = strchr(spStart,',');
      if (spStr == NULL)
        return ret;
      spStart = spStr+1;
      DBGPRINTF("\n-> decode_QSORF,spStart[%s]",spStart);
      if (spStart >= spEnd)
        return ret;
    }
    //have data
    spEnd = strchr(spStart, ',');
    if (spEnd != NULL)
    {
      *spEnd = 0;
      strcpy(buff, spStart);
      ret = nLen;
    }
  }
  return ret;
}


//[0D][0A]0,120.132.55.236,10001,50,7b22436f6e6e496e74223a2232303030222c22534e223a2230222c225761726e537461223a2231227d0d0a0d0a0d0a0d0a0d,0[0D][0A][0D][0A]OK[0D][0A]
short NBIOT::decode_AT_CMD(short cmd,char *spData)
{
  short ret = -1;
  short nLen;
  nLen = strlen(spData);
  DBGPRINTF("\n-> decode_AT_CMD:[%d],[%s],[%d]:[%s]",cmd,AT_CMD_STRING[cmd],nLen,spData);
  //DBGPRINTF("\n decode_AT_CMD:[%d]",cmd);

  if ((cmd < CONST_AT_CMD_MAX_NUM) && (cmd >= 0) && nLen > 0) 
  {
    switch (cmd)
    {
      case ATI:
        break;
      case ATE:
        break;
      case ATW:
        break;
      case IPR:
        break;
      case CGMI:
        break;
      case CGMM:
        break;
      case CGMR:
        break;
      case CSQ:
        break;
      case CESQ:
        ret = decode_CESQ(spData);
        break;
      case CGSN:
        ret = decode_CGSN(spData);
        break;
      case CPIN:
        break;
      case CEREG:
        break;
      case CSCON:
        break;
      case COPS:
        break;
      case CGATT:
        break;
      case CGDCONT:
        break;
      case CGACT:
        break;
      case CGPADDR:
        ret = decode_CGPADDR(spData);
        break;
      case CIMI:
        break;
      case CFUN:
        break;
      case CMEE:
        break;
      case CCLK:
        ret = decode_CCLK(spData);
        break;
      case CBC:
        break;
      case CPSMS:
        break;
      case CEDRXS:
        break;
      case CEDRXRDP:
        break;
      case CTZR:
        break;
      case CIPCA:
        break;
      case CCIOTOPT:
        break;
      case CGAPNRC:
        break;
      case CEER:
        break;
      case QGACT:
        break;
      case QBAND:
        break;
      case QRST:
        break;
      case QSPCHSC:
        break;
      case QLOCKF:
        break;
      case QCSEARFCN:
        break;
      case QCGDEFCONT:
        break;
      case QNBIOTRAI:
        break;
      case QNBIOTEVENT:
        break;
      case QATWAKEUP:
        break;
      case QENG:
        break;
      case QCCID:
        break;
      case QPOWD:
        break;
      case QSCLK:
        break;
      case QRELLOCK:
        break;
      case QIRD:
        ret = decode_QIRD(spData);
        break;
      default:
        ret = decode_others(spData);
        break;
    }
  }
  return ret;
}


short NBIOT::available()
{
  short ret = -1;
  while (APP_SERIAL.available()) {
    uint8_t chR;
    ulRecvTimeOutTick = ulReset_interval();
    chR = APP_SERIAL.read();
    if (isprint(chR))
      DBGPRINTF("%c",chR);
    else
      DBGPRINTF("[%02X]", chR);
    au8recvBuff[nRecvLen++] = chR;
    if (nRecvLen >= sizeof(recvStru.cmdString)) {
        nRecvLen = 0;
    }
    if(ulGet_interval(ulRecvTimeOutTick) > CONST_AT_SIO_BUSY_TIME_IN_MS)
    {      
       modemExist = true;
       ret = nRecvLen;
       memset(&recvStru,0,sizeof(recvStru));
       memcpy(recvStru.cmdString,au8recvBuff,nRecvLen);
       recvStru.cmd = lastCMD;
    }
    external_loop_process();

  }
  return ret;
}


void NBIOT::send_CMD(short nLen, char *data)
{
  int i;
  //DBGPRINTF("\n-> send_CMD [%s]",data);
  DBGPRINTF("\n-> send_CMD [%d]",strlen(data));
  clean_SIO_data();
  for (i=0;i<nLen;i++)
  {
    APP_SERIAL.write(data[i]);
  }
  APP_SERIAL.write(CONST_CR);
  APP_SERIAL.write(CONST_LF);
}


void NBIOT::trans_data_HEX(short nLen, uint8_t *fromBuff, uint8_t *toBuff)
{
  short i;
  uint8_t *spTarget;
  spTarget = toBuff;
  for (i=0;i<nLen;i++){
    *spTarget = ascii((fromBuff[i]>> 4));
    spTarget++;
    *spTarget = ascii((fromBuff[i] &0xF));
    spTarget++;
  }
  *spTarget='\0';
  //DBGPRINTF("\n-> original data:[%d],[%s]",strlen((char *) fromBuff),(char *) toBuff);
  //DBGPRINTF("\n-> trans 2 HEX data:[%d],[%s]",strlen((char *) toBuff),(char *)toBuff);
}


void NBIOT::trans_HEX_data(short nLen, uint8_t *fromBuff, uint8_t *toBuff)
{
  short i;
  uint8_t *spTarget;
  spTarget = toBuff;
  for (i=0;i<nLen;i++){
    *spTarget = (ascii_hex(fromBuff[i*2]) << 4) + (ascii_hex(fromBuff[i*2+1]));
    spTarget++;
  }
  DBGPRINTF("\n-> trans 2 data:[%s]",toBuff);
}


short NBIOT::send_data(short nLen, uint8_t *data)
{
  switch(_workMode)
  {
    case CONST_AT_COMM_MODE_COAP:
      send_data_Coap(nLen,data);
      break;
    case CONST_AT_COMM_MODE_UDP:
      send_data_tcpudp(CONST_AT_CONN_UDP,nLen,data);
      break;
    case CONST_AT_COMM_MODE_TCP:
      send_data_tcpudp(CONST_AT_CONN_TCP,nLen,data);
      break;
    case CONST_AT_COMM_MODE_CT:
    default:
      send_data_tcpudp(CONST_AT_CONN_UDP,nLen,data);
      break;
  }
}


short NBIOT::send_data_Coap(short nLen, uint8_t *data)
{
  
}

/*
//based on BC26_TCP/IP_AT_Commands_Manual_V1.0
short NBIOT::send_data_tcpudp(short socketType, short nLen, uint8_t *data)
{
  short ret = -1;
  int i;
  DBGPRINTF("\n-> begin send_data udp [%s]", data);
  
  // check attached status
  if (is_device_attached())
  {
    // create socket
    socketNo = 0;
    switch (socketType)
    {
      case CONST_AT_CONN_TCP:
        gen_AT_CMD(QIOPEN,socketNo, "TCP");
        break;
      case CONST_AT_CONN_UDP:
      default:
        gen_AT_CMD(QIOPEN,socketNo, "UDP");
        break;  
    }
    send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
    read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT, 4);
    DBGPRINTF("\n-> QIOPEN: %s", recvStru.cmdString);
    
    ret = decode_QIOPEN(recvStru.cmdString);
    if (ret == 0)
    {     
      // transdata to new format
      trans_data_HEX(nLen, data);

      // send data
      sendDataLen = nLen;
      //DBGPRINTF("\n-> DEBUG:send data:[%d],[%s]",sendDataLen,au8sendBuff);
      gen_AT_CMD(QISENDEX, sendDataLen, (char *) au8sendBuff);
      send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);

      //DBGPRINTF("\n-> DEBUG:[%s]",sendStru.cmdString);
      read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT, 4);
      DBGPRINTF("\n-> QISENDEX: %s", recvStru.cmdString);
      ret = decode_QISENDEX(recvStru.cmdString);
      clean_SIO_data();
      errorCount = 0;
      //delay_cb(500);
      //check data
      recv_data_check();
    }
    // close socket
    gen_AT_CMD(QICLOSE);
    send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
    read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT, 2);
    DBGPRINTF("\n-> QICLOSE: %s", recvStru.cmdString);
    //socketNo = -1;
    //delay_cb(500);
    clean_SIO_data();
    DBGPRINT("\n-> end send_data");
  }
  if (ret == -1){
    errorCount++;
    if(errorCount > CONST_AT_ERROR_RESET){
        bNeedReset = true;
        errorCount = 0;
    }
  }
  return ret;
}
*/


//based on internet information 2019/05/21 backup
//不重新连接服务器, 而是每次检查连接是否存在的方式. 
short NBIOT::send_data_tcpudp(short socketType, short nLen, uint8_t *data)
{
  short ret = -1;
  short dataLen = -1;
  DBGPRINTF("\n-> begin send_data tcpudp [%d],[%d],[%s]", socketType,socketNo, data);
  // check attached status
  if (is_device_attached())
  {
    //check if the socket is still existing 
    //if (!is_conn_actived() && socketNo >= 0)
    if (!is_device_singalOK() && socketNo >= 0)
    {
      //Disconnect Socket
      gen_AT_CMD(QSODIS);
      send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
      read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT, 2);
      DBGPRINTF("\n-> QSODIS: %s", recvStru.cmdString);
      // close socket
      gen_AT_CMD(QSOCL);
      send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
      read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT, 2);
      DBGPRINTF("\n-> QSOCL: %s", recvStru.cmdString);
      socketNo = -1; //reconnect to server
      errorCount++;
      delay_cb(1);
    }
    
    if (socketNo < 0)
    {
      // create socket
      DBGPRINTF("\n-> socketNo [%d], will create ",socketNo);
      gen_AT_CMD(QSOC, socketType, "");
      send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
      read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT, 4);
      //clean_SIO_data();
      DBGPRINTF("\n-> QSOC: %s", recvStru.cmdString);
      decode_QSOC(recvStru.cmdString);
      DBGPRINTF("\n-> socketNo: %d", socketNo);
      //Connect to server
      gen_AT_CMD(QSOCON);
      send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
      read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT,2);
      delay_cb(1);
    }
    
    if (socketNo >= 0)
    {
      DBGPRINTF("\n-> socketNo [%d], send data ",socketNo);
      clean_SIO_data();
      // transdata to new format
      trans_data_HEX(nLen, data,au8sendBuff);
      
      // send data
      sendDataLen = nLen;
      //DBGPRINTF("\n-> DEBUG:send data:[%d],[%s]",sendDataLen,au8sendBuff);
      gen_AT_CMD(QSOSEND, sendDataLen, (char *) au8sendBuff);
      short tryTimes = CONST_DEFAULT_TRY_TIMES;
      while (tryTimes > 0)
      {     
        send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
		responseErrorCount++;
        delay_cb(100);
        //debug
        send_CMD(4,"ATE0");
        //DBGPRINTF("\n-> DEBUG:[%s]",sendStru.cmdString);
        read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT*5);
		
        DBGPRINTF("\n-> QSOSEND:[%d], %s", tryTimes,recvStru.cmdString);
        dataLen = decode_QSOSEND(recvStru.cmdString);
        DBGPRINTF("\n-> dataLen[%d]",dataLen);
        if (dataLen >=0 || socketNo == -1)
		{
          break;
		}
        tryTimes--;
      }
      
      if (dataLen > 0)
      {
        // check and read data
        short nLen;
        nLen = recv_data_read(dataLen,au8recvBuff);//debug 默认返回借用了
        //DBGPRINTF("\n-> after call:[%d][%s]",nLen, au8recvBuff);
		if (nLen > 0)
			responseErrorCount = 0;
        
        trans_HEX_data(nLen,au8recvBuff,(uint8_t *) recvData.data);
        recvData.data[nLen+1] = 0;
        recvData.codingType = _DEF_COMM_CODING_TYPE_BINARY;
        DBGPRINTF("\n-> recvData.data [%s]",recvData.data);
        recvDataList.lpush(recvData.data);
        dataCount = recvDataList.len();
        
      }
      if (dataLen < 0)
      {
        errorCount++;
      }
      //clean_SIO_data();
      //delay_cb(500);
    }
    //socketNo = -1;
    //delay_cb(500);
    clean_SIO_data();
    DBGPRINT("\n-> end send_data");
    ret = 0;
  }
  return ret;
}

/*

//based on internet information 2019/05/21 backup
//每次都是重新连接服务器的方式
short NBIOT::send_data_tcpudp(short socketType, short nLen, uint8_t *data)
{
  short ret = -1;
  int i;
  DBGPRINTF("\n-> begin send_data udp [%s]", data);
  // check attached status
  if (is_device_attached())
  {
    // create socket
    gen_AT_CMD(QSOC, socketType, "");
    send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
    read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT, 4);
    //clean_SIO_data();
    DBGPRINTF("\n-> QSOC: %s", recvStru.cmdString);
    decode_QSOC(recvStru.cmdString);
    DBGPRINTF("\n-> socketNo: %d", socketNo);
    if (socketNo >= 0)
    {
      //Connect to server
      gen_AT_CMD(QSOCON);
      send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
      read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT,2);

      // transdata to new format
      trans_data_HEX(nLen, data);

      // send data
      sendDataLen = nLen;
      //DBGPRINTF("\n-> DEBUG:send data:[%d],[%s]",sendDataLen,au8sendBuff);
      gen_AT_CMD(QSOSEND, sendDataLen, (char *) au8sendBuff);
      send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);

      //DBGPRINTF("\n-> DEBUG:[%s]",sendStru.cmdString);
      read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT);
      DBGPRINTF("\n-> QSOSEND: %s", recvStru.cmdString);
      //clean_SIO_data();
      //delay_cb(500);
      //Disconnect Socket
      gen_AT_CMD(QSODIS);
      send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
      read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT, 2);
      DBGPRINTF("\n-> QSODIS: %s", recvStru.cmdString);
      // close socket
      gen_AT_CMD(QSOCL);
      send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
      read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT, 2);
      DBGPRINTF("\n-> QSOCL: %s", recvStru.cmdString);
    }
    socketNo = -1;
    //delay_cb(500);
    clean_SIO_data();
    DBGPRINT("\n-> end send_data");
    ret = 0;
  }
  return ret;
}

*/


short NBIOT::send_data_CT(short nLen, uint8_t *data)
{
  sendData.modeNum = calc_msg_mode(nLen);
  sendData.SN = SN++;
  if (SN >= 0x10)
    SN = 0;
  sendData.codingType = _codingType;
  memset(sendData.data,CONST_AT_CMD_DATA_FILL_CHAR,CONST_AT_MSG_DATA_MAX_LENGTH);
  sendData.data[CONST_AT_MSG_DATA_MAX_LENGTH] = 0;
  encode_data(sendData.data,(char *) data,nLen);
  send_data_to_modem();
}

//默认使用sendData 结构体
short NBIOT::send_data_to_modem()
{
  char chR;
  char chData;
  short nDataLen;
  short i;
  DBGPRINT("\n-> send_data_to_modem:\n ");
  //根据modeNum得到需要发送的数据长度，不包括modeNum,SN,和codingType
  nDataLen = MSG_LEN_LIST[sendData.modeNum];
  //发送AT NMGS
  sprintf((char *)au8sendBuff,"AT+NMGS=%d,",(nDataLen+3));
  for(i=0;i<strlen((char *)au8sendBuff);i++)
  {
    chR = au8sendBuff[i];
    DBGPRINT(chR);
    APP_SERIAL.write(chR);    
  }
  //转换并发送modeNum
  chR = hex(sendData.modeNum>>4);
  DBGPRINT(chR);
  APP_SERIAL.write(chR);
  chR = hex(sendData.modeNum&0xF);
  DBGPRINT(chR);
  APP_SERIAL.write(chR);
  //转换并发送SN 把数转换成16进制字符
  chData = hex(sendData.SN);
  chR = hex(chData>>4);
  DBGPRINT(chR);
  APP_SERIAL.write(chR);
  chR = hex(chData&0xF);
  DBGPRINT(chR);
  APP_SERIAL.write(chR);
  //转换并发送codingType 把数转换成16进制字符
  chData = hex(sendData.codingType);
  chR = hex(chData>>4);
  DBGPRINT(chR);
  APP_SERIAL.write(chR);
  chR = hex(chData&0xF);
  DBGPRINT(chR);
  APP_SERIAL.write(chR);
  //发送数据
  for (i=0;i<nDataLen;i++)
  {
    chR = hex(sendData.data[i]>>4);
    DBGPRINT(chR);
    APP_SERIAL.write(chR);  
    chR = hex(sendData.data[i]&0xF);
    DBGPRINT(chR);
    APP_SERIAL.write(chR);  
  }
  // send LF/CR
  APP_SERIAL.write(CONST_CR);
  APP_SERIAL.write(CONST_LF); 
  
}


//根据数据类型,进行数据编码
short NBIOT::encode_data(char *targetData,char *srcData,short nLen)
{
  short ret;
  switch (_codingType)
  {
    case _DEF_COMM_CODING_TYPE_BCD_NUM:
      //BCD 码的额外开销是2倍
      ret = bcd_encode((unsigned char *)targetData,(unsigned char *)srcData,nLen);
      break;
    case _DEF_COMM_CODING_TYPE_B6400_NUM:
      //standard base64 额外开销是 1/3
      ret = Base64encode(targetData,srcData,nLen);
      break;
    case _DEF_COMM_CODING_TYPE_B6401_NUM:
    default:
      //standard base64 额外开销是 1/3 + 4 bytes
      ret = simpleB64_encode(targetData,srcData,nLen);
      break;
  }
  DBGPRINTF("\n-> encode:[%s]",targetData);
  //为了满足CT的profile 要求
  targetData[ret-1] = CONST_AT_CMD_DATA_FILL_CHAR;
  DBGPRINTF("\n-> CT encode:[%s]",targetData);
  return ret;
}


//根据数据类型,进行数据编码
short NBIOT::decode_data(uint8_t localCodingType, char *targetData,char *srcData,short nLen)
{
  short ret;
  switch (localCodingType)
  {
    case _DEF_COMM_CODING_TYPE_BCD_NUM:
      //BCD 码的额外开销是2倍
      ret = bcd_decode((unsigned char *)targetData,(unsigned char *)srcData,nLen);
      break;
    case _DEF_COMM_CODING_TYPE_B6400_NUM:
      //standard base64 额外开销是 1/3
      ret = Base64decode(targetData,srcData);
      break;
    case _DEF_COMM_CODING_TYPE_B6401_NUM:
    default:
      //standard base64 额外开销是 1/3 + 4 bytes
      ret = simpleB64_decode(targetData,srcData,nLen);
      break;
  }
  DBGPRINTF("\n-> decode:[%s]",targetData);
  return ret;
}


// 根据数据长度，计算采用的CT profile类型。
// 对应使用 MODE_NUM_LIST 和 MSG_LEN_LIST 
short NBIOT::calc_msg_mode(short nLen)
{
  short ret = 0;
  short i;
  short nRealLen;
  nRealLen = nLen;

  DBGPRINTF("\n-> calc_msg_mode:len[%d]",nLen);

  switch (_codingType)
  {
    case _DEF_COMM_CODING_TYPE_BCD_NUM:
      //BCD 码的额外开销是2倍
      nRealLen = nLen * 2;
      break;
    case _DEF_COMM_CODING_TYPE_B6400_NUM:
      //standard base64 额外开销是 1/3
      nRealLen = ((nLen+2)/3) * 4;
      break;
    case _DEF_COMM_CODING_TYPE_B6401_NUM:
      //standard base64 额外开销是 1/3 + 4 bytes
      nRealLen = ((nLen+2)/3) * 4 + 4;
      break;
    default:
      nRealLen = ((nLen+2)/3) * 4 + 4;
      break;    
  }
  if (nRealLen > MSG_LEN_LIST[0])
  {
    for (i=1;i<CONST_MODE_NUM_COUNT;i++)
    {
      if ((nRealLen > MSG_LEN_LIST[i-1]) && (nRealLen <= MSG_LEN_LIST[i]))
      {
        ret = i;
        break;
      }
    }
  }
  //转换为电信CT profile 代号。
  ret = MODE_NUM_LIST[ret];
  DBGPRINTF(",real len[%d],msgMode[%d]\n",nRealLen,ret);
  return ret;
}


short NBIOT::read_with_wait(short microSeconds)
{
  read_with_wait(microSeconds,CONST_AT_DEFAULT_LF_COUNT);
}


// LFCount 是说收到LF的次数
short NBIOT::read_with_wait(short microSeconds,short LFCount)
{
  short ret = -1;
  unsigned long ulTick;
  short theCount = 0;
  DBGPRINTF("\n-> ======read_with_wait begin LFCount[%d]\n",LFCount);
  //clean_SIO_data();
  nRecvLen = 0;
  ulTick = ulReset_interval();
  while (ulGet_interval(ulTick) < (microSeconds)) {
    if (APP_SERIAL.available()) {
      uint8_t chR;
      chR = APP_SERIAL.read();
        ulRecvTimeOutTick = ulReset_interval();
      //DBGPRINTF("[ %02X] %c", chR, chR);
      if (isprint(chR))
        DBGPRINTF("%c",chR);
      else
        DBGPRINTF("[%02X]", chR);
      //DBGPRINTF("%c",chR);
      au8recvBuff[nRecvLen++] = chR;
      if ((nRecvLen >= sizeof(recvStru.cmdString))) {
        nRecvLen = 0;
      }
      if (chR == CONST_LF){
        theCount++;        
      }
      if (theCount >= LFCount)
      {
        DBGPRINTF("\n-> LFCount[%d] = [%d]",LFCount,theCount);
        break;
      }
      external_loop_process();
    }
    delay_cb(0);
    external_loop_process();
  }
  if(nRecvLen > 0)
  {  
	
    //if ((chR == CONST_LF) && (nRecvLen > 2) && (theCount>= (LFCount*2) )) {
    DBGPRINTF("\n-> prepare to copy data:[%d] nRevnLen[%d]",theCount,nRecvLen);
    modemExist = true;
    ret = nRecvLen;
    memset(&recvStru,0,sizeof(recvStru));
    memcpy(recvStru.cmdString,au8recvBuff,nRecvLen);
    recvStru.cmd = lastCMD;
    DBGPRINTF("\n-> cmd:[%d]",recvStru.cmd);
	if (strstr(recvStru.cmdString,"socket") != NULL){
		DBGPRINT("\n-> *********catched a socket problem*************");
		socketNo = -1;
	}
  }
  DBGPRINT("\n-> ======read_with_wait end");
}


short NBIOT::read_IMEI()
{
  short ret = -1;
  short tryTimes = CONST_DEFAULT_TRY_TIMES;
  while (tryTimes > 0)
  {
    tryTimes--;
    // 读取设备IMEI
    gen_AT_CMD(CGSN);
    send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
    read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT,4);
    ret = decode_AT_CMD(recvStru.cmd,recvStru.cmdString);
    DBGPRINTF("\n-> **** IMEI:[%s] ****",IMEI);
    if (!ret)
      break;
  }
}


short NBIOT::read_IP()
{
  short ret = -1;
  short tryTimes = CONST_DEFAULT_TRY_TIMES;
  while(tryTimes > 0)
  {
    tryTimes--;
    //读取IP
    gen_AT_CMD(CGPADDR);
    send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
    read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT,4);
    ret = decode_AT_CMD(recvStru.cmd,recvStru.cmdString);
    if (ret == 0)
    {
      deviceReady = true;
      DBGPRINTF("\n->IP:[%s]",_localIP);
      break;
    }
    delay_cb(CONST_DEFAULT_TRY_DELAY);
  }  
  return ret;
}

  
bool NBIOT::is_device_exist()
{
  bool ret = false;
  short tryTimes = CONST_DEFAULT_TRY_TIMES;
  while (tryTimes > 0)
  {
    tryTimes--;
    uint16_t crc = 0;
    DBGPRINTF("\n-> is_device_exist:[%d] ",tryTimes);
    clean_SIO_data();
    gen_AT_CMD(ATI);
    send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
    gen_AT_CMD(ATE, 0, "");
    send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);

    //wait for 2 seconds to get data;
    unsigned long ulTick;
    //vReset_interval(ulTick);
    ulTick = ulReset_interval();
    DBGPRINTLN("\n-> wait for receive: ");
    nRecvLen = 0;
    while (ulGet_interval(ulTick) < 2000) {
      if (APP_SERIAL.available()) {
        uint8_t chR;
        chR = APP_SERIAL.read();
        if (isprint(chR))
          DBGPRINTF("%c", chR);
        else
          DBGPRINTF("[%02X]", chR);
        //DBGPRINTF("[%02X] %c", chR,chR);
        //DBGPRINTF("%c",chR);
        // receive "\r\n", device is exist.
        if ((chR == '\n') && (nRecvLen >= 1)) {
          if (au8recvBuff[nRecvLen - 1] == '\r') {
            ret = true;
            break;
          }
        }
        au8recvBuff[nRecvLen++] = chR;
        if ((nRecvLen >= sizeof(recvStru.cmdString))) {
          nRecvLen = 0;
        }
        external_loop_process();

      }
      delay_cb(1);
      external_loop_process();
    }
    DBGPRINTF("\n-> ret:[%d]", ret);
    if (ret)
      break;
  }
  gen_AT_CMD(ATE, 0, "");
  send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
  clean_SIO_data();
  return ret;
}


bool NBIOT::is_device_attached()
{
  bool ret = false;
  //入网
  gen_AT_CMD(CGATT);
  send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
  read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT, 4);
  ret = decode_AT_CMD(recvStru.cmd, recvStru.cmdString);
  if (!ret)
      ret = true;
  return ret;
}


bool NBIOT::is_device_singalOK()
{
  bool ret = false;
  //信号情况
  gen_AT_CMD(CESQ);
  send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
  read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT, 4);
  short rssi;
  rssi = decode_AT_CMD(recvStru.cmd, recvStru.cmdString);
  if (!rssi) {
    DBGPRINTF("\n-> network signal is ok [%d]", rssi);
    ret = true;
  }
  clean_SIO_data();
  return ret;
}


bool NBIOT::is_conn_actived()
{
  bool ret = false;
  //连接情况
  gen_AT_CMD(QISTATE);
  send_CMD(strlen(sendStru.cmdString), sendStru.cmdString);
  read_with_wait(CONST_AT_BC26_ECHO_TIME_OUT, 4);
  short rssi;
  rssi = decode_AT_CMD(recvStru.cmd, recvStru.cmdString);
  if (!rssi) {
    DBGPRINTF("\n-> connection is ok [%d]", rssi);
    ret = true;
  }
  clean_SIO_data();
  return ret;
}


/* load application data, return True==Success*/
bool NBIOT::bLoad_config()
{
  disable_interrupts();
  bool bRet = false;
  int nLen;
  char *spTemp;
  char *bigBuffPtr = (char *) au8recvBuff;

  DBGPRINTF("\n-> bLoad_config %s", CONST_APP_NBIOT_RECV_FILE_NAME);
  delay_cb(10);
  if (SPIFFS.exists(CONST_APP_NBIOT_RECV_FILE_NAME))
  {
    File configFile = SPIFFS.open(CONST_APP_NBIOT_RECV_FILE_NAME, "r");
    //DBGPRINTF("\n-> bLoad_config %s", CONST_APP_NBIOT_RECV_FILE_NAME);
    if (!configFile)
    {
      DBGPRINTF("->Failed to open config file :%s", CONST_APP_NBIOT_RECV_FILE_NAME);
      enable_interrupts();
      return bRet;
    }
    // while (True)
    while (configFile.available())
    {
      nLen = configFile.readBytesUntil('\n', bigBuffPtr, sizeof(au8recvBuff) - 2);
      if (nLen <= 0)
        break;
      DBGPRINTF("\n-> bigBuffPtr %s", bigBuffPtr);
      bigBuffPtr[nLen - 1] = '\0'; //trim
      spTemp = strchr((char *)bigBuffPtr, ':');
      if (spTemp == NULL)
        break;//not found;
      spTemp++;

      if (memcmp(bigBuffPtr, "IMEI", 4) == 0) {
        strncpy(saveIMEI, spTemp, CONST_IMEI_NUMBER_LENGTH - 1);
      }
      if (memcmp(bigBuffPtr, "MODE", 4) == 0) {
        saveWorkMode = atoi(spTemp);
      }
    }

    // Real world application would store these values in some variables for later use
    DBGPRINTF("\n->Loaded data: workMode[%d],IMEI[%s]", saveWorkMode, saveIMEI);
    configFile.close();
  }
  else
  {
     DBGPRINTF("\n-> file %s is not exist", CONST_APP_NBIOT_RECV_FILE_NAME);
  }
  bRet = true;

  DBGPRINTLN("\n-> Application Config ok\n");
  enable_interrupts();
  return bRet;
}


/* save application data, return True = success */
bool NBIOT::bSave_config()
{
  disable_interrupts();

  char *bigBuffPtr = (char *) au8recvBuff;

  DBGPRINTF("\n-> bSave_config %s",CONST_APP_NBIOT_RECV_FILE_NAME);

  File configFile = SPIFFS.open(CONST_APP_NBIOT_RECV_FILE_NAME, "w");
  if (!configFile)
  {
    DBGPRINTLN("->Failed to open config file for writing");
    //SPIFFS.format();
    configFile = SPIFFS.open(CONST_APP_NBIOT_RECV_FILE_NAME, "w");
    if (!configFile)
    {
      DBGPRINTF("->Failed to open config file :%s", CONST_APP_NBIOT_RECV_FILE_NAME);
      enable_interrupts();
      return false;
    }
  }
  else{
      
    DBGPRINTF("\n-> IMEI:%s",IMEI);
    configFile.print("IMEI:");
    configFile.println(IMEI);

    DBGPRINTF("\n-> MODE:%d",_workMode);
    configFile.print("MODE:");
    configFile.println(_workMode);

    configFile.close();
  }
  DBGPRINTLN("-> end\n");
  enable_interrupts();
  return true;
}
