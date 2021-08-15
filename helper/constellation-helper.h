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

#ifndef CONSTELLATION_HELPER_H
#define CONSTELLATION_HELPER_H

#include "ns3/qkd-satellite-container.h"


namespace ns3 {

class ConstellationHelper
{
public:
  ConstellationHelper (){}
  ~ConstellationHelper (){}
  uint32_t GetPlanes (void) const;
  uint32_t GetNumber (void) const;
  void AddPlane (const QkdSatelliteContainer& plane);
  QkdSatelliteContainer DistributeEvenly (
    double sma,   double ecc, double inc,
    double raan,  double aop, double ma,
    uint32_t num, bool isDegree = true
  );
  QkdSatelliteContainer GetPlane (uint32_t plane) const;
private:
  std::vector<QkdSatelliteContainer> m_plane;
};

}

#endif /* CONSTELLATION_HELPER_H */