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

#include "qkd-protocol-helper.h"
#include "ns3/space-point-to-point-net-device.h"
#include "ns3/qkd-satellite.h"
#include "ns3/qkd-station.h"
#include "ns3/fso-tx-device.h"
#include "ns3/fso-rx-device.h"

namespace ns3 {

QkdProtocolHelper::QkdProtocolHelper ()
{
  m_qkdProtocolFactory.SetTypeId ("ns3::QkdProtocolBB84TwoDecoy");
}

QkdProtocolHelper::~QkdProtocolHelper ()
{
}

void
QkdProtocolHelper::SetQkdProtocolAttribute (std::string name, const AttributeValue &value)
{
  m_qkdProtocolFactory.Set (name, value);
}

void
QkdProtocolHelper::Install (Ptr<QkdSatellite> satellite)
{
  for (uint32_t i = 0;i < satellite->GetNDevices ();++i)
  {
    Ptr<SpacePointToPointNetDevice> netDevice = satellite->GetDevice (i)->GetObject<SpacePointToPointNetDevice> ();
    if (netDevice)
    {
    }
  }
}

void
QkdProtocolHelper::Install (QkdSatelliteContainer c)
{
  for (QkdSatelliteContainer::Iterator i = c.Begin ();i != c.End ();i++)
  {
    Install (*i);
  }
}

void
QkdProtocolHelper::Install (Ptr<QkdStation> station)
{
  ;
}

void
QkdProtocolHelper::Install (QkdStationContainer c)
{
  for (QkdStationContainer::Iterator i = c.Begin ();i != c.End ();i++)
  {
    Install (*i);
  }
}

} // namespace ns3