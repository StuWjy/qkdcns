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

#ifndef ADI_HELPER_H
#define ADI_HELPER_H

#include <map>
#include "ns3/nstime.h"
#include "ns3/ptr.h"
#include "ns3/callback.h"
#include "ns3/qkd-device-container.h"
#include "access-manager.h"
#include "adi-link-helper.h"
#include "adi-type-define.h"
#include "adi-satellite-container.h"
#include "adi-station-container.h"

namespace ns3 {

class CoordTurntable;
class Channel;
class FsoChannel;
class SpacePointToPointChannel;
class QkdSatellite;
class QkdStation;
class QkdDevice;


class AdiHelper
{
public:
  typedef std::pair<Ptr<Turntable>, Ptr<Turntable>> ISL;
  typedef std::vector<ISL> ISLs;
  typedef std::map<adi::Turntable*, Ptr<Turntable>> TurntableMapFromAdiToNs3;
  AdiHelper ();
  ~AdiHelper ();
  static bool Register (Ptr<Turntable> turntable);
  static bool Register (QkdSatellite* sat);
  static bool Register (QkdStation* sta);
  static void RegisterS2G (QkdDeviceContainer qkdTxs, QkdDeviceContainer qkdRxs);
  static void RegisterS2G (Ptr<QkdDevice> qkdTx, Ptr<QkdDevice> qkdRx);
  static void RegisterISL (QkdDeviceContainer qkdTxs, QkdDeviceContainer qkdRxs);
  static void RegisterISL (Ptr<QkdDevice> qkdTx, Ptr<QkdDevice> qkdRx);
  static void Update ();
private:
  static void CreateS2GChannel (const AccessManager::AccessData& access);
  static void CreateISLChannel (const adi::LinkInfo& link);
  static void DoUpdateS2G ();
  static void DoUpdateISL (ISL& isl);
  /**
   * \brief Find the link data with given allowed state and forbidden state,
   * besides, the distance should be also less than maxDistance,
   * the total count satisfication states shoulde be greater than minCount
   * \param[in] linkInfoList  the list of link informations
   * \param[in] includedState the state to be included
   * \param[in] excludedState the state to be excluded
   * \param[in] maxDistance   the maximum distance
   * \param[in] minCount      the minimum count of total satisfication states
   * \return the vector that stores whether the link information is satisfied
   */
  static std::vector<bool> DoFindLinkData (
    adi::LinkInfoList& linkInfoList,
    uint8_t includedState,
    uint8_t excludedState,
    double  maxDistance = 1500.0,
    uint32_t minCount = 60u);

  /**
   * \brief Check the link Information is satisfied,
   * the distance should be also less than maxDistance,
   * the total count satisfication states with given allowed state and forbidden state shoulde be greater than minCount
   * \param[in] linkInfo      the link informations
   * \param[in] allowedState  the allowed state
   * \param[in] forbidenState the forbidden state
   * \param[in] maxDistance   the maximum distance
   * \param[in] minCount      the minimum count of total satisfication states
   * \return true if satisfied, false otherwise
   */
  static bool IsLinkInfoSatisfied (
    const adi::LinkInfo& linkInfo,
    uint8_t allowedState,
    uint8_t forbiddenState,
    double  maxDistance,
    uint32_t minCount);

  static adi::Interval            m_simulationInterval;
  static adi::SatelliteContainer  m_satelliteContainer;
  static adi::StationContainer    m_stationContainer;
  static adi::LinkHelper          m_accessHelper;
  static adi::LinkInfoList        m_accessDatas;
  static TurntableMapFromAdiToNs3 m_turntableMaps;
  struct ScheduleOfISL
  {
    Time start;
    Time stop;
    adi::Pointing srcPointing;
    adi::Pointing dstPointing;
    bool operator< (const ScheduleOfISL& schedule)
    {
      return start < schedule.start;
    }
  };

  static ISLs m_ISLs;
  static std::map<ISL, adi::LinkHelper> m_linkHelper;
  static std::map<ISL, adi::LinkInfoList> m_linkDatas;
  typedef std::map<ISL, std::vector<ScheduleOfISL>> ScheduleOfISLList;
  static ScheduleOfISLList m_scheduleOfISLList;
};

} // namespace ns3

#endif /* ADI_HELPER_H */