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

#ifndef __WIN__
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <termios.h> 
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include "getopt.h"
#else
#include <winsock.h>
#include "unistd.h"
#include "network.h"
#endif

#include <errno.h>
#include <fcntl.h>

#include <cstdlib>
#include <cstring>
#include<iostream>
#include<sstream>
#include<iomanip>


#include "cgidata.h"
#include "utils.h"
#include "datadisplay.h"

using namespace std;

union SHandleID{
#ifndef __WIN__
	int    m_Handle;
	int   m_Socket;
#else
	HANDLE  m_Handle;
	SOCKET  m_Socket;
#endif
};

#ifdef __WIN__
OVERLAPPED osReader = { 0 };
typedef int socklen_t;
#endif

void getOptions(int argc, char * const argv[], DDOptions &opt){
	int c;
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"no-packets",    0, 0, 'z'},
			{"no-statistics", 0, 0, 's'},
			{"no-unsynced",   0, 0, 'u'},
			{"summarize",     0, 0, 'e'},
			{"columns",       0, 0, 'c'},
			{"realtime",      0, 0, 'r'},
			{"hex",           0, 0, 'x'},
			{"verbose",       0, 0, 'v'},
			{"help",          0, 0, 'h'},
			{"device",        1, 0, 'd'},
			{"udp",           0, 0, 'n'},
			{"port",          1, 0, 'p'},
			{"broadcast",     0, 0, 'b'},
			{0,               0, 0, 0}
		};

		c = getopt_long (argc, argv, "crhvd:o:xpsuenb", long_options, &option_index);
		if (c == -1) break;

		switch (c) {
			case 'c':
				opt.formatColumns =true;
				if(opt.verbose) cout<<"\noption \"-c\"";
				break;

			case 'r':
				opt.formatRealTimeCSV =true;
				if(opt.verbose) cout<<"\noption \"-r\"";
				break;

			case 'x':
				opt.formatHex = true;
				if(opt.verbose) cout<<"\noption \"-x\"";
				break;

			case 'z':
				opt.printPackets = false;
				if(opt.verbose) cout<<"\noption \"-z\"";
				break;

			case 's':
				opt.printStatistics = false;
				if(opt.verbose) cout<<"\noption \"-s\"";
				break;

			case 'u':
				opt.printUnsyncedChars = false;
				if(opt.verbose) cout<<"\noption \"-u\"";
				break;

			case 'e':
				opt.summarize = true;
				if(opt.verbose) cout<<"\noption \"-e\"";
				break;

			case 'v':
				opt.verbose = true;
				if(opt.verbose) cout<<"\noption \"-v\"";
				break;


			case 'd':
				opt.device = optarg;
				if(opt.verbose) cout<<"\noption \"-d\" with arg "<<optarg;
			 break;

			case 'p':
				opt.port = static_cast<u16>(strtol(optarg,0,10));
				if(opt.verbose) cout<<"\noption \"-p\" with arg "<<optarg;
			 break;

			case 'n':
				opt.useUDP = true;
				if(opt.verbose) cout<<"\noption \"-d\"";
			 break;

		case 'b':
			opt.broadcast = 1;
			if (opt.verbose) cout << "\noption \"-b\"";
			break;

			case 'h':
				cout<<
				"\ndatadisplay is a program to display the data export of TECHNODOLLY camera cranes"
				"\nin a human-readable fashion. The output is written to standard output."
				"\nSee written documentation for details and examples."
				"\nCopyright (c) 2008 by Jan Frohn."
				"\nSend bug reports or comments to jan.frohn@supertechno.com."
				"\n"
				"\nUsage: datadisplay [OPTIONS]\n"
				"\nOPTIONS"
				"\n"
				"\n1. general options:"
				"\n -h, --help           show this help screen"
				"\n -v, --verbose        increase verbosity"
				"\n -d, --device DEV     read from character device or file DEV. Defaults to /dev/ttyS0."
				"\n -n, --udp            read UDP data from network. By default, it reads all UDP packets"
				"\n                      which are addressed to port 15245 on any network interface on" 
				"\n                      this machine. The port number can be changed by parameter "
				"\n                      '--port'." 
				"\n                      When option '--udp' is choosen, the '--device' option has "
				"\n                      no effect." 
				"\n -p, --port           UDP port number on this machine which packets must be addressed "
				                         "to. Default is 15246."
				"\n -b, --broadcast       UDP broadcast option for a socket"
				"\n -s, --no-statistics  don't display some statistical information for each packet."
				"\n -z, --no-packets     don't display packet data."
				"\n -u, --no-unsynced    don't display characters when the receiver is out of sync."
				"\n -e, --summarize      display a summary at the end (start frame, end frame, etc.)."
				"\n -x, --hex            show floats as hex values. Good for debugging."
				"\n -c, --columns        ouput raw column based text. Each packet generates a line"
				"\n                      and each field corresponds to a column. Good for interfacing to" 
				"\n                      external programs. The order of columns corresponds to"
				"\n                      the order of fields in CGIDataCartesian."
				"\n -r, --realtime       ouput raw column based text equivalent to the ASCII real" 
				"\n                      time data string (comma separated). Use this to convert binary"
				"\n                      to ASCII data."
				"\n"<<flush;
				exit(0);
				break;

			case '?'://An error message is already generated internally!
				exit(-1);
				break;

			default:
				cout<<"?? getopt returned character code 0x"<<std::hex<<c<<std::dec<<" ??\n";
		}
	}

	if(optind < argc) {
		cout<<"\nNon-option arguments found:";
		while (optind < argc){
			cout<<"\n"<<argv[optind++];
		}
		cout<<"\nExiting!";
		exit(-1);
	}
}

