/*
 ****************************************************************************
 ****************************************************************************
 * Copyright (c) 2008 Jan Frohn. All rights reserved.                       *
 ****************************************************************************
 * Permission to use, copy, modify, distribute, and sell this software for  *
 * any purpose is hereby granted without fee, provided that the above       *
 * copyright notice and this permission notice appear in all copies.        *
 * This file is distributed in the hope that it will be useful, but WITHOUT *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    *
 * FITNESS FOR A PARTICULAR PURPOSE.                                        *
 ****************************************************************************
 ****************************************************************************
  */

#ifdef __KERNEL__
#include <linux/types.h>
//typedef __u64 u64;
//typedef __u32 u32;
//typedef __u16 u16;
//typedef __u8 u8;
#include "math_builtin.h"
#else
#include <math.h>
#include <stddef.h>
#endif 

#include "cgidata.h"

u32 checkSum(const void *data, unsigned int size){
  int length = size / sizeof(u32), i; 
  u32 *u32Ptr = (u32*)(data); 
  u32 sum = 0 - u32Ptr[1];
  for(i=2; i<length-1; i++) sum += u32Ptr[i];
  return(sum);
}

#define SM_WAIT_1 (0)
#define SM_WAIT_2 (1)
#define SM_WAIT_3 (2)
#define SM_WAIT_4 (3)
#define SM_SYNC_HAPPENED (4)

//const double pi  = 3.1415926535897932384626433;//alternatively: 2.0 * atan2(0.0, 1.0)
//const double pi2 = 2.0 * 3.1415926535897932384626433;
//const double packetDuration = 115200.0 / ((8 + 1 + 1) * CGI_DATA_LENGTH); 

const int offsetOfFirstCGIDatum = offsetof(struct CGIDataCartesian, packetNumber);


struct binaryDataSyncState{
  u32 state;
  u16 charIndex; 
};


/*
 * This function must be called sequentially for every received character from the serial port
 * and builds up the receive data structure of type CGIDataCartesian (except the sync-val itself). 
 * After having received a complete sync sequence, it sequentially stores the characters in input 
 * to *data.
 *
 * The return values are:
 *
 * -1 if an ckeck-sum error has been detected. 
 *  0 if waiting for the first  sync character.
 *  1 if waiting for the second sync character.
 *  2 if waiting for the third  sync character.
 *  3 if waiting for the forth  sync character.
 *  4 if the full sync sequence has been sucesfully received.
 *  Elsewise the index of the character written to *data plus one. 
 *
 * Consequently, fillData returns 5 when the lowest address byte of packetNumber has been written 
 * and CGI_DATA_LENGTH when the packet has been completely received.   
 */
int fillData(u8 input, struct CGIDataCartesian *data){
  static struct binaryDataSyncState syncState ={SM_WAIT_1, 0};
  switch (syncState.state){
    case SM_WAIT_1:
      if(input == TDDE_SYNC_VAL_1){
        syncState.state = SM_WAIT_2;
        return(1);
      }
      //rt_printk("SM_WAIT_1 error!");
      return(0);

    case SM_WAIT_2:
      if(input == TDDE_SYNC_VAL_2){
        syncState.state = SM_WAIT_3;
        return(2);
      }
      //rt_printk("SM_WAIT_2 error!");
      if(input == TDDE_SYNC_VAL_1){
        syncState.state = SM_WAIT_2;
        return(1);
      }
      syncState.state = SM_WAIT_1;
      return(0);

    case SM_WAIT_3:
      if(input == TDDE_SYNC_VAL_3){
        syncState.state = SM_WAIT_4;
        return(3);
      }
      //rt_printk("SM_WAIT_3 error!");
      if(input == TDDE_SYNC_VAL_1){
        syncState.state = SM_WAIT_2;
        return(1);
      }
      syncState.state = SM_WAIT_1;
      return(0);

    case SM_WAIT_4:
      if(input == TDDE_SYNC_VAL_4){
        syncState.state = SM_SYNC_HAPPENED;
        syncState.charIndex = offsetOfFirstCGIDatum;
        return(offsetOfFirstCGIDatum);
      }
      //rt_printk("SM_WAIT_4 error!");
      if(input == TDDE_SYNC_VAL_1)  syncState.state = SM_WAIT_2;
      else                          syncState.state = SM_WAIT_1;
      return(0);

    case SM_SYNC_HAPPENED:
      ((char*)(data))[syncState.charIndex] = input;
      if(syncState.charIndex == CGI_DATA_LENGTH - 1){
        u32 sum;
        syncState.state = SM_WAIT_1;
        sum = checkSum((void*)data, CGI_DATA_LENGTH);
        if(data->checkSum != sum) {
          /*
					 * Ich sollte eine genersiche Ausgabefunktion fuer Kernel- und Userspace vorsehen!
					 */
          //rt_printk("sum: %x data->checkSum: %x", sum, data->checkSum);
					return(-1);
        }
        return(CGI_DATA_LENGTH);
      }
      return(++syncState.charIndex);
    default:  //should never happen!
      syncState.state = SM_WAIT_1;
      //rt_printk("state machine error!");
      return(0);
  }
}

