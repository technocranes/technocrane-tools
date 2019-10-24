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



#include "unistd.h"
#include <cstddef>
#include "getopt.h"
#include <errno.h>
#include "systime.h"
#include <time.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "cgidata.h"
#include "utils.h"
#include "datagenerator.h"

#include "drand48.h"
#include "usleep.h"

#include "network.h"

#include <vector>

using namespace std;


void getOptions(int argc, char * const argv[], DGOptions &opt){
  int c;
  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
      {"x",             0, 0, opt.x},
      {"y",             0, 0, opt.y},
      {"z",             0, 0, opt.z},
      {"pan",           0, 0, opt.pan},
      {"tilt",          0, 0, opt.tilt},
      {"roll",          0, 0, opt.roll},
      {"zoom",          0, 0, opt.zoom},
      {"focus",         0, 0, opt.focus},
      {"iris",          0, 0, opt.iris},
      {"panTilt",       0, 0, opt.panTilt},
      {"xyPan",         0, 0, opt.xyPan},
      {"loop",          0, 0, opt.loop},
      //{"eight",         0, 0, opt.eight},
      {"random",        0, 0, opt.random},
      {"random-full",   0, 0, opt.randomFull},
      {"run-start",     1, 0, opt.run_start},
      {"run-end",       1, 0, opt.run_end},
      {"cam-start",     1, 0, opt.cam_start},
      {"cam-end",       1, 0, opt.cam_end},

      {"no-smooth",     0, 0, 's'},
      {"verbose",       0, 0, 'v'},
      {"help",          0, 0, 'h'},
      {"ascii",         0, 0, 't'},
      {"timecode",      0, 0, opt.time_code},

      {"cycle",         1, 0, 'c'},
      {"amplitude",     1, 0, 'a'},
      {"offset",        1, 0, 'o'},
      {"phase",         1, 0, 'p'},
      {"fps",           1, 0, 'f'},
      {"errors",        1, 0, 'e'},
      {"device",        1, 0, 'd'},
	  {"network",		1, 0, opt.network},
	  {"open",			1, 0, opt.filename},
      {0,               0, 0, 0}
    };

    c = getopt_long (argc, argv, "shtva:c:d:e:f:o:p:", long_options, &option_index);
    if (c == -1) break;

    char *nextP = (char*)1;
    double per, amp, offset, phase, fps, dBuf;
	int iBuf;
    long errorFreq;

    switch (c) {
      case DGOptions::x:
        opt.xOpt = new moveParameters(5.0, 1.0, 0.0);
        opt.xOpt->set(opt);
        if(opt.verbose) cout<<"\nlong option --"<<long_options[option_index].name<<flush;
        break;

      case DGOptions::y:
        opt.yOpt = new moveParameters(5.0, 1.0, 0.0);
        opt.yOpt->set(opt);
        if(opt.verbose) cout<<"\nlong option --"<<long_options[option_index].name<<flush;
        break;

      case DGOptions::z:
        opt.zOpt = new moveParameters(5.0, 1.0, 0.0);
        opt.zOpt->set(opt);
        if(opt.verbose) cout<<"\nlong option --"<<long_options[option_index].name<<flush;
        break;


      case DGOptions::pan:
        opt.panOpt = new moveParameters(5.0, 45.0, 0.0);
        opt.panOpt->set(opt);
        if(opt.verbose) cout<<"\nlong option --"<<long_options[option_index].name<<flush;
        break;

      case DGOptions::tilt:
        opt.tiltOpt = new moveParameters(5.0, 45.0, 0.0);
        opt.tiltOpt->set(opt);
        if(opt.verbose) cout<<"\nlong option --"<<long_options[option_index].name<<flush;
        break;

      case DGOptions::roll:
        opt.rollOpt = new moveParameters(5.0, 45.0, 0.0);
        opt.rollOpt->set(opt);
        if(opt.verbose) cout<<"\nlong option --"<<long_options[option_index].name<<flush;
        break;


      case DGOptions::zoom:
        opt.zoomOpt = new moveParameters(5.0, 0.5, 0.5);
        opt.zoomOpt->set(opt);
        if(opt.verbose) cout<<"\nlong option --"<<long_options[option_index].name<<flush;
        break;

      case DGOptions::focus:
        opt.focusOpt = new moveParameters(5.0, 0.5, 0.5);
        opt.focusOpt->set(opt);
        if(opt.verbose) cout<<"\nlong option --"<<long_options[option_index].name<<flush;
        break;

      case DGOptions::iris:
        opt.irisOpt = new moveParameters(5.0, 0.5, 0.5);
        opt.irisOpt->set(opt);
        if(opt.verbose) cout<<"\nlong option --"<<long_options[option_index].name<<flush;
        break;

      case DGOptions::panTilt:
        opt.panTiltOpt = new moveParameters(10.0, 45.0, 0.0);
        opt.panTiltOpt->set(opt);
        if(opt.verbose) cout<<"\nlong option --"<<long_options[option_index].name<<flush;
        break;

      case DGOptions::loop:
        opt.loopOpt = new moveParameters(10.0, 3.0, 0.0);
        opt.loopOpt->set(opt);
        if(opt.verbose) cout<<"\nlong option --"<<long_options[option_index].name<<flush;
        break;

      case DGOptions::xyPan:
        opt.xyPanOpt = new moveParameters(10.0, 3.0, 0.0);
        opt.xyPanOpt->set(opt);
        if(opt.verbose) cout<<"\nlong option --"<<long_options[option_index].name<<flush;
        break;

      case DGOptions::random:
        opt.randomBool = true;
        if(opt.verbose) cout<<"\nlong option --"<<long_options[option_index].name<<flush;
        break;

      case DGOptions::randomFull:
        opt.randomFullBool = true;
        if(opt.verbose) cout<<"\nlong option --"<<long_options[option_index].name<<flush;
        break;

      case DGOptions::time_code:
        opt.timecodeOn = true;
        if(opt.verbose) cout<<"\nlong option --"<<long_options[option_index].name<<flush;
        break;

      case DGOptions::run_start:
        dBuf = strtod(optarg , &nextP);
        if(*nextP == '\0'){
          opt.runStart = dBuf;
          if(opt.verbose) cout<<"\nSetting running-start time to "<<dBuf;
          break;
        }
        cout<<"\nInvalid numeric \""<<optarg<<"\" in option --run-start! Exiting...\n";
        exit(-1);

      case DGOptions::run_end:
        dBuf = strtod(optarg , &nextP);
        if(*nextP == '\0'){
          opt.runEnd = dBuf;
          if(opt.verbose) cout<<"\nSetting running-end time to "<<dBuf;
          break;
        }
        cout<<"\nInvalid numeric \""<<optarg<<"\" in option --run-end! Exiting...\n";
        exit(-1);
 
      case DGOptions::cam_start:
        dBuf = strtod(optarg , &nextP);
        if(*nextP == '\0'){
          opt.camStart = dBuf;
          if(opt.verbose) cout<<"\nSetting camera-start time to "<<dBuf;
          break;
        }
        cout<<"\nInvalid numeric \""<<optarg<<"\" in option --cam-start! Exiting...\n";
        exit(-1);

      case DGOptions::cam_end:
        dBuf = strtod(optarg , &nextP);
        if(*nextP == '\0'){
          opt.camEnd = dBuf;
          if(opt.verbose) cout<<"\nSetting camnera-end time to "<<dBuf;
          break;
        }
        cout<<"\nInvalid numeric \""<<optarg<<"\" in option --cam-end! Exiting...\n";
        exit(-1);

	  case DGOptions::network:
		  iBuf = strtol(optarg, &nextP, 10);
		  if (*nextP == '\0' && iBuf >= 0 && iBuf <= 65535)  // With 65535 max port number
		  {
			  if (opt.verbose) cout << "\n Setting UDP client port to " << iBuf << "\n";
			  opt.client_port = iBuf;
			  opt.generator_type = DGOptions::GENERATOR_NETWORK;
			  break;
		  }
		  cout << "\nInvalid numeric \"" << optarg << "\" in option --network! Exiting...\n";
		  exit(-1);
      case 'c':
        //cout<<"\noption \"-p\""<<flush;
        per = strtod(optarg , &nextP);
        //cout<<"\nnextP: "<<(int)nextP<<"   optarg: "<<(int)optarg<<"   *nextP: "<<*nextP;
        if(*nextP == '\0'){
          opt.periode = per;
          if(opt.verbose) cout<<"\nSetting periode to "<<per;
          break;
        }
        cout<<"\nInvalid numeric \""<<optarg<<"\" in option -p! Exiting...\n";
        exit(-1);

      case 'a':
		  {
		  if (optarg != nullptr)	// this is a HACK to run on windows
		  {
			amp = strtod(optarg , &nextP);
			if(*nextP == '\0'){
			  opt.amplitude = amp;
			  if(opt.verbose) cout<<"\nSetting amplitude to "<<amp;
			  break;
			}
		  }
		  //<<optarg_a is nullptr
        cout<<"\nInvalid numeric \""<<"\" in option -a! Exiting...\n";
        exit(-1);
		  }
      case 'o':
        offset = strtod(optarg , &nextP);
        if(*nextP == '\0'){
          opt.offset = offset;
          if(opt.verbose) cout<<"\nSetting offset to "<<offset;
          break;
        }
        cout<<"\nInvalid numeric \""<<optarg<<"\" in option -o! Exiting...\n";
        exit(-1);

      case 'p':
        phase = strtod(optarg , &nextP);
        if(*nextP == '\0'){
          opt.phase = pi2 * (1.0 / 360.0) * phase;
          if(opt.verbose) cout<<"\nSetting phase to "<<phase;
          break;
        }
        cout<<"\nInvalid numeric \""<<optarg<<"\" in option -p! Exiting...\n";
        exit(-1);

       case 'f':
        fps = strtod(optarg , &nextP);
        if((*nextP == '\0') && (fps <= 180) && (fps > 1.0)){
          opt.fps = fps;
          if(opt.verbose) cout<<"\nSetting fps to "<<fps;
          break;
        }
        cout<<"\nInvalid numeric \""<<optarg<<"\" in option -f! Must be in ]1..180]. Exiting.\n";
        exit(-1);

       case 'e':
        errorFreq = strtol(optarg , &nextP, 10);
        if(*nextP == '\0'){
          opt.errorFreq = errorFreq;
          if(opt.verbose) cout<<"\nSetting errorFreq to "<<errorFreq;
          break;
        }
        cout<<"\nInvalid integer \""<<optarg<<"\" in option -e! Must be in [1..UINT_MAX]. "
              "Exiting.\n";
        exit(-1);

      case 'd':
        opt.device = optarg;
        if(opt.verbose) cout<<"\noption \"-d\" with arg "<<optarg;
        break;
 
	  case DGOptions::filename:
		  opt.sourcefname = optarg;
		  if(opt.verbose) cout<<"\noption \"-d\" with arg "<<optarg;
		  break;

      case 's':
        opt.smooth = false;
        if(opt.verbose) cout<<"\noption \"-s\"";
        break;

      case 'v':
        opt.verbose = true;
        if(opt.verbose) cout<<"\noption \"-v\"";
        break;

      case 't':
        opt.ascii = true;
        if(opt.verbose) cout<<"\noption \"-t\"";
        break;

      case 'h':
        cout<<
        "\ndatagenerator is a program to simulate the data export of TECHNODOLLY camera cranes."
        "\nSee written documentation for details and examples."
        "\nCopyright (c) 2008 by Jan Frohn."
        "\nSend bug reports or comments to jan.frohn@supertechno.com."
        "\n"
        "\nUsage: datagenerator [OPTIONS]\n"
        "\nOPTIONS"
        "\n"
        "\n1. General options:"
        "\n -h, --help           show this help screen"
        "\n -v, --verbose        increase verbosity"
        "\n -t, --ascii          generate ASCII output instead of binary"
        "\n -d, --device DEV     write to character device or file DEV. Defaults to /dev/ttyS0."
		"\n --network PORT		start UDP streaming on a specified port number" 
        "\n -o, --open FILENAME	 send packets from a cgi file"
		"\n -e, --errors NUM     generate a checksum error for each NUMth packet."
        "\n -f, --fps FLT        generate FLT packets per second. Defaults to 25."
        "\n --timecode           generate timecode. In ASCII mode, the timecode is appended in the"
        "\n                      usual format hh:mm:ss:ff at the end of each data line. In binary"
        "\n                      mode, the format defined in 'struct CGIDataCartesianVersion2'"
        "\n                      is used. At the moment, only PAL timecode (25 fps) is generated,"
        "\n                      consequently, it doesn't make much sense to combine the timecode"
        "\n                      option with anything other then 25 fps."
        "\n"
        "\n2. Options affecting the camera moves:"
        "\n -c, --cycle  FLT     set the periode for subsequent moves to FLT seconds. The default"
        "\n                      depends on the move type."
        "\n -a, --amplitude FLT  set the amplitude for subsequent moves to FLT. The default"
        "\n                      depends on the move type."
        "\n -a, --offset FLT     set the offset for subsequent moves to FLT. The default"
        "\n                      depends on the move type."
        "\n -p, --phase FLT      set the phase for subsequent moves to FLT degrees. The default"
        "\n                      is zero."
        "\n -s, --no-smooth      use trapezoidal oscillations instead of sinusoidal ones."
        "\n"
        "\n --run-start          These options do not affect the positional data output itself."
        "\n --run-end            They just set the start and end time (in seconds) for indicator " 
        "\n                      bit \"running pre-programmed move\". The default is 0 and " 
        "\n                      infinity, repsectively (i.e. bit is set all the time)."
        "\n"
        "\n --cam-start          These options do not affect the positional data output itself."
        "\n --cam-end            They just set the start and end time (in seconds ) for indicator "
        "\n                      bit \"camera on\". The default is 0 and infinity,"
        "\n                      repsectively (i.e. bit is set all the time)."
        "\n"
        "\n3. Options selecting the camera moves:"
        "\n     --x"
        "\n     --y"
        "\n     --z"
        "\n     --pan"
        "\n     --tilt"
        "\n     --roll"
        "\n     --zoom"
        "\n     --focus"
        "\n     --iris          generate simple oscillating moves for the respective axes."
        "\n                     See written documentation for details."
        "\n"
        "\n     --panTilt       pan and tilt draw a circle (smooth) or square (non-smooth)"
        "\n                     in object plane."
        "\n     --xyPan         a circle (smooth) or square (non-smooth) in xy-plane,"
        "\n                     camera watching the origin."
        "\n     --loop          an aircraft inside loop."
        "\n"
        "\n     --random        put binary random data in all fields except"
        "\n                     sync pattern, packetNumber, frameNumber, time and check sum."
        "\n     --random-full   put binary random data in all fields except"
        "\n                     sync pattern and check sum."
        "\n"<<flush;
        exit(0);
        break;

 
      case '?'://An error message is already generated internally!
        exit(-1);
        break;

      default:
        cout<<"?? getopt returned character code 0x"<<hex<<c<<dec<<" ??\n";
    }
  }
  cout<<flush;
  if(optind < argc) {
    cout<<"\nNon-option arguments found:";
    while (optind < argc){
      cout<<"\n"<<argv[optind++];
    }
    cout<<"\nExiting!";
    exit(-1);
  }
}


