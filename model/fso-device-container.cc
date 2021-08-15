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

#include "fso-device-container.h"
#include "ns3/names.h"

namespace ns3 {

FsoDeviceContainer::FsoDeviceContainer ()
{
}

FsoDeviceContainer::~FsoDeviceContainer ()
{
}

FsoDeviceContainer::Iterator
FsoDeviceContainer::Begin (void) const
{
  return m_devices.cbegin ();
}

FsoDeviceContainer::Iterator
FsoDeviceContainer::End (void) const
{
  return m_devices.cend ();
}
uint32_t
FsoDeviceContainer::GetN (void) const
{
  return m_devices.size ();
}

Ptr<FsoDevice>
FsoDeviceContainer::Get (uint32_t i) const
{
  return m_devices[i];
}

void
FsoDeviceContainer::Add (FsoDeviceContainer other)
{
  for (Iterator i = other.Begin ();i != other.End ();++i)
  {
    m_devices.push_back (*i);
  }
}
void
FsoDeviceContainer::Add (Ptr<FsoDevice> device)
{
  m_devices.push_back (device);
}

// void
// FsoDeviceContainer::Add (std::string deviceName)
// {
//   Ptr<FsoDevice> device = Names::Find<FsoDevice> (deviceName);
//   m_devices.push_back (device);
// }

} // namespace ns3