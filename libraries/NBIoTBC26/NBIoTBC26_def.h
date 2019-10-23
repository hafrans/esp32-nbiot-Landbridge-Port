/*
   NBIoTBC26_def.h

   Library to NBIoTBC26 AT command interface
   To follow the spec of CMCC/CU/CT
    ---

    Copyright (C) 2019  Steven Lian (steven.lian@gmail.com)
    modified on 2019/05/19    
*/

#pragma once

#ifndef _NBIoTBC26_def_H
#define _NBIoTBC26_def_H

// CONST VAR BEGIN
#define CONST_DEFAULT_TRY_TIMES 5
#define CONST_DEFAULT_TRY_DELAY 500

#define CONST_AT_ERROR_RESET 10 //大约是10分钟必须通信一次,考虑5秒钟通信间隔.
#define CONST_AT_RESPONSE_ERROR_RESET (CONST_DEFAULT_TRY_TIMES*3)

#define CONST_AT_COMM_MODE_COAP 0
#define CONST_AT_COMM_MODE_UDP 1 //default
#define CONST_AT_COMM_MODE_TCP 2
#define CONST_AT_COMM_MODE_CT 3 

//默认服务器地址
#define CONST_AT_DEFAULT_COAP_ADDR "180.101.147.115"
#define CONST_AT_DEFAULT_UDP_ADDR "47.95.209.147"
#define CONST_AT_DEFAULT_CT_ADDR "180.101.147.115"

// 默认端口定义
// 默认连接模式

#define CONST_AT_CONN_TCP 1
#define CONST_AT_CONN_UDP 2

//COAP模式
#define CONST_AT_DEFAULT_COAP_PORT 5683
#define CONST_AT_DEFAULT_UDP_PORT 10001
#define CONST_AT_DEFAULT_TCP_PORT 10000
#define CONST_AT_DEFAULT_CT_PORT 5683
#define CONST_AT_DEFAULT_SEC_PORT 5684

// 默认band
//COAP模式
#define CONST_AT_DEFAULT_COAP_BAND 5
#define CONST_AT_DEFAULT_UDP_BAND 5
#define CONST_AT_DEFAULT_TCP_BAND 5
#define CONST_AT_DEFAULT_CT_BAND 5
#define CONST_AT_DEFAULT_CMCC_BAND 8
#define CONST_AT_DEFAULT_CU_BAND 8									

// 默认APN
#define CONST_AT_DEFAULT_NONE_APN ""	
//COAP模式
#define CONST_AT_DEFAULT_COAP_APN "CTNB"
#define CONST_AT_DEFAULT_UDP_APN "CTNB"
#define CONST_AT_DEFAULT_TCP_APN "CTNB"
#define CONST_AT_DEFAULT_CT_APN "CTNB"

//默认等待LF计数
#define CONST_AT_DEFAULT_LF_COUNT 10

//format string
#define CONST_AT_COMMAND_FMT_TEST1 "AT+%s=%d"
#define CONST_AT_COMMAND_FMT_TEST2 "AT+%s=%s"
#define CONST_AT_COMMAND_FMT_SPECIAL "AT%s"
#define CONST_AT_COMMAND_FMT_READ "AT+%s%d"
#define CONST_AT_COMMAND_FMT_SET "AT+%s=%s"
#define CONST_AT_COMMAND_FMT_EXEC "AT+%s=%d"

//response string
#define CONST_AT_COMMAND_RESPONSE_OK  "OK"
#define CONST_AT_COMMAND_RESPONSE_ERROR  "ERROR"
#define CONST_LF '\n'
#define CONST_CR '\r'

//CMD
#define CONST_AT_CMD_MAX_NUM 64
#define CONST_AT_CMD_MAX_LEN 12

#define CONST_IMSI_NUMBER_LENGTH (15+2)
#define CONST_IMEI_NUMBER_LENGTH (15+1)
#define CONST_IP_NUMBER_LENGTH (15+1)


