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
#ifndef DYN_INTERFACE_CM5_POSIX_SERIAL_H
#define DYN_INTERFACE_CM5_POSIX_SERIAL_H
#ifdef HAVE_POSIXSERIAL

#include "dynapi.h"
#include <iostream>
#include <fcntl.h>   /* File control definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace dynapi {
  class DynInterfaceCM5PosixSerial: public DynInterface {
    public:
    DynInterfaceCM5PosixSerial(std::string device) throw (XDynComm);
    ~DynInterfaceCM5PosixSerial();
    
    virtual void getIfInfo(IfInfo &info);
    
    private:
    void probe() throw (XDynComm);  ///< probe interfaces
    virtual std::vector<Ax> scanBus() throw (XDynComm);
    virtual void writeData(unsigned char id, unsigned char address, unsigned char *const data, unsigned char length) throw (XDynComm);
    
    virtual void readData(unsigned char id, unsigned char address, unsigned char* data, unsigned char length) throw (XDynComm);

    
    void readAllJunk() throw ();   ///< read bytes from tty as long as new bytes arrive
    std::string readLine() throw(XDynComm);
/*    unsigned char readByte() throw (XDynCommTimeout); ///< read one byte
    void readSpecialCode(const unsigned char code) throw (XDynCommTimeout); ///< wait for special char
    void helperGetInputs(IfInputs &inputs, unsigned char buffer[]);*/
    
    virtual boost::shared_ptr<DynInterface> getSharedPtr();
    virtual void setSharedPtr(boost::shared_ptr<DynInterface> ptr);
    
private:    
    std::string _device;
    struct termios _oldTermIOS;
    struct termios _newTermIOS;
    int fd; /* File descriptor for the port */
    
    dyn_if_type _ifType; ///< interface type, set in ctor
    std::string _readLineBuffer;
    
    static const int READ_WAIT_US, READ_TIMEOUT_US;
    
    unsigned int _numDevices;
    
    boost::weak_ptr<DynInterface> _myPtr;
  };

} // namespace dynapi  
#endif
#endif
// EOF

