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

#include "ns3/simulator.h"
#include "ns3/assert.h"
#include "ns3/node.h"
#include "ns3/output-stream-wrapper.h"
#include "ns3/constant.h"
#include "ns3/util.h"
#include "ns3/qkd-device.h"
#include "ns3/qkd-node.h"
#include "ns3/qkd-satellite.h"
#include "ns3/qkd-station.h"
#include "ns3/turntable.h"
#include "ns3/fso-device.h"
#include "ns3/fso-channel.h"
#include "ns3/space-point-to-point-channel.h"
#include "ns3/space-point-to-point-net-device.h"
#include "adi-helper.h"
#include "adi-constant.h"
#include "adi-satellite-list.h"
#include "adi-station-list.h"
#include "adi-satellite.h"
#include "adi-station.h"

namespace ns3 {

using namespace adi;

NS_LOG_COMPONENT_DEFINE ("AdiHelper");

SatelliteContainer  AdiHelper::m_satelliteContainer  = SatelliteContainer ();
StationContainer    AdiHelper::m_stationContainer    = StationContainer ();
LinkHelper          AdiHelper::m_accessHelper        = LinkHelper ();
LinkInfoList        AdiHelper::m_accessDatas         = LinkInfoList ();
std::map<AdiHelper::ISL, adi::LinkHelper>   AdiHelper::m_linkHelper = std::map<ISL, adi::LinkHelper> ();
std::map<AdiHelper::ISL, adi::LinkInfoList> AdiHelper::m_linkDatas  = std::map<ISL, adi::LinkInfoList> ();
AdiHelper::TurntableMapFromAdiToNs3 AdiHelper::m_turntableMaps = TurntableMapFromAdiToNs3 ();
AdiHelper::ISLs AdiHelper::m_ISLs = ISLs ();
// AdiHelper::QkdWorkingList AdiHelper::m_qkdWorkingList = QkdWorkingList ();
AdiHelper::ScheduleOfISLList AdiHelper::m_scheduleOfISLList = ScheduleOfISLList ();

AdiHelper::AdiHelper ()
{
}

AdiHelper::~AdiHelper ()
{
}

bool
AdiHelper::Register (Ptr<Turntable> turntable)
{
  adi::Turntable* adi = turntable->GetAdiTurntable ();
  if (m_turntableMaps.find (adi) == m_turntableMaps.end ())
  {
    m_turntableMaps[adi] = turntable;
    return true;
  }
  else
  {
    NS_ASSERT_MSG (false, "The turntable of ns3 has been registered");
    return false;
  }
}

bool
AdiHelper::Register (QkdSatellite* sat)
{
  adi::Satellite* _sat = sat->GetAdiSat ();
  m_satelliteContainer << _sat;
  NS_LOG_INFO (_sat << " registered by" << sat);
  return true;
}

bool
AdiHelper::Register (QkdStation* sta)
{
  adi::Station* _sta = sta->GetAdiSta ();
  m_stationContainer << _sta;
  NS_LOG_INFO (_sta << " registered by" << sta);
  return true;
}

void
AdiHelper::RegisterS2G (QkdDeviceContainer qkdTxs, QkdDeviceContainer qkdRxs)
{
  for (uint32_t i = 0;i < qkdTxs.GetN ();++i)
  {
    for (uint32_t j = 0;j < qkdRxs.GetN ();++j)
    {
      RegisterS2G (qkdTxs.Get (i), qkdRxs.Get (j));
    }
  }
}

void
AdiHelper::RegisterS2G (Ptr<QkdDevice> qkdTx, Ptr<QkdDevice> qkdRx)
{
  adi::Turntable* tx = qkdTx->GetFsoDevice ()->GetTurntable ()->GetAdiTurntable ();
  adi::Turntable* rx = qkdRx->GetFsoDevice ()->GetTurntable ()->GetAdiTurntable ();
  if (tx && rx)
  {
    const Face& txFace = tx->GetFace ();
    const Face& rxFace = rx->GetFace ();
    if ((txFace == Bottom && rxFace == Top))
    {
      m_accessHelper.AddLink (Link (tx, rx, IN_FOV | SRC2DST | DST2SRC, DST_DAY | BEYOND_DISTANCE));
    }
    else
    {
      if (txFace != Bottom)
      {
        NS_LOG_WARN ("There is no turntable installed on bottom side of the satellite");
      }
      if (rxFace != Top)
      {
        NS_LOG_WARN ("There is no turntable installed on top side of the station");
      }
    }
  }
  else
  {
    if (tx == 0)
    {
      NS_LOG_WARN ("There is no turntable installed on the satellite");
    }
    if (rx == 0)
    {
      NS_LOG_WARN ("There is no turntable installed on the station");
    }
  }
}


void
AdiHelper::RegisterISL (QkdDeviceContainer qkdTxs, QkdDeviceContainer qkdRxs)
{
  NS_ASSERT (qkdTxs.GetN () == qkdRxs.GetN ());
  for (uint32_t i = 0;i < qkdTxs.GetN ();++i)
  {
    RegisterISL (qkdTxs.Get (i), qkdRxs.Get (i));
  }
}

void
AdiHelper::RegisterISL (Ptr<QkdDevice> qkdTx, Ptr<QkdDevice> qkdRx)
{
  Ptr<Turntable> src = qkdTx->GetFsoDevice ()->GetTurntable ();
  Ptr<Turntable> dst = qkdRx->GetFsoDevice ()->GetTurntable ();
  ISL isl = std::make_pair (src, dst);
  if (std::find (m_ISLs.begin (), m_ISLs.end (), isl) != m_ISLs.end ())
  {
    NS_LOG_WARN ("Registered an existed inter-satellite link");
    return;
  }
  m_ISLs.push_back (isl);
  adi::Turntable* _src_ = src->GetAdiTurntable ();
  adi::Turntable* _dst_ = dst->GetAdiTurntable ();
  const Face& txFace = _src_->GetFace ();
  const Face& rxFace = _dst_->GetFace ();
  if ((txFace == Front && rxFace == Back)
  || (txFace == Back && rxFace == Front)
  || (txFace == Left && rxFace == Right)
  || (txFace == Right && rxFace == Left))
  {
    m_linkHelper[isl].AddLink (
      Link (
        _src_,
        _dst_,
        SRC2DST | DST2SRC,
        SRC_DAY | DST_DAY
      )
    );
  }
}

void
AdiHelper::Update ()
{
  DoUpdateS2G ();
  if (EnableISL)
  {
    for (uint32_t i = 0;i < m_ISLs.size ();++i)
    {
      DoUpdateISL (m_ISLs[i]);
    }
  }
}

void
AdiHelper::CreateS2GChannel (const AccessManager::AccessData& access)
{
  NS_ASSERT (access.src);
  NS_ASSERT (access.dst);
  Ptr<SpacePointToPointChannel> netChannel = CreateObject<SpacePointToPointChannel> ();
  Ptr<NetDevice> netTx = access.src->GetFsoDevice ()->GetNetDevice ();
  Ptr<NetDevice> netRx = access.dst->GetFsoDevice ()->GetNetDevice ();
  adi::LinkDatas netLinkDatas {access.netStart, access.netStop};
  netChannel->Attach (netTx, netRx, netLinkDatas);
  Ptr<FsoChannel> fsoChannel = CreateObject<FsoChannel> ();
  Ptr<FsoDevice> fsoTx = access.src->GetFsoDevice ();
  Ptr<FsoDevice> fsoRx = access.dst->GetFsoDevice ();
  adi::LinkDatas fsoLinkDatas {access.fsoStart, access.fsoStop};
  fsoChannel->Attach (fsoTx, fsoRx, fsoLinkDatas);
  fsoChannel->AggregateObject (netChannel);
}

void
AdiHelper::CreateISLChannel (const adi::LinkInfo& link)
{
  NS_ASSERT (link.src);
  NS_ASSERT (link.dst);
  Ptr<Turntable> src = Ptr<Turntable> ((Turntable*) link.src->GetNs3 ());
  Ptr<Turntable> dst = Ptr<Turntable> ((Turntable*) link.dst->GetNs3 ());
  NS_ASSERT (src);
  NS_ASSERT (dst);
  Ptr<SpacePointToPointChannel> netChannel = CreateObject<SpacePointToPointChannel> ();
  Ptr<NetDevice> netTx = src->GetFsoDevice ()->GetNetDevice ();
  Ptr<NetDevice> netRx = dst->GetFsoDevice ()->GetNetDevice ();
  netChannel->Attach (netTx, netRx, link.linkDatas);
  Ptr<FsoChannel> fsoChannel = CreateObject<FsoChannel> ();
  Ptr<FsoDevice> fsoTx = src->GetFsoDevice ();
  Ptr<FsoDevice> fsoRx = dst->GetFsoDevice ();
  fsoChannel->Attach (fsoTx, fsoRx, adi::LinkDatas {link.linkDatas.begin (), link.linkDatas.end () - 5});
  fsoChannel->AggregateObject (netChannel);
}

void
AdiHelper::DoUpdateS2G ()
{
  Time now = Now ();
  // calculate the new link datas by adi
  // the interval to be calculated
  static adi::DateTime simStart = SimulationStart;
  static adi::DateTime simStop = SimulationStart + Day;
  Interval simInterval (simStart, simStop);
  adi::LinkInfoList tmpLinkInfoList;
  if (EnableS2G)
  {
    m_accessHelper.SetInterval (simInterval);
    m_accessDatas = m_accessHelper.CalcLink ();
    // find the access as net access that both in fov and both in shadow
    std::vector<bool> selected = DoFindLinkData (m_accessDatas, SRC2DST | DST2SRC, DST_DAY | BEYOND_DISTANCE);
    AccessManager::AccessList& access = AccessManager::SelectTasks (m_accessDatas, selected);
    for (uint32_t i = 0;i < access.size ();++i)
    {
      if (access[i].selected)
      {
        // Create the net and fso channels after link available
        Time start = ToTime (access[i].netStart->time);
        Time delay = start - Now ();
        Simulator::Schedule (delay, &CreateS2GChannel, access[i]);
      }
    }
  }
  // start and stop of simulation will be the next day for next calculation
  simStart = simStart + Day;
  simStop = simStop + Day;
  Simulator::Schedule (Days (1.0), &AdiHelper::DoUpdateS2G);
}

void
AdiHelper::DoUpdateISL (ISL& isl)
{
  Ptr<Turntable> src = isl.first;
  Ptr<Turntable> dst = isl.second;
  LinkHelper& linkHelper = m_linkHelper[isl];
  LinkInfoList& linkInfoList = m_linkDatas[isl];
  DateTime simStart = ToTime (Now ());
  DateTime simStop = ToTime (Now ()) + Day;
  linkHelper.SetInterval (Interval (simStart, simStop));
  linkInfoList = linkHelper.CalcLink ();
  // std::vector<bool> selected = std::vector<bool> (linkInfoList.size (), true);
  if (linkInfoList.back ().linkDatas.back ().time == simStop)
  {
    linkInfoList.pop_back ();
  }
  simStop = linkInfoList.back ().linkDatas.back ().time + Second;
  for (LinkInfoList::const_iterator it = linkInfoList.cbegin ();it != linkInfoList.cend ();++it)
  {
    Time start = ToTime (it->linkDatas.front ().time);
    Time stop  = ToTime (it->linkDatas.back ().time);
    Time delay = stop - Now ();
    Simulator::Schedule (start - Now (), &CreateISLChannel, *it);
    Simulator::Schedule (
      delay,
      &Turntable::SetTargetPointing,
      src,
      start,
      it->linkDatas.front ().fromSrc
    );
    Simulator::Schedule (
      delay,
      &Turntable::SetTargetPointing,
      dst,
      start,
      it->linkDatas.front ().fromDst
    );
  }
  Simulator::Schedule (
    ToTime (simStop) - Now (),
    &AdiHelper::DoUpdateISL,
    isl
  );
}

std::vector<bool>
AdiHelper::DoFindLinkData (
  adi::LinkInfoList& linkInfoList,
  uint8_t allowedState,
  uint8_t forbiddenState,
  double  maxDistance,
  uint32_t minCount)
{
  std::vector<bool> result = std::vector<bool>(linkInfoList.size (), false);
  for (uint32_t i = 0;i < linkInfoList.size ();++i)
  {
    if (IsLinkInfoSatisfied (linkInfoList[i], allowedState, forbiddenState, maxDistance, minCount))
    {
      result[i] = true;
    }
    else
    {
      result[i] = false;
    }
  }
  return result;
}

bool
AdiHelper::IsLinkInfoSatisfied (
  const adi::LinkInfo& linkInfo,
  uint8_t includedState,
  uint8_t excludedState,
  double  maxDistance,
  uint32_t minCount)
{
  const adi::LinkDatas& linkDatas = linkInfo.linkDatas;
  uint32_t count = 0;
  bool satisfied = false;
  for (uint32_t j = 0;j < linkDatas.size ();++j)
  {
    double distance = linkDatas[j].distance;
    uint8_t state = linkDatas[j].state;
    // the state must be allowed state, the forbidden state is excluded, besides, the distance should be not greater than maxDistance
    satisfied = ((state & includedState) == includedState) && ((state & excludedState) == 0) && (distance <= maxDistance);
    if (satisfied)
    {
      count++;
    }
  }
  if (count >= minCount)
  {
    return true;
  }
  return false;
}


}