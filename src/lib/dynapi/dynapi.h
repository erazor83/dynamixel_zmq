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
#ifndef DYNAPI_H
#define DYNAPI_H

/** @file
public header file */

#include <exception>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "types.h"
#include "ax.h"

#ifdef WIN32
#pragma warning( disable : 4290 )
#endif
/// namespace containing all the dynamixel stuff
namespace dynapi {
class Ax;

  
  
  /** base exception class. 
  
  @note All exceptions are (directly or indirectly) inherited
  from std::exception. 
  
  They can be caught the following way:
  @code 
  try {
    ...
    // this may throw exceptions...
    ...
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  @endcode
  */
  class XDyn: public std::exception {
    public:
    XDyn();
    XDyn(std::string message);
    virtual ~XDyn() throw();
    virtual const char* what() const throw();
    protected: 
    std::string msg;
  };
  /// communication exception
  class XDynComm: public XDyn {
    public:
    XDynComm();
    XDynComm(std::string message);
    virtual ~XDynComm() throw();
    virtual const char* what() const throw();
  };
  /// communication timeout
  class XDynCommTimeout: public XDynComm {
  public:
    XDynCommTimeout();
    XDynCommTimeout(std::string message);
    virtual ~XDynCommTimeout() throw();
    virtual const char* what() const throw();
  };
  /// data not allowed
  class XDynDataIllegal: public XDyn {
  public:
    XDynDataIllegal();
    XDynDataIllegal(std::string message);
    virtual ~XDynDataIllegal() throw();
    virtual const char* what() const throw();
  };
  /// functionality not supported by device
  class XDynNotSupported: public XDyn {
  public:
    XDynNotSupported();
    XDynNotSupported(std::string message);
    virtual ~XDynNotSupported() throw();
    virtual const char* what() const throw();
  };
 

  /** if device status is not OK
    @code 
    try {
      ...
      // this may throw exceptions...
      ...
    } catch (dynapi::XStatusError &e) {
      // test for overload error
      if (e.errorCode() & XDynStatusError::OVERLOAD) {
        std::cerr << "Overload error!" << std::endl;
      } else {
        throw();  // continue at next catch
      }
    } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
    }
    @endcode
  */
  class XDynStatusError: public XDyn {
  private:
    XDynStatusError() {}
  public:
    XDynStatusError(dyn_err_type error);
    virtual ~XDynStatusError() throw();
    virtual const char* what() const throw();
    /** bitfield containing status flags
     @see dyn_err_type
     */
    virtual dyn_err_type errorCode() const throw();
    /// set if input voltage is out of range defined by Ax::setLowVoltageLimit and Ax::setHighVoltageLimit 
    static const dyn_err_type INPUT_VOLTAGE = 1;  
    /// set if goal position is out of range defined by Ax::setCwAngleLimit and Ax::setCcwAngleLimit
    static const dyn_err_type ANGLE_LIMIT = 2;
    /// set if the current temperature is higher than defined by Ax::setHighTemperatureLimit
    static const dyn_err_type OVERHEATING = 4;
    /// set if parameter is out of range
    static const dyn_err_type RANGE = 8;
    /// set if checksum error occured during communication
    static const dyn_err_type CHECKSUM = 16;
    /// set if torque has been higher than Ax::setMaxTorque
    static const dyn_err_type OVERLOAD = 32;
    /// undefined instruction
    static const dyn_err_type INSTRUCTION = 64;
    /// all flags set (useful for Ax::setAlarmLed() or Ax::setAlarmShutdown()
    static const dyn_err_type ALL = 127;
  private:
    dyn_err_type _errorCode;
  };


  /// specifies connection type
  enum dyn_if_conn {
    /// unknown connection type (not properly set)
    DYN_IC_UNKNOWN,  
    /** for CM-5 serial (57600,8,n,1 Xon/Xoff) */
    DYN_IC_SERIAL
  };

  /// specifies interface model
  enum dyn_if_type {
    /// unknown device (not properly set?)
    DYN_IF_UNKNOWN,  
    /** CM-5 serial (57600,8,n,1 Xon/Xoff) */ 
    DYN_IF_CM5
  };
 
  /** enum to specify interface or one of it's extensions. */
  enum dyn_device_type {
    /// Dynamixel AX-12 Servo
    DYN_DEV_AX12 = 0,
    /// Dynamixel AX-1 Sensor
    DYN_DEV_AX1 = 1
  };
 


  
  
  /** general information about device (connection, model, etc) 
  */
  class IfInfo {
    public:
    /** ctor. @note initializes all values to 0. */
    IfInfo();

    dyn_if_type ifType;           ///< interface model  
    dyn_if_conn ifConn;           ///< connection type  
    std::string device;           ///< filename and path  
  };


  /** Abstract base class for accessing Robotis Dynamixel interfaces.
  This base class has been provided to hide the different ways how
  to connect a bus (CM-5, etc.)

  A factory function dynapi::dynGetIfFromFactory provides instances
  of the appropriate classes for the given interface type and
  connection.
  @see dynapi::dynGetIfFromFactory
  */
  class DynInterface {
    public: 
    /// dtor
    virtual ~DynInterface() {};
    
    /// gather general information on the connected interface.
    virtual void getIfInfo(IfInfo &info) = 0;
   
    /// scan bus for devices
    virtual std::vector<Ax> scanBus() throw (XDynComm) = 0;
   
    virtual void writeData(unsigned char id, unsigned char address, unsigned char *const data, unsigned char length) throw (XDyn) = 0;
    
    virtual void readData(unsigned char id, unsigned char address, unsigned char* data, unsigned char length) throw (XDyn) = 0;
    
    virtual boost::shared_ptr<DynInterface> getSharedPtr() = 0;
    virtual void setSharedPtr(boost::shared_ptr<DynInterface> ptr) = 0;
  };
  
  /** A factory function to provide instances
  of the appropriate classes for the given interface type and
  connection.
  The different classes are completely hidden.

  Instances can be created by the factory this way:
  @code
  // serial connection to CM-5
  DynInterface *ifserial = dynGetIfFromFactory("/dev/ttyS0", DYN_IC_SERIAL);
  ...
  delete(ifserial);
  @endcode

  Do not forget to destruct the instances after work (delete). An elegant way to
  do this automatically is using the std::auto_ptr mechanism:
  @code
  #include <memory>
  ...
  try {
    // open a connection to cm-5 interface
    // (shared_ptr provides automatic garbage collection, ptr will be released
    // if last shared_ptr is destructed)
    boost::shared_ptr<DynInterface> dyni (dynGetIfFromFactory(device, connection));

    // show some information on the interface
    IfInfo ii;
    dyni->getIfInfo(ii);
    cout << ii << endl << endl;
    
    // get device list
    std::vector<dynapi::Ax> devices( dyni->scanBus() );
    cout << devices << endl << endl;
 
    // when leaving this block (exception or regular exit), dyni will be released
  } catch ... 
  @endcode
  @throws XDyn if connection tpye (backend) is not supported or device cannot 
  be opened. See @ref backends for a complete list of devices and backends
  supported by dynapi. 

  @param device
  depends on backend:
  - serial connection: Name of the device node, e.g. /dev/ttyS0 
  @param connection connection type, see dynapi::dyn_if_conn
  */
  boost::shared_ptr<DynInterface> dynGetIfFromFactory(std::string device, dyn_if_conn connection) throw (XDyn);

}

/// easy way to print information about interface to a stream
std::ostream& operator<<(std::ostream& s, const dynapi::IfInfo &x);

#endif
