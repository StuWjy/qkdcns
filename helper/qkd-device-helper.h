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

#ifndef QKD_DEVICE_HELPER_H
#define QKD_DEVICE_HELPER_H

#include "ns3/object-factory.h"
#include "ns3/qkd-device.h"
#include "ns3/qkd-device-container.h"

namespace ns3 {

class QkdNode;
class QkdSatelliteContainer;
class QkdStationContainer;

class QkdDeviceHelper
{
public:
  QkdDeviceHelper ();
  ~QkdDeviceHelper ();

  void SetDeviceFace (DeviceFace face);
  void SetDeviceType (DeviceType type);

  /**
   * Set an attribute value to be propagated to each key pool created by the
   * helper.
   *
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set
   *
   * Set these attributes on each ns3::QkdKeyPool created
   * by QkdDeviceHelper::Install
   */
  void SetKeyPoolAttribute (std::string name, const AttributeValue &value);

  QkdDeviceContainer Install (Ptr<QkdNode> node);

  QkdDeviceContainer Install (QkdSatelliteContainer c);
  QkdDeviceContainer Install (QkdStationContainer c);
private:
  ObjectFactory m_keyPoolFactory;   //!< Factory for qkd key pool
  DeviceFace    m_face;
  DeviceType    m_type;
};

} // namespace ns3

#endif /* QKD_DEVICE_HELPER_H */