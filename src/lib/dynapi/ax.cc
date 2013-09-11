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

#include "ax.h"
#include "dynapi.h"
#include <sstream>
#include <boost/shared_ptr.hpp>
#include <iostream>

namespace dynapi {
const int Ax::BAUD_RATE_RAW_DEFAULT = 1;  ///< baud rate: default, 1000000 bps.
const int Ax::BAUD_RATE_RAW_1M    =   1;  ///< baud rate 1000000 bps.
const int Ax::BAUD_RATE_RAW_500K  =   3;  ///< baud rate  500000 bps.
const int Ax::BAUD_RATE_RAW_400K  =   4;  ///< baud rate  400000 bps.
const int Ax::BAUD_RATE_RAW_250K  =   7;  ///< baud rate  250000 bps.
const int Ax::BAUD_RATE_RAW_200K  =   9;  ///< baud rate  200000 bps.
const int Ax::BAUD_RATE_RAW_115K2 =  16;  ///< baud rate  115200 bps (true bps will be 117647 bps).
const int Ax::BAUD_RATE_RAW_57K6  =  34;  ///< baud rate   57600 bps (true bps will be  57143 bps).
const int Ax::BAUD_RATE_RAW_19K2  = 103;  ///< baud rate   19200 bps (true bps will be  19231 bps).
const int Ax::BAUD_RATE_RAW_9K6   = 207;  ///< baud rate    9600 bps (true bps will be   9615 bps).

///////////////////////////////////////////    A X  -  I m p l       ///////////////////

class AxImpl {
public:
  AxImpl(boost::shared_ptr<DynInterface> host, unsigned char id);

  virtual ~AxImpl();

  
  /// @name base functionality of AX devices
  //@{
  virtual unsigned int getModelNumber() const;
  virtual std::string getModelName() const;
  virtual unsigned int getFirmwareVersion() const;

  virtual unsigned char getId() const;
  virtual void setId(unsigned char id);

  virtual float getBaudRateBps() const;
  virtual void setBaudRateBps(float bps);
  virtual unsigned char getBaudRateRaw() const;
  virtual void setBaudRateRaw(unsigned char baudRate);

  virtual unsigned int getReturnDelayTimeInUs() const;
  virtual void setReturnDelayTimeInUs(unsigned int us);
  
  virtual unsigned char getHighTemperatureLimit() const;
  virtual void setHighTemperatureLimit(unsigned char temp);
  
  virtual unsigned char getLowVoltageLimit() const;
  virtual void setLowVoltageLimit(unsigned char voltage);

  virtual unsigned char getHighVoltageLimit() const;
  virtual void setHighVoltageLimit(unsigned char voltage);

  virtual Ax::status_return_level getStatusReturnLevel() const;
  virtual void setStatusReturnLevel(Ax::status_return_level status);

  virtual unsigned char getPresentVoltage() const;
  virtual unsigned char getPresentTemperature() const;
  
  virtual bool getRegisteredInstruction() const;
  virtual void setRegisteredInstruction(bool ins);
  
  virtual bool isLocked() const;
  virtual void setLock();
  
  static const int BAUD_RATE_RAW_DEFAULT = 1;
  static const int BAUD_RATE_RAW_1M    =   1;
  static const int BAUD_RATE_RAW_500K  =   3;
  static const int BAUD_RATE_RAW_400K  =   4;
  static const int BAUD_RATE_RAW_250K  =   7;
  static const int BAUD_RATE_RAW_200K  =   9;
  static const int BAUD_RATE_RAW_115K2 =  16;
  static const int BAUD_RATE_RAW_57K6  =  34;
  static const int BAUD_RATE_RAW_19K2  = 103;
  static const int BAUD_RATE_RAW_9K6   = 207;  
  //@}
  
