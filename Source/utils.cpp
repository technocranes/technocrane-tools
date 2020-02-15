/*
 ****************************************************************************
 ****************************************************************************
 * Copyright (c) 2008 Jan Frohn. All rights reserved.                       *
 ****************************************************************************
 * This program may be distributed and/or modified                          *
 * under the terms of the GNU General Public License version 2 as published *
 * by the Free Software Foundation and appearing in the file gpl.txt        *
 * included in the packaging of this file.                                  *
 * This program is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                     *
 * See the GNU General Public License for more details.                     *
 ****************************************************************************
 ****************************************************************************
 * Dieses Programm und saemtliche Dateien sind urheberrechtlich geschuetzt. *
 * Der Inhaber aller Rechte an diesem Programm ist Jan Frohn.               *
 * (c) 2008, Jan Frohn                                                      *
 ****************************************************************************
 * Dieses Programm kann unter den Bedingungen der GNU General Public        *
 * License Version 2, wie von der Free Software Foundation veroeffentlicht, *
 * weitergeben und/oder modifiziert werden. Die englischsprachige           * 
 * Originalversion der Lizenz kann der Datei gpl.txt entnommen werden, die  *
 * dem Packet beiliegt.                                                     *
 * Die Veroeffentlichung dieses Programms erfolgt in der Hoffnung, dass es  *
 * Ihnen von Nutzen sein wird, aber OHNE IRGENDEINE GARANTIE, sogar ohne    *
 * die implizite Garantie der MARKTREIFE oder der VERWENDBARKEIT FÜR EINEN  *
 * BESTIMMTEN ZWECK. Details finden Sie in der GNU General Public License.  *
 ****************************************************************************
 ****************************************************************************
 */
#if(defined(_MSC_VER) || defined(__MINGW64__))
#ifndef __WIN__
#define __WIN__
#endif
#endif

#ifdef __WIN__
#include <Windows.h>
#include <random>
#include "unistd.h"
#include "drand48.h"
#else
#include <termios.h>
#endif
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include "cgidata.h"
#include "utils.h"




#ifndef __WIN__
/*
 * See also: man termios. man setserial and  man stty my also be interesting.
 */
void setSerialAttributes(int fd){
	struct termios uart;
	cfmakeraw(&uart);
	 
	uart.c_cflag &= ~(CSIZE | CSTOPB | PARENB | CRTSCTS | PARODD | CMSPAR);
	uart.c_cflag |=   CS8 | CREAD | CLOCAL;  

	uart.c_iflag &= ~(INPCK | ISTRIP | INLCR | ICRNL | IUCLC | IGNBRK | BRKINT | IGNCR 
	                        | IXOFF  | IXON  | PARMRK |IGNPAR);
	uart.c_iflag |=   (0);  

	uart.c_oflag &= ~( OPOST);
	uart.c_oflag |=   (0);  

	uart.c_lflag &= ~(ISIG | ICANON | ECHO);
	uart.c_lflag |= (0);

	cfsetospeed(&uart, B115200);
	tcsetattr(fd, TCSANOW, &uart);
	
	//The code below can be used to put DTR pin and RTS pin to a certain logic level.
	//This may be helpfull with some RS422 <-> RS232 converters with are powered from these pins
	 
	//int serial = TIOCM_DTR | TIOCM_RTS;
	//ioctl(fd, TIOCMBIC, &serial);//clear bits
	//ioctl(fd, TIOCMBIS, &serial);//set bits
} 
#else
void system_error(char *name) {
	// Retrieve, format, and print out a message from the last error.  The 
	// `name' that's passed should be in the form of a present tense noun 
	// (phrase) such as "opening file".
	//
	char *ptr = NULL;
	FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_BUFFER |
	FORMAT_MESSAGE_FROM_SYSTEM,
	0,
	GetLastError(),
	0,
	(char *)&ptr,
	1024,
	NULL);

	fprintf(stderr, "\nError %s: %s\n", name, ptr);
	LocalFree(ptr);
}
std::random_device  rd;
std::mt19937    mt(rd());
std::uniform_int_distribution<int>  dist(1, 20);

