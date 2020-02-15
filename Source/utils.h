#ifndef __UTILS_H__
#define __UTILS_H__

#include <cstdlib>
#include <cstring>
#include <iostream>
#include "cgidata.h"
#ifdef __WIN__
#include <Windows.h>
#include "drand48.h"
#endif

static inline u32 getU32FromVoid(const void *buf){
	u32 ret;
	memcpy((void*)&ret, buf, sizeof(u32));
	return(ret);
}

#if(!defined(_MSC_VER) && !defined(_MINGW_VER))
void setSerialAttributes(int fd);
#else
bool setSerialAttributes(HANDLE fd);
#endif

extern void printPacket       (std::ostream &o, const CGIDataCartesian &data);
extern void printPacketHex    (std::ostream &o, const CGIDataCartesian &data);
extern void printPacketColumns(std::ostream &o, const CGIDataCartesian &data);
extern void printPacketCSV    (std::ostream &o, const CGIDataCartesian &data);

extern int initRand();
extern u32 random32();

static inline double drand(const double & lower, const double &upper){
	return(lower + drand48() * (upper - lower));
}

static inline u8 getSequentialChar(){
	static u32 state = 0;
	u32 val32 = state;
	if(state & 0x1) val32 |= 0x100; else val32 &= ~0x100;
	u8 val = (val32 >> 1) & 0xff;
	state++;
	return(val);
}


extern std::ostream& operator<< (std::ostream& o, const CGIDataCartesian &data);

#endif //__UTILS_H__
