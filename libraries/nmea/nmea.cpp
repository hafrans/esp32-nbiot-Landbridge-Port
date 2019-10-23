
/* Copyright © 2014 Kosma Moczek <kosma@cloudyourcar.com>
 * Copyright © 2019 Steven Lian steve.lian@gmail.com
 * This program is free software. It comes without any warranty, to the extent
 * permitted by applicable law. You can redistribute it and/or modify it.
 */

#include "nmea.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>


NMEA::NMEA()
{
  latitude.value =0;
  latitude.scale =0;

  longitude.value =0;
  longitude.scale =0;

  speed.value =0;
  speed.scale =0;

  course.value =0;
  course.scale =0;

  variation.value =0;
  variation.scale =0;

  hdop.value =0;
  hdop.scale =0;

  altitude.value =0;
  altitude.scale =0;

  height.value =0;
  height.scale =0;

  dgps_age.value =0;
  dgps_age.scale =0;

  altitude_units = 0;
  height_units = 0;

  nmea_system = 0;
  nmea_type = 0;

  fix_quality = 0;
  satellites_tracked = 0;

  year = 2019;
  month = 05;
  day = 13;
  hours = 12;
  minutes = 50;
  seconds = 59;
  microseconds = 0;

}

int NMEA::getNmeaType(char *strIn)
{   
    int ret = NMEA_TYPE_NONE;
	char *strP;
	strP = strIn;
	strP +=3;
    if (!strncmp(strP,"RMC",3))
        ret = NMEA_TYPE_RMC;
    else if (!strncmp(strP,"GGA",3))
        ret = NMEA_TYPE_GGA;
    else if (!strncmp(strP,"GSA",3))
        ret = NMEA_TYPE_GSA;
    else if (!strncmp(strP,"GLL",3))
        ret = NMEA_TYPE_GLL;
    else if (!strncmp(strP,"GST",3))
        ret = NMEA_TYPE_GST;
    else if (!strncmp(strP,"GSV",3))
        ret = NMEA_TYPE_GSV;
    else if (!strncmp(strP,"VTG",3))
        ret = NMEA_TYPE_VTG;
    else if (!strncmp(strP,"ZDA",3))
        ret = NMEA_TYPE_ZDA;
	return ret;
}

bool NMEA::parse(char *strIn)
{
  bool ret = true;
  nmea_type = NMEA_TYPE_NONE;
  if (checkValid(strIn) != false)
  {
    nmea_type = getNmeaType(strIn);
    //base on sentence decode string, $GPRMC,$GPVTG,$GPGGA,$GPGSA,$GPGSV,$GPGLL
    switch (nmea_type)
    {
      case NMEA_TYPE_RMC:
        decodeRMC(strIn);
        checkDateTime();
      break;

      case NMEA_TYPE_GGA:
        decodeGGA(strIn);
        checkDateTime();
      break;

      case NMEA_TYPE_GSA:
        decodeGSA(strIn);
      break;

      case NMEA_TYPE_GLL:
        decodeGLL(strIn);
        checkDateTime();
      break;

      case NMEA_TYPE_GST:
        decodeGST(strIn);
      break;

      case NMEA_TYPE_GSV:
        decodeGSV(strIn);
      break;

      case NMEA_TYPE_VTG:
        decodeVTG(strIn);
      break;

      case NMEA_TYPE_ZDA:
        decodeZDA(strIn);
      break;

      default:
        ret = false;
        break;

    }


  }
  else
  {
     ret = false;
  }
  return ret;
}

bool NMEA::checkValid(char *strIn)
{
  bool ret = false;
  if (*strIn == '$')
    ret = true;
  return ret;
}

bool NMEA::isfield(char c)
{
    return isprint((unsigned char) c) && c != ',' && c != '*';
}

