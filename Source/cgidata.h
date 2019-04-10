#ifndef _CGIDATA
#define _CGIDATA

#define CGIDATA_HEADER_VERSION "cgidata20130306"

/* windows with MS Visual Studio */
#ifdef _MSC_VER
typedef unsigned __int8 u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;
#pragma pack(push, 4)//set maximum alignment to 4
#endif // _MSC_VER

/* linux */
#ifndef _MSC_VER
#ifndef __KERNEL__
#include <stdint.h>
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
#else
#include <linux/types.h>
#endif
#endif // _MSC_VER

#ifdef __GNUC__
/*
 * Withh GCC, this pragma ensures that structures below are packed. See commet to CGIDataCartesian 
 * below. Using other compilers, you may have to use another directive to achieve packing.
 */
#pragma pack(4)//set maximum alignment to 4
#endif



#define TDDE_SYNC_VAL   (0x7f7a5aa5)

#define TDDE_SYNC_VAL_1 ((TDDE_SYNC_VAL & 0xff)       >> 0)
#define TDDE_SYNC_VAL_2 ((TDDE_SYNC_VAL & 0xff00)     >> 8)
#define TDDE_SYNC_VAL_3 ((TDDE_SYNC_VAL & 0xff0000)   >> 16)
#define TDDE_SYNC_VAL_4 ((TDDE_SYNC_VAL & 0xff000000) >> 24)

#define LENS_MAX  (100.0)

/*
 * This format is compact and close to the physical time code layout (of both LTC and VITC).
 * The MSB (CGIProtocolIndiator) is used to distinguish between CGI-data format 
 * version 1 (floating point time and integer frameNumber) and version 2 (time code). 
 * In version 1, this  bit is never 1 (well, actually it would become 1 after 828 days at 30 fps!), 
 * in version 2 its always one.
 */
struct timeCodeStruct{
  unsigned int frames        : 5;
  unsigned int seconds       : 6;
  unsigned int minutes       : 6;
  unsigned int hours         : 5;

  unsigned int dropFrame     : 1;
  unsigned int colorFrame    : 1;
  unsigned int phaseCorrection : 1;//defined for LTC only
  unsigned int evenFieldMark : 1;//defined for VITC only
  unsigned int userBitFlags  : 2;//0..3,  represents bits 43 & 27 of physical LTC / VITC
  unsigned int spacer        : 2;

  unsigned int isLTC         : 1; 
  //should be set to 1 if data export packets contain time code!
  unsigned int CGIProtocolIndicator : 1;
};

#ifdef __cplusplus

/*
 * returns an integer containing just frames + 32 * seconds + 64 * minutes + 64 * hours
 */
static inline int TC2Int(const timeCodeStruct &t){
		return((*(int*)&t) & 0x3fffff);
}

static inline bool operator>(const timeCodeStruct &t1, const timeCodeStruct &t2){
  return(TC2Int(t1) > TC2Int(t2));  
}

static inline bool operator>=(const timeCodeStruct &t1, const timeCodeStruct &t2){
  return(TC2Int(t1) >= TC2Int(t2));  
}

static inline bool operator<(const timeCodeStruct &t1, const timeCodeStruct &t2){
  return(TC2Int(t1) < TC2Int(t2));  
}

static inline bool operator<=(const timeCodeStruct &t1, const timeCodeStruct &t2){
  return(TC2Int(t1) <= TC2Int(t2));  
}

static inline bool operator==(const timeCodeStruct &t1, const timeCodeStruct &t2){
  return(TC2Int(t1) == TC2Int(t2));  
}




#endif
struct timeCodeAPIStruct{
  u8 frames;           //0..29 or 0..25
  u8 seconds;          //0..59
  u8 minutes;          //0..59
  u8 hours;            //0..23
  u8  userBitFlags;    //0..3,  represents bits 43 & 27 of physical LTC / VITC
  char dropFrame;      //==0 or != 0, represents bit 10 of physical LTC / VITC
  char colorFrame;     //==0 or != 0, represents bit 11 of physical LTC / VITC
  char phaseCorrection;//==0 or != 0, represents bit 59 of physical LTC
  char evenFieldMark;  //==0 or != 0, represents bit 59 of physical VITC
  char isLTC;          //==0 if VITC, !=0 if LTC;
  u32 userBits;  
};

enum{TCTYPE_PAL, TCTYPE_NTSC_DP, TCTYPE_NTSC_30, TCTYPE_24};


const char*				getTCStringFromTCS(const struct timeCodeStruct *TC, char *ret);
timeCodeAPIStruct		timeCode2TCAPI(const struct timeCodeStruct *TC);
timeCodeStruct			TCAPI2timeCode(const struct timeCodeAPIStruct *API);


void time2TimeCodeAPI(struct timeCodeAPIStruct *TC, double seconds, int TCType);


/*
 * This is the definition of the 64-bit bitfield used in 
 * CGIDataCartesianVersionX. At the moment, it holds just two bits and a float:
 * "running" indicates that TECHNODOLLY is running a preprogrammed move. 
 * "cameraOn" indicates that camera is on.
 * trackPos holds the track position. This is helpful to get rid of the ambiguity
 * of conversion from cartesian to polar+track coordinates.    
 */
