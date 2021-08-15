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

#ifndef QKD_PROTOCOL_HELPER_H
#define QKD_PROTOCOL_HELPER_H

#include "ns3/object-factory.h"
#include "ns3/qkd-satellite-container.h"
#include "ns3/qkd-station-container.h"

namespace ns3 {

class QkdSatellite;
class QkdStation;

class QkdProtocolHelper
{
public:
  QkdProtocolHelper ();
  ~QkdProtocolHelper ();
  void SetQkdProtocolAttribute (std::string name, const AttributeValue &value);
  void Install (Ptr<QkdSatellite> satellite);
  void Install (QkdSatelliteContainer c);
  void Install (Ptr<QkdStation> station);
  void Install (QkdStationContainer c);
private:
  ObjectFactory m_qkdProtocolFactory;
};

} // namespace

#endif /* QKD_PROTOCOL_HELPER_H */