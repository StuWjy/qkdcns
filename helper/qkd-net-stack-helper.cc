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

#include "qkd-net-stack-helper.h"
#include "ns3/node.h"
#include "ns3/object.h"
#include "ns3/object-factory.h"
#include "ns3/qkd-satellite.h"
#include "ns3/qkd-satellite-container.h"
#include "ns3/qkd-station.h"
#include "ns3/qkd-station-container.h"
#include "ns3/fso-tx-device.h"
#include "ns3/fso-rx-device.h"
#include "ns3/q3p-l3-protocol.h"
#include "ns3/qnet-ipv4-l3-protocol.h"
#include "ns3/qkd-key-pool.h"
#include "ns3/ipv4.h"
#include "ns3/traffic-control-layer.h"
#include "ns3/udp-l4-protocol.h"
#include "ns3/packet-socket-factory.h"

namespace ns3 {

QkdNetStackHelper::QkdNetStackHelper ()
: m_routing (0)
{
  Initialize ();
}

QkdNetStackHelper::~QkdNetStackHelper ()
{
}

void
QkdNetStackHelper::Initialize ()
{
  Ipv4StaticRoutingHelper staticRouting;
  SetRoutingHelper (staticRouting);
}

void 
QkdNetStackHelper::SetRoutingHelper (const Ipv4RoutingHelper &routing)
{
  delete m_routing;
  m_routing = routing.Copy ();
}

void
QkdNetStackHelper::CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId)
{
  ObjectFactory factory;
  factory.SetTypeId (typeId);
  Ptr<Object> protocol = factory.Create <Object> ();
  node->AggregateObject (protocol);
}

void
QkdNetStackHelper::Install (Ptr<QkdSatellite> satellite) const
{
  if (satellite->GetObject<Ipv4> () != 0)
  {
    NS_FATAL_ERROR ("QkdNetStackHelper::Install (): Aggregating " 
                    "an InternetStack to a satellite with an existing Ipv4 object");
    return;
  }
  CreateAndAggregateObjectFromTypeId (satellite, "ns3::QnetIpv4L3Protocol");
  CreateAndAggregateObjectFromTypeId (satellite, "ns3::Q3pL3Protocol");
  CreateAndAggregateObjectFromTypeId (satellite, "ns3::Icmpv4L4Protocol");
  CreateAndAggregateObjectFromTypeId (satellite, "ns3::TrafficControlLayer");
  CreateAndAggregateObjectFromTypeId (satellite, "ns3::UdpL4Protocol");
  satellite->GetObject<TrafficControlLayer> ()->SetNode (satellite);
  Ptr<PacketSocketFactory> factory = CreateObject<PacketSocketFactory> ();
  satellite->AggregateObject (factory);
  Ptr<Ipv4> ipv4 = satellite->GetObject<Ipv4> ();
  Ptr<Ipv4RoutingProtocol> ipv4Routing = m_routing->Create (satellite);
  ipv4->SetRoutingProtocol (ipv4Routing);
}

void
QkdNetStackHelper::Install (Ptr<QkdStation> station) const
{
  if (station->GetObject<Ipv4> () != 0)
  {
    NS_FATAL_ERROR ("QkdNetStackHelper::Install (): Aggregating " 
                    "an InternetStack to a station with an existing Ipv4 object");
    return;
  }

  CreateAndAggregateObjectFromTypeId (station, "ns3::QnetIpv4L3Protocol");
  CreateAndAggregateObjectFromTypeId (station, "ns3::Q3pL3Protocol");
  CreateAndAggregateObjectFromTypeId (station, "ns3::Icmpv4L4Protocol");
  CreateAndAggregateObjectFromTypeId (station, "ns3::TrafficControlLayer");
  CreateAndAggregateObjectFromTypeId (station, "ns3::UdpL4Protocol");
  station->GetObject<TrafficControlLayer> ()->SetNode (station);
  Ptr<PacketSocketFactory> factory = CreateObject<PacketSocketFactory> ();
  station->AggregateObject (factory);
  Ptr<Ipv4> ipv4 = station->GetObject<Ipv4> ();
  Ptr<Ipv4RoutingProtocol> ipv4Routing = m_routing->Create (station);
  ipv4->SetRoutingProtocol (ipv4Routing);
}

void
QkdNetStackHelper::Install (QkdSatelliteContainer c) const
{
  for (QkdSatelliteContainer::Iterator i = c.Begin (); i != c.End (); ++i)
  {
    Install (*i);
  }
}

void
QkdNetStackHelper::Install (QkdStationContainer c) const
{
  for (QkdStationContainer::Iterator i = c.Begin (); i != c.End (); ++i)
  {
    Install (*i);
  }
}


}