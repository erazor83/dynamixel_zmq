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

#include "dynapi.h"
#include "dyninterfacecm5posixserial.h"
#include <sstream>

/** @file
implementation */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace dynapi {


/***************************************************************************
 *  d y n G e t I f F r o m F a c t o r y                                  *
***************************************************************************/

boost::shared_ptr<DynInterface> dynGetIfFromFactory(std::string device, dyn_if_conn connection) throw (XDyn) {
  switch (connection) {
#ifdef HAVE_POSIXSERIAL
    case DYN_IC_SERIAL: 
      {
        boost::shared_ptr<DynInterface> tmp(new DynInterfaceCM5PosixSerial(device));
        tmp->setSharedPtr(tmp);
        return tmp;
      }
      break;
#endif
    default: 
      throw XDyn("connection type not supported");
  }
  // compiler warnings
  std::string dev(device);
}



/***************************************************************************
 *  X D y n                                                                *
***************************************************************************/

/// base exception class
XDyn::XDyn(): msg("XDyn") {}
XDyn::XDyn(std::string message): msg("XDyn: ") { msg += message; }
XDyn::~XDyn() throw() {}
const char* XDyn::what() const throw() { return msg.c_str(); }

/// communication exception
XDynComm::XDynComm() { msg="XDynComm"; }
XDynComm::XDynComm(std::string message): XDyn() { msg="XDynComm: "+message; }
XDynComm::~XDynComm() throw() {}
const char* XDynComm::what() const throw() { return msg.c_str(); }

/// communication timeout
XDynCommTimeout::XDynCommTimeout() { msg="XDynCommTimeout"; }
XDynCommTimeout::XDynCommTimeout(std::string message): XDynComm() { msg="XDynCommTimeout: "+message; }
XDynCommTimeout::~XDynCommTimeout() throw() {}
const char* XDynCommTimeout::what() const throw() { return msg.c_str(); }

/// data not allowed
XDynDataIllegal::XDynDataIllegal() { msg="XDynDataIllegal"; }
XDynDataIllegal::XDynDataIllegal(std::string message): XDyn() { msg="XDynDataIllegal: "+message; }
XDynDataIllegal::~XDynDataIllegal() throw() {}
const char* XDynDataIllegal::what() const throw() { return msg.c_str(); }

/// functionality not supported by device
XDynNotSupported::XDynNotSupported() { msg="XDynNotSupported"; }
XDynNotSupported::XDynNotSupported(std::string message): XDyn() { msg="XDynNotSupported: "+message; }
XDynNotSupported::~XDynNotSupported() throw() {}
const char* XDynNotSupported::what() const throw() { return msg.c_str(); }


/// functionality not supported by device
//XDynStatusError::XDynStatusError() { msg="XDynNotSupported"; }
XDynStatusError::XDynStatusError(dyn_err_type error) { _errorCode = error; }
XDynStatusError::~XDynStatusError() throw() {}
dyn_err_type XDynStatusError::errorCode() const throw() { return _errorCode; }
const char* XDynStatusError::what() const throw() { 
  std::ostringstream ostr;
  ostr << "XDynStatusError: ";
  if (_errorCode&dynapi::XDynStatusError::INPUT_VOLTAGE) ostr<<"INPUT_VOLTAGE|";
  if (_errorCode&dynapi::XDynStatusError::ANGLE_LIMIT) ostr<<"ANGLE_LIMIT|";
  if (_errorCode&dynapi::XDynStatusError::OVERHEATING) ostr<<"OVERHEATING|";
  if (_errorCode&dynapi::XDynStatusError::RANGE) ostr<<"RANGE|";
  if (_errorCode&dynapi::XDynStatusError::CHECKSUM) ostr<<"CHECKSUM|";
  if (_errorCode&dynapi::XDynStatusError::OVERLOAD) ostr<<"OVERLOAD|";
  if (_errorCode&dynapi::XDynStatusError::INSTRUCTION) ostr<<"INSTRUCTION|";
  std::string o( ostr.str() );
  o.erase(o.length()-1);
  return o.c_str();
}

} // namespace


std::ostream& operator<<(std::ostream& s, const dynapi::IfInfo &x) {
  // save stream settings (we want to switch to hex and back)
  std::ios_base::fmtflags oldflags( s.flags() );

  // print some information
  s<<std::dec<<((dynapi::DYN_IF_CM5==x.ifType)?"CM-5":"--not detected--")
  <<" at \""
  <<x.device<<"\" ("<<(dynapi::DYN_IC_SERIAL==x.ifConn?"serial":
                       "unknown")<<")";

  // restore settings
  s.flags(oldflags);
  return s;
}



#endif

// EOF