bool NMEA::scan(const char *sentence, const char *format, ...)
{
    bool result = false;
    bool optional = false;
    va_list ap;
    va_start(ap, format);

    const char *field = sentence;
#define next_field() \
    do { \
        /* Progress to the next field. */ \
        while (isfield(*sentence)) \
            sentence++; \
        /* Make sure there is a field there. */ \
        if (*sentence == ',') { \
            sentence++; \
            field = sentence; \
        } else { \
            field = NULL; \
        } \
    } while (0)

    while (*format) {
        char type = *format++;

        if (type == ';') {
            // All further fields are optional.
            optional = true;
            continue;
        }

        if (!field && !optional) {
            // Field requested but we ran out if input. Bail out.
            goto parse_error;
        }

        switch (type) {
            case 'c': { // Single character field (char).
                char value = '\0';

                if (field && isfield(*field))
                    value = *field;

                char * vaPointer;
                vaPointer = va_arg(ap, char *);

                *vaPointer = value;
#ifdef DEBUG_PRINT
                printf("\n%x",vaPointer);
#endif
            } break;

            case 'd': { // Single character direction field (int).
                int value = 0;

                if (field && isfield(*field)) {
                    switch (*field) {
                        case 'N':
                        case 'E':
                            value = 1;
                            break;
                        case 'S':
                        case 'W':
                            value = -1;
                            break;
                        default:
                            goto parse_error;
                    }
                }

                int * vaPointer;
                vaPointer = va_arg(ap, int *);

                *vaPointer = value;
#ifdef DEBUG_PRINT
                printf("\n%x",vaPointer);
#endif
            } break;

            case 'f': { // Fractional value with scale (struct nmea_float).
                int sign = 0;
                int32_t value = -1;
                int32_t scale = 0;

                if (field) {
                    while (isfield(*field)) {
                        if (*field == '+' && !sign && value == -1) {
                            sign = 1;
                        } else if (*field == '-' && !sign && value == -1) {
                            sign = -1;
                        } else if (isdigit((unsigned char) *field)) {
                            int digit = *field - '0';
                            if (value == -1)
                                value = 0;
                            if (value > (INT_LEAST32_MAX-digit) / 10) {
                                /* we ran out of bits, what do we do? */
                                if (scale) {
                                    /* truncate extra precision */
                                    break;
                                } else {
                                    /* integer overflow. bail out. */
                                    goto parse_error;
                                }
                            }
                            value = (10 * value) + digit;
                            if (scale)
                                scale *= 10;
                        } else if (*field == '.' && scale == 0) {
                            scale = 1;
                        } else if (*field == ' ') {
                            /* Allow spaces at the start of the field. Not NMEA
                             * conformant, but some modules do this. */
                            if (sign != 0 || value != -1 || scale != 0)
                                goto parse_error;
                        } else {
                            goto parse_error;
                        }
                        field++;
                    }
                }

                if ((sign || scale) && value == -1)
                    goto parse_error;

                if (value == -1) {
                    /* No digits were scanned. */
                    value = 0;
                    scale = 0;
                } else if (scale == 0) {
                    /* No decimal point. */
                    scale = 1;
                }
                if (sign)
                    value *= sign;

                nmea_float * vaPointer;
                vaPointer = va_arg(ap, nmea_float *);

                *vaPointer = (nmea_float) {value, scale};
#ifdef DEBUG_PRINT
                printf("\n%x",vaPointer);
#endif

            } break;

            case 'i': { // Integer value, default 0 (int).
                int value = 0;

                if (field) {
                    char *endptr;
                    value = strtol(field, &endptr, 10);
                    if (isfield(*endptr))
                        goto parse_error;
                }

                int * vaPointer;
                vaPointer = va_arg(ap, int *);

                *vaPointer = value;
#ifdef DEBUG_PRINT
                printf("\n%x",vaPointer);
#endif

            } break;

            case 's': { // String value (char *).
                char *buf = va_arg(ap, char *);
#ifdef DEBUG_PRINT
                printf("\n%x",buf);
#endif

                if (field) {
                    while (isfield(*field))
                        *buf++ = *field++;
                }

                *buf = '\0';
            } break;

            case 't': { // NMEA talker+sentence identifier (char *).
                // This field is always mandatory.
                if (!field)
                    goto parse_error;

                if (field[0] != '$')
                    goto parse_error;
                for (int f=0; f<5; f++)
                    if (!isfield(field[1+f]))
                        goto parse_error;

                char *buf = va_arg(ap, char *);
#ifdef DEBUG_PRINT
                printf("\n%x",buf);
#endif
                memcpy(buf, field+1, 5);
                buf[5] = '\0';
            } break;

            case 'D': { // Date (int, int, int), -1 if empty.
                int * vaPointer;
                vaPointer = va_arg(ap, int *);

                *vaPointer = 0;
#ifdef DEBUG_PRINT
                printf("\n%x",vaPointer);
#endif
                int d = -1, m = -1, y = -1;

                if (field && isfield(*field)) {
                    // Always six digits.
                    for (int f=0; f<6; f++)
                        if (!isdigit((unsigned char) field[f]))
                            goto parse_error;

                    char dArr[] = {field[0], field[1], '\0'};
                    char mArr[] = {field[2], field[3], '\0'};
                    char yArr[] = {field[4], field[5], '\0'};
                    d = strtol(dArr, NULL, 10);
                    m = strtol(mArr, NULL, 10);
                    y = strtol(yArr, NULL, 10);
                }

                day = d;
                month = m;
                year = y;
            } break;

            case 'T': { // Time (int, int, int, int), -1 if empty.
                int * vaPointer;
                vaPointer = va_arg(ap, int *);

                *vaPointer = 0;
#ifdef DEBUG_PRINT
                printf("\n%x",vaPointer);
#endif
                int h = -1, i = -1, s = -1, u = -1;

                if (field && isfield(*field)) {
                    // Minimum required: integer time.
                    for (int f=0; f<6; f++)
                        if (!isdigit((unsigned char) field[f]))
                            goto parse_error;

                    char hArr[] = {field[0], field[1], '\0'};
                    char iArr[] = {field[2], field[3], '\0'};
                    char sArr[] = {field[4], field[5], '\0'};
                    h = strtol(hArr, NULL, 10);
                    i = strtol(iArr, NULL, 10);
                    s = strtol(sArr, NULL, 10);
                    field += 6;

                    // Extra: fractional time. Saved as microseconds.
                    if (*field++ == '.') {
                        uint32_t value = 0;
                        uint32_t scale = 1000000LU;
                        while (isdigit((unsigned char) *field) && scale > 1) {
                            value = (value * 10) + (*field++ - '0');
                            scale /= 10;
                        }
                        u = value * scale;
                    } else {
                        u = 0;
                    }
                }

                hours = h;
                minutes = i;
                seconds = s;
                microseconds = u;
            } break;

            case '_': { // Ignore the field.
            } break;

            default: { // Unknown.
                goto parse_error;
            }
        }

        next_field();
    }

    result = true;

parse_error:
    va_end(ap);
    return result;
}