#ifndef __KERNEL__
void time2TimeCodeAPI(struct timeCodeAPIStruct *TC, double seconds, int TCType){
  double integralPart;
  double secondsFrac = modf(seconds, &integralPart), minutesFrac, hoursFrac, daysFrac;
  (void)TCType;

  TCType = TCTYPE_PAL;
  TC->frames   = (u8)(secondsFrac  * 25.0);
  minutesFrac = modf(integralPart / 60.0, &integralPart);
  TC->seconds  = (u8)(minutesFrac * 60.0);
  hoursFrac   = modf(integralPart / 60.0, &integralPart);
  TC->minutes  = (u8)(hoursFrac   * 60.0);
  daysFrac    = modf(integralPart / 24.0, &integralPart);
  TC->hours    = (u8)(daysFrac      * 24.0); 
}
#endif


const char* getTCStringFromTCS(const struct timeCodeStruct *TC, char *ret){
  ret[10] = TC->frames  % 10  + '0'; 
  ret[9]  = TC->frames  / 10   + '0'; 
  ret[8]  = ':'; 
  ret[7]  = TC->seconds % 10 + '0'; 
  ret[6]  = TC->seconds / 10  + '0'; 
  ret[5]  = ':'; 
  ret[4]  = TC->minutes % 10 + '0'; 
  ret[3]  = TC->minutes / 10  + '0'; 
  ret[2]  = ':'; 
  ret[1]  = TC->hours   % 10   + '0'; 
  ret[0]  = TC->hours   / 10    + '0'; 
  ret[11] = 0; 
  return(ret);
}

struct timeCodeAPIStruct timeCode2TCAPI(const struct timeCodeStruct *TC){
  struct timeCodeAPIStruct TCRet;
  TCRet.frames          = TC->frames;
  TCRet.seconds         = TC->seconds;
  TCRet.minutes         = TC->minutes;
  TCRet.hours           = TC->hours;
  TCRet.dropFrame       = TC->dropFrame;
  TCRet.colorFrame      = TC->colorFrame;
  TCRet.userBitFlags    = TC->userBitFlags;
  TCRet.isLTC           = TC->isLTC;
  TCRet.evenFieldMark   = TC->evenFieldMark;
  TCRet.phaseCorrection = TC->phaseCorrection;
  return(TCRet);
}

 struct timeCodeStruct TCAPI2timeCode(const struct timeCodeAPIStruct *API){
  struct timeCodeStruct TC;
  TC.frames              = API->frames;
  TC.seconds             = API->seconds;
  TC.minutes             = API->minutes;
  TC.hours               = API->hours;
  TC.dropFrame           = API->dropFrame     ? 1 : 0;
  TC.colorFrame          = API->colorFrame;
  TC.userBitFlags        = API->userBitFlags  & 0x3;
  TC.isLTC               = API->isLTC         ? 1 : 0;
  TC.evenFieldMark       = API->evenFieldMark ? 1 : 0;
  TC.phaseCorrection     = API->phaseCorrection;
  TC.CGIProtocolIndicator = 1;
  return(TC);
}

char CGIHasTimecode(const struct CGIDataCartesian *CGI){
  struct CGIDataCartesianVersion1 *ptr = (struct CGIDataCartesianVersion1 *)CGI;
  return(ptr->frameNumber & 0x80000000 ? 1 : 0);
}