void setRandom(CGIDataCartesian &data, bool full){
  u32 *u32Ptr = (u32*)(&data); 
  int start = full ? offsetof(CGIDataCartesian, packetNumber) / 4 :
                     offsetof(CGIDataCartesian, x)            / 4;
  int end   =        offsetof(CGIDataCartesian, checkSum)     / 4;
  for(int i = start; i < end; i++) u32Ptr[i] = random32();
}

void setDefaults(CGIDataCartesian &data){
  struct CGIDataCartesianVersion1 *data1 = (struct CGIDataCartesianVersion1*)(void*)&data;
  data.syncVal      = TDDE_SYNC_VAL;
  data.packetNumber = 0;
  data1->frameNumber  = 0;

  data.x     = 0.0;
  data.y     = 0.0;
  data.z     = 0.0;
  data.pan   = 0.0;
  data.tilt  = 0.0;
  data.roll  = 0.0;
  data.zoom  = 0.0;
  data.focus = 0.0;
  data.iris  = 0.0;
  u8 *u8Ptr = (u8 *)&data.spare;
  for(int i = 0; i < (int)sizeof(data.spare); i++) u8Ptr[i] = 0xf0 + (u8)i;
  //for(int i = 0; i < (int)sizeof(data.spare); i++) data.spare[i] = 0xf0 + (u8)i;
  data.checkSum = checkSum((void*)&data, sizeof(data));
}

