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

#include "qkd-net-device-helper.h"
#include "ns3/space-point-to-point-net-device.h"
#include "ns3/net-device-queue-interface.h"
#include "ns3/queue.h"

namespace ns3 {

QkdNetDeviceHelper::QkdNetDeviceHelper ()
{
  m_netDeviceFactory.SetTypeId ("ns3::SpacePointToPointNetDevice");
  m_netQueueFactory.SetTypeId ("ns3::DropTailQueue<Packet>");
}

QkdNetDeviceHelper::~QkdNetDeviceHelper ()
{
}

void
QkdNetDeviceHelper::SetNetDeviceAttribute (std::string name, const AttributeValue &value)
{
  m_netDeviceFactory.Set (name, value);
}

void
QkdNetDeviceHelper::SetQueueAttribute (std::string name, const AttributeValue &value)
{
  m_netQueueFactory.Set (name, value);
}

NetDeviceContainer
QkdNetDeviceHelper::Install (QkdDeviceContainer qkdDevices)
{
  NetDeviceContainer netDevices;
  for (size_t i = 0;i < qkdDevices.GetN ();++i)
  {
    netDevices.Add (Install (qkdDevices.Get (i)));
  }
  return netDevices;
}

NetDeviceContainer
QkdNetDeviceHelper::Install (Ptr<QkdDevice> qkdDevice)
{
  NetDeviceContainer netDevices;
  Ptr<SpacePointToPointNetDevice> netDevice = m_netDeviceFactory.Create<SpacePointToPointNetDevice> ();
  Ptr<Queue<Packet>> queue = m_netQueueFactory.Create<Queue<Packet>> ();
  Ptr<NetDeviceQueueInterface> ndqi = CreateObject<NetDeviceQueueInterface> ();
  qkdDevice->GetNode ()->AddDevice (netDevice);
  qkdDevice->SetNetDevice (netDevice);
  netDevice->SetQueue (queue);
  netDevice->AggregateObject (ndqi);
  ndqi->GetTxQueue (0)->ConnectQueueTraces (queue);
  Ptr<FsoDevice> fsoDevice = qkdDevice->GetFsoDevice ();
  Ptr<Node> node = qkdDevice->GetNode ();
  if (fsoDevice)
  {
    netDevice->AggregateObject (fsoDevice);
  }
  if (node)
  {
    netDevice->SetNode (node);
  }
  netDevices.Add (netDevice);
  return netDevices;
}

}