/* Copyright © 2014 Kosma Moczek <kosma@cloudyourcar.com>
 * Copyright © 2019 Steven Lian steve.lian@gmail.com
 * This program is free software. It comes without any warranty, to the extent
 * permitted by applicable law. You can redistribute it and/or modify it.
 */

#ifndef NMEA_H_INCLUDED
#define NMEA_H_INCLUDED

#define uint32_t unsigned long int
#define uint16_t unsigned short
#define uint8_t unsigned char

#define int32_t long int
#define int16_t short
#define int8_t char

#define INT_LEAST32_MAX 2147483647

//#define DEBUG_PRINT

enum nmea_type_id
{
  NMEA_TYPE_NONE,
  NMEA_TYPE_RMC,
  NMEA_TYPE_GGA,
  NMEA_TYPE_GSA,
  NMEA_TYPE_GLL,
  NMEA_TYPE_GST,
  NMEA_TYPE_GSV,
  NMEA_TYPE_VTG,
  NMEA_TYPE_ZDA,
};

enum nmea_system_type
{
  NMEA_SYSTEM_BEIDU,
  NMEA_SYSTEM_GPS,
  NMEA_SYSTEM_GLONASS,
  NMEA_SYSTEM_MULTI,
};

// FAA mode added to some fields in NMEA 2.3.
enum nmea_faa_mode {
    NMEA_FAA_MODE_AUTONOMOUS = 'A',
    NMEA_FAA_MODE_DIFFERENTIAL = 'D',
    NMEA_FAA_MODE_ESTIMATED = 'E',
    NMEA_FAA_MODE_MANUAL = 'M',
    NMEA_FAA_MODE_SIMULATED = 'S',
    NMEA_FAA_MODE_NOT_VALID = 'N',
    NMEA_FAA_MODE_PRECISE = 'P',
};

typedef struct {
    int32_t value;
    int32_t scale;
} nmea_float;

typedef struct{
    int nr;
    int elevation;
    int azimuth;
    int snr;
} sat_info_st;


class NMEA
{
public:
  // data
  // GPS general data
  nmea_float latitude;
  nmea_float longitude;
  nmea_float speed;
  nmea_float course;
  nmea_float variation;
  nmea_float altitude;
  nmea_float height;
  nmea_float dgps_age;

  int altitude_units;
  int height_units;

  //system
  int nmea_system; //
  int nmea_type; //nmea_sentence_id
  bool valid;
  //quality
  int fix_quality;
  int satellites_tracked;

  //date time
  int month;
  int day;
  int hours;
  int minutes;
  int seconds;
  int year;
  int microseconds;
  int timeZone;

  //gsa
  char mode;
  char status; //gll
  int fix_type;
  int sats[12];
  nmea_float pdop;
  nmea_float hdop;
  nmea_float vdop;

  //gsv
  int totalMsgs;
  int msgNr;
  int totalSats;
  sat_info_st satInfo[4];

  //vtg
  nmea_float true_track_degrees;
  nmea_float magnetic_track_degrees;
  nmea_float speed_knots;
  nmea_float speed_kph;
  nmea_faa_mode faa_mode;

  // func
  NMEA();
  bool parse(char *strIn); // return true parse is successful
  int getNmeaType(char *strIn);
  int calTimeZone();
  int calTimeZone(int lon);
  void printInfo();

private:
  // data
  int noneData;
  // func
  bool isfield(char c);
  bool scan(const char *sentence, const char *format, ...);

  bool checkValid(char *strIn);
  bool decodeRMC(char *strIn);
  bool decodeGGA(char *strIn);
  bool decodeGSA(char *strIn);
  bool decodeGLL(char *strIn);
  bool decodeGST(char *strIn);
  bool decodeGSV(char *strIn);
  bool decodeVTG(char *strIn);
  bool decodeZDA(char *strIn);

  bool checkDateTime();

};

#endif // NMEA_H_INCLUDED
