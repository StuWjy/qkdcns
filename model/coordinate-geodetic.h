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

#ifndef COORDINATE_GEODETIC_H
#define COORDINATE_GEODETIC_H

#include <string>
#include "ns3/ptr.h"

namespace ns3 {

class CoordGeodetic
{
public:
  /**
   * \brief Default constructor
   */
  CoordGeodetic ();

  /**
   * \brief Constructor
   * \param lat the latitude (default: radians)
   * \param lon the longitude (default: radians)
   * \param alt the altitude in kilometers
   * \param isDegree whether the latitude and longitude is in degree
   */
  CoordGeodetic (double lat, double lon, double alt, bool isDegree = false);

  /**
   * \brief Copy constructor
   * \param geo the object to copy from
   */
  CoordGeodetic (const CoordGeodetic &geo);

  /**
   * \brief Assignment operator
   * \param geo the object to copy from
   */
  CoordGeodetic& operator= (const CoordGeodetic &geo);

  /**
   * \brief Output information string
   * \return the string
   */
  std::string ToString () const;
  double m_latitude;  //!< latitude of geodetic, in radians
  double m_longitude; //!< longitude of geodetic, in radians
  double m_altitude;  //!< altitude of geodetic, in kilometers
};

std::ostream& operator<< (std::ostream& os, const CoordGeodetic& geo);

}

#endif