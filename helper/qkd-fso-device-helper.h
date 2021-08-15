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

#ifndef QKD_FSO_DEVICE_HELPER_H
#define QKD_FSO_DEVICE_HELPER_H

#include "ns3/object-factory.h"
#include "ns3/fso-device.h"
#include "ns3/fso-device-container.h"
#include "ns3/qkd-node.h"
#include "ns3/box.h"

namespace ns3 {

class QkdFsoDeviceHelper
{
public:
  QkdFsoDeviceHelper ();
  ~QkdFsoDeviceHelper ();

  /**
   * Set an attribute value to be propagated to each FsoTxDevice created by the
   * helper.
   *
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set
   *
   * Set these attributes on each ns3::FsoTxDevice created
   * by QkdFsoDeviceHelper::Install
   */
  void SetFsoTxDeviceAttribute (std::string name, const AttributeValue &value);

  /**
   * Set an attribute value to be propagated to each FsoRxDevice created by the
   * helper.
   *
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set
   *
   * Set these attributes on each ns3::FsoRxDevice created
   * by QkdFsoDeviceHelper::Install
   */
  void SetFsoRxDeviceAttribute (std::string name, const AttributeValue &value);

  /**
   * Set an attribute value to be propagated to each Turntable created by the
   * helper.
   *
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set
   *
   * Set these attributes on each ns3::Turntable created
   * by QkdFsoDeviceHelper::Install
   */
  void SetTurntableAttribute (std::string name, const AttributeValue &value);

  FsoDeviceContainer Install (QkdDeviceContainer qkdDevices);
  FsoDeviceContainer Install (Ptr<QkdDevice> qkdDevice);
private:
  ObjectFactory m_txDeviceFactory;  //!< Factory for FsoTxDevice
  ObjectFactory m_rxDeviceFactory;  //!< Factory for FsoRxDevice
  ObjectFactory m_turntableFactory; //!< Factory for Turntable
};

} // namespace ns3

#endif /* QKD_FSO_DEVICE_HELPER_H */