void printStatistics(std::ostream &o, const dataReceptionStatus &DRS){
	o<<std::dec<<"\n------------------------------";
	o<<"\ntotal chars read: "<<DRS.totalCharsRead<<"  (equivalent to "
	 <<DRS.totalCharsRead / (double) CGI_DATA_LENGTH <<" packets)";
	o<<"\nreceived packets: "<<DRS.completePackets;
	o<<"\nchecksum errors:  "<<DRS.totalCheckSumErrors;
	o<<"\nmissed syncs:     "<<DRS.totalMissedSyncs;
	o<<"\n------------------------------";
}


void printData(std::ostream &o, const CGIDataCartesian &data, const DDOptions &opt){
	if(opt.formatColumns){
		printPacketColumns(o, data);
		return;
	}
	if(opt.formatRealTimeCSV){
		printPacketCSV(o, data);
		return;
	}
	if(opt.formatHex) printPacketHex(o, data);
	else              printPacket   (o, data);
}


void printEndStatistics(std::ostream &o, const dataReceptionStatus &DRS, const DDOptions &opt){
	o<<std::dec<<"\n------------------------------";
	o<<std::dec<<"\n summary:                     ";
	o<<std::dec<<"\n------------------------------";
	printData(o, DRS.startDatum, opt);
	o<<std::dec<<"\n------------------------------";
	printData(o, DRS.endDatum, opt);
	o<<std::dec<<"\n------------------------------";
	o<<std::dec<<"\n------------------------------";
	o<<"\ntotal time:   "<<DRS.totalTime;
	o<<"\nframerate:    "<<DRS.completePackets / DRS.totalTime;
	o<<"\n------------------------------";
}

void printFrom(std::ostream &o, std::string fromAddress){
	o<<"\n"<<fromAddress;
	o<<std::dec<<"\n------------------------------";
}
void getStatistics(const CGIDataCartesian &data, dataReceptionStatus &DRS, bool isFirst){
	struct CGIDataCartesianVersion1 *data1 = (struct CGIDataCartesianVersion1*)(void*)&data;
	if(isFirst){
		DRS.startTime = data1->time;
		DRS.startDatum = data;
	}
	/*
	if(isLast){
		DRS.endTime = data1->time;
		DRS.totalTime = DRS.endTime - DRS.startTime;
		cerr<<"end:"<<data1->time<<flush;
	}
	*/
}

int readData(const SHandleID& fd, char *buf, int bufSize, const DDOptions &opt, 
             std::string *from = 0){
	if(!opt.useUDP){
#ifndef __WIN__
		return(read(fd.m_Handle, buf, bufSize));
#else
		DWORD charsRead = 0;
		if(!ReadFile(fd.m_Handle, buf, bufSize, &charsRead, &osReader)){
			if (GetLastError() == ERROR_IO_PENDING){
				// waiting on read
				Sleep(1000);
			}
		}
		return charsRead;
#endif
	}else{
		struct sockaddr_in srcAddr;
		socklen_t srcAddrLen = sizeof(srcAddr);
		int charsRead = recvfrom(fd.m_Socket, buf, bufSize, 0, (struct sockaddr *) &srcAddr, 
		                         &srcAddrLen);

#ifdef __WIN__
		if (charsRead < 0){
			cout << "Error while receiving from UDP: " << WSAGetLastError() << std::endl;
		}
#endif
		if(from){
			std::ostringstream os;
			os<<"received "<< charsRead <<" bytes from "<<inet_ntoa(srcAddr.sin_addr)
			  <<":"<<ntohs(srcAddr.sin_port);
			*from = os.str();
		}
		return(charsRead);
	}
}