/*
 * This transforms to [0, 2 * pi]. 
 */
double Mod2PI(double arg){
  double x = fmod(arg, pi2);
  if(x < 0) x += pi2;
  return(x);
}

/*
 * x must be in[0, pi/2]opt.
 */
double trapezoidal(double x){
  if(x < 0.25 * pi) return(x * 4.0 * (1.0 / pi));
  return(1.0);
}

double trapezoidalSin(double x){
  x = Mod2PI(x);
  if(x < 0.5 * pi) return( trapezoidal(x));  
  if(x <       pi) return( trapezoidal(pi  - x));
  if(x < 1.5 * pi) return(-trapezoidal(x   - pi));
                   return(-trapezoidal(pi2 - x));  
}


double trapezoidalCos(double x){
  return(trapezoidalSin(x +  0.5 * pi));
}

void setOscillatorData(double time, float &data, const moveParameters &opt){
  const double freq = 1.0 / opt.periode;
  time = fmod(time, opt.periode); //build time modulus periode, i.e. time now in [0, periode[
  if(opt.smooth) data  = opt.amplitude *            sin(pi2 * time * freq + opt.phase);
  else           data  = opt.amplitude * trapezoidalSin(pi2 * time * freq + opt.phase);
  data += opt.offset;
}

/*
 * A combined pan and tilt move which draws a circle (smooth) or square (non-smooth) in object 
 * plane.
 */