bool setSerialAttributes(HANDLE fd){
	DCB dcb;

	memset(&dcb, 0, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);

	if(!GetCommState(fd, &dcb) )
		system_error("getting comm state");

	if(!BuildCommDCB("baud=115200 parity=n data=8 stop=1", &dcb))
		system_error("building comm DCB");
	/*
	//dcb.DCBlength = sizeof(dcb);
	dcb.BaudRate = CBR_115200;     // set the baud rate
	dcb.ByteSize = 8;             // data size, xmit, and rcv
	dcb.Parity = PARITY_NONE;      // PARITY_NONE  // parity odd
	dcb.StopBits = ONESTOPBIT; // ONESTOPBIT;    // one stop bit
	//dcb.fOutxCtsFlow = FALSE;
	//dcb.fRtsControl = RTS_CONTROL_DISABLE;
	//dcb.fBinary = TRUE;
	*/
	BOOL fSuccess = SetCommState(fd, &dcb);

	if(FALSE == fSuccess)
		return false;

	fSuccess = GetCommState(fd, &dcb);
	if(FALSE == fSuccess)
		return false;

	BOOL buff_success = SetupComm(fd, 1024, 1024);
	
	if (FALSE == buff_success)
		return false;

	COMMTIMEOUTS cmt;
	// ReadIntervalTimeout in ms
	cmt.ReadIntervalTimeout = 1000;
	cmt.ReadTotalTimeoutMultiplier = 1000;
	cmt.ReadTotalTimeoutConstant=1000;
	BOOL timeout_flag = SetCommTimeouts(fd, &cmt);

	// reset driver cache
	//BOOL purgeRes = PurgeComm(fd, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	if(!EscapeCommFunction(fd, CLRDTR))
		system_error("clearing DTR");
	Sleep(200);
	if(!EscapeCommFunction(fd, SETDTR))
		system_error("setting DTR");

	return true;
}
#endif

void printPacketColumns(std::ostream &o, const CGIDataCartesian &data){
	struct CGIDataCartesianVersion2 *data2 = (struct CGIDataCartesianVersion2*)(void*)&data;
	struct CGIDataCartesianVersion1 *data1 = (struct CGIDataCartesianVersion1*)(void*)&data;
	const int spatialPrec = 6;
	const int angularPrec = 6;
	const int lensPrec    = 4;

	o<<std::dec<<std::setw(6)<<std::left;
	o<<std::setw(6)<<data.packetNumber<<" ";
	if(!data2->timeCode.CGIProtocolIndicator){
		o<<std::setw(6)<<data1->frameNumber<<" ";
		o<<std::setw(11)<<std::setprecision(10)<<data1->time<<" ";     
	}else{
		o<<std::setw(2)<<data2->timeCode.hours<<":"
		 <<std::setw(2)<<data2->timeCode.minutes<<":"
		 <<std::setw(2)<<data2->timeCode.seconds<<":"
		 <<std::setw(2)<<data2->timeCode.frames<<" ";
	} 
	o<<std::setw(spatialPrec + 6)<<std::setprecision(spatialPrec)<<data.x<<" ";
	o<<std::setw(spatialPrec + 6)<<std::setprecision(spatialPrec)<<data.y<<" ";
	o<<std::setw(spatialPrec + 6)<<std::setprecision(spatialPrec)<<data.z<<" ";

	o<<std::setw(angularPrec + 6)<<std::setprecision(angularPrec)<<data.pan<< " ";
	o<<std::setw(angularPrec + 6)<<std::setprecision(angularPrec)<<data.tilt<<" ";
	o<<std::setw(angularPrec + 6)<<std::setprecision(angularPrec)<<data.roll<<" ";

	o<<std::setw(lensPrec + 6)<<std::setprecision(lensPrec)<<data.zoom <<" ";
	o<<std::setw(lensPrec + 6)<<std::setprecision(lensPrec)<<data.focus<<" ";
	o<<std::setw(lensPrec + 6)<<std::setprecision(lensPrec)<<data.iris <<" ";
	o<<std::setw(lensPrec + 6)<<std::setprecision(lensPrec)<<data.spare.trackPos
	 <<"\n"<<std::flush;
}

void printPacketCSV(std::ostream &o, const CGIDataCartesian &data){
	//struct CGIDataCartesianVersion2 *data2 = (struct CGIDataCartesianVersion2*)(void*)&data;
	struct CGIDataCartesianVersion1 *data1 = (struct CGIDataCartesianVersion1*)(void*)&data;
	char buf[450];
	sprintf_s(buf, sizeof(char)*450, "R%d.00,%f,%f,%f,%f,%f,%f,%f,%f,%f, %f\n", 
	        data1->frameNumber, data.x, data.y, data.z, data.pan, data.tilt, data.roll,
	        data.zoom, data.focus, data.iris, data.spare.trackPos);
	o<<buf<<std::flush;
}

void printPacket(std::ostream &o, const CGIDataCartesian &data){
	o<<data<<std::flush;;
}


