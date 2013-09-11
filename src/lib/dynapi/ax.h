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

#ifndef DYNAPI_AX_H
#define DYNAPI_AX_H

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "types.h"


namespace dynapi {

class DynInterface;

class AxImpl;

/** Ax represents a Dynamixel AX device.
Ax is the base class for communication with Dynamixel AX devices. 

Currently the implementation is based on synchronous communication with 
the device (i.e. each write will wait for the communication to be completed 
and also waits for response of the device. Note that this may change in
future releases.

Ax corrently provides functionality for AX-12 actuator and AX-S1 sensor.
Both share a set of common methods (e.g. concerning ID, baud rate, etc.), but
offer a set of methods only available one of them. Methods Ax::isActuator() 
and Ax::isSensor() can be used to find out about the abilities of the device.
Calling methods which are not supported by the device result in the exception
XDynNotSupported().

Ax is a lightwight frontend implementation which can be copied fast and 
efficiently. It releys on boost::shared_ptr to maintain pointers to a backend
implementation which differs for each device type to be supported. An instance
of the backend will be constructed upon device creation, i.e. by 
DynInterface::scanBus() and will be automatically destructed when the last
instance of boost::shared_ptr which points to it is destructed.
The implementation may also hold a shared_ptr to the instance of the interface
which won't be destructed until the last instance of an Ax device using the
interface is gone. In short: Users don't have to care about destruction of instances
of Ax or DynInterface!

@note Some of the settings made are persistent, i.e. stored to the eeporm of the
device and restored after poweron.
*/
class Ax
{
friend class DynInterface;
friend class DynInterfaceCM5PosixSerial;

protected:
  
  /**
   * ctor only available to DynInterface classes.
   * @param host shared_ptr to host
   * @param id id of device
   */
  Ax(boost::shared_ptr<DynInterface> host, unsigned char id);

public:
  /**
   * virtual dtor.
   */
  virtual ~Ax();

  
  /** @name Base functionality of AX devices.
   * Currently AX-12 actuator and AX-S1 sensor module are known to the 
   * developers. This is the common base functionality for these devices,
   * future devices may not support some methods, which will result in
   * the exception XDynNotSupported() been thrown.
   */
  //@{
  /**
   * read model number.
   * Default is 12 for AX-12 and 13 for AX-S1.
   * @return model number.
   * @see getModelName
   */
  virtual unsigned int getModelNumber() const;
  /**
   * read model name. 
   * Uses getModelNumber to read model number and
   * returns the name of the device for known model
   * numbers. For unknown model numbers it returns
   * a string containing "AX-unknown" and a string
   * representation of the model number.
   * @return model name
   */
  virtual std::string getModelName() const;
  
  /**
   * read version of firmware.
   * @return version of firmware 
   */
  virtual unsigned int getFirmwareVersion() const;

  /**
   * read ID of device.
   * default factory setting: 1
   * @return ID of device
   * @note persistent (eeprom)
   */
  virtual unsigned char getId() const;
  /**
   * set ID of the device.
   * @param id ID, allowed range: 0..253; default factory setting: 1
   * @note persistent (eeprom)
   */
  virtual void setId(unsigned char id);
  
  /**
   * read baud rate of device.
   * @return baud rate in bits per second.
   @ see getBaudRateRaw(), setBaudRateBps()
   */
  virtual float getBaudRateBps() const;
  /**
   * set baud rate of device.
   * It may not be possible to set baud rate exactly, because
   * setBaudRateRaw does only allow discrete steps. BPS will be
   * bps = 2000000 / (getBaudRateRaw() + 1).
   * @param bps baud rate in bits per second.
   * @see setBaudRateRaw(), getBaudRateBps()
   * @note persistent (eeprom)
   */
  virtual void setBaudRateBps(float bps);
  /**
   * read baud rate of device.
   * setBaudRateBps returns a raw value which translates to
   * bits per second:
   * bps = 2000000 / (getBaudRateRaw() + 1).
   * Default: 1.
   * @return raw value stored in eeprom.
   * @see getBaudRateBps(), setBaudRateRaw()
   * @note persistent (eeprom)
   */
  virtual unsigned char getBaudRateRaw() const;
  /**
   * set baud rate of device.
   * bps = 2000000 / (baudRate + 1).
   * There are predefined constants for common baud
   * rates: BAUD_RATE_RAW_...
   * @param baudRate raw value.
   * @see setBaudRateBps(), getBaudRateRaw(), 
   * BAUD_RATE_RAW_DEFAULT, BAUD_RATE_RAW_1M, BAUD_RATE_RAW_500K, 
   * BAUD_RATE_RAW_400K, BAUD_RATE_RAW_250K, BAUD_RATE_RAW_200K,
   * BAUD_RATE_RAW_115K2, BAUD_RATE_RAW_57K6, BAUD_RATE_RAW_19K2,
   * BAUD_RATE_RAW_9K6
   * @note persistent (eeprom)
   */
  virtual void setBaudRateRaw(unsigned char baudRate);