void setPanTiltData(double time, CGIDataCartesian &data, const moveParameters &opt){
  moveParameters pPan = opt, pTilt = opt;
  pTilt.offset    =  pPan.amplitude;
  pTilt.amplitude = -pPan.amplitude;
  pTilt.phase = 0.5 * pi;

  setOscillatorData(time, data.pan,  pPan);
  setOscillatorData(time, data.tilt, pTilt);
}

/*
 * A circle in xy-plane, camera watching the origin.
 */
void setXYPanData(double time, CGIDataCartesian &data, const moveParameters &opt){
  moveParameters pX = opt, pY = opt;
  pX.offset  = 0.0;
  pY.offset  = 0.0;
  pY.phase  += 0.5 * pi;
  setOscillatorData(time, data.x, pX);
  setOscillatorData(time, data.y, pY);
  
  //double BPAngle = -atan2(data.y, data.x);//This equals atan2(-data.y, data.x)
  double angle = -atan2(-data.y, -data.x);//This equals BPAngle + 180 degrees
  //cout<<"\nx: "<<data.x<<"/"<<opt.amplitude *  cos(angle);
  //cout<<"\ny: "<<data.y<<"/"<<opt.amplitude * -sin(angle);
  data.pan = 360.0 * angle / pi2;
}

/*
 * An aircraft inside loop.
 */
