/***************************************************************************
 * dynApi - Robotis Dynamixel c++ progamming api                           *
 *  Copyright (C) 2004,2005 by                                             *
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

#ifndef DYNAPI_TYPES_H
#define DYNAPI_TYPES_H
  
namespace dynapi {
  
  /**
  type for bitfilds of error flags.
  @code         ...
    dyn_err_type status;
    ...
    // test for overload error
    if (status & XDynStatusError::OVERLOAD) {
        std::cerr << "Overload error!" << std::endl;
    }
  @endcode
  @see XDynStatusError::INPUT_VOLTAGE, XDynStatusError::ANGLE_LIMIT, XDynStatusError::OVERHEATING, XDynStatusError::RANGE, XDynStatusError::CHECKSUM, XDynStatusError::OVERLOAD, XDynStatusError::INSTRUCTION,
  XDynStatusError::ALL
  */
  typedef unsigned char dyn_err_type;

}

#endif