bool NMEA::decodeRMC(char *strIn)
{
// $GPRMC,081836,A,3751.65,S,14507.36,E,000.0,360.0,130998,011.3,E*62
  bool ret = true;
  char nmeatype[6];
  char validity;
  int latitude_direction;
  int longitude_direction;
  int variation_direction;
  //debug
  /*
  printf("\n addr:\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x",
       nmeatype,
       &noneData,
       &validity,
       &latitude,
       &latitude_direction,
       &longitude,
       &longitude_direction,
       &speed,
       &course,
       &noneData,
       &variation,
       &variation_direction);
  printf("\n---------");
  */
  if (!scan(strIn,"tTcfdfdffDfd",
       nmeatype,
       &noneData,
       &validity,
       &latitude,
       &latitude_direction,
       &longitude,
       &longitude_direction,
       &speed,
       &course,
       &noneData,
       &variation,
       &variation_direction))
    ret = false;
  if (ret)
  {
    valid = (validity == 'A');
    latitude.value *= latitude_direction;
    longitude.value *= longitude_direction;
    variation.value *= variation_direction;
  }
  return ret;
}


bool NMEA::decodeGGA(char *strIn)
{
  // $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
  bool ret = true;
  char nmeatype[6];
  int latitude_direction;
  int longitude_direction;
  char *currP, *endP,*nextP;
  currP = strIn+7;
  endP = strchr(currP,',');
  nextP = endP+1;
  //debug
  /*
  printf("\n addr:\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x",
                nmeatype,
       &noneData,
       &latitude,
       &latitude_direction,
       &longitude,
       &longitude_direction,
       &fix_quality,
       &satellites_tracked,
       &hdop,
       &altitude,
       &altitude_units,
       &height,
       &height_units,
       &dgps_age);
  printf("\n---------");
  */
  if (!scan(strIn,"tTfdfdiiffcfcf_",
       nmeatype,
       &noneData,
       &latitude,
       &latitude_direction,
       &longitude,
       &longitude_direction,
       &fix_quality,
       &satellites_tracked,
       &hdop,
       &altitude,
       &altitude_units,
       &height,
       &height_units,
       &dgps_age))
    ret = false;
  if (ret)
  {
    latitude.value *= latitude_direction;
    longitude.value *= longitude_direction;
  }
  return ret;

}