void setLoopData(double time, CGIDataCartesian &data, const moveParameters &opt){
  moveParameters pX = opt, pZ = opt;
  pX.offset  = 0.0;
  pX.smooth = true;
  pZ.offset  = 0.0;
  pZ.phase  += 0.5 * pi;
  pZ.amplitude  = -pX.amplitude;
  pZ.smooth = true;
  setOscillatorData(time, data.x, pX);
  setOscillatorData(time, data.z, pZ);
  //double BTAngle = atan2(data.z, data.x);
  double angle   = atan2(data.x, -data.z);//equals BTAngle + 90 degrees
  data.tilt = 360.0 * angle / pi2;
}

/*
 * Not yet defined, does nothing at the moment.
 */
void setEightData(double time, CGIDataCartesian &data, const moveParameters &opt){
  const double freq = 1.0 / opt.periode;
  time = fmod(time, opt.periode); //build time modulus periode, i.e. time now in [0, periode[
  (void)data;
  (void)freq;
}

void setBitfieldData(double time, CGIDataCartesian &data, const DGOptions &opt){
  u64 *spare64 = (u64 *)&data.spare;
  *spare64 = 0; //reset everything within the bitfield
  if((time >= opt.runStart) && (time <= opt.runEnd)) data.spare.running  = 1; 
  if((time >= opt.camStart) && (time <= opt.camEnd)) data.spare.cameraOn = 1;  
}

