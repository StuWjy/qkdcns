/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2021 Innovation Academy for Microsatellites of CAS
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

#include "constellation-helper.h"

namespace ns3 {

uint32_t
ConstellationHelper::GetPlanes (void) const
{
  return m_plane.size ();
}

uint32_t
ConstellationHelper::GetNumber (void) const
{
  uint32_t number = 0;
  for (const QkdSatelliteContainer& c : m_plane)
  {
    number += c.GetN ();
  }
  return number;
}

void
ConstellationHelper::AddPlane (const QkdSatelliteContainer& plane)
{
  m_plane.push_back (plane);
}

QkdSatelliteContainer
ConstellationHelper::DistributeEvenly (
  double sma,   double ecc, double inc,
  double raan,  double aop, double ma,
  uint32_t num, bool isDegree
)
{
  QkdSatelliteContainer c;
  c.Create (num);
  for (uint32_t i = 0;i < num;++i)
  {
    c[i]->GetAdiSat ()->SetElement (sma, ecc, inc, raan, aop, ma, isDegree);
    ma += 2.0 * M_PI / num;
  }
  m_plane.push_back (c);
  return c;
}

QkdSatelliteContainer
ConstellationHelper::GetPlane (uint32_t plane) const
{
  if (plane < GetPlanes ())
  {
    return m_plane[plane];
  }
  else
  {
    NS_ASSERT (false);
    return QkdSatelliteContainer ();
  }
}

}