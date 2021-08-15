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

#include "qkd-fso-device-helper.h"
#include "ns3/fso-tx-device.h"
#include "ns3/fso-rx-device.h"

namespace ns3 {

QkdFsoDeviceHelper::QkdFsoDeviceHelper ()
{
  m_txDeviceFactory.SetTypeId ("ns3::FsoTxDevice");
  m_rxDeviceFactory.SetTypeId ("ns3::FsoRxDevice");
  m_turntableFactory.SetTypeId ("ns3::Turntable");
}

QkdFsoDeviceHelper::~QkdFsoDeviceHelper ()
{
}

void
QkdFsoDeviceHelper::SetFsoTxDeviceAttribute (std::string name, const AttributeValue &value)
{
  m_txDeviceFactory.Set (name, value);
}

void
QkdFsoDeviceHelper::SetFsoRxDeviceAttribute (std::string name, const AttributeValue &value)
{
  m_rxDeviceFactory.Set (name, value);
}

void
QkdFsoDeviceHelper::SetTurntableAttribute (std::string name, const AttributeValue &value)
{
  m_turntableFactory.Set (name, value);
}

FsoDeviceContainer
QkdFsoDeviceHelper::Install (QkdDeviceContainer qkdDevices)
{
  FsoDeviceContainer fsoDevices;
  for (size_t i = 0;i < qkdDevices.GetN ();++i)
  {
    fsoDevices.Add (Install (qkdDevices.Get (i)));
  }
  return fsoDevices;
}

FsoDeviceContainer
QkdFsoDeviceHelper::Install (Ptr<QkdDevice> qkdDevice)
{
  FsoDeviceContainer fsoDevices;
  Ptr<FsoDevice> fsoDevice;
  switch (qkdDevice->GetType ())
  {
    case Tx: fsoDevice = m_txDeviceFactory.Create<FsoTxDevice> (); break;
    case Rx: fsoDevice = m_rxDeviceFactory.Create<FsoRxDevice> (); break;
    default: NS_ASSERT (false);
  }
  Ptr<Node> node = qkdDevice->GetNode ();
  NS_ASSERT (node);
  qkdDevice->SetFsoDevice (fsoDevice);
  Ptr<Turntable> turntable = m_turntableFactory.Create<Turntable> ();
  turntable->SetFace (qkdDevice->GetFace ());
  fsoDevice->SetTurntable (turntable);
  if (node)
  {
    fsoDevice->SetNode (node);
  }
  fsoDevice->AggregateObject (turntable);
  Ptr<NetDevice> netDevice = qkdDevice->GetNetDevice ();
  if (netDevice)
  {
    fsoDevice->AggregateObject (netDevice);
  }
  fsoDevices.Add (fsoDevice);
  return fsoDevices;
}

}