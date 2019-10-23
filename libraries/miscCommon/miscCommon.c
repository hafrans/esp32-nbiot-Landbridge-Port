#include "miscCommon.h"

unsigned long ulReset_interval()
{
  return millis();
}

unsigned long ulAdd_interval(unsigned long addTick)
{
  unsigned long ret = millis();
  ret += addTick;
  //make sure ret !=0
  if (ret==0)
    ret++;
  return ret;
}


unsigned long ulGet_interval(unsigned long checkTick)
{
  unsigned long cur = millis();
  if (cur >= checkTick)
    return cur - checkTick;
  else
    return 0xffffffff - checkTick + cur;
}

unsigned long ulReset_microseconds()
{
  return micros();
}

unsigned long ulAdd_microseconds(unsigned long addTick)
{
  unsigned long ret = micros();
  ret+=addTick;
  //make sure ret !=0
  if (ret==0)
    ret++;
  return ret;
}

unsigned long ulGet_microseconds(unsigned long checkTick)
{
  unsigned long cur = micros();
  if (cur >= checkTick)
    return cur - checkTick;
  else
    return 0xffffffff - checkTick + cur;;
}

static unsigned short secondCount; //the seconds since system restart;
static unsigned long secondTicks;

void second_loop()
{
  if (ulGet_interval(secondTicks) >= 1000)
  {
    secondCount++;
    if (secondCount==0){
      secondCount++;      
    }
    secondTicks=ulReset_interval();
  }

}
unsigned short  nReset_seconds()
{
  return secondCount;
}

unsigned short  nGet_seconds(unsigned short checkSeconds)
{
  unsigned short cur = secondCount;
  if (cur >= checkSeconds)
    return cur - checkSeconds;
  else
    return 0xffff - checkSeconds + cur;
}

unsigned short  nAdd_seconds(unsigned short addSeconds)
{
  unsigned short ret = secondCount;
  ret += addSeconds;
  if (ret==0)
    ret++;
  return ret;
}




/*
   conver a hex data to ascii format
   ex: 0x1--> 0x31 ('1')
*/
uint8_t hex(int nHex)
{
  uint8_t cc;
  cc = 0x30;
  switch (nHex)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      cc = 0x30 + nHex;
      break;
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
    case 0x0E:
    case 0x0F:
      cc = 'A' + nHex - 0x0A;
      break;
  }
  //DBGPRINTF("hex=[%x]\n",hex);
  //DBGPRINTF("cc=[%x]\n",cc);
  return cc;
}

/*
   conver a hex data to ascii format
   ex: 0x1--> 0x31 ('1')
*/
uint8_t ascii(int nHex)
{
  return hex(nHex);
}

/*
char isprint (unsigned char c)
{
    if ( c >= 0x20 && c <= 0x7e )
        return 1;
    return 0;
}
*/

/*
   conver a ascii data to int format
   ex: 0x31('1')--> 0x1 ('1')
*/
uint8_t ascii_hex(uint8_t cc)
{
  uint8_t hex;
  hex = 0x00;
  switch (cc)
  {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      hex = cc - 0x30;
      break;
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
      hex = cc - 'a' + 0x0A;
      break;
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
      hex = cc - 'A' + 0x0A;
  }
  return hex;
}


int int16(char *hex)
{    
  int i,t;    
  int sum=0;    
  for (i=0;hex[i];i++)    
  {        
    if (hex[i]=='x' || hex[i]=='X')            
      continue;        
    if (hex[i]<='9' && hex[i]>='0')              
      t=hex[i]-'0';        
    else if (hex[i]<='f' && hex[i]>='a')             
      t=hex[i]-'a'+10;        
    else if (hex[i]<='F' && hex[i]>='A')             
      t=hex[i]-'A'+10;        
    else            
      break;        
    sum=(sum<<4)+t;     
  }      
  return sum;
}


void convert_to_ip_format(char *ptr, uint8_t *ip, short nLen)
{
  if (nLen == 3) {
    sprintf(ptr, "%d.%d.%d", *(ip), *(ip + 1), *(ip + 2));
  }
  else {
    sprintf(ptr, "%d.%d.%d.%d", *(ip), *(ip + 1), *(ip + 2), *(ip + 3));
  }
}

void convert_to_hex_format(char *ptr, uint8_t *data, short nLen)
{
  short i;
  char *spStart;
  spStart = ptr;
  for (i = 0; i < nLen; i++)
  {
    sprintf(spStart, "%02X ", data[i]);
    spStart += 3;
  }
  *spStart = 0;
}

void convert_to_dec_format(char *ptr, uint8_t *data, short nLen)
{
  short i;
  char *spStart;
  spStart = ptr;
  for (i = 0; i < nLen; i++)
  {
    sprintf(spStart, "%03d ", data[i]);
    spStart += 4;
  }
  *spStart = 0;
}


// return a char position in a char *
int strchrPos(char achData[], char chData)
{
  int nRet = -1, pos = 0;
  while (1)
  {
    if (achData[pos] == '\0')
      break;
    if (achData[pos] == chData)
    {
      nRet = pos;
      break;
    }
    pos++;
  }
  return nRet;
}



//return the pointer to trimed string;
char * strTrim(char *str)
{
  char *p = str;
  while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
    p ++;
  str = p;
  p = str + strlen(str) - 1;
  while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
    -- p;
  *(p + 1) = 0;
  return str;
}



int nGet_json_pos(char achData[], char achKey[], char **spPos)
{
  int nRet = -1;
  char *spStart, *spEnd;
  int nLen;
  nLen = strlen(achKey);
  spStart = strstr(achData, achKey);
  if (spStart != NULL)
  {
    spStart += (1 + nLen);
    if (*(spStart) == ':')
    {
      spStart++;
      if ((*spStart) == '\"')
      {
        //string
        spStart++;
        spEnd = strchr((spStart), '\"');
      }
      else if ((*spStart) == '[')
      {
        //list
        spStart++;
        spEnd = strchr((spStart), ']');
      }
      else if ((*spStart) == '{')
      {
        //dict
        spStart++;
        spEnd = strchr((spStart), '}');
      }
      else
      {
        spEnd = strchr((spStart), ',');
      }
      *spPos = spStart;
      nRet = spEnd - spStart;
    }

  }

  return nRet;
}



char *get_short_split(char *ptr, short *nData, char chSplit)
{
  char *ret = NULL;
  long nT1;
  ret=get_long_split(ptr,&nT1,chSplit);
  *nData=(short) nT1;
  return ret;
}


char *get_int_split(char *ptr, int *nData, char chSplit)
{
  char *ret = NULL;
  long nT1;
  ret=get_long_split(ptr,&nT1,chSplit);
  *nData=(int) nT1;
  return ret;
}


char *get_long_split(char *ptr, long *nData, char chSplit)
{
  char *ret = NULL;
  char *spStart, *spEnd;
  spStart = ptr;
  spEnd = strchr(spStart, chSplit);
  *spEnd = '\0';

  if (strlen(spStart) > 10) {
    return ret;
  }

  if (strlen(spStart) > 0)
  {
    *nData = atol(spStart);
  }
  spEnd++;
  ret = spEnd;
  return ret;
}


char *get_str_split_with_len(char *ptr, char **spData, char chSplit,short nMaxLen)
{
  char *ret = NULL;
  char *spStart, *spEnd;
  spStart = ptr;
  spEnd = strchr(spStart, chSplit);
  *spEnd = '\0';
  if (strlen(spStart) >= (nMaxLen - 1)) {
    return ret;
  }
  if (strlen(spStart) > 0)
  {
    *spData = spStart;
  }
  spEnd++;
  ret = spEnd;
  return ret;
}


char *get_char_split(char *ptr, char *spData, char chSplit)
{
  char *ret = NULL;
  char *spStart, *spEnd;
  spStart = ptr;
  spEnd = strchr(spStart, chSplit);
  *spEnd = '\0';
  if (strlen(spStart) >= 2) {
    return ret;
  }
  if (strlen(spStart) > 0)
  {
    *spData = *spStart;
  }
  spEnd++;
  ret = spEnd;
  return ret;
}