  /**
   * get delay between instruction packet and status packet.
   * The device may wait some time after execution of instructions
   * to send a status packet.
   * @return time in microseconds. Max: 512, default: 0.
   * @note persistent (eeprom)
   */
  virtual unsigned int getReturnDelayTimeInUs() const;
  /**
   * set delay between instruction packet and status packet.
   * The device may wait some time after execution of instructions
   * to send a status packet.
   * @param us time in microseconds. Max: 512, default: 0.
   * @note persistent (eeprom)
   */
  virtual void setReturnDelayTimeInUs(unsigned int us);
  
  
  /**
   * get temperature limit.
   * @return temperature in Degrees Celsius. Default: 85.
   * @note persistent (eeprom)
   */
  virtual unsigned char getHighTemperatureLimit() const;
  /**
   * set temperature limit.
   * If limit is reached during operation, device will 
   * throw XDynStatusError with XDynStatusError::OVERHEATING
   * set; other actions may be taken, see setAlarmLed() and 
   * setAlarmShutdown().
   * @param temp temperature in Degrees Celsius. Max: 150, default: 85.
   * @note persistent (eeprom)
   */
  virtual void setHighTemperatureLimit(unsigned char temp);
  
  /**
   * get lower voltage limit.
   * @return lower voltage limit in V*10, that is 80 is 8 Volts.
   * @note persistent (eeprom)
   */
  virtual unsigned char getLowVoltageLimit() const;
  /**
   * set lower voltage limit.
   * If limit is reached during operation, device will 
   * throw XDynStatusError with XDynStatusError::INPUT_VOLTAGE
   * set; other actions may be taken, see setAlarmLed() and 
   * setAlarmShutdown().
   * @param voltage lower voltage limit in V*10, that is 80 is 8 Volts.
   * @note persistent (eeprom)
   */
  virtual void setLowVoltageLimit(unsigned char voltage);

  /**
   * get upper voltage limit.
   * @return upper voltage limit in V*10, that is 80 is 8 Volts.
   * @note persistent (eeprom)
   */
  virtual unsigned char getHighVoltageLimit() const;
  /**
   * set upper voltage limit.
   * If limit is reached during operation, device will 
   * throw XDynStatusError with XDynStatusError::INPUT_VOLTAGE
   * set; other actions may be taken, see setAlarmLed() and 
   * setAlarmShutdown().
   * @param voltage upper voltage limit in V*10, that is 80 is 8 Volts.
   * @note persistent (eeprom)
   */
  virtual void setHighVoltageLimit(unsigned char voltage);

  /// should sensor return status packets?
  enum status_return_level {
    SRL_NONE,      ///< do not sent status
    SRL_READ_DATA, ///< sent status after instruction READ_DATA (note: not for broadcasts, that is id 254)
    SRL_ALL        ///< sent status atfer all instructions (note: not for broadcasts, that is id 254)
  };

  /**
   * get status return settings.
   * @see status_return_level, setStatusReturnLevel
   * @return settings.
   */
  virtual status_return_level getStatusReturnLevel() const;
  /**
   * set status return settings.
   * @note CURRENTLY only SRL_ALL is supported. SRL_NONE and SRL_READ_DATA will make dynapi parser fail.
   * @param status settings
   * @see status_return_level
   * @note persistent (eeprom)
   */
  virtual void setStatusReturnLevel(status_return_level status);