  /// @name AX-12
  //@{
  virtual bool isActuator() const { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned int getCwAngleLimit() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setCwAngleLimit(unsigned int) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned int getCcwAngleLimit() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setCcwAngleLimit(unsigned int ) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned int getMaxTorque() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setMaxTorque(unsigned int ) { throw XDynNotSupported(__FUNCTION__); } 
  
  virtual dyn_err_type getAlarmLed() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setAlarmLed(dyn_err_type) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual dyn_err_type getAlarmShutdown() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setAlarmShutdown(dyn_err_type) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned int getDownCalibration() const { throw XDynNotSupported(__FUNCTION__); }
  virtual unsigned int getUpCalibration() const { throw XDynNotSupported(__FUNCTION__); }

  virtual bool isTorqueEnabled() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setTorqueEnabled(bool ) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual bool isLedEnabled() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setLed(bool) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned char getCwComplianceMargin() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setCwComplianceMargin(unsigned char ) { throw XDynNotSupported(__FUNCTION__); }
  virtual unsigned char getCcwComplianceMargin() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setCcwComplianceMargin(unsigned char ) { throw XDynNotSupported(__FUNCTION__); }
  virtual unsigned char getCwComplianceSlope() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setCwComplianceSlope(unsigned char ) { throw XDynNotSupported(__FUNCTION__); }
  virtual unsigned char getCcwComplianceSlope() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setCcwComplianceSlope(unsigned char ) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned int getGoalPosition() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setGoalPosition(unsigned int ) { throw XDynNotSupported(__FUNCTION__); }
  virtual void setGoalPosition(unsigned int , unsigned int ) { throw XDynNotSupported(__FUNCTION__); }
  virtual void setGoalPosition(unsigned int , unsigned int , unsigned int ) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned int getMovingSpeed() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setMovingSpeed(unsigned int ) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned int getTorqueLimit() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setTorqueLimit(unsigned int ) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned int getPresentPosition() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void getPresentPosition(unsigned int &, unsigned int &, int &) const { throw XDynNotSupported(__FUNCTION__); }
  virtual unsigned int getPresentSpeed() const { throw XDynNotSupported(__FUNCTION__); }
  virtual int getPresentLoad() const { throw XDynNotSupported(__FUNCTION__); }
  
  virtual bool isMoving() const { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned int getPunch() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setPunch(unsigned int ) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual bool isSensor() const { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned char getObstacleDetectedCompareValue() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setObstacleDetectedCompareValue(unsigned char ) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned char getLightDetectedCompareValue() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setLightDetectedCompareValue(unsigned char ) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned char getIrDataLeft() const { throw XDynNotSupported(__FUNCTION__); }
  virtual unsigned char getIrDataCenter() const { throw XDynNotSupported(__FUNCTION__); }
  virtual unsigned char getIrDataRight() const  { throw XDynNotSupported(__FUNCTION__); }
  virtual void getIrData(unsigned char &, unsigned char &, unsigned char &) const { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned char getLuminosityLeft() const { throw XDynNotSupported(__FUNCTION__); }
  virtual unsigned char getLuminosityCenter() const { throw XDynNotSupported(__FUNCTION__); }
  virtual unsigned char getLuminosityRight() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void getLuminosity(unsigned char &, unsigned char &, unsigned char &) const { throw XDynNotSupported(__FUNCTION__); }
  
  virtual bool isObstacleDetected() const { throw XDynNotSupported(__FUNCTION__); }
  virtual bool isLuminosityDetected() const { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned char getSoundData() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setSoundData(unsigned char ) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned char getSoundDataMaxHold() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setSoundDataMaxHold(unsigned char ) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned char getSoundDetectedCount() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setSoundDetectedCount(unsigned char ) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned int getSoundDetectedTime() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setSoundDetectedTime(unsigned int ) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned char getBuzzerIndex() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setBuzzerIndex(unsigned char ) { throw XDynNotSupported(__FUNCTION__); }

  virtual unsigned char getBuzzerTime() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setBuzzerTime(unsigned char ) { throw XDynNotSupported(__FUNCTION__); }

  virtual bool hasIrRemoconArrived() { throw XDynNotSupported(__FUNCTION__); }
  virtual unsigned int getIrRemoconRx() { throw XDynNotSupported(__FUNCTION__); }
  virtual void setIrRemoconTx(unsigned int ) { throw XDynNotSupported(__FUNCTION__); }
  
  virtual unsigned char getObstableDetectedCompare() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setObstableDetectedCompare(unsigned char ) { throw XDynNotSupported(__FUNCTION__); }
    
  virtual unsigned char getLightDetectedCompare() const { throw XDynNotSupported(__FUNCTION__); }
  virtual void setLightDetectedCompare(unsigned char ) { throw XDynNotSupported(__FUNCTION__); }
  
protected:
  
  
  unsigned int _modelNumber;
  unsigned int _firwareVersion;
  unsigned char _id;
  mutable unsigned char _buffer[100];
  boost::shared_ptr<DynInterface> _if;

};


AxImpl::AxImpl(boost::shared_ptr<DynInterface> host, unsigned char id) {
//std::cout << __FUNCTION__ << std::endl;
  _id = id;
  _if = host;
  _modelNumber = getModelNumber();
  _firwareVersion = getFirmwareVersion();
  _id = getId();
  if (_id != id) throw XDynDataIllegal("id inconsistancy.");
}

AxImpl::~AxImpl() {
//std::cout << __FUNCTION__ << std::endl;
}

unsigned int AxImpl::getModelNumber() const {
  _if->readData(_id, 0, _buffer, 2);
  unsigned int i = _buffer[1];
  i = i << 8;
  i |= _buffer[0];
  return i;
}

std::string AxImpl::getModelName() const {
  switch (_modelNumber) {
    case 12: 
      return "AX-12";
    case 13: 
      return "AX-S1";
    default:
      {
        std::ostringstream b;
        b << "AX-unknown (" << _modelNumber << ")";
        return b.str();
      }
  }
}

unsigned int AxImpl::getFirmwareVersion() const {
  _if->readData(_id, 2, _buffer, 1);
  return _buffer[0];
}

unsigned char AxImpl::getId() const {
  _if->readData(_id, 3, _buffer, 1);
  return _buffer[0];
}

void AxImpl::setId(unsigned char id) {
  _if->writeData(_id, 3, &id, 1);
  _id = getId();
}

float AxImpl::getBaudRateBps() const {
  return 2000000.0f / (getBaudRateRaw() + 1.0f);
}

void AxImpl::setBaudRateBps(float bps) {
  //b=2000000/(v+1)
  unsigned int value = static_cast<int>( (bps / 2000000.0f) - 1.0f + .5f ); //+.5 is round()
  if (value > 255) throw XDynDataIllegal("bps too high");
  setBaudRateRaw( static_cast<unsigned char>(value) );
}

unsigned char AxImpl::getBaudRateRaw() const {
  _if->readData(_id, 4, _buffer, 1);
  return _buffer[0];
}

void AxImpl::setBaudRateRaw(unsigned char baudRate) {
  _if->writeData(_id, 4, &baudRate, 1);
  _id = getId();
}

unsigned int AxImpl::getReturnDelayTimeInUs() const {
  _if->readData(_id, 5, _buffer, 1);
  return _buffer[0] * 2;
}

void AxImpl::setReturnDelayTimeInUs(unsigned int us)  {
  us = us / 2;
  if (us > 255) throw XDynDataIllegal("timeout too big");
  _buffer[0] = static_cast<unsigned char>( us );
  _if->writeData(_id, 5, _buffer, 1);
  _id = getId();
}

unsigned char AxImpl::getHighTemperatureLimit() const {
  _if->readData(_id, 11, _buffer, 1);
  return _buffer[0];
}

void AxImpl::setHighTemperatureLimit(unsigned char temp) {
  _if->writeData(_id, 11, &temp, 1);
}
  
unsigned char AxImpl::getLowVoltageLimit() const {
  _if->readData(_id, 12, _buffer, 1);
  return _buffer[0];
}

void AxImpl::setLowVoltageLimit(unsigned char voltage) {
  _if->writeData(_id, 12, &voltage, 1);
}

unsigned char AxImpl::getHighVoltageLimit() const {
  _if->readData(_id, 13, _buffer, 1);
  return _buffer[0];
}

void AxImpl::setHighVoltageLimit(unsigned char voltage) {
  _if->writeData(_id, 13, &voltage, 1);
}

Ax::status_return_level AxImpl::getStatusReturnLevel() const {
  _if->readData(_id, 16, _buffer, 1);
  return static_cast<Ax::status_return_level>( _buffer[0] );
}

void AxImpl::setStatusReturnLevel(Ax::status_return_level status) {
  _if->writeData(_id, 47, reinterpret_cast<unsigned char *>(&status), 1);
}

unsigned char AxImpl::getPresentVoltage() const {
  _if->readData(_id, 42, _buffer, 1);
  return _buffer[0];
}

unsigned char AxImpl::getPresentTemperature() const {
  _if->readData(_id, 43, _buffer, 1);
  return _buffer[0];
}
  
bool AxImpl::getRegisteredInstruction() const {
  _if->readData(_id, 44, _buffer, 1);
  return (0 != _buffer[0]);
}

void AxImpl::setRegisteredInstruction(bool ins) {
  _buffer[0] = ins;
  _if->writeData(_id, 44, _buffer, 1);
}
  
bool AxImpl::isLocked() const {
  _if->readData(_id, 47, _buffer, 1);
  return (0 != _buffer[0]);
}

void AxImpl::setLock() {
  unsigned char ins = 1;
  _if->writeData(_id, 47, &ins, 1);
}




///////////////////////////////////////////    A X 1 2 I m p l   ///////////////////

class Ax12Impl: public AxImpl {
public:
  Ax12Impl(boost::shared_ptr<DynInterface> host, unsigned char id);
  virtual ~Ax12Impl() {
//std::cout << __FUNCTION__ << std::endl;
  }
  
  virtual bool isActuator() const { return true; }
  
  virtual unsigned int getCwAngleLimit() const { 
    _if->readData(_id, 6, _buffer, 2);
    unsigned int limit = _buffer[1];
    limit = limit << 8;
    limit |= _buffer[0];
    return limit;
  }
  virtual void setCwAngleLimit(unsigned int limit) {
    _buffer[0] = static_cast<unsigned char>( limit & 0xff );
    _buffer[1] = static_cast<unsigned char>( (limit & 0xff00) >> 8 );
    _if->writeData(_id, 6, _buffer, 2);
  }
  
  virtual unsigned int getCcwAngleLimit() const { 
    _if->readData(_id, 8, _buffer, 2);
    unsigned int limit = _buffer[1];
    limit = limit << 8;
    limit |= _buffer[0];
    return limit;
  }
  virtual void setCcwAngleLimit(unsigned int limit) {
    _buffer[0] = static_cast<unsigned char>( limit & 0xff );
    _buffer[1] = static_cast<unsigned char>( (limit & 0xff00) >> 8 );
    _if->writeData(_id, 8, _buffer, 2);
  }
  
  virtual unsigned int getMaxTorque() const { 
    _if->readData(_id, 14, _buffer, 2);
    unsigned int limit = _buffer[1];
    limit = limit << 8;
    limit |= _buffer[0];
    return limit;
  }
  virtual void setMaxTorque(unsigned int torque) {
    _buffer[0] = static_cast<unsigned char>( torque & 0xff );
    _buffer[1] = static_cast<unsigned char>( (torque & 0xff00) >> 8 );
    _if->writeData(_id, 14, _buffer, 2);
  }
  
  virtual dyn_err_type getAlarmLed() const {
    _if->readData(_id, 17, _buffer, 1);
    return _buffer[0];
  }
  
  virtual void setAlarmLed(dyn_err_type status) { 
    _if->writeData(_id, 17, &status, 1);
  }
    
  virtual dyn_err_type getAlarmShutdown() const {
    _if->readData(_id, 18, _buffer, 1);
    return _buffer[0];
  }
  virtual void setAlarmShutdown(dyn_err_type status) {
    _if->writeData(_id, 18, &status, 1);
  }
  
  virtual unsigned int getDownCalibration() const { 
    _if->readData(_id, 20, _buffer, 2);
    unsigned int limit = _buffer[1];
    limit = limit << 8;
    limit |= _buffer[0];
    return limit;
  }
  virtual unsigned int getUpCalibration() const { 
    _if->readData(_id, 22, _buffer, 2);
    unsigned int limit = _buffer[1];
    limit = limit << 8;
    limit |= _buffer[0];
    return limit;
  }

  virtual bool isTorqueEnabled() const { 
    unsigned char ret; 
    _if->readData(_id, 24, &ret, 1);
    return (0!=ret);
  }
  virtual void setTorqueEnabled(bool enable) { 
    _if->writeData(_id, 24, reinterpret_cast<unsigned char*>(&enable), 1);
  }
  
  virtual bool isLedEnabled() const {
    _if->readData(_id, 25, _buffer, 1);
    return (0 != _buffer[0]); 
  }
  virtual void setLed(bool led) {
    _buffer[0] = led;
    _if->writeData(_id, 25, _buffer, 1); 
  }
  
  virtual unsigned char getCwComplianceMargin() const {
    _if->readData(_id, 26, _buffer, 1);
    return _buffer[0];
  }
  virtual void setCwComplianceMargin(unsigned char margin) { 
    _if->writeData(_id, 26, &margin, 1);
  }
  virtual unsigned char getCcwComplianceMargin() const {
    _if->readData(_id, 27, _buffer, 1);
    return _buffer[0];
  }
  virtual void setCcwComplianceMargin(unsigned char margin) {
    _if->writeData(_id, 27, &margin, 1);
  }
  virtual unsigned char getCwComplianceSlope() const { 
    _if->readData(_id, 28, _buffer, 1);
    return _buffer[0];
  }
  virtual void setCwComplianceSlope(unsigned char slope) {
      _if->writeData(_id, 28, &slope, 1);
  }
  virtual unsigned char getCcwComplianceSlope() const {
    _if->readData(_id, 29, _buffer, 1);
    return _buffer[0];
  }
  virtual void setCcwComplianceSlope(unsigned char slope) {
    _if->writeData(_id, 29, &slope, 1);
  }
  
  virtual unsigned int getGoalPosition() const { 
    _if->readData(_id, 30, _buffer, 2);
    unsigned int pos = _buffer[1];
    pos = pos << 8;
    pos |= _buffer[0];
    return pos;
  }
  virtual void setGoalPosition(unsigned int pos) { 
    _buffer[0] = static_cast<unsigned char>( pos & 0xff );
    _buffer[1] = static_cast<unsigned char>( (pos & 0xff00) >> 8 );
    _if->writeData(_id, 30, _buffer, 2);  
  }
  virtual void setGoalPosition(unsigned int pos, unsigned int speed) {    
    _buffer[0] = static_cast<unsigned char>( pos & 0xff );
    _buffer[1] = static_cast<unsigned char>( (pos & 0xff00) >> 8 );
    _buffer[2] = static_cast<unsigned char>( speed & 0xff );
    _buffer[3] = static_cast<unsigned char>( (speed & 0xff00) >> 8 );
    _if->writeData(_id, 30, _buffer, 4);
  }
  virtual void setGoalPosition(unsigned int pos, unsigned int speed, unsigned int torqueLimit) { 
    _buffer[0] = static_cast<unsigned char>( pos & 0xff );
    _buffer[1] = static_cast<unsigned char>( (pos & 0xff00) >> 8 );
    _buffer[2] = static_cast<unsigned char>( speed & 0xff );
    _buffer[3] = static_cast<unsigned char>( (speed & 0xff00) >> 8 );
    _buffer[4] = static_cast<unsigned char>( torqueLimit & 0xff );
    _buffer[5] = static_cast<unsigned char>( (torqueLimit & 0xff00) >> 8 );
    _if->writeData(_id, 30, _buffer, 6);
  }
    
  virtual unsigned int getMovingSpeed() const {
    _if->readData(_id, 32, _buffer, 2);
    unsigned int speed = _buffer[1];
    speed = speed << 8;
    speed |= _buffer[0];
    return speed;
  }
  virtual void setMovingSpeed(unsigned int speed) { 
    _buffer[0] = static_cast<unsigned char>( speed & 0xff );
    _buffer[1] = static_cast<unsigned char>( (speed & 0xff00) >> 8 );
    _if->writeData(_id, 30, _buffer, 2);  
  }
  
  virtual unsigned int getTorqueLimit() const { 
    _if->readData(_id, 48, _buffer, 2);
    unsigned int torque = _buffer[1];
    torque = torque << 8;
    torque |= _buffer[0];
    return torque;
  }
  virtual void setTorqueLimit(unsigned int torque) { 
    _buffer[0] = static_cast<unsigned char>( torque & 0xff );
    _buffer[1] = static_cast<unsigned char>( (torque & 0xff00) >> 8 );
    _if->writeData(_id, 30, _buffer, 2);
  }
  
  virtual unsigned int getPresentPosition() const { 
    _if->readData(_id, 36, _buffer, 6);
    int pos = _buffer[1];
    pos = pos << 8;
    pos |= _buffer[0];
    return pos;
  }
  virtual void getPresentPosition(unsigned int &pos, unsigned int &speed, int &load) const {
    _if->readData(_id, 36, _buffer, 6);
    pos = _buffer[1];
    pos = pos << 8;
    pos |= _buffer[0];
    speed = _buffer[3];
    speed = speed << 8;
    speed |= _buffer[2];
    load = (_buffer[5] & 3);
    load = load << 8;
    load |= _buffer[4];
    if (_buffer[5] & 4) load *= -1; // clockwise = neg.
 }
  virtual unsigned int getPresentSpeed() const { throw XDynNotSupported(__FUNCTION__); }
  virtual int getPresentLoad() const {
    _if->readData(_id, 40, _buffer, 2);
    int load = _buffer[0];
    load |= (_buffer[1] & 3) << 8;
    if (_buffer[1] & 4) load *= -1; // clockwise = neg.
    return load;
  }
  
  virtual bool isMoving() const { 
    unsigned char rv; 
    _if->readData(_id, 46, &rv, 1); 
    return (0!=rv); 
  }
  
  virtual unsigned int getPunch() const { 
    _if->readData(_id, 48, _buffer, 2);
    unsigned int punch = _buffer[1];
    punch = punch << 8;
    punch |= _buffer[0];
    return punch;
  }
  virtual void setPunch(unsigned int punch) {
    _buffer[0] = static_cast<unsigned char>( punch & 0xff );
    _buffer[1] = static_cast<unsigned char>( (punch & 0xff00) >> 8 );
    _if->writeData(_id, 30, _buffer, 2);
  }
  
  virtual bool isSensor() const { return false; }
};


Ax12Impl::Ax12Impl(boost::shared_ptr<DynInterface> host, unsigned char id): AxImpl(host,id) {
//std::cout << __FUNCTION__ << std::endl;
}




///////////////////////////////////////////    A X S 1 I m p l   ///////////////////

class AxS1Impl: public AxImpl {
public:
  AxS1Impl(boost::shared_ptr<DynInterface> host, unsigned char id);
  virtual ~AxS1Impl() {
//std::cout << __FUNCTION__ << std::endl;
  }  
  
  virtual bool isActuator() const { return false; }
  virtual bool isSensor() const { return true; }
  
  virtual unsigned char getObstacleDetectedCompareValue() const { 
    _if->readData(_id, 20, _buffer, 1);
    return _buffer[0];
  }
  virtual void setObstacleDetectedCompareValue(unsigned char val) { 
    _if->writeData(_id, 20, &val, 1);
  }
  
  virtual unsigned char getLightDetectedCompareValue() const {
    _if->readData(_id, 21, _buffer, 1);
    return _buffer[0];
  }
  virtual void setLightDetectedCompareValue(unsigned char val) {
    _if->writeData(_id, 21, &val, 1); 
  }
  
  virtual unsigned char getIrDataLeft() const {
    _if->readData(_id, 26, _buffer, 1);
    return _buffer[0];
  }
  virtual unsigned char getIrDataCenter() const {
    _if->readData(_id, 27, _buffer, 1);
    return _buffer[0];
  }
  virtual unsigned char getIrDataRight() const {
    _if->readData(_id, 28, _buffer, 1);
    return _buffer[0];
  }  
  virtual void getIrData(unsigned char &left, unsigned char &center, unsigned char &right) const { 
    _if->readData(_id, 26, _buffer, 3);
    left   = _buffer[0];
    center = _buffer[1];
    right  = _buffer[2];
  }
  
  virtual unsigned char getLuminosityLeft() const {
    _if->readData(_id, 29, _buffer, 1);
    return _buffer[0];
  }
  virtual unsigned char getLuminosityCenter() const {
    _if->readData(_id, 30, _buffer, 1);
    return _buffer[0];
  }
  virtual unsigned char getLuminosityRight() const { 
    _if->readData(_id, 31, _buffer, 1);
    return _buffer[0];
  }
  virtual void getLuminosity(unsigned char &left, unsigned char &center, unsigned char &right) const {
    _if->readData(_id, 29, _buffer, 3);
    left   = _buffer[0];
    center = _buffer[1];
    right  = _buffer[2];
  }
  
  virtual bool isObstacleDetected() const {
    _if->readData(_id, 32, _buffer, 1);
    return (0 != _buffer[0]);
  }
  virtual bool isLuminosityDetected() const {
    _if->readData(_id, 33, _buffer, 1);
    return (0 != _buffer[0]);
  }

  virtual unsigned char getSoundData() const {
    _if->readData(_id, 35, _buffer, 1);
    return _buffer[0];
  }
  virtual void setSoundData(unsigned char data) {
    _if->writeData(_id, 35, &data, 1);
  }
  
  virtual unsigned char getSoundDataMaxHold() const {
    _if->readData(_id, 36, _buffer, 1);
    return _buffer[0];
  }
  virtual void setSoundDataMaxHold(unsigned char data) {
    _if->writeData(_id, 36, &data, 1);
  }
  
  virtual unsigned char getSoundDetectedCount() const {
    _if->readData(_id, 37, _buffer, 1);
    return _buffer[0];
  }
  virtual void setSoundDetectedCount(unsigned char cnt) { 
    _if->writeData(_id, 37, &cnt, 1);
  }
  
  virtual unsigned int getSoundDetectedTime() const {
    _if->readData(_id, 38, _buffer, 2);
    unsigned int time = _buffer[1];
    time = time << 8;
    time |= _buffer[0];
    return time;
  }
  virtual void setSoundDetectedTime(unsigned int time) {
    _buffer[0] = static_cast<unsigned char>( time & 0xff );
    _buffer[1] = static_cast<unsigned char>( (time & 0xff00) >> 8 );
    _if->writeData(_id, 38, _buffer, 2);
  }
  
  virtual unsigned char getBuzzerIndex() const {
    _if->readData(_id, 40, _buffer, 1);
    return _buffer[0];
  }
  virtual void setBuzzerIndex(unsigned char ind) { 
    _if->writeData(_id, 40, &ind, 1);
  }

  virtual unsigned char getBuzzerTime() const {
    _if->readData(_id, 41, _buffer, 1);
    return _buffer[0];
  }
  virtual void setBuzzerTime(unsigned char time) { 
    _if->writeData(_id, 41, &time, 1);
  }

  virtual bool hasIrRemoconArrived() const {
    _if->readData(_id, 46, _buffer, 1);
    return (0 != _buffer[0]);
  }
  virtual unsigned int getIrRemoconRx() const {
    _if->readData(_id, 48, _buffer, 2);
    unsigned int val = _buffer[1];
    val = val << 8;
    val |= _buffer[0];
    return val;  
  }
  virtual void setIrRemoconTx(unsigned int data) { 
    _buffer[0] = static_cast<unsigned char>( data & 0xff );
    _buffer[1] = static_cast<unsigned char>( (data & 0xff00) >> 8 );
    _if->writeData(_id, 50, _buffer, 2);
  }
  
  virtual unsigned char getObstableDetectedCompare() const {
    _if->readData(_id, 52, _buffer, 1);
    return _buffer[0];
  }
  virtual void getObstableDetectedCompare(unsigned char value) const {
    _if->writeData(_id, 52, &value, 1);
  }
    
  virtual unsigned char getLightDetectedCompare() const {
    _if->readData(_id, 53, _buffer, 1);
    return _buffer[0];
  }
  virtual void setLightDetectedCompare(unsigned char value) { 
    _if->writeData(_id, 53, &value, 1);
  }
};


AxS1Impl::AxS1Impl(boost::shared_ptr<DynInterface> host, unsigned char id): AxImpl(host,id) {
//std::cout << __FUNCTION__ << std::endl;
}


///////////////////////////////////////////    A X        ///////////////////

Ax::Ax(boost::shared_ptr<DynInterface> host, unsigned char id) {
//std::cout << __FUNCTION__ << std::endl;
  // probe
  _impl = boost::shared_ptr<AxImpl>( new AxImpl(host, id) );
  switch (_impl->getModelNumber() ) {
    case 12: // AX-12
      _impl = boost::shared_ptr<AxImpl>( new Ax12Impl(host, id) );
      break;
    case 13:
      _impl = boost::shared_ptr<AxImpl>( new AxS1Impl(host, id) ); 
      break;
  }
}

Ax::~Ax() {
//std::cout << __FUNCTION__ << std::endl;
}


unsigned int Ax::getModelNumber() const { return _impl->getModelNumber(); }
std::string Ax::getModelName() const { return _impl->getModelName(); }
unsigned int Ax::getFirmwareVersion() const { return _impl->getFirmwareVersion(); }

unsigned char Ax::getId() const { return _impl->getId(); }
void Ax::setId(unsigned char id) { _impl->setId(id); }

float Ax::getBaudRateBps() const { return _impl->getBaudRateBps(); }
void Ax::setBaudRateBps(float bps) { _impl->setBaudRateBps(bps); }
unsigned char Ax::getBaudRateRaw() const { return _impl->getBaudRateRaw(); }
void Ax::setBaudRateRaw(unsigned char baudRate) { _impl->setBaudRateRaw(baudRate); }

unsigned int Ax::getReturnDelayTimeInUs() const { return _impl->getReturnDelayTimeInUs(); }
void Ax::setReturnDelayTimeInUs(unsigned int us) { _impl->setReturnDelayTimeInUs(us); }

unsigned char Ax::getHighTemperatureLimit() const { return _impl->getHighTemperatureLimit(); }
void Ax::setHighTemperatureLimit(unsigned char temp) { _impl->setHighTemperatureLimit(temp); }

unsigned char Ax::getLowVoltageLimit() const { return _impl->getLowVoltageLimit(); }
void Ax::setLowVoltageLimit(unsigned char voltage) { _impl->setLowVoltageLimit(voltage); }

unsigned char Ax::getHighVoltageLimit() const { return _impl->getHighVoltageLimit(); }
void Ax::setHighVoltageLimit(unsigned char voltage) { _impl->setHighVoltageLimit(voltage); }

Ax::status_return_level Ax::getStatusReturnLevel() const { return _impl->getStatusReturnLevel(); }
void Ax::setStatusReturnLevel(status_return_level status) { _impl->setStatusReturnLevel(status); }

unsigned char Ax::getPresentVoltage() const { return _impl->getPresentVoltage(); }
unsigned char Ax::getPresentTemperature() const { return _impl->getPresentTemperature(); }

bool Ax::getRegisteredInstruction() const { return _impl->getRegisteredInstruction(); }
void Ax::setRegisteredInstruction(bool ins) { return _impl->setRegisteredInstruction(ins); }

bool Ax::isLocked() const { return _impl->isLocked(); }
void Ax::setLock() { _impl->setLock(); }

bool Ax::isActuator() const { return _impl->isActuator(); }

unsigned int Ax::getCwAngleLimit() const { return _impl->getCwAngleLimit(); }
void Ax::setCwAngleLimit(unsigned int limit) { _impl->setCwAngleLimit(limit); }

unsigned int Ax::getCcwAngleLimit() const { return _impl->getCcwAngleLimit(); }
void Ax::setCcwAngleLimit(unsigned int limit) { _impl->setCcwAngleLimit(limit); }

unsigned int Ax::getMaxTorque() const { return _impl->getMaxTorque(); }
void Ax::setMaxTorque(unsigned int torque) { _impl->setMaxTorque(torque); }

dyn_err_type Ax::getAlarmLed() const { return _impl->getAlarmLed(); }
void Ax::setAlarmLed(dyn_err_type status) { _impl->setAlarmLed(status); }

dyn_err_type Ax::getAlarmShutdown() const { return _impl->getAlarmShutdown(); }
void Ax::setAlarmShutdown(dyn_err_type status) { _impl->setAlarmShutdown(status); }

unsigned int Ax::getDownCalibration() const { return _impl->getDownCalibration(); }
unsigned int Ax::getUpCalibration() const { return _impl->getDownCalibration(); }

bool Ax::isTorqueEnabled() const { return _impl->isTorqueEnabled(); }
void Ax::setTorqueEnabled(bool enable) { _impl->setTorqueEnabled(enable); }

bool Ax::isLedEnabled() const { return _impl->isLedEnabled(); }
void Ax::setLed(bool led) { _impl->setLed(led); }

unsigned char Ax::getCwComplianceMargin() const { return _impl->getCwComplianceMargin(); }
void Ax::setCwComplianceMargin(unsigned char margin) { _impl->setCwComplianceMargin(margin); }
unsigned char Ax::getCcwComplianceMargin() const { return _impl->getCcwComplianceMargin(); }
void Ax::setCcwComplianceMargin(unsigned char margin) { _impl->setCcwComplianceMargin(margin); }
unsigned char Ax::getCwComplianceSlope() const { return _impl->getCwComplianceSlope(); }
void Ax::setCwComplianceSlope(unsigned char slope) { _impl->setCwComplianceSlope(slope); }
unsigned char Ax::getCcwComplianceSlope() const { return _impl->getCcwComplianceSlope(); }
void Ax::setCcwComplianceSlope(unsigned char slope) { _impl->setCcwComplianceSlope(slope); }

unsigned int Ax::getGoalPosition() const { return _impl->getGoalPosition(); }
void Ax::setGoalPosition(unsigned int pos) { _impl->setGoalPosition(pos); }
void Ax::setGoalPosition(unsigned int pos, unsigned int speed) { _impl->setGoalPosition(pos, speed); }
void Ax::setGoalPosition(unsigned int pos, unsigned int speed, unsigned int torqueLimit) { 
 _impl->setGoalPosition(pos, speed, torqueLimit); }

unsigned int Ax::getMovingSpeed() const { return _impl->getMovingSpeed(); }
void Ax::setMovingSpeed(unsigned int speed) { _impl->setMovingSpeed(speed); }

unsigned int Ax::getTorqueLimit() const { return _impl->getTorqueLimit(); }
void Ax::setTorqueLimit(unsigned int torque) { _impl->setTorqueLimit(torque); }

unsigned int Ax::getPresentPosition() const { return _impl->getPresentPosition(); }
void Ax::getPresentPosition(unsigned int &pos, unsigned int &speed, int &load) const { 
  _impl->getPresentPosition(pos, speed, load); }
unsigned int Ax::getPresentSpeed() const { return _impl->getPresentSpeed(); }
int Ax::getPresentLoad() const { return _impl->getPresentLoad(); }

bool Ax::isMoving() const { return _impl->isMoving(); }

unsigned int Ax::getPunch() const { return _impl->getPunch(); }
void Ax::setPunch(unsigned int punch) { _impl->setPunch(punch); }
    
bool Ax::isSensor() const { return _impl->isSensor(); }

unsigned char Ax::getObstacleDetectedCompareValue() const { return _impl->getObstacleDetectedCompareValue(); }
void Ax::setObstacleDetectedCompareValue(unsigned char val) { _impl->setObstacleDetectedCompareValue(val); }

unsigned char Ax::getLightDetectedCompareValue() const { return _impl->getLightDetectedCompareValue(); }
void Ax::setLightDetectedCompareValue(unsigned char val) { _impl->setLightDetectedCompareValue(val); }

unsigned char Ax::getIrDataLeft() const { return _impl->getIrDataLeft(); }
unsigned char Ax::getIrDataCenter() const { return _impl->getIrDataCenter(); }
unsigned char Ax::getIrDataRight() const { return _impl->getIrDataRight(); }
void Ax::getIrData(unsigned char &left, unsigned char &center, unsigned char &right) const { 
  left = _impl->getIrDataLeft();
  center = _impl->getIrDataCenter();
  right = _impl->getIrDataRight();
}

unsigned char Ax::getLuminosityLeft() const { return _impl->getLuminosityLeft(); }
unsigned char Ax::getLuminosityCenter() const { return _impl->getLuminosityCenter(); }
unsigned char Ax::getLuminosityRight() const { return _impl->getLuminosityRight(); }
void Ax::getLuminosity(unsigned char &left, unsigned char &center, unsigned char &right) const { 
  return _impl->getLuminosity(left, center, right);
}

bool Ax::isObstacleDetected() const { return _impl->isObstacleDetected(); }
bool Ax::isLuminosityDetected() const { return _impl->isLuminosityDetected(); }

unsigned char Ax::getSoundData() const { return _impl->getSoundData(); }
void Ax::setSoundData(unsigned char data) { _impl->setSoundData(data); }

unsigned char Ax::getSoundDataMaxHold() const { return _impl->getSoundDataMaxHold(); }
void Ax::setSoundDataMaxHold(unsigned char data) { _impl->setSoundDataMaxHold(data); }

unsigned char Ax::getSoundDetectedCount() const { return _impl->getSoundDetectedCount(); }
void Ax::setSoundDetectedCount(unsigned char cnt) { _impl->setSoundDetectedCount(cnt); }

unsigned int Ax::getSoundDetectedTime() const { return _impl->getSoundDetectedTime(); }
void Ax::setSoundDetectedTime(unsigned int time) { _impl->setSoundDetectedTime(time); }

unsigned char Ax::getBuzzerIndex() const { return _impl->getBuzzerIndex(); }
void Ax::setBuzzerIndex(unsigned char ind) { _impl->setBuzzerIndex(ind); }

unsigned char Ax::getBuzzerTime() const { return _impl->getBuzzerTime(); }
void Ax::setBuzzerTime(unsigned char time) { _impl->setBuzzerTime(time); }

bool Ax::hasIrRemoconArrived() { return _impl->hasIrRemoconArrived(); }
unsigned int Ax::getIrRemoconRx() { return _impl->getIrRemoconRx(); }
void Ax::setIrRemoconTx(unsigned int data) { _impl->setIrRemoconTx(data); }

unsigned char Ax::getObstableDetectedCompare() const { return _impl->getObstableDetectedCompare(); }
void Ax::setObstableDetectedCompare(unsigned char value) { _impl->setObstableDetectedCompare(value); }
  
unsigned char Ax::getLightDetectedCompare() const { return _impl->getLightDetectedCompare(); }
void Ax::setLightDetectedCompare(unsigned char value) { _impl->setLightDetectedCompare(value); }


} // namespace

std::ostream& operator<<(std::ostream& s, const dynapi::Ax &ax) {
  s << ax.getModelName() << " (sw rev. " << ax.getFirmwareVersion() 
    << ") at id " << static_cast<int>( ax.getId() ) 
    << ", delay " << ax.getReturnDelayTimeInUs() 
    << "us, bus speed " << ax.getBaudRateBps() << "bps";
  return s;
}

std::ostream& operator<<(std::ostream& s, const std::vector<dynapi::Ax> &axvec) {
  for (std::vector<dynapi::Ax>::const_iterator it=axvec.begin(); it != axvec.end(); ++it) {
    try {
      s<< *it << std::endl;
    } catch (dynapi::XDyn &x) {
      s<< x.what();
    }
  }
  return s;
}


// EOF