#define CONST_AT_DATA_LIST_COUNT 6 // 接收数据缓冲队列
#define CONST_AT_DEFAULT_DATA_CHECK_TICK_LEN (1000*6) // 数据标准检测周期

#define CONST_AT_DEFAULT_STATUS_CHECK_TICK_LEN (1000*3) // 标准检测周期
//#define CONST_AT_LONG_STATUS_CHECK_TICK_LEN (1000*3) // 设备联网后，长周期检测
#define CONST_AT_LONG_STATUS_CHECK_TICK_LEN (1000*60*10) // 设备联网后，长周期检测

enum AT_CMD_ENUM {
	ATI,   //Display Product Identification Information 
	ATE,   //Set Command Echo Mode 
	ATW,   //Store Current Parameters to User Defined Profile 
	IPR,   //Set TE-TA Fixed Local Rate 
	CGMI,   //Request Manufacturer Identification 
	CGMM,   //Request Model Identification 
	CGMR,   //Request Manufacturer Revision 
	CSQ,   //Signal Quality Report 
	CESQ,   //Extended Signal Quality 
	CGSN,   //Request Product Serial Number 
	CPIN,   //Enter PIN 
	CEREG,   //EPS Network Registration Status 
	CSCON,   //Signalling Connection Status 
	COPS,   //Operator Selection 
	CGATT,   //PS Attach or Detach 
	CGDCONT,   //Define a PDP Context 
	CGACT,   //PDP Context Activation/Deactivation
	CGPADDR,   //Show PDP Addresses 
	CIMI,   //Request International Mobile Subscriber Identity 
	CFUN,   //Set UE Functionality 
	CMEE,   //Report Mobile Termination Error 
	CCLK,   //Return Current Date and Time 
	CBC,   //Query Power Supply Voltage 
	CPSMS,   //Power Saving Mode Setting 
	CEDRXS,   //eDRX Setting 
	CEDRXRDP,   //eDRX Read Dynamic Parameters 
	CTZR,   //Time Zone Reporting 
	CIPCA,   //Initial PDP Context Activation 
	CCIOTOPT,   //CloT Optimization Configuration 
	CGAPNRC,   //APN Rate Control 
	CEER,   //Extended Error Report 
	QGACT,   //Activate/Deactivate a PDN Context 
	QBAND,   //Get and Set Mobile Operation Band 
	QRST,   //Automatically Reset 
	QSPCHSC,   //Set Scrambling Algorithm 
	QLOCKF,   //Lock NB-IoT Frequency
	QCSEARFCN,   //Clear NB-IoT Stored EARFCN List 
	QCGDEFCONT,   //Set Default PSD Connection Settings 
	QNBIOTRAI,   //NB-IoT Release Assistance Indication 
	QNBIOTEVENT,   //Enable/Disable NB-IoT Related Event Report 
	QATWAKEUP,   //Enable/Disable Deep Sleep Wakeup Indication 
	QENG,   //Engineering Mode 
	QCCID,   //USIM Card Identification 
	QPOWD,   //Power off the Module 
	QSCLK,   //Configure Sleep Mode 
	QRELLOCK,   //Release Sleep Lock of AT Commands\
	//
	QIOPEN,    //Open a Socket Service 
	QICLOSE,    //Close a Socket Service 
	QISTATE,    //Query Socket Service Status 
	QISEND,    //Send Text String Data 
	QIRD,    //Retrieve the Received TCP/IP Data 
	QISENDEX,    //Send Hex String Data 
	QISWTMD,    //Switch Data Access Modes 
	QPING,    //Ping a Remote Server 
	QNTP,    //Synchronize Local Time with NTP Server 
	QIDNSGIP,    //Get IP Address by Domain Name 
	QICFG,    //Configure Optional Parameters 
	//
    QSOC,   //create a socket, 1 IPV4, 2, IPV6;1 TCP, 2 UDP; 1 IP, 2 ICMP;	
	QSOCON, //Connect to server
	QSOSEND, //Send data command
	QSORF, //retrive data command
	QSODIS, //Disconnect Socket
	QSOCL, //Close the socket
	//
	QIGETERROR    //Query the Last Error Code 
};