  /**
   * get current voltage.
   * @return voltage limit in V*10, that is 80 is 8 Volts.
   */
  virtual unsigned char getPresentVoltage() const;
  /**
   * get current temperature.
   * @return temperature in Degrees Celsius.
   */
  virtual unsigned char getPresentTemperature() const;
  
  /**
   * test if device has been prepared for delayed execution of an instruction.
   * @return true, if execution of instruction is pending.
   */
  virtual bool getRegisteredInstruction() const;
  /**
   * cancel a delayed execution of an instruction (using parameter "false").
   * @return false.
   */
  virtual void setRegisteredInstruction(bool ins);
  
  /**
   * test if device is locked.
   * If locked, only the following setter methods can be used:
   * setTorqueEnabled(), setLed(), setCwComplianceMargin(), setCwComplianceSlope(),
   * setGoalPosition(), setMovingSpeed(), setTorqueLimit() 
   * / setObstacleDetectedCompareValue(), setLightDetectedCompareValue()
   * @note once locked, it can only be undone by poweroff.
   * @return true if device is locked.
   */
  virtual bool isLocked() const;
  
  /** 
   * lock device.
   * If locked, only the following setter methods can be used:
   * setTorqueEnabled(), setLed(), setCwComplianceMargin(), setCwComplianceSlope(),
   * setGoalPosition(), setMovingSpeed(), setTorqueLimit() 
   * / setObstacleDetectedCompareValue(), setLightDetectedCompareValue()
   * @note once locked, it can only be undone by poweroff.
   */
  virtual void setLock();
  
  static const int BAUD_RATE_RAW_DEFAULT; ///< baud rate: default, 1000000 bps.
  static const int BAUD_RATE_RAW_1M;      ///< baud rate 1000000 bps.
  static const int BAUD_RATE_RAW_500K;    ///< baud rate  500000 bps.
  static const int BAUD_RATE_RAW_400K;    ///< baud rate  400000 bps.
  static const int BAUD_RATE_RAW_250K;    ///< baud rate  250000 bps.
  static const int BAUD_RATE_RAW_200K;    ///< baud rate  200000 bps.
  static const int BAUD_RATE_RAW_115K2;   ///< baud rate  115200 bps (true bps will be 117647 bps).
  static const int BAUD_RATE_RAW_57K6;    ///< baud rate   57600 bps (true bps will be  57143 bps).
  static const int BAUD_RATE_RAW_19K2;    ///< baud rate   19200 bps (true bps will be  19231 bps).
  static const int BAUD_RATE_RAW_9K6;     ///< baud rate    9600 bps (true bps will be   9615 bps).
  //@}
  
  /** @name AX-12.
   * The documentation for this device is incomplete or could be improved.
   * See the manual at 
   * http://www.crustcrawler.com/products/bioloid/docs/AX-12.pdf
   * for more information.
   *
   * This group of methods is particularly intended for AX-12 devices; other
   * devices may not support this operation and throw XDynNotSupported().
   */
  //@{
  virtual bool isActuator() const; ///< returns true if device is an actuator (that is true for AX-12).
  
  /**
   * get angle limit for clockwise rotation.
   * Default: 0
   * @return angle limit
   */
  virtual unsigned int getCwAngleLimit() const;
  /**
   * set angle limit for clockwise rotation.
   * In "normal" mode, servo can move 300 degrees between 5 o'clock (which equals a value 0) 
   * counter-clockwise to 7 o'clock (which is a value of 1023). 
   * In endless turn mode (servo works like a motor), setCwAngleLimit() and setCcwAngleLimit()
   * must be set to 0 (see manual).
   * Default: 0; max: 1023.
   * @param limit limit to set
   * @note persistent (eeprom)
   */
  virtual void setCwAngleLimit(unsigned int limit);
  
