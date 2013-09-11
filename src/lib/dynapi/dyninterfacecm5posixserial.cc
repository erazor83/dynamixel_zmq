/***************************************************************************
 *  dynApi - Robotis Dynamixel c++ progamming api                          *
 *  Copyright (C) 2008 by                                                  *
 *  Holger Friedrich <holgerf@vsi.cs.uni-frankfurt.de>,                    *
 *  Visual Sensorics and Information processing lab,                       * 
 *  University of Frankfurt                                                *
 *                                                                         *
 *  This library is free software; you can redistribute it and/or          *
 *  modify it under the terms of the GNU Lesser General Public             *
 *  License as published by the Free Software Foundation; either           *
 *  version 2.1 of the License, or (at your option) any later version.     *
 *                                                                         *
 *  This library is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *  Lesser General Public License for more details.                        *
 *                                                                         *
 *  You should have received a copy of the GNU Lesser General Public       *
 *  License along with this library; if not, write to the Free Software    *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA02110-1301 USA*
 ***************************************************************************/
#ifdef HAVE_POSIXSERIAL
#include "dyninterfacecm5posixserial.h"
#include <fcntl.h>   /* File control definitions */
#include <iostream>
#include <iomanip>
#include <sstream>
#include <errno.h>   /* EAGAIN, etc. */
#include <cassert>
#include "dyninternal.h"
#include "ax.h"
#include <deque>
#include <boost/weak_ptr.hpp>