// simpleEncode 的额外开销
#define CONST_CT_MSG_OVERHEAD_LEN 4

// 针对中国电信华为平台的profile定义,对应的string 类型长度，
// 由于采用base64编码，实际可以传输长度是 (消息长度-OVERHEAD)/4*3,
// 例如：mode 0,空中消息长度是 20，实际数据是 (20-4)/4*3 = 12 bytes.
// 另外增加两个字节，分别表示序号SN，和codingType. 
#define CONST_AT_ADDTIONAL_SN_LEN 1
#define CONST_AT_ADDTIONAL_CODINGTYPE_LEN 1

#define CONST_MODE_NUM_COUNT 7

#define CONST_CT_MSG_MODE_NUM_0 0 
#define CONST_CT_MSG_MODE_LEN_0 20 
#define CONST_CT_MSG_MODE_NUM_1 2
#define CONST_CT_MSG_MODE_LEN_1 40 
#define CONST_CT_MSG_MODE_NUM_2 4 
#define CONST_CT_MSG_MODE_LEN_2 60 
#define CONST_CT_MSG_MODE_NUM_3 6 
#define CONST_CT_MSG_MODE_LEN_3 80 
#define CONST_CT_MSG_MODE_NUM_4 8 
#define CONST_CT_MSG_MODE_LEN_4 120 
#define CONST_CT_MSG_MODE_NUM_5 10 
#define CONST_CT_MSG_MODE_LEN_5 160 
#define CONST_CT_MSG_MODE_NUM_6 12
#define CONST_CT_MSG_MODE_LEN_6 200 

#define CONST_UDP_MSG_MODE_LEN 1500 


//codingType 定义，定义继承自LoRa代码及定义
#define _DEF_COMM_CODING_TYPE_JSON_NUM  0
#define _DEF_COMM_CODING_TYPE_BCD_NUM  1 
#define _DEF_COMM_CODING_TYPE_B6400_NUM  2 //standard base64
#define _DEF_COMM_CODING_TYPE_B6401_NUM  3 //加密的base64,simpleEncode
#define _DEF_COMM_CODING_TYPE_B6402_NUM  4
#define _DEF_COMM_CODING_TYPE_B64C1_NUM  5
#define _DEF_COMM_CODING_TYPE_BINARY  6  //直接发送,无需编码


#define CONST_CT_DEFAULT_CODING_TYPE _DEF_COMM_CODING_TYPE_B6400_NUM


// 缓冲区长度是最大数据类型长度 +8
#define CONST_AT_CMD_DATA_FILL_CHAR ' '
#define CONST_AT_MSG_DATA_MAX_LENGTH (CONST_UDP_MSG_MODE_LEN)

#define CONST_AT_CMD_STRING_LENGTH (CONST_AT_MSG_DATA_MAX_LENGTH+16)
typedef struct {
  char modeNum;
  char SN;
  char codingType;
  char data[CONST_AT_MSG_DATA_MAX_LENGTH+8];
} stDataStruct;

typedef struct {
  short cmd;
  char cmdString[CONST_AT_CMD_STRING_LENGTH];
} stATQueryCMD;

typedef struct {
  uint32_t devID; // 设备ID
  uint16_t length; // 消息长度(bytes),不含stMsgHeader
  uint8_t cmd; // 命令字
  uint8_t SN; // 序列号
} stMsgHeader;

#define CONST_HOST_ADDRESS_LENGTH 48
#define CONST_APN_LENGTH 24

#define CONST_AT_CMD_BUFF_SIZE (CONST_AT_MSG_DATA_MAX_LENGTH*2+16)

// CONST VAR END

/* sample command and response


*/

#endif // _NBIoTBC26_def_H