  /**
   * get angle limit for counter-clockwise rotation.
   * Default: 1023
   * @return angle limit
   * @note persistent (eeprom)
   */
  virtual unsigned int getCcwAngleLimit() const;
  /**
   * set angle limit for counter-clockwise rotation.
   * In "normal" mode, servo can move 300 degrees between 5 o'clock (which equals a value 0) 
   * counter-clockwise to 7 o'clock (which is a value of 1023). 
   * In endless turn mode (servo works like a motor), setCwAngleLimit() and setCcwAngleLimit()
   * must be set to 0 (see manual).
   * Default: 0; max: 1023.
   * @note persistent (eeprom)
   * @param limit limit to set
   */
  virtual void setCcwAngleLimit(unsigned int limit);
  
  /**
   * get max torque after poweron.
   * This is not the value the present torque is compared against but the default value
   * for the torque limit after poweron. See setTorqueLimit()/getTorqueLimit().
   * @return torque. Default: 1023, Max: 1023.
   * @see getTorqueLimit()
   * @note persistent (eeprom)
   */
  virtual unsigned int getMaxTorque() const;
  /**
   * set max torque after poweron.
   * This is not the value the present torque is compared against but the default value
   * for the torque limit after poweron. See setTorqueLimit()/getTorqueLimit().
   * @see getTorqueLimit()
   * @note persistent (eeprom)
   * @param torque torque after poweron. Default: 1023, Max: 1023.
   * @note persistent (eeprom)
   */
  virtual void setMaxTorque(unsigned int torque);  
  
  /**
   * get bitfield that specifies on which error conditions the LED will blink.
   * @return bitfield.
   * @note persistent (eeprom)
   */
  virtual dyn_err_type getAlarmLed() const;
  /**
   * set bitfield that specifies on which error conditions the LED will blink.
   * @param status bitfield.
   * @note persistent (eeprom)
   */
  virtual void setAlarmLed(dyn_err_type status);
  
  /**
   * get bitfield that specifies on which error conditions torque will be shut down.
   * @note to recover from shutdown call setTorqueEnabled()
   * @return bitfield
   * @note persistent (eeprom)
   */
  virtual dyn_err_type getAlarmShutdown() const;
  /**
   * set bitfield that specifies on which error conditions torque will be shut down.
   * @note to recover from shutdown call setTorqueEnabled()
   * @param status bitfield
   * @note persistent (eeprom)
   */
  virtual void setAlarmShutdown(dyn_err_type status);
  
  /**
   * internal calibration data. cannot be changed by user.
   * @return data.
   * @note persistent (eeprom)
   */
  virtual unsigned int getDownCalibration() const;
  /**
   * internal calibration data. cannot be changed by user.
   * @return data.
   * @note persistent (eeprom)
   */
  virtual unsigned int getUpCalibration() const;

  /**
   * test if torque is enabled.
   * @return true if torque is enabled, false for free spin mode.
   */
  virtual bool isTorqueEnabled() const;
  /**
   * enable/disable torque.
   * @param enable true to enable torque, false for free spin mode
   */
  virtual void setTorqueEnabled(bool enable);
  
  /**
   * get status of LED.
   * @return true if LED is switched on.
   */
  virtual bool isLedEnabled() const;
  /**
   * switch LED on or off
   * @param led true to enable
   */
  virtual void setLed(bool led);
  
   
  
  /**
   * define dynamics.
   * See the manual at 
   * http://www.crustcrawler.com/products/bioloid/docs/AX-12.pdf
   * for more information.
   */
  virtual unsigned char getCwComplianceMargin() const;
  /**
   * define dynamics.
   * See the manual at 
   * http://www.crustcrawler.com/products/bioloid/docs/AX-12.pdf
   * for more information.
   */
  virtual void setCwComplianceMargin(unsigned char margin);
  /**
   * define dynamics.
   * See the manual at 
   * http://www.crustcrawler.com/products/bioloid/docs/AX-12.pdf
   * for more information.
   */
  virtual unsigned char getCcwComplianceMargin() const;
  /**
   * define dynamics.
   * See the manual at 
   * http://www.crustcrawler.com/products/bioloid/docs/AX-12.pdf
   * for more information.
   */
  virtual void setCcwComplianceMargin(unsigned char margin);
  /**
   * define dynamics.
   * See the manual at 
   * http://www.crustcrawler.com/products/bioloid/docs/AX-12.pdf
   * for more information.
   */
  virtual unsigned char getCwComplianceSlope() const;
  /**
   * define dynamics.
   * See the manual at 
   * http://www.crustcrawler.com/products/bioloid/docs/AX-12.pdf
   * for more information.
   */
  virtual void setCwComplianceSlope(unsigned char slope);
  /**
   * define dynamics.
   * See the manual at 
   * http://www.crustcrawler.com/products/bioloid/docs/AX-12.pdf
   * for more information.
   */
  virtual unsigned char getCcwComplianceSlope() const;
  /**
   * define dynamics.
   * See the manual at 
   * http://www.crustcrawler.com/products/bioloid/docs/AX-12.pdf
   * for more information.
   */
  virtual void setCcwComplianceSlope(unsigned char slope);
  
