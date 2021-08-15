/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2020 Innovation Academy for Microsatellites of CAS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Wang Junyong (wangjunyong@microsate.com)
 */

#ifndef COORDINATE_TURNTABLE_H
#define COORDINATE_TURNTABLE_H

#include <string>
#include "ns3/nstime.h"
#include "ns3/vector.h"
#include "adi-type-define.h"

namespace ns3 {

class CoordTurntable
{
public:
  friend class Turntable;

  /**
   * \brief Default constructor
   */
  CoordTurntable ();

  /**
   * \brief Copy constructor
   * \param turntable  the object to copy from
   */
  CoordTurntable (
    const CoordTurntable &turntable
  );

  /**
   * \brief Constructor
   * \param[in] pointing  the pointing of turntable
   */
  CoordTurntable (
    const adi::Pointing &pointing
  );

  /**
   * \brief Constructor
   * \param[in] azimuth       the azimuth angle, in radian
   * \param[in] azimuthRate   the azimuth angular rate, in radian
   * \param[in] pitch         the pitch angle, in radian
   * \param[in] pitchRate     the pitch angular rate, in radian
   */
  CoordTurntable (
    const double &azimuth,
    const double &azimuthRate,
    const double &pitch,
    const double &pitchRate
  );
  /**
   * \brief Deconstructor
   */
  ~CoordTurntable ();
  
  /**
   * \brief Copy constructor
   * \param turntable the object to copy from
   */
  CoordTurntable& operator= (const CoordTurntable &turntable);

  /**
   * \brief Output information string
   */
  std::string ToString (bool isDegree = true) const;
  
  /**
   * \brief Get the included angle between this turntable and given
   * \param[in] turntable the given coordinate system of turntable
   * \return the included angle, in radian
   */
  double GetIncludedAngle (const CoordTurntable &turntable) const;

  double GetAzimuth () const;
  double GetPitch () const;
  double GetAzimuthRate () const;
  double GetPitchRate () const;
private:
  static CoordTurntable ConvertTo (const Vector& position, const Vector& velocity);
  double m_azimuth;       //!< Azimuth, in radian
  double m_azimuthRate;   //!< Azimuth rate, in radian/second
  double m_pitch;         //!< Pitch, in radian
  double m_pitchRate;     //!< Pitch rate, in radian/second
};

std::ostream& operator<< (std::ostream& os, const CoordTurntable& coord);

}
#endif