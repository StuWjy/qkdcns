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

#ifndef FSO_DEVICE_CONTAINER_H
#define FSO_DEVICE_CONTAINER_H

#include "fso-device.h"
#include <vector>

namespace ns3 {

class FsoDeviceContainer
{
public:
  typedef std::vector<Ptr<FsoDevice>> FsoDevices;
  typedef FsoDevices::const_iterator Iterator;
  FsoDeviceContainer ();
  virtual ~FsoDeviceContainer ();
  Iterator Begin (void) const;
  Iterator End (void) const;
  uint32_t GetN (void) const;
  Ptr<FsoDevice> Get (uint32_t i) const;
  void Add (FsoDeviceContainer other);
  void Add (Ptr<FsoDevice> device);
  // void Add (std::string deviceName);
private:
  FsoDevices m_devices;
};

} // namespace ns3

#endif /* FSO_DEVICE_CONTAINER_H */