  /**
   * get goal position.
   * In "normal" mode, servo can move 300 degrees between 5 o'clock (which equals a value 0) 
   * counter-clockwise to 7 o'clock (which is a value of 1023). 
   *
   * If limit is reached during operation, device will 
   * throw XDynStatusError with XDynStatusError::ANGLE_LIMIT
   * set; other actions may be taken, see setAlarmLed() and 
   * setAlarmShutdown().
   * @return pos
   */
  virtual unsigned int getGoalPosition() const;
  /**
   * set goal position.
   * In "normal" mode, servo can move 300 degrees between 5 o'clock (which equals a value 0) 
   * counter-clockwise to 7 o'clock (which is a value of 1023). 
   *
   * If limit is reached during operation, device will throw 
   * XDynStatusError; other actions may be taken, see setAlarmLed() and 
   * setAlarmShutdown().
   * @param pos position
   */  
  virtual void setGoalPosition(unsigned int pos);
  /**
   * get goal position and speed.
   * In "normal" mode, servo can move 300 degrees between 5 o'clock (which equals a value 0) 
   * counter-clockwise to 7 o'clock (which is a value of 1023). 
   *
   * If limit is reached during operation, device will throw 
   * XDynStatusError; other actions may be taken, see setAlarmLed() and 
   * setAlarmShutdown().
   * @param pos position (0..1023)
   * @param speed speed  (0..1023)
   * @see setMovingSpeed()
   */  
  virtual void setGoalPosition(unsigned int pos, unsigned int speed);
  /**
   * get goal position and speed.
   * In "normal" mode, servo can move 300 degrees between 5 o'clock (which equals a value 0) 
   * counter-clockwise to 7 o'clock (which is a value of 1023). 
   *
   * If limit is reached during operation, device will throw 
   * XDynStatusError; other actions may be taken, see setAlarmLed() and 
   * setAlarmShutdown().
   * @param pos position (0..1023)
   * @param speed speed  (0..1023)
   * @param torqueLimit torque limit (0..1023)
   * @see setMovingSpeed(), setTorqueLimit();
   */  
  virtual void setGoalPosition(unsigned int pos, unsigned int speed, unsigned int torqueLimit);
  
  /**
   * get default speed.
   * @return default speed, used by setGoalPosition(pos)
   */
  virtual unsigned int getMovingSpeed() const;
  /**
   * set default speed.
   * @param speed default speed, used by setGoalPosition(pos)
   */
  virtual void setMovingSpeed(unsigned int speed);
  
  /**
   * get torque limit.
   * @return torque limit.
   * @see setMaxTorque()
   */
  virtual unsigned int getTorqueLimit() const;
  /**
   * set torque limit.
   * @param torque torque limit.
   */
  virtual void setTorqueLimit(unsigned int torque);
  
  /**
   * get current position.
   * servo can move 300 degrees between 5 o'clock (which equals a value 0) 
   * counter-clockwise to 7 o'clock (which is a value of 1023). 
   * @return current position.
   * @see setGoalPosition
   */
  virtual unsigned int getPresentPosition() const;
  /**
   * get current position, speed, load.
   * @param pos position
   * @param speed speed
   * @param load load.
   * @see getPresentPostion(void), getPresentSpeed(), getPresentLoad()
   */
  virtual void getPresentPosition(unsigned int &pos, unsigned int &speed, int &load) const;
  /**
   * get current speed.
   * @return speed.
   */
  virtual unsigned int getPresentSpeed() const;
  /**
   * get current load, that is force exerted on device.
   * The value returned is signed. We stick to the mathematical notation of rotation, i.e.
   * positive rotation is counter-clockwise. Thus, load in counter-clockwise direction is 
   * positive, load in clockwise direction is negative.
   * @note makes only sense if torque currently is enabled (setTorqueEnable)
   * @return current load
   */
  virtual int getPresentLoad() const;
  