int main(int argc, char **argv){
	DDOptions opt;
	getOptions(argc, argv, opt);
	CGIDataCartesian data;
	class dataReceptionStatus DRS;
	const int bufSize = 300;
	char buf[bufSize] = { 0 };
	
	bool isRegularFile = false;
	std::string fromAddress;
	SHandleID    fd;

	if(!opt.useUDP){
#ifndef __WIN__
		fd.m_Handle = open(opt.device.c_str(), O_RDONLY);
		if(fd.m_Handle == -1){
			cout<<"\n"<<strerror(errno)<<"!\ncannot open "<<opt.device<<"!\n";
			return(-1); 
		}
		struct stat stats;
		fstat(fd.m_Handle, &stats);
		if((stats.st_mode & S_IFMT) == S_IFREG){
			isRegularFile = true;
		}
#else
		fd.m_Handle = CreateFile(opt.device.c_str(),
		                         GENERIC_READ,
		                         0,
		                         0,
		                         OPEN_EXISTING,
		                         FILE_FLAG_OVERLAPPED,
		                         0);
		if (fd.m_Handle == INVALID_HANDLE_VALUE){
			// error opening port; abort
			cout << "\n" << GetLastError() << "!\ncannot open " << opt.device.c_str() << "!\n";
			return(-1);
		}
#endif
		setSerialAttributes(fd.m_Handle);
	}else{
#ifdef __WIN__
		char one = 1;
		Network_Initialize();
#else
		int one = 1;
#endif
		struct sockaddr_in serverAddr, clientAddr;
		int serverAddrLen = sizeof(serverAddr);
		fd.m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (opt.broadcast > 0){
			// This option is needed on the socket in order to be able to receive broadcast messages
			if (setsockopt(fd.m_Socket, SOL_SOCKET, SO_BROADCAST, &one, sizeof(one)) < 0){
#ifdef __WIN__
				cout << "Error in setting Broadcast option " << WSAGetLastError() << std::endl;
				closesocket(fd.m_Socket);
#else
				cout<<"Error in setting Broadcast option:n"<<strerror(errno);
#endif
				return 0;
			}

			if (setsockopt(fd.m_Socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0){
#ifdef __WIN__
				cout << "Error in setting ReuseAddr option " << WSAGetLastError() << std::endl;
				closesocket(fd.m_Socket);
#else
				cout<<"Error in setting ReuseAddr option: "<<strerror(errno);

#endif
				return 0;
			}
		}

		serverAddr.sin_family = AF_INET;
		//serverAddr.sin_addr.s_addr = inet_addr("192.168.8.248");
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		//serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		serverAddr.sin_port = htons(opt.port);
		
		bind(fd.m_Socket, (struct sockaddr *)&serverAddr, serverAddrLen);

#ifdef __WIN__
		int last_error = WSAGetLastError();
		if (last_error > 0){
			cout << "socket last error: " << last_error << std::endl;
		}
#endif
		/*
		 * With the line below, I can restrict my reception to packets originating from IP 
		 * address and port in clientAddr.
		 */
		(void)clientAddr;
		//clientAddr.sin_family = AF_INET; 
		//clientAddr.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("192.168.0.112");
		//clientAddr.sin_port = htons(15246);
		//connect(fd.m_Socket, (struct sockaddr *)&clientAddr, sizeof(struct sockaddr_in));
	}

	while(1){
		int charsRead = readData(fd, buf, bufSize, opt, &fromAddress);
		
		static int charCount = -1;
		//static int errorCount = -1;
		int fillState = CGI_DATA_LENGTH;
		cout<<"chars read: "<<charsRead<<"\n";
		for(int i = 0; i < charsRead; i++){
			DRS.totalCharsRead++;
			charCount++;
			u8 input = buf[i];
			
			fillState = fillData(input, &data);

			if(fillState == -1){
				if(opt.verbose) cout<<"\nchecksum error!"<<flush;
				cerr<<"\nchecksum error!"<<flush;
				DRS.totalCheckSumErrors++;
				printData(cout, data, opt);
				printStatistics(cout, DRS);
				continue;              
			}

			if(fillState == CGI_DATA_LENGTH){
				getStatistics(data, DRS, (DRS.completePackets == 0));
				DRS.completePackets++;
				if(opt.useUDP && opt.printStatistics) printFrom(cout, fromAddress);
				if(opt.printPackets)    printData(cout, data, opt);
				if(opt.printStatistics) printStatistics(cout, DRS);
				continue;           
			}

			if(DRS.totalCharsRead - fillState - DRS.nextSync > 0){
				if(opt.printUnsyncedChars){
					if((DRS.totalCharsRead - fillState - DRS.nextSync) % 8 == 1) cout<<endl;
					cout<<"0x"<<std::hex<<std::setw(2)<<(int)input<<"  ";
				}
				DRS.totalMissedSyncs++;
			}

			if(fillState == offsetOfFirstCGIDatum){
				//set nextSync to expected start of next sync-pattern
				DRS.nextSync = DRS.totalCharsRead + CGI_DATA_LENGTH - 4;
			}
			
		}
		if(!charsRead && isRegularFile) break;

		if (charsRead < 0){
			return 1;
		}
	}
	struct CGIDataCartesianVersion1 *data1 = (struct CGIDataCartesianVersion1*)(void*)&data;
	DRS.endDatum = data;
	DRS.endTime = data1->time;
	DRS.totalTime = DRS.endTime - DRS.startTime;
	if(opt.summarize) printEndStatistics(cout, DRS, opt);
	cout<<std::endl;

#ifdef __WIN__
	Network_Cleanup();
#endif
	return(0);
}