struct spareArea{
  unsigned int running   :  1;
  unsigned int cameraOn  :  1;
  unsigned int syncDelay : 20;//unit: microsecs
  unsigned int spare     : 10;
  float trackPos;
  //30 bits for future use
};
/*
 * This is the definition of the 32-bit bitfield used in 
 * CGIDataPolar. At the moment, it holds just two bits:
 * "running" indicates that TECHNODOLLY is running a preprogrammed move. 
 * "cameraOn" indicates that camera is on.    
 */
struct spareArea32{
  unsigned int running  :  1;
  unsigned int cameraOn :  1;
  unsigned int spare    : 30;
  //30 bits for future use
};





/*
 * Make sure that your compiler packs the structure below (check that CGI_DATA_LENGTH equals 64).
 */
struct CGIDataCartesianVersion1{
                             //Version without time code, the only one until November 2012
  u32 syncVal;               //always 0x7f7a5aa5 to detect beginning of packet
  u32 packetNumber;
  u32 frameNumber;           //MSB is always 0. If MSB is set, it means that we actually 
                             //have a packet of version 2 containing timecode.
  float time;                //[seconds]
  float x,y,z;               //[m]
  float pan, tilt, roll;     //[degrees]
  float zoom, focus, iris;   //A negative value indicates that calibrated lens values are used. 
                             //Uncalibrated: % from one stop to the other
	                           //Calibrated: zoom  = - focal length [mm]
                             //            focus = - 1 / focal distance [1/m]
                             //            iris  = - f-stop
                             
  struct spareArea spare;    //64 bits, most are free for future use.
  u32 checkSum;
};



struct CGIDataCartesianVersion2{
                             //Version with time code.
  u32 syncVal;               //always 0x7f7a5aa5 to detect beginning of packet
  u32 packetNumber;
  struct timeCodeStruct timeCode;  
                             //MSB is always 1. If MSB is 0, it means that we actually 
                             //have a packet of version 1 containing frame-numbers and time.
                            
  u32 timeCodeUserBits;      
  float x,y,z;               //[m]
  float pan, tilt, roll;     //[degrees]
  float zoom, focus, iris;   //A negative value indicates that calibrated lens values are used. 
                             //Uncalibrated: % from one stop to the other
	                           //Calibrated: zoom  = - focal length [mm]
                             //            focus = - 1 / focal distance [1/m]
                             //            iris  = - f-stop
  struct spareArea spare;    //64 bits, most are free for future use.
  u32 checkSum;
};

#define CGI_PROTOCOL_WITHOUT_TIMECODE (1)
#define CGI_PROTOCOL_WITH_TIMECODE    (2)

#ifndef CGI_PROTOCOL_VERSION
#define CGI_PROTOCOL_VERSION CGI_PROTOCOL_WITH_TIMECODE
#endif 

#if(CGI_PROTOCOL_VERSION == CGI_PROTOCOL_WITHOUT_TIMECODE)
#define CGIDataCartesian CGIDataCartesianVersion1
#else
#define CGIDataCartesian CGIDataCartesianVersion2
#endif



char CGIHasTimecode(const struct CGIDataCartesian *CGI);




//#define CGI_DATA_LENGTH (offsetof(CGIDataCartesian, checkSum) + sizeof(u32))
#define CGI_DATA_LENGTH (sizeof(struct CGIDataCartesianVersion1))


struct CGIDataPolar{
  u32 syncVal;               //always 0x7f7a5aa5 to detect beginning of packet
  u32 packetNumber;
  u32 frameNumber;
  float time;                //[seconds]
  float tele;                //[m]
  float BP, BT;              //[degrees]
  float pan, tilt, roll;     //[degrees]
  float zoom, focus, iris;   //scaled to [0..1] from one stop to the other
  float track;               //[m]
  struct spareArea32 spare;  //32 bits, most are free for later use.
  u32 checkSum;
};

#ifdef __GNUC__
#pragma pack()//sets the default
#endif

#ifdef _MSC_VER
#pragma pack(pop) //sets back default aligment
#endif // _MSC_VER

#define pi					3.1415926535897932384626433//alternatively: 2.0 * atan2(0.0, 1.0);
#define pi2					2.0 * 3.1415926535897932384626433
#define packetDuration		115200.0 / ((8 + 1 + 1) * CGI_DATA_LENGTH)
/*
pi  = 3.1415926535897932384626433;//alternatively: 2.0 * atan2(0.0, 1.0)
const double pi2 = 2.0 * 3.1415926535897932384626433;
const double packetDuration = 115200.0 / ((8 + 1 + 1) * CGI_DATA_LENGTH); 
*/
extern const int offsetOfFirstCGIDatum;

u32 checkSum(const void *data, unsigned int size);
int fillData(u8 input, struct CGIDataCartesian *data);


#endif //_CGIDATA