  /**
   * returns true if the servo is currently moving.
   * @return true if the servo is currently moving.
   */
  virtual bool isMoving() const;
  
  /**
   * get minimum current delivered to motor.
   * Initial value: 32, Max. 1023.
   * @return minimum current delivered to motor.
   */
  virtual unsigned int getPunch() const;
  /**
   * set minimum current delivered to motor.
   * Initial value: 32, Max. 1023.
   * @param punch minimum current delivered to motor.
   */
  virtual void setPunch(unsigned int punch);
//@}
    
  /** @name AX-S1 Sensor Module.
   * Sorry, documentation is still missing.
   * See the manual at http://www.crustcrawler.com/products/bioloid/docs/AX-S1.pdf 
   * for more information.
   *
   * This group of methods is particularly intended for AX-S1 devices; other
   * devices may not support this operation and throw XDynNotSupported().
   */
  //@{
  virtual bool isSensor() const; ///< returns true if device is a sensor (that is true for AX-S1).
  
  /**
   * obstacle detected compare value. 
   * @return value.
   * @note persistent (eeprom)
   */
  virtual unsigned char getObstacleDetectedCompareValue() const;
  /**
   * obstacle detected compare value. 
   * @param val value
   * @note persistent (eeprom)
   */
  virtual void setObstacleDetectedCompareValue(unsigned char val);
  
  /**
   * light detected compare value. 
   * @return value.
   * @note persistent (eeprom)
   */
  virtual unsigned char getLightDetectedCompareValue() const;
  /**
   * light detected compare value. 
   * @return value.
   * @note persistent (eeprom)
   */
  virtual void setLightDetectedCompareValue(unsigned char val);
  
  virtual unsigned char getIrDataLeft() const;
  virtual unsigned char getIrDataCenter() const;
  virtual unsigned char getIrDataRight() const;
  virtual void getIrData(unsigned char &left, unsigned char &center, unsigned char &right) const;
  
  virtual unsigned char getLuminosityLeft() const;
  virtual unsigned char getLuminosityCenter() const;
  virtual unsigned char getLuminosityRight() const;
  virtual void getLuminosity(unsigned char &left, unsigned char &center, unsigned char &right) const;
  
  virtual bool isObstacleDetected() const;
  virtual bool isLuminosityDetected() const;
  
  virtual unsigned char getSoundData() const;
  virtual void setSoundData(unsigned char data);
  
  virtual unsigned char getSoundDataMaxHold() const;
  virtual void setSoundDataMaxHold(unsigned char data);
  
  virtual unsigned char getSoundDetectedCount() const;
  virtual void setSoundDetectedCount(unsigned char cnt);
  
  virtual unsigned int getSoundDetectedTime() const;
  virtual void setSoundDetectedTime(unsigned int time);
  
  virtual unsigned char getBuzzerIndex() const;
  virtual void setBuzzerIndex(unsigned char ind);

  virtual unsigned char getBuzzerTime() const;
  virtual void setBuzzerTime(unsigned char time);

  virtual bool hasIrRemoconArrived();
  virtual unsigned int getIrRemoconRx();
  virtual void setIrRemoconTx(unsigned int data);
  
  virtual unsigned char getObstableDetectedCompare() const;
  virtual void setObstableDetectedCompare(unsigned char value);
    
  virtual unsigned char getLightDetectedCompare() const;
  virtual void setLightDetectedCompare(unsigned char value);
  //@}
  

private:
  boost::shared_ptr<AxImpl> _impl;
};

} // namespace

/// easy way to print information about interface to a stream
std::ostream& operator<<(std::ostream& s, const dynapi::Ax &ax);
std::ostream& operator<<(std::ostream& s, const std::vector<dynapi::Ax> &axvec);

#endif
