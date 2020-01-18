#ifndef DATADISPLAY
#define DATADISPLAY

#ifndef _MSC_VER
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include <cfloat>

/*
 * This class holds the command line options and provides some related functionality. Remind that 
 * some parameters (like framerate fps) affect the data output in general and should be specified 
 * only once. Other parameters (like amplitude) affect just the calculation of one (or eventually 
 * several) axes and may specified more then once. An example: 
 * To make the pan swing between -45 and +45 and make the tile swing between -90 and +90 you could 
 * specify --amplitude 45 --pan --amplitude 90 --tilt.   
 */
class DGOptions{
 public:
  enum{x = 256, y, z, pan, tilt, roll, zoom, focus, iris, panTilt, xyPan, loop, eight, 
       random, randomFull, run_start, run_end, cam_start, cam_end, time_code, filename}; 
  
  enum{frameRatePAL = 25};

  DGOptions() : paramInvalid(-DBL_MAX), fps (25), 
                periode(paramInvalid), amplitude(paramInvalid), offset(paramInvalid), 
		phase(paramInvalid),
                runStart(0.0), runEnd(DBL_MAX), camStart(0.0), camEnd(DBL_MAX), errorFreq(0),
                      xOpt(0),     yOpt(0),       zOpt(0), 
                    panOpt(0),  tiltOpt(0),    rollOpt(0), 
                   zoomOpt(0), focusOpt(0),    irisOpt(0), 
                panTiltOpt(0), xyPanOpt(0),    loopOpt(0), eightOpt(0), 
                smooth(true), randomBool(false), randomFullBool(false), verbose(false), 
		ascii(false), timecodeOn(false), useUDP(false), port(15246), broadcast(0),
	  IP(ntohl(inet_addr("127.0.0.1"))), device("/dev/ttyS0"),
		cgi_fname("")
  {
#ifdef _MSC_VER
	device = "COM1";
#endif
  }

  //unit: microseconds.Acceptable range: 1..999999
  u32 framePeriodeMuSec()   const { return((u32)(1000000.0 / fps + 0.5)); } 
  bool paramSet(double val) const { return(val != paramInvalid);          }
  const double paramInvalid; 
  double fps, periode, amplitude, offset, phase, runStart, runEnd, camStart, camEnd;
  u64 errorFreq;
  class moveParameters *xOpt, *yOpt, *zOpt, *panOpt, *tiltOpt, *rollOpt, 
                             *zoomOpt, *focusOpt, *irisOpt, 
                             *panTiltOpt, *xyPanOpt, *loopOpt, *eightOpt; 
  bool smooth, randomBool, randomFullBool, verbose, ascii, timecodeOn;
  std::string device;
  bool useUDP;
  u16 port;
  char broadcast;
  u32 IP;
  // cgi file to get packets from
  std::string	cgi_fname;	
};

/*
 * This class describes the parameters which influence a simple one-dimensional oscillating move. 
 * For the complex multidimensional moves (e.g. panTilt, loop, etc.) some parameters may have a 
 * non-obvious meaning or no meaning at all. See documentation for the respective move. 
 */
class moveParameters{
 public:
  moveParameters(double periodeArg, double amplitudeArg, double offsetArg) : 
    periode(periodeArg), amplitude(amplitudeArg), offset(offsetArg), phase(0.0), smooth(true) {}
  
  void set(const DGOptions &opt){
    if(opt.paramSet(opt.periode))   periode   = opt.periode;
    if(opt.paramSet(opt.amplitude)) amplitude = opt.amplitude;
    if(opt.paramSet(opt.offset))    offset    = opt.offset;
    if(opt.paramSet(opt.phase))     phase     = opt.phase;
    smooth = opt.smooth;
  }
  double periode, amplitude, offset, phase;
  bool smooth;

};

#endif//DATADISPLAY
