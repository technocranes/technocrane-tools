#ifndef DATADISPLAY
#define DATADISPLAY

#include "cgidata.h"
/*
 * This class holds command line options.
 */
class DDOptions{
 public:
 
	DDOptions() 
	  : printPackets (true)
	  , printStatistics(true)
	  , printUnsyncedChars(true)
	  , formatColumns(false)
	  , formatRealTimeCSV(false)
	  , formatHex(false)
	  , verbose(false)
	  , summarize(false)
	  , useUDP(false)
	  , port(15246)
	  , broadcast(0)
	  , device("/dev/ttyS2") 
	{
#ifdef _MSC_VER
		device = "COM2";
#endif
	}
	
	bool printPackets;
	bool printStatistics;
	bool printUnsyncedChars;
	bool formatColumns;
	bool formatRealTimeCSV;
	bool formatHex;
	bool verbose;
	bool summarize;
	bool useUDP;
	u16 port;  // this is a server port
	u16 client_port;  // this is a client socket port
	char broadcast;
	std::string device; 
};

/*
 * This class holds iformations about reception history.
 */
class dataReceptionStatus{
 public:

	dataReceptionStatus() : totalCharsRead(0),  nextSync(CGI_DATA_LENGTH), totalCheckSumErrors(0),
	                        totalMissedSyncs(0), completePackets(0) {}

	int totalCharsRead;
	int nextSync;
	int totalCheckSumErrors;
	int totalMissedSyncs;
	int completePackets;
	double startTime, endTime, totalTime;
	CGIDataCartesian startDatum, endDatum;
};


#endif//DATADISPLAY
