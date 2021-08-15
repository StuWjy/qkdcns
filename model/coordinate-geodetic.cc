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

#include <sstream>
#include <iomanip>
#include "ns3/log.h"
#include "ns3/output-stream-wrapper.h"
#include "util.h"
#include "coordinate-geodetic.h"


namespace ns3 {

CoordGeodetic::CoordGeodetic ()
: m_latitude (0.0)
, m_longitude (0.0)
, m_altitude (0.0)
{
}

CoordGeodetic::CoordGeodetic (double lat, double lon, double alt, bool isDegree)
: m_latitude (lat)
, m_longitude (lon)
, m_altitude (alt)
{
  if (isDegree)
  {
    m_latitude = DegreeToRadian (m_latitude);
    m_longitude = DegreeToRadian (m_longitude); 
  }
}

CoordGeodetic::CoordGeodetic (const CoordGeodetic &geo)
{
  m_latitude = geo.m_latitude;
  m_longitude = geo.m_longitude;
  m_altitude = geo.m_altitude;
}

CoordGeodetic&
CoordGeodetic::operator = (const CoordGeodetic &geo)
{
  if (this == &geo)
  {
    return *this;
  }
  m_latitude = geo.m_latitude;
  m_longitude = geo.m_longitude;
  m_altitude = geo.m_altitude;
  return *this;
}

std::string
CoordGeodetic::ToString () const
{
  std::stringstream ss;
  ss << "   Latitude(deg)  Longitude(deg)    Altitude(km)" << std::endl;
  ss << std::setiosflags (std::ios::right | std::ios::fixed | std::ios::showpos) << std::setprecision(4) << std::setw (16) << m_latitude;
  ss << std::setiosflags (std::ios::right | std::ios::fixed | std::ios::showpos) << std::setprecision(4) << std::setw (16) << m_longitude;
  ss << std::setiosflags (std::ios::right | std::ios::fixed | std::ios::showpos) << std::setprecision(4) << std::setw (16) << m_altitude;
  ss << std::endl;
  return ss.str ();
}

std::ostream&
operator<< (std::ostream& os, const CoordGeodetic& geo)
{
  os << geo.ToString ();
  return os;
}

}