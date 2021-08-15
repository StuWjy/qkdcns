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

#include "qkd-device-helper.h"
#include "ns3/qkd-satellite-container.h"
#include "ns3/qkd-station-container.h"
#include "ns3/qkd-device-container.h"
#include "ns3/fso-tx-device.h"
#include "ns3/fso-rx-device.h"
#include "ns3/space-point-to-point-net-device.h"
#include "ns3/turntable.h"
#include "ns3/qkd-key-pool.h"
#include "ns3/q3p-tx-cache.h"
#include "ns3/q3p-rx-cache.h"
#include "ns3/constant.h"
#include "ns3/queue.h"
#include "ns3/net-device-queue-interface.h"

namespace ns3 {

QkdDeviceHelper::QkdDeviceHelper ()
{
  m_keyPoolFactory.SetTypeId ("ns3::QkdKeyPool");
  m_face = adi::ErrFace;
  m_type = Tx;
}

QkdDeviceHelper::~QkdDeviceHelper ()
{
}

void
QkdDeviceHelper::SetDeviceFace (DeviceFace face)
{
  m_face = face;
}

void
QkdDeviceHelper::SetDeviceType (DeviceType type)
{
  m_type = type;
}

void
QkdDeviceHelper::SetKeyPoolAttribute (std::string n1, const AttributeValue &v1)
{
  m_keyPoolFactory.Set (n1, v1);
}

QkdDeviceContainer
QkdDeviceHelper::Install (Ptr<QkdNode> node)
{
  QkdDeviceContainer qkdDevices;
  Ptr<QkdDevice> qkdDevice = CreateObject<QkdDevice> ();
  qkdDevice->SetFace (m_face);
  qkdDevice->SetType (m_type);
  qkdDevice->SetNode (node);
  node->AddQkdDevice (qkdDevice);
  qkdDevices.Add (qkdDevice);
  if (node->GetObject<QkdKeyPool> () == 0)
  {
    Ptr<QkdKeyPool> pool = m_keyPoolFactory.Create<QkdKeyPool> ();
    node->AggregateObject (pool);
  }
  return qkdDevices;
}

QkdDeviceContainer
QkdDeviceHelper::Install (QkdSatelliteContainer c)
{
  QkdDeviceContainer qkdDevices;
  for (QkdSatelliteContainer::Iterator i = c.Begin ();i != c.End ();++i)
  {
    qkdDevices.Add (Install (*i));
  }
  return qkdDevices;
}

QkdDeviceContainer
QkdDeviceHelper::Install (QkdStationContainer c)
{
  QkdDeviceContainer qkdDevices;
  for (QkdStationContainer::Iterator i = c.Begin ();i != c.End ();++i)
  {
    qkdDevices.Add (Install (*i));
  }
  return qkdDevices;
}

} // namespace ns3