/** @file
implementation */
namespace dynapi {


static const int READ_MAXWAIT_US = 1000;

const int DynInterfaceCM5PosixSerial::READ_WAIT_US = 20;
const int DynInterfaceCM5PosixSerial::READ_TIMEOUT_US=100;

void DynInterfaceCM5PosixSerial::getIfInfo(IfInfo &info) {
  info = IfInfo(); // new, all members cleared
  info.ifType = _ifType;
  info.ifConn = DYN_IC_SERIAL;
  info.device = _device;
}


std::vector<Ax> DynInterfaceCM5PosixSerial::scanBus() throw (XDynComm) {
  std::deque< int > list;
  
  std::string scan = "scan\n";
  int n = write(fd, scan.c_str(), 5);
      if (n < 0)
        throw XDynComm("DynInterfaceCM5PosixSerial: write() of 5 bytes failed!");
/*    {
      int res;
      int tries = 50;
      do {
        res = fsync(fd);
        if (0 != res) usleep(1);
        ++tries;
      } while ( (EAGAIN == res) || (0 == tries) );
      if (0 == tries) throw XDynComm("DynInterfaceCM5PosixSerial::writeData: Timeout0");
    }*/
      // seems to be ok, copy buffer to save last write
  int tries = 10000; // scanning is slow...
  unsigned int deviceCount = 0;
  while (tries) {
    try {
      std::string line = readLine();
      
      if (line.find(" Dynamixels Found.") != std::string::npos) {
        size_t dp = line.find("(");
        if (dp != std::string::npos) {
          line.erase(dp, std::string::npos);
          while (!line.empty() && ' '==line[0]) line.erase(0,1);
//          std::cout << __FUNCTION__ << ":" << __LINE__ << " \"" << line.c_str() <<"\""<< std::endl;
          // should be a number now
          
          _numDevices = str2long(line);
          break;
        }
      } else {
        size_t pos;
        size_t ll = line.length();
        for (int i=0; i<=253; ++i) {
          std::ostringstream ostr;
          ostr << "[" << std::setw(3) << std::setfill('0') << i << ":";
          if( (pos = line.find( ostr.str() )) != std::string::npos) {
            if (pos+5 >= ll) throw XDynComm("DynInterfaceCM5PosixSerial: scanBus() failed. Parser Error #1.");
            if ('O'==line[pos+5]) {
//              std::cout << __FUNCTION__ << ":" << __LINE__ << " device #" << i << " found" << std::endl;      
              list.push_back(i);
              //Ax *p;
              try {
                //p=new Ax(_myPtr, i);
                
              } catch (XDyn &x) {
                std::cout << x.what() << std::endl;
              }
              //list.push_back( boost::shared_ptr<Ax>(p) );
              ++deviceCount;
            } else if ('-'==line[pos+5]) {
              // not found
            } else {
              // parser error
              throw XDynComm("DynInterfaceCM5PosixSerial: scanBus() failed. Parser Error #2.");
            }
          }
        }
//std::cout << __FUNCTION__ << ":" << __LINE__ << " \"" << line <<"\""<< std::endl;      
      }
    } catch (XDynCommTimeout &x) { usleep(READ_MAXWAIT_US); };
    --tries;
  }
  
  if (tries <= 0) throw XDynComm("DynInterfaceCM5PosixSerial: scanBus() failed");
  if (deviceCount != _numDevices) throw XDynComm("DynInterfaceCM5PosixSerial: scanBus() failed. Parser error #3.");

  std::vector<Ax> devices;
  for (std::deque<int>::iterator it=list.begin(); it != list.end(); ++it) {
    try {
      devices.push_back( Ax(boost::shared_ptr<DynInterface>(_myPtr), *it) );
    } catch (XDyn &x) {
      std::cout << x.what() << std::endl;
    }
  }
  return devices;

}

DynInterfaceCM5PosixSerial::DynInterfaceCM5PosixSerial(std::string device) throw(XDynComm)
: _device(device) {
//std::cout << __FUNCTION__ << std::endl;

  fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {
   /*
    * Could not open the port.
    */
    throw XDynComm("DynInterfaceCM5PosixSerial: unable to open tty");
  } else {
    // ok, port opened
    // now probe for RoboIF and IntelligentIF
    
    try {
      //_myPtr.reset(this);
      probe();
      if (DYN_IF_UNKNOWN==_ifType) throw XDynComm("DynInterfaceCM5PosixSerial: unable to detect Interface");
    
      //scanBus();
    } catch (...) {
      //_myPtr.reset();
      throw;
    }
    
  }
 
}

DynInterfaceCM5PosixSerial::~DynInterfaceCM5PosixSerial() {
std::cout << __FUNCTION__ << std::endl;
//  std::cout << _myPtr.use_count() << std::endl;
//  _myPtr.reset(static_cast<DynInterface*>(0));
//  std::cout << _myPtr.use_count() << std::endl;
  if (DYN_IF_CM5 == _ifType) {
    // deactivate device
#if 0
    unsigned char buf[1];
    buf[0] = ROBO_IF_OFF;
    int n = write(fd, buf, 1);
    if (n == 1) {
      try{ 
        n = ~ROBO_IF_OFF;
        readSpecialCode(n);
      } catch (...) {
	    } // dtors must _not_ throw
      // ok, thats it
    } // else: error, but dtors must _not_ throw 
#endif
  }
  tcsetattr(fd, TCSAFLUSH, &_oldTermIOS);  // restore old settings, do not care if it works
  close(fd);                      // close port, do not care if it works
}


void DynInterfaceCM5PosixSerial::probe() throw (XDynComm) {
  _ifType = DYN_IF_UNKNOWN;
  
  fcntl(fd, F_SETFL, FNDELAY);        // non blocking mode
  
  tcgetattr(fd, &_oldTermIOS);   // to be restored in later...
  
  
  tcgetattr(fd, &_newTermIOS);   // get the current settings
  // raw terminal
  cfmakeraw(&_newTermIOS);       // make raw struct (see man 3 termios)
  // baud rate
  cfsetospeed(&_newTermIOS, B57600);   // set baud rate to 57600 (for CM-5)
  cfsetispeed(&_newTermIOS, B57600);       // set in baud rate := out baud rate
  // 8,n,1
/*  newTermIOS.c_cflag &= ~PARENB; // N
  newTermIOS.c_cflag &= ~CSTOPB; // 1
  newTermIOS.c_cflag &= ~CSIZE;  // use size mask
  newTermIOS.c_cflag |= CS8;     // 8
  newTermIOS.c_cflag |= IXON;    // xon/xoff for output
  newTermIOS.c_cflag |= IXOFF;   // xon/xoff for input
  newTermIOS.c_cflag |= IXANY;   // automatic restart on typing
  newTermIOS.c_lflag &= ~( ICANON | ECHO | ECHOE | ISIG);*/
  
  _newTermIOS.c_iflag |= IXON;    // xon/xoff for output
  _newTermIOS.c_iflag |= IXOFF;   // xon/xoff for input
  //_newTermIOS.c_iflag |= IXANY;   // automatic restart on typing
  _newTermIOS.c_iflag |= IGNBRK;
  
/*std::cout << "cf " << oldTermIOS.c_cflag << std::endl;
std::cout << "if " << oldTermIOS.c_iflag << std::endl;
std::cout << "of " << oldTermIOS.c_oflag << std::endl;
std::cout << "lf " << oldTermIOS.c_lflag << std::endl;
std::cout << "li " << (int)oldTermIOS.c_line  << std::endl;
std::cout << "cc";
for (int j=0; j<NCCS; ++j)
std::cout << " " << (int)oldTermIOS.c_cc[j];
std::cout << std::endl;
std::cout << "is " << oldTermIOS.c_ispeed << std::endl;
std::cout << "os " << oldTermIOS.c_ospeed  << std::endl;
  
newTermIOS.c_cc[5] = 5;

std::cout << "cf " << newTermIOS.c_cflag << std::endl;
std::cout << "if " << newTermIOS.c_iflag << std::endl;
std::cout << "of " << newTermIOS.c_oflag << std::endl;
std::cout << "lf " << newTermIOS.c_lflag << std::endl;
std::cout << "li " << (int)newTermIOS.c_line  << std::endl;
std::cout << "cc";
for (int j=0; j<NCCS; ++j)
std::cout << " " << (int)newTermIOS.c_cc[j];
std::cout << std::endl;
std::cout << "is " << newTermIOS.c_ispeed << std::endl;
std::cout << "os " << newTermIOS.c_ospeed  << std::endl;*/
  


  // do not wait for line endings!
  _newTermIOS.c_cc[5] = 5;
  tcsetattr(fd, 0, &_newTermIOS);      // invoke new settings
  
  unsigned char buffer[4];
  //buffer[0] = 0x11; // XON
  buffer[0] = '\r';
  buffer[1] = '\n';
  buffer[2] = 0;
//   write(fd, buffer, 2);
//  readAllJunk(); 
//  readAllJunk(); 
   
 
  int n;
  n = write(fd, buffer, 2);
  if (n < 0)
      throw XDynComm("DynInterfaceCM5PosixSerial: write() of 2 bytes failed!");
  n = write(fd, buffer, 2);
  if (n < 0)
      throw XDynComm("DynInterfaceCM5PosixSerial: write() of 2 bytes failed!");
  
  int tries = 20;
  while (tries > 0) {
  
    try {
      size_t pos = 0;
      std::string line = readLine();
//      std::cout << "\"" << line << "\"" << std::endl;
      if (line.find("[CID:") != std::string::npos) {
        _ifType = DYN_IF_CM5;
        break;
      }
      
      // version number
      if ((pos=line.find("[CM-5 Version ")) != std::string::npos) {
        std::string version = line.substr(pos+14);
        if ((pos=version.find("]")) != std::string::npos) {
          version.erase(pos,std::string::npos);
          if ((pos=version.find(".")) != std::string::npos) {
            try {
              long h = str2long( version.substr(0,pos) );
              long l = str2long( version.substr(pos+1,std::string::npos) );
              std::cout << "CM-5 Version " << h << "." << l << std::endl;
            } catch (XDyn &) {}
          }
        }
      }
      
    } catch (XDynCommTimeout &x) {
      usleep(100000); // 100ms
      n = write(fd, buffer, 2);
      if (n < 0)
       throw XDynComm("DynInterfaceCM5PosixSerial: write() of 2 bytes failed!");
}
    
    --tries;
    //std::cout << "#" << tries << std::endl;
  }
  if (tries <= 0) throw XDynComm("DynInterfaceCM5PosixSerial: CM-5 not detected. Port? Power?");

  // ok, now we have clarified interface type
}

void DynInterfaceCM5PosixSerial::readAllJunk() throw () {
  bool enough=false;
  int b;
  unsigned char c;
  while (!enough) {
    while(1==(b=read(fd,&c,1))) {
//    std::cout<<c<<std::flush;
    };            // read junk
    usleep(20);                              // wait for a small time
    if (1>(b=read(fd,&c,1))) enough = true;  // see if more bytes come in
//    if (b==1) std::cout<<c<<std::endl;
  }
}

std::string DynInterfaceCM5PosixSerial::readLine() throw (XDynComm) {
  static const int BUFFER_LEN = 2000;
  char buffer[BUFFER_LEN];
  int waitCount=0;
  size_t pos=0;
  while (1) {
    size_t endpos = _readLineBuffer.length();
    for (size_t i=pos; i<endpos; ++i) {  
//      std::cout << "r:" << (int)readLineBuffer[i] << std::endl;
      if ('\n' == _readLineBuffer[i]) {
        _readLineBuffer.erase(i,1);
        --endpos;
      }
      if ('\r' == _readLineBuffer[i]) {
        std::string readLineBuffer2 = _readLineBuffer;
      
      
        readLineBuffer2.erase(i, std::string::npos);
        _readLineBuffer.erase(0, i+1);
        
        return readLineBuffer2;
      }
      ++pos;
    }
    int b = read(fd, &buffer[0], sizeof(char)*(BUFFER_LEN-2) );
//    std::cout << "b=" << (int)b << std::endl;
    if (b>0) {
//      for (int i=0; i<b; ++i) std::cout << "R:" << (int)buffer[i] << std::endl;
      buffer[b]=0;
      pos = _readLineBuffer.length();
      _readLineBuffer.append(buffer);
//      std::cout << "l=" << readLineBuffer.length() << std::endl;
    } else {
      waitCount+=READ_WAIT_US;
      if (READ_TIMEOUT_US<waitCount) throw XDynCommTimeout();
      usleep(READ_WAIT_US);       // wait for a small time
    }
  }
}



void DynInterfaceCM5PosixSerial::writeData(unsigned char id, unsigned char address, unsigned char *const data, unsigned char length) throw (XDynComm) {
  if (0 == length) throw XDynDataIllegal("DynInterfaceCM5PosixSerial::writeData: no data given");
  if (0 == data) throw XDynDataIllegal("DynInterfaceCM5PosixSerial::writeData: no data given");
  {
    std::ostringstream ostr;
    ostr << "cid " << static_cast<int>( id ) <<"\r";
    std::string buffer = ostr.str();
    ssize_t n = write(fd, buffer.c_str(), buffer.size() );
    if (n < static_cast<ssize_t>( buffer.size() ) )
      throw XDynComm("DynInterfaceCM5PosixSerial: write() of n bytes failed!");
  }
 
    std::ostringstream costr;
    costr << "w " << static_cast<int>(address);
    for (unsigned char i = 0; i<length; ++i) {
      costr << " " << static_cast<int>( data[i] );
    }
    costr << " \r";
    std::string buffer = costr.str();
// std::cout << buffer << std::endl;
    ssize_t n = write(fd, buffer.c_str(), buffer.size() );
    if (n < static_cast<ssize_t>( buffer.size() ) )
      throw XDynComm("DynInterfaceCM5PosixSerial: write() of n bytes failed!");
  

  // parse response
  int tries = 50;
  size_t pos;
  std::ostringstream ostr;
  ostr << "<-[Dynamixel]:255 255 " << std::setw(3) << std::setfill('0')
       << static_cast<int>(id) << " 002 ";
  std::string target = ostr.str();
  while (tries > 0) {
    try {
      std::string line = readLine();
//if (!line.empty()) std::cout<<"\""<<line<<"\""<<std::endl;
      if ((line.find("Bad Command") != std::string::npos)) { // resend
//        std::cout << "resend: " << buffer << std::endl;
        ssize_t n = write(fd, buffer.c_str(), buffer.size() );
        if (n < static_cast<ssize_t>( buffer.size() ) )
         throw XDynComm("DynInterfaceCM5PosixSerial: write() of n bytes failed!");
      }
      else if ((pos=line.find(target)) != std::string::npos) {
        // read response. waiting for status packet.
        
        line.erase(0,target.length()+pos); 
        // now line contains status code and appendix
        
        if ((pos=line.find(' ')) != std::string::npos) {
          line.erase(pos, std::string::npos); // finally, only status code...
          
          long st = str2long(line);
          if (st != 0) throw XDynStatusError(st); else return;
        }
      }
    } catch (XDynCommTimeout &) { usleep(READ_MAXWAIT_US); }
    --tries;
  }
  if (0 == tries) throw XDynComm("DynInterfaceCM5PosixSerial::writeData: Timeout1");
}
    
void DynInterfaceCM5PosixSerial::readData(unsigned char id, unsigned char address, unsigned char* data, unsigned char length) throw (XDynComm) {
  if (0 == length) throw XDynDataIllegal("DynInterfaceCM5PosixSerial::readData: no data given");
  if (0 == data) throw XDynDataIllegal("DynInterfaceCM5PosixSerial::readData: no data given");
  {
    std::ostringstream ostr;
    ostr << "cid " << static_cast<int>( id ) <<"\r";
    std::string buffer = ostr.str();
    ssize_t n = write(fd, buffer.c_str(), buffer.size() );
    if (n < static_cast<ssize_t>( buffer.size() ) )
      throw XDynComm("DynInterfaceCM5PosixSerial: write() of n bytes failed!");
  }
  {
    std::ostringstream ostr;
    ostr << "r " << static_cast<int>(address);
    ostr <<  " " << static_cast<int>(length);
    ostr << "\r";
    std::string buffer = ostr.str();
    ssize_t n = write(fd, buffer.c_str(), buffer.size() );
    if (n < static_cast<ssize_t>( buffer.size() ) )
      throw XDynComm("DynInterfaceCM5PosixSerial: write() of n bytes failed!");
 }
  
  // parse response
  int tries = 100;
  size_t pos;
  std::ostringstream ostr;
  ostr << "<-[Dynamixel]:255 255 " << std::setw(3) << std::setfill('0')
       << static_cast<int>(id) << " " << std::setw(3) << std::setfill('0')
       << static_cast<int>(length)+2 << " ";
  std::string target = ostr.str();
  while (tries > 0) {
    try {
      std::string line = readLine();
      if ((pos=line.find(target)) != std::string::npos) {
        // read response. waiting for status packet and data.
        
        line.erase(0,target.length()+pos); 
        // now line contains status code and appendix
        
        if ((pos=line.find(' ')) != std::string::npos) {
          std::string error = line.substr(0,pos);
          line.erase(0,pos+1); // finally, only data
          
          int errCode = str2long(error);
          // parse data!
          std::string number;
          for (int i=0; i<length; ++i) {
            number = line.substr(i*4, 3);
            data[i] = str2long( number.c_str() );
          }            
          if (errCode) throw XDynStatusError(errCode); else return;
        }
      }
    } catch (XDynCommTimeout &) { usleep(READ_MAXWAIT_US); }
    --tries;
  }
  if (0 == tries) throw XDynComm("DynInterfaceCM5PosixSerial::readData: Timeout2");
}

boost::shared_ptr<DynInterface> DynInterfaceCM5PosixSerial::getSharedPtr() {
  return boost::shared_ptr<DynInterface>(_myPtr);
}

void DynInterfaceCM5PosixSerial::setSharedPtr(boost::shared_ptr<DynInterface> ptr) {
  _myPtr = boost::weak_ptr<DynInterface>( ptr );
}

} // namespace
#endif
// EOF