std::ostream& operator<< (std::ostream& o, const CGIDataCartesian &data){
	struct CGIDataCartesianVersion2 *data2 = (struct CGIDataCartesianVersion2*)(void*)&data;
	struct CGIDataCartesianVersion1 *data1 = (struct CGIDataCartesianVersion1*)(void*)&data;
	
	o<<std::dec;
	o<<"\npacketNumber:  "<<data.packetNumber;
	if(!data2->timeCode.CGIProtocolIndicator){
		o<<"\nframeNumber:   "<<data1->frameNumber;
		o<<"\ntime:          "<<data1->time;     
	}else{
		o<<"\ntimecode:      "<<std::setw(2)<<data2->timeCode.hours<<":"
		                      <<std::setw(2)<<data2->timeCode.minutes<<":"
		                      <<std::setw(2)<<data2->timeCode.seconds<<":"
		                      <<std::setw(2)<<data2->timeCode.frames;
		o<<"\nTC user bits:  0x"<<std::hex<<data2->timeCodeUserBits<<std::dec
		 <<" '"<<(char)((data2->timeCodeUserBits & 0xff000000) >> 24)
		 <<      (char)((data2->timeCodeUserBits & 0x00ff0000) >> 16)
		 <<      (char)((data2->timeCodeUserBits & 0x0000ff00) >>  8)
		 <<      (char)((data2->timeCodeUserBits & 0x000000ff))
		 <<"'";     
		
	}
	 
	o<<"\nx:             "<<data.x;
	o<<"\ny:             "<<data.y;
	o<<"\nz:             "<<data.z;

	o<<"\npan:           "<<data.pan;
	o<<"\ntilt:          "<<data.tilt;
	o<<"\nroll:          "<<data.roll;

	o<<"\nzoom:          "<<data.zoom;
	o<<"\nfocus:         "<<data.focus;
	o<<"\niris:          "<<data.iris;


	o<<std::hex<<std::setw(8)<<std::setfill('0');
	o<<"\nrec / run / camera:  "<<data.spare.recording<< " / " <<data.spare.running<<" / "
	 <<data.spare.cameraOn;
	//o<<"\nspare2:        0x"<<std::setw(8)<<getU32FromVoid&(data.spare[4]);
	o<<"\nspare-32:      0x"<<std::setw(8)<< getU32FromVoid(&(data.spare));
	o<<"\ntrack:         "<<data.spare.trackPos;
	o<<"\nsyncDelay:     "<<data.spare.syncDelay * 1E-6;
	o<<"\nchecksum:      0x"<<std::setw(8)<< *(u32 *)&(data.checkSum);
	u32 checkSumExpected = checkSum((void*)&data, sizeof(data)); 
	if(data.checkSum != checkSumExpected)
		o<<"\nchecksum exp.: 0x"<<*(u32*)&checkSumExpected;
	return(o);
}

void printPacketHex(std::ostream &o, const CGIDataCartesian &data){
	struct CGIDataCartesianVersion1 *data1 = (struct CGIDataCartesianVersion1*)(void*)&data;
 
	o<<std::hex<<std::setfill('0');
	 
	o<<"\npacketNumber:        0x"<<std::setw(8)<< data.packetNumber;
	o<<"\nframeNumber / TC:    0x"<<std::setw(8)<< data1->frameNumber;
	o<<"\ntime / TC user bits: 0x"<<std::setw(8)<< getU32FromVoid(&data1->time);
	       
	o<<"\nx:                   0x"<<std::setw(8)<< getU32FromVoid(&data.x);
	o<<"\ny:                   0x"<<std::setw(8)<< getU32FromVoid(&data.y);
	o<<"\nz:                   0x"<<std::setw(8)<< getU32FromVoid(&data.z);
	o<<"\npan:                 0x"<<std::setw(8)<< getU32FromVoid(&data.pan);
	o<<"\ntilt:                0x"<<std::setw(8)<< getU32FromVoid(&data.tilt);
	o<<"\nroll:                0x"<<std::setw(8)<< getU32FromVoid(&data.roll);
	o<<"\nzoom:                0x"<<std::setw(8)<< getU32FromVoid(&data.zoom);
	o<<"\nfocus:               0x"<<std::setw(8)<< getU32FromVoid(&data.focus);
	o<<"\niris:                0x"<<std::setw(8)<< getU32FromVoid(&data.iris);

	o<<"\nspare-32:            0x"<<std::setw(8)<< getU32FromVoid(&data.spare);
	o<<"\nsyncDelay:           0x"<<std::setw(8)<< data.spare.syncDelay;
	o<<"\ntrack:               0x"<<std::setw(8)<< getU32FromVoid(&data.spare.trackPos);

	o<<"\nchecksum:            0x"<<std::setw(8)<< getU32FromVoid(&data.checkSum);
	u32 checkSumExpected = checkSum((void*)&data, sizeof(data)); 
	if(data.checkSum != checkSumExpected)
	  o<<"\nchecksum exp.:       0x"<<std::setw(8)<<*(u32*)&checkSumExpected;
	o<<std::flush;
}

int initRand(){
#ifndef __WIN__
	unsigned short seed16Array[3];
	int randomFile = open("/dev/random", O_RDONLY);
	if(randomFile < 0){
		std::cerr<<"\nCannot open /dev/random! Exiting.";
		return(-1);
	}
	read(randomFile, &seed16Array, sizeof(seed16Array));
	seed48(seed16Array);
#endif

	return(0);
}

u32 random32(){
	u64 x;
	/*
	 * mrand48() returns values uniformly distributed in [-2^31, 2^31]. If we just consider the
	 * lower 32 bit, the vlaue 0x80000000 would have two times the probability of the others. Hence 
	 * we skip the case where mrand() returns 0x8000000. 
	 */
	do{
#ifndef __WIN__
	x = (u64) mrand48();//interpret result as unsigned
#else
	x = (u64) dist(mt);
#endif
	}while(x == 0x8000000);// == 2^31
	return((u32)(x & 0xffffffff));
}
