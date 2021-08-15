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

#ifndef QKD_NET_STACK_HELPER_H
#define QKD_NET_STACK_HELPER_H

#include "ns3/ptr.h"
#include "ns3/ipv4-static-routing-helper.h"

namespace ns3 {

class Node;
class QkdSatellite;
class QkdSatelliteContainer;
class QkdStation;
class QkdStationContainer;

class QkdNetStackHelper
{
public:
  QkdNetStackHelper ();
  ~QkdNetStackHelper ();
  void Initialize ();
  void SetRoutingHelper (const Ipv4RoutingHelper &routing);
  static void CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId);
  void Install (QkdSatelliteContainer c) const;
  void Install (QkdStationContainer c) const;
  void Install (Ptr<Node> node) const;
  void Install (Ptr<QkdSatellite> satellite) const;
  void Install (Ptr<QkdStation> station) const;
private:
  const Ipv4RoutingHelper *m_routing;
};

}

#endif /* QKD_NET_STACK_HELPER_H */