bool NMEA::decodeGSA(char *strIn)
{
// $GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39
  bool ret = true;
  char nmeatype[6];
  printf("\n decodeGSA begin");
  //debug
  
  printf("\n addr:\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x",
       nmeatype,
       &mode,
       &sats[0],
       &sats[1],
       &sats[2],
       &sats[3],
       &sats[4],
       &sats[5],
       &sats[6],
       &sats[7],
       &sats[8],
       &sats[9],
       &sats[10],
       &sats[11],
       &pdop,
       &hdop,
       &vdop);
  printf("\n---------");
  
  if (!scan(strIn,"tciiiiiiiiiiiiifff",
       nmeatype,
       &mode,
       &sats[0],
       &sats[1],
       &sats[2],
       &sats[3],
       &sats[4],
       &sats[5],
       &sats[6],
       &sats[7],
       &sats[8],
       &sats[9],
       &sats[10],
       &sats[11],
       &pdop,
       &hdop,
       &vdop))
    ret = false;
  if (ret)
  {
  }
  printf("\n decodeGSA end");
  return ret;
}


bool NMEA::decodeGLL(char *strIn)
{
// $GPGLL,3723.2475,N,12158.3416,W,161229.487,A,A*41$;
  bool ret = true;
  char nmeatype[6];
  int latitude_direction;
  int longitude_direction;
  //debug
  /*
  printf("\n addr:\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x",
       nmeatype,
       &latitude,
       &latitude_direction,
       &longitude,
       &longitude_direction,
       &noneData,
       &status,
       &mode);
  printf("\n---------");
  */
  if (!scan(strIn,"tfdfdTc;c",
       nmeatype,
       &latitude,
       &latitude_direction,
       &longitude,
       &longitude_direction,
       &noneData,
       &status,
       &mode))
    ret = false;
  if (ret)
  {
    latitude.value *= latitude_direction;
    longitude.value *= longitude_direction;
  }
  return ret;

}


bool NMEA::decodeGST(char *strIn)
{
// $GPGST,024603.00,3.2,6.6,4.7,47.3,5.8,5.6,22.0*58
  bool ret = true;
  char nmeatype[6];
  //debug

  return ret;

}


bool NMEA::decodeGSV(char *strIn)
{
  // $GPGSV,3,1,11,03,03,111,00,04,15,270,00,06,01,010,00,13,06,292,00*74
  // $GPGSV,3,3,11,22,42,067,42,24,14,311,43,27,05,244,00,,,,*4D
  // $GPGSV,4,2,11,08,51,203,30,09,45,215,28*75
  // $GPGSV,4,4,13,39,31,170,27*40
  // $GPGSV,4,4,13*7B
  bool ret = true;
  char nmeatype[6];
  //debug
  /*
  printf("\n addr:\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x",
       nmeatype,
       &totalMsgs,
       &msgNr,
       &totalSats,
       &satInfo[0].nr,
       &satInfo[0].elevation,
       &satInfo[0].azimuth,
       &satInfo[0].snr,
       &satInfo[1].nr,
       &satInfo[1].elevation,
       &satInfo[1].azimuth,
       &satInfo[1].snr,
       &satInfo[2].nr,
       &satInfo[2].elevation,
       &satInfo[2].azimuth,
       &satInfo[2].snr,
       &satInfo[3].nr,
       &satInfo[3].elevation,
       &satInfo[3].azimuth,
       &satInfo[3].snr);
  printf("\n---------");
  */
  if (!scan(strIn,"tiii;iiiiiiiiiiiiiiii",
       nmeatype,
       &totalMsgs,
       &msgNr,
       &totalSats,
       &satInfo[0].nr,
       &satInfo[0].elevation,
       &satInfo[0].azimuth,
       &satInfo[0].snr,
       &satInfo[1].nr,
       &satInfo[1].elevation,
       &satInfo[1].azimuth,
       &satInfo[1].snr,
       &satInfo[2].nr,
       &satInfo[2].elevation,
       &satInfo[2].azimuth,
       &satInfo[2].snr,
       &satInfo[3].nr,
       &satInfo[3].elevation,
       &satInfo[3].azimuth,
       &satInfo[3].snr))
    ret = false;
  if (ret)
  {

  }
  return ret;

}


bool NMEA::decodeVTG(char *strIn)
{
// $GPVTG,054.7,T,034.4,M,005.5,N,010.2,K*48
// $GPVTG,156.1,T,140.9,M,0.0,N,0.0,K*41
// $GPVTG,096.5,T,083.5,M,0.0,N,0.0,K,D*22
// $GPVTG,188.36,T,,M,0.820,N,1.519,K,A*3F
  bool ret = true;
  char nmeatype[6];
  char c_true, c_magnetic, c_knots, c_kph, c_faa_mode;
  //debug
  /*
  printf("\n addr:\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x\n%x",
       nmeatype,
       &true_track_degrees,
       &c_true,
       &magnetic_track_degrees,
       &c_magnetic,
       &speed_knots,
       &c_knots,
       &speed_kph,
       &c_kph,
       &c_faa_mode);
  printf("\n---------");
  */
  if (!scan(strIn,"tfcfcfcfc;c",
       nmeatype,
       &true_track_degrees,
       &c_true,
       &magnetic_track_degrees,
       &c_magnetic,
       &speed_knots,
       &c_knots,
       &speed_kph,
       &c_kph,
       &c_faa_mode))
    ret = false;
  // check chars
  if (c_true != 'T' ||
      c_magnetic != 'M' ||
      c_knots != 'N' ||
      c_kph != 'K')
    ret = false;
  if (ret)
  {
      faa_mode = (enum nmea_faa_mode)c_faa_mode;
  }
  return ret;

}


bool NMEA::decodeZDA(char *strIn)
{

}

bool NMEA::checkDateTime()
{
    bool ret = true;
    if (year < 100){
        year += 2000; //convert 19 year --> 2019
    }
    timeZone = calTimeZone();
}


int NMEA::calTimeZone()
{
    int lon = 0;
    if (longitude.scale > 0)
        lon = longitude.value/longitude.scale/100;
    return calTimeZone(lon);
}


int NMEA::calTimeZone(int lon)
{
    int ret = 0;//default UTC-0
    int quotient;//
    int remainder;
    quotient = lon/15;
    remainder = (abs(lon)-(abs(quotient) * 15))*2;
    if (remainder <= 15)
    {
        ret = quotient;
    }
    else
    {
        ret = quotient + (lon > 0 ?  1 : -1);
    }
    return ret;
}


void NMEA::printInfo()
{
  int i;
  printf("\n********* Info Begin ************");
  
  printf("\n date time:[%04d-%02d-%02d %02d:%02d:%02d %03d] UTC[%d]",year,month,day,hours, minutes, seconds,microseconds,timeZone);
  printf("\n latitude:[%ld,%ld]",latitude.value,latitude.scale);
  printf("\n longitude:[%ld,%ld]",longitude.value,longitude.scale);
  printf("\n speed:[%ld,%ld]",speed.value,speed.scale);
  printf("\n altitude:[%ld,%ld]",altitude.value,altitude.scale);
  printf("\n altitude_units:[%C]",altitude_units);
  printf("\n height:[%ld,%ld]",height.value,height.scale);
  printf("\n height_units:[%C]",height_units);
  printf("\n fix_quality:[%d]",fix_quality);
  printf("\n satellites_tracked:[%d]",satellites_tracked);
  printf("\n pdop:[%ld,%ld]",pdop.value,pdop.scale);
  printf("\n hdop:[%ld,%ld]",hdop.value,hdop.scale);
  printf("\n vdop:[%ld,%ld]",vdop.value,vdop.scale);
  printf("\n dgps_age:[%ld,%ld]",dgps_age.value,dgps_age.scale);
  printf("\n course:[%ld,%ld]",course.value,course.scale);
  printf("\n variation:[%ld,%ld]",variation.value,variation.scale);
  printf("\n mode:[%C], status:[%C], fix_type [%d]",mode,status, fix_type);
  for (i=0;i<12;i++)
  {
      printf("\n sats[%d]=[%d]",i,sats[i]);
  }
  printf("\n totalMsgs:[%d], msgNr:[%d], totalSats [%d]",totalMsgs,msgNr, totalSats);
  for (i=0;i<4;i++)
  {
      printf("\n sats:[%d], nr[%d],elevation[%d],azimuth[%d],snr[%d]",i,satInfo[i].nr,satInfo[i].elevation,satInfo[i].azimuth,satInfo[i].snr);
  }
  printf("\n true_track_degrees:[%ld,%ld]",true_track_degrees.value,true_track_degrees.scale);
  printf("\n magnetic_track_degrees:[%ld,%ld]",magnetic_track_degrees.value,magnetic_track_degrees.scale);
  printf("\n speed_knots:[%ld,%ld]",speed_knots.value,speed_knots.scale);
  printf("\n speed_kph:[%ld,%ld]",speed_kph.value,speed_kph.scale);
  printf("\n faa_mode:[%C]",faa_mode);
  
  printf("\n*********  Info End *************");

}
