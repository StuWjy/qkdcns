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
#include "ns3/math.h"
#include "ns3/vector.h"
#include "ns3/output-stream-wrapper.h"
#include "ns3/util.h"
#include "ns3/simulator.h"
#include "coordinate-turntable.h"

namespace ns3 {

CoordTurntable::CoordTurntable ()
: m_azimuth     (0.0)
, m_azimuthRate (0.0)
, m_pitch       (0.0)
, m_pitchRate   (0.0)
{
}

CoordTurntable::CoordTurntable (
  const double &azimuth,
  const double &azimuthRate,
  const double &pitch,
  const double &pitchRate
  )
: m_azimuth     (azimuth)
, m_azimuthRate (azimuthRate)
, m_pitch       (pitch)
, m_pitchRate   (pitchRate)
{
}
CoordTurntable::~CoordTurntable ()
{
}

CoordTurntable::CoordTurntable (const CoordTurntable &turntable)
: m_azimuth     (turntable.m_azimuth)
, m_azimuthRate (turntable.m_azimuthRate)
, m_pitch       (turntable.m_pitch)
, m_pitchRate   (turntable.m_pitchRate)
{
}

CoordTurntable::CoordTurntable (const adi::Pointing &pointing)
: m_azimuth     (pointing.angle.azimuth)
, m_azimuthRate (pointing.rate.azimuth)
, m_pitch       (pointing.angle.pitch)
, m_pitchRate   (pointing.rate.pitch)
{
}

CoordTurntable&
CoordTurntable::operator= (const CoordTurntable &turntable)
{
  if (this == &turntable)
  {
    return *this;
  }
  m_azimuth     = turntable.m_azimuth;
  m_azimuthRate = turntable.m_azimuthRate;
  m_pitch       = turntable.m_pitch;
  m_pitchRate   = turntable.m_pitchRate;
  return *this;
}

std::string
CoordTurntable::ToString (bool isDegree) const
{
  std::stringstream ss;
  std::ostringstream az, pi;
  std::ostringstream azRate, piRate;
  // os << "--------------------------------------------------------------------------------" << std::endl;
  if (isDegree)
  {
    // os << "Azimuth(deg)    rate(deg/sec)   Pitch(deg)      rate(deg/sec)      " << std::endl;
    az << RadianToDegree (m_azimuth);
    pi << RadianToDegree (m_pitch);
    azRate << RadianToDegree (m_azimuthRate);
    piRate << RadianToDegree (m_pitchRate);
  }
  else
  {
    // os << "Azimuth(rad)    rate(rad/sec)   Pitch(rad)      rate(rad/sec)      " << std::endl;
    az << m_azimuth;
    pi << m_pitch;
    azRate << m_azimuthRate;
    piRate << m_pitchRate;
  }
  ss << std::setw (32) << ToTime (Now ())
     << "|"
     << std::setiosflags (std::ios::right | std::ios::fixed | std::ios::showpos) << std::setprecision(3) << std::setw (12) << az.str ()
     << "|"
     << std::setiosflags (std::ios::right | std::ios::fixed | std::ios::showpos) << std::setprecision(3) << std::setw (12) << azRate.str ()
     << "|"
     << std::setiosflags (std::ios::right | std::ios::fixed | std::ios::showpos) << std::setprecision(3) << std::setw (12) << pi.str ()
     << "|"
     << std::setiosflags (std::ios::right | std::ios::fixed | std::ios::showpos) << std::setprecision(3) << std::setw (12) << piRate.str ()
     << "|";
  return ss.str ();
}

double
CoordTurntable::GetIncludedAngle (const CoordTurntable &turntable) const
{
  const double &p1 = m_pitch;
  const double &p2 = turntable.m_pitch;
  const double &a1 = m_azimuth;
  const double &a2 = turntable.m_azimuth;
  double cos_theta = sin (p1) * sin (p2) + cos (p1) * cos (p2) * cos (a1 - a2);
  cos_theta = std::min (1.0, std::max (-1.0, cos_theta));
  return acos (cos_theta);
}

double
CoordTurntable::GetAzimuth () const
{
  return m_azimuth;
}

double
CoordTurntable::GetPitch () const
{
  return m_pitch;
}

double
CoordTurntable::GetAzimuthRate () const
{
  return m_azimuthRate;
}

double
CoordTurntable::GetPitchRate () const
{
  return m_pitchRate;
}

CoordTurntable
CoordTurntable::ConvertTo (const Vector& position, const Vector& velocity)
{
  double x = position.x;
  double y = position.y;
  double z = position.z;
  double r = position.GetLength ();
  
  double vx = velocity.x;
  double vy = velocity.y;
  double vz = velocity.z;
  double azimuth = atan2 (y, x);
  double pitch = -atan (z / sqrt (x * x + y * y));

  double d = sqrt (x * x + y * y);
  double u = x * vx + y * vy;
  // double v = x * vx + y * vy + z * vz;
  double tmp0 = z * u - d * d * vz;
  double pitchRate = tmp0 / (r * r * d);
  double azimuthRate = (x * vy - y * vx) / (x * x + y * y);
  // double distanceRate = v / r;

  CoordTurntable coord (azimuth, azimuthRate, pitch, pitchRate);
  return coord;
}

std::ostream& operator<< (std::ostream& os, const CoordTurntable& coord)
{
  os << coord.ToString (true);
  return os;
}

}