void setData(double time, CGIDataCartesian &data, const DGOptions &opt){

  setBitfieldData(time, data, opt);

  if(opt.randomBool){
    setRandom(data, false);
    return;
  }
  if(opt.randomFullBool){
    setRandom(data, true);
    return;
  }
  if(opt.xOpt)       setOscillatorData(time, data.x,     *opt.xOpt);
  if(opt.yOpt)       setOscillatorData(time, data.y,     *opt.yOpt);
  if(opt.zOpt)       setOscillatorData(time, data.z,     *opt.zOpt);
  if(opt.panOpt)     setOscillatorData(time, data.pan,   *opt.panOpt);
  if(opt.tiltOpt)    setOscillatorData(time, data.tilt,  *opt.tiltOpt);
  if(opt.rollOpt)    setOscillatorData(time, data.roll,  *opt.rollOpt);
  if(opt.zoomOpt)    setOscillatorData(time, data.zoom,  *opt.zoomOpt);
  if(opt.focusOpt)   setOscillatorData(time, data.focus, *opt.focusOpt);
  if(opt.irisOpt)    setOscillatorData(time, data.iris,  *opt.irisOpt);

  if(opt.panTiltOpt) setPanTiltData(time, data, *opt.panTiltOpt);
  if(opt.xyPanOpt)   setXYPanData  (time, data, *opt.xyPanOpt);
  if(opt.loopOpt)    setLoopData(time, data, *opt.loopOpt);
  if(opt.eightOpt)   setEightData  (time, data, *opt.eightOpt);
}

int binary2ASCII(const CGIDataCartesian &data, char *buf){
	  char openingChar = data.spare.running ? 'R' : 'S';
    struct CGIDataCartesianVersion2 *data2 = (struct CGIDataCartesianVersion2*)(void*)&data;
    struct CGIDataCartesianVersion1 *data1 = (struct CGIDataCartesianVersion1*)(void*)&data;
    char TCBuf[20];
    u32 frameNumber;
    if(data2->timeCode.CGIProtocolIndicator){
      std::sprintf(TCBuf,",%02d:%02d:%02d:%02d", data2->timeCode.hours,   data2->timeCode.minutes, 
                                                 data2->timeCode.seconds, data2->timeCode.frames);
      frameNumber = data2->timeCode.frames + 
				            DGOptions::frameRatePAL * (data2->timeCode.seconds + 
                    60.0                    * (data2->timeCode.minutes + 
										60.0                    * (data2->timeCode.hours))); 
		}else{
      TCBuf[0] = 0;
      frameNumber = data1->frameNumber;
    }

    return(std::sprintf(buf, "%c%d.00,%.4f,%.4f,%.4f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f%s\n",
                        openingChar, frameNumber, 
                        data.x, data.y, data.z,
                        data.pan, data.tilt, data.roll,
                        data.zoom, data.focus, data.iris, TCBuf));
   }




#ifndef _MSC_VER  
	int fd = 0;
#else
	int lSocket = 0;
	HANDLE fd=0;
#endif

int StartServer(DGOptions &opt)
{
	if (opt.generator_type & DGOptions::GENERATOR_NETWORK)
	{
	
		lSocket = Network_StartServer(opt.server_port);
	}

	if (opt.generator_type & DGOptions::GENERATOR_SERIAL)
	{
		#ifndef _MSC_VER  

		  fd = open(opt.device.c_str(), O_WRONLY | O_CREAT);

		  if(fd == -1){
			  cout<<"\n"<<strerror(errno)<<"!\ncannot open "<<opt.device.c_str()<<"!\n";
			return(-1); 
		  }

		#else

		  fd = CreateFile( opt.device.c_str(),
									GENERIC_READ | GENERIC_WRITE,
									0,
									NULL,	// default security attributes
									OPEN_EXISTING,
									0,		// not overlapped I/O
									NULL ); // hTemplate must be NULL for comm devices

		  if (INVALID_HANDLE_VALUE == fd) 
			{
				//  Handle the error.
				printf ("CreateFile failed with error %d.\n", GetLastError());
				return (1);
			}

		#endif

		setSerialAttributes(fd);
	}

  return 0;
}

int ClientSend(const DGOptions &opt, unsigned char *buf, int length, int &charsWritten)
{
	if (opt.generator_type & DGOptions::GENERATOR_NETWORK)
	{
		charsWritten = Network_ClientSend(lSocket, opt.client_port, buf, length);
	}

	if (opt.generator_type & DGOptions::GENERATOR_SERIAL)
	{
			#ifndef _MSC_VER
				  charsWritten = write(fd, &buf, length); 
			#else
				DWORD written;	
				for (int i=0; i<length; ++i)
				  WriteFile(fd, &buf[i], 1, &written, NULL);

				 charsWritten = length;
			#endif
	}

	return charsWritten;
}

bool LoadPackets(const char *filename, const float frameRate, std::vector<CGIDataCartesian> &packets)
{
	bool lSuccess = false;
	std::ifstream in(filename);

	try
	{
		if (!in) 
		{
			throw std::exception( "loadCGI ERROR: Cannot open file to read CGI data." );
		}

		// check if should read ASCII or binary

		int c;
		while((c = in.get()) != EOF && c <= 127) 
			;

		// rewind
		//in.seekg(0);
		in.close();
		
		//CGIDataCartesianVersion1 data;
		CGIDataCartesian data;
		memset( &data, 0, sizeof(CGIDataCartesian) );
		int packetNumber = 0;

		if(c == EOF) {
			/* file is all ASCII */

			in.open( filename, std::fstream::in );

			char line[1024];
			char startLetter='-';
			int  frameNumber=-1;
			
			int temp=0;
			bool okStatus=true;
			do
			{
				in.getline(line,1024,'\n');
				if (in.eof()) break;
				int parse=sscanf(line,
					"%c%d.00,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%f",
					&startLetter, &frameNumber,
					&data.x, &data.y, &data.z,
					&data.pan, &data.tilt, &data.roll,
					&data.zoom, &data.focus, &data.iris,
					&temp, &data.spare.trackPos);
				/*
				if (m_verbose>1) 
				{
					char buffer[1024];
					sprintf(buffer,
						"%c%d.00,%.4f,%.4f,%.4f,%.3f,%.4f,%.4f,%.4f,%.4f,%.4f\n",
						startLetter,frameNumber,
						data.x,   data.y, data.z,
						data.pan, data.tilt, data.roll,
						data.zoom, data.focus, data.iris);
					log() << buffer;
				}
				*/
				if (parse!=13) 
				{
					throw std::exception("loadCGI ERROR: Parse error for line ...");
				}
            
				data.packetNumber = packetNumber;
			
				CGIDataCartesianVersion1* ptr=(CGIDataCartesianVersion1*) & data;
			
				ptr->frameNumber = frameNumber;
				ptr->time = static_cast<float>(frameNumber/frameRate);

				//m_data.push_back(*ptr);
				packets.push_back(data);

				packetNumber += 1;
			}
			while (!in.eof());
			/*
			if (m_data.empty())
			{
				throw TcFBXConverterError( 
					std::string("loadCGI ERROR: Cannot parse data file '")
					+ inputFile + std::string("'") );
			}

			if (m_verbose) 
			{
				log() << "loadCGI: file '" <<  inputFile << "' contains " 
					<< m_data.size()  << " samples. TS of first, last data: "   
					<<data.time << " seconds" << " s." <<  std::endl;
			} 
			*/
		}
		else
		{
			// load as binary
			in.open( filename, std::fstream::in | std::fstream::binary );

			do
			{
				in.read( (char*) &data, sizeof(CGIDataCartesian) );
				data.packetNumber = packetNumber;

				packets.push_back(data);
				packetNumber += 1;
			}
			while (!in.eof() );
		}

		lSuccess = true;
	}
	catch( const std::exception &e )
	{
		printf( "[ERROR: Technocrane Importer] - %s\n", e.what() );
	}

	return lSuccess;
}

int main(int argc, char **argv){
  if(initRand()) return(-1);
  DGOptions opt;
  getOptions(argc, argv, opt);
  CGIDataCartesian data;
  struct CGIDataCartesianVersion1 *data1 = (struct CGIDataCartesianVersion1*)(void*)&data;
  struct CGIDataCartesianVersion2 *data2 = (struct CGIDataCartesianVersion2*)(void*)&data;
  setDefaults(data);

  // this is for option to send packets from a source file
  size_t currPacketFrame=0;
  std::vector<CGIDataCartesian>		packets;
  if (opt.sourcefname.size() > 0)
  {
	  LoadPackets(opt.sourcefname.c_str(), (float)opt.fps, packets);
  }

  if (opt.generator_type & DGOptions::GENERATOR_NETWORK)
  {
	  if (false == Network_Initialize() )
	  {
		  printf ("Network Initialize failed with error %d.\n", GetLastError());
		  return (1);
	  }
  }

  int status = StartServer(opt);
  if (0 != status)
	  return status;

  double time;
  timeval2 actualTime, nextTime, sleepTime, stepTime, startTime, relTime;
  timerclear(&stepTime);
  stepTime.tv_usec = opt.framePeriodeMuSec();
  gettimeofday(&actualTime, 0);
  startTime = nextTime  = actualTime;
  for(u64 loop = 0; ; loop++){

	int charsWritten, length=0;

    /*
     * Some test stuff for new XPLD UARTs
     */
    /*
    const int bufLength = 100;
    char buf[bufLength];  
    
    for(int i = 0; i < bufLength; i++){
      buf[i] = getSequentialChar();
      if((loop == 1000) && (i == 0)) buf[i]++;
		}
    charsWritten = write(fd, &buf, bufLength);
    //cout<<hex<<((u16)buf[0] & 0xff)<<" ";
    if(charsWritten != bufLength){
      cerr<<"Could write only "<<charsWritten<<" bytes of "<<length<<". Exiting...";
      return(-1);
    }
    //usleep(20000);
    //if(loop == 10) break;
    continue;
    */

    timersub(&nextTime, &startTime, &relTime);
    time         = relTime.tv_sec + 0.000001 * relTime.tv_usec;
    data.packetNumber = (u32)  loop;

	// source cgi file - based data for the packet
	if (packets.size() > 0)
	{
		CGIDataCartesian	&filePacket = packets[currPacketFrame];
		
		data.x = filePacket.x;
		data.y = filePacket.y;
		data.z = filePacket.z;
		data.pan = filePacket.pan;
		data.tilt = filePacket.tilt;
		data.roll = filePacket.roll;
		data.zoom = filePacket.zoom;
		data.focus = filePacket.focus;
		data.iris = filePacket.iris;
		data.spare = filePacket.spare;

		currPacketFrame += 1;
		if (currPacketFrame >= packets.size())
			currPacketFrame = 0;
	}

    if(!opt.timecodeOn){
      data1->frameNumber  = (u32)  loop;
      data1->time         = (float)time;
    }else{
      struct timeCodeAPIStruct TC;
      time2TimeCodeAPI(&TC, time, 0);
      data2->timeCode = TCAPI2timeCode(&TC);
      data2->timeCodeUserBits = ('u'<<24) + ('s'<<16) + ('e'<<8) + 'r'; 

	  float test = -0.25f;
	  data2->timeCodeUserBits = *(u32*)&test;
    }

	// parameteric-based data for the packet
	if ( 0 == packets.size() )
	{
		setData(time, data, opt);
	}
    data.checkSum = checkSum((void*)&data, sizeof(data));
    if(opt.errorFreq){
      if((loop % opt.errorFreq) == (opt.errorFreq - 1)){
        data.checkSum++;
        cout<<"\n"<<loop<<flush;
      }
    } 
    
    gettimeofday(&actualTime, 0);
    if(timercmp(&nextTime, &actualTime, >)){
      timersub(&nextTime, &actualTime, &sleepTime);
      if(opt.verbose) cout<<" Sleeping for "<<sleepTime.tv_usec / 1000000.0<<" s."<<flush;
      ShortSleep(0.001f * (float) sleepTime.tv_usec);
	  //Sleep(100);
    }

	// write to COM port
    if(opt.ascii)
	{
		// ascii
      char buf[400];
      length = binary2ASCII(data, buf);

	  ClientSend(opt, (unsigned char*) buf, length, charsWritten);

      cout<<"\nWriting "<<length<<" bytes"; 
    
	}
	else
	{
		// binary
      length = sizeof(data);

	  ClientSend(opt, (unsigned char*) &data, length, charsWritten);
    }

	// check for error
    if(charsWritten != length){
      cerr<<"Could write only "<<charsWritten<<" bytes of "<<length<<". Exiting...";
      return(-1);
    }

    timeradd(&nextTime, &stepTime, &nextTime);
    if(opt.verbose){
      timersub(&actualTime, &startTime, &relTime);
      cout<<"\nSent a packet at "<<relTime.tv_sec<<"."<<setw(6)<<setfill('0')
          <<relTime.tv_usec<<" s.";
    }
	Sleep(40);
  } // next loop

  if (opt.generator_type & DGOptions::GENERATOR_NETWORK)
  {
	  Network_StopServer(lSocket);
	  Network_Cleanup();
  }

  return(0);
}
