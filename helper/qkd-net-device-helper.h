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

#ifndef QKD_NET_DEVICE_HELPER_H
#define QKD_NET_DEVICE_HELPER_H

#include "ns3/object-factory.h"
#include "ns3/space-point-to-point-net-device.h"
#include "ns3/net-device-container.h"
#include "ns3/qkd-node.h"

namespace ns3 {

class QkdNetDeviceHelper
{
public:
  QkdNetDeviceHelper ();
  ~QkdNetDeviceHelper ();

  void SetInstalledSurface (uint8_t surface);

  /**
   * Set an attribute value to be propagated to each FsoTxDevice created by the
   * helper.
   *
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set
   *
   * Set these attributes on each ns3::FsoTxDevice created
   * by QkdNetDeviceHelper::Install
   */
  void SetNetDeviceAttribute (std::string name, const AttributeValue &value);

  /**
   * Set an attribute value to be propagated to each Queue created by the
   * helper.
   *
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set
   *
   * Set these attributes on each ns3::Queue created
   * by QkdNetDeviceHelper::Install
   */
  void SetQueueAttribute (std::string name, const AttributeValue &value);

  NetDeviceContainer Install (QkdDeviceContainer qkdDevices);
  NetDeviceContainer Install (Ptr<QkdDevice> qkdDevice);
private:
  ObjectFactory m_netDeviceFactory;  //!< Factory for space point-to-point net device
  ObjectFactory m_netQueueFactory;   //!< Factory for net device queue
};

} // namespace ns3

#endif /* QKD_NET_DEVICE_HELPER_H */