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

#include <queue>
#include "ns3/qkd-satellite.h"
#include "ns3/qkd-satellite-list.h"
#include "ns3/qkd-station.h"
#include "ns3/qkd-station-list.h"
#include "ns3/turntable.h"
#include "ns3/util.h"
#include "access-manager.h"
#include "adi-constant.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("AccessManager");

AccessManager::AccessList AccessManager::m_accesses = AccessList ();
AccessManager::TurntableStateList AccessManager::m_turntableStateList = TurntableStateList ();

bool
AccessManager::AccessData::operator< (const AccessData& access) const
{
  return wValue > access.wValue;
}

bool
AccessManager::TurntableState::operator< (const TurntableState& state) const
{
  return time < state.time;
}

AccessManager::TurntableStates::iterator
AccessManager::TryInsert (
  Ptr<Turntable>        turntable,
  const TurntableState& startState,
  const TurntableState& stopState)
{
  TurntableStates& turntableStates = m_turntableStateList[turntable];
  Time start = startState.time;
  Time stop = stopState.time;
  CoordTurntable formerPointing;
  CoordTurntable latterPointing;
  NS_ASSERT (start >= turntableStates.front ().time);
  NS_ASSERT (stop >= turntableStates.front ().time);
  Time formerStop;
  Time latterStart;
  // if start time is greater than the last state time of turntable
  // then the latter state will not be considered
  if (start > turntableStates.back ().time)
  {
    formerStop = turntableStates.back ().time;
    formerPointing = turntableStates.back ().pointing;
    Time timeSwingFromFormer = BangBangController::SwingTime (
      formerPointing,
      startState.pointing,
      turntable->GetAzimuthLimiter (),
      turntable->GetPitchLimiter ());
    if (formerStop + timeSwingFromFormer <= start)
    {
      return turntableStates.end ();
    }
  }
  else
  {
    NS_ASSERT (turntableStates.size () % 2 == 1);
    for (uint32_t i = 0;i < (turntableStates.size () - 1) / 2;++i)
    {
      TurntableStates::iterator former = 2 * i + turntableStates.begin ();
      TurntableStates::iterator latter = 2 * i + 1 + turntableStates.begin ();
      formerStop = former->time;
      formerPointing = former->pointing;
      latterStart = latter->time;
      latterPointing = latter->pointing;
      if (!(start >= formerStop && start <= latterStart && stop >= formerStop && stop <= latterStart))
      {
        continue;
      }
      Time timeSwingFromFormerState = BangBangController::SwingTime (
        formerPointing,
        startState.pointing,
        turntable->GetAzimuthLimiter (),
        turntable->GetPitchLimiter ());
      Time timeSwingToLatterState = BangBangController::SwingTime (
        stopState.pointing,
        latterPointing,
        turntable->GetAzimuthLimiter (),
        turntable->GetPitchLimiter ());
      // bool isStartInRegion = 0 > ((formerStop + timeSwingFromFormerState - start).GetSeconds () * (latterStart- timeSwingToLatterState - start).GetSeconds ());
      // bool isStopInRegion = 0 > ((formerStop + timeSwingFromFormerState- stop).GetSeconds () * (latterStart - timeSwingToLatterState- stop).GetSeconds ());
      // if (isStartInRegion && isStopInRegion)
      // {
      //   return latter;
      // }
      if (formerStop + timeSwingFromFormerState <= start && stop + timeSwingToLatterState <= latterStart)
      {
        return latter;
      }
    }
  }
  return turntableStates.begin ();
}

bool
AccessManager::TryInsert (
  Ptr<Turntable>        src,
  const TurntableState& srcStartState,
  const TurntableState& srcStopState,
  Ptr<Turntable>        dst,
  const TurntableState& dstStartState,
  const TurntableState& dstStopState)
{
  TurntableStates::iterator srcIt;
  TurntableStates::iterator dstIt;
  TurntableStates& srcTurntableStates = m_turntableStateList[src];
  TurntableStates& dstTurntableStates = m_turntableStateList[dst];
  srcIt = TryInsert (src, srcStartState, srcStopState);
  dstIt = TryInsert (dst, dstStartState, dstStopState);
  if (srcIt != srcTurntableStates.begin () && dstIt != dstTurntableStates.begin ())
  {
    TurntableStates srcStates {srcStartState, srcStopState};
    srcTurntableStates.insert (srcIt, srcStates.begin (), srcStates.end ());
    TurntableStates dstStates {dstStartState, dstStopState};
    dstTurntableStates.insert (dstIt, dstStates.begin (), dstStates.end ());
    return true;
  }
  return false;
}

AccessManager::AccessList&
AccessManager::SelectTasks (const adi::LinkInfoList& datas, const std::vector<bool>& satisfied)
{
  std::vector<bool> result = std::vector<bool> (datas.size (), false);
  m_accesses.clear ();
  m_turntableStateList.clear ();
  AddAccessData (datas, satisfied);
  CalcScheme ();
  AssignTurntableTask ();
  NS_LOG_INFO (ToString ());
  return m_accesses;
}

void
AccessManager::CalcScheme (void)
{
  /**
   * For each access
   */
  std::map<Ptr<QkdStation>, double> m_weights;
  std::map<Ptr<QkdStation>, uint64_t> m_gaps;
  double totalKeyGap = QkdSatelliteList::GetTotalKeyGap ();
  NS_ASSERT (totalKeyGap >= 0);
  if (totalKeyGap == 0)
  {
    for (uint32_t i = 0;i < QkdStationList::GetN ();++i)
    {
      m_weights[QkdStationList::Get (i)] = 1.0;
    }
  }
  else
  {
    for (uint32_t i = 0;i < QkdStationList::GetN ();++i)
    {
      double gap = QkdSatelliteList::GetKeyGap (QkdStationList::Get (i));
      m_gaps[QkdStationList::Get (i)] = gap;
      m_weights[QkdStationList::Get (i)] = exp (gap * 1.0 / totalKeyGap);
    }
  }
  for (uint32_t i = 0;i < m_accesses.size ();++i)
  {
    Ptr<QkdStation> _dst = m_accesses[i].dst->GetNode ()->GetObject<QkdStation> ();
    m_accesses[i].wValue = m_accesses[i].value * m_weights[_dst];
  }
    // sort the access descending by their value
  std::sort (m_accesses.begin (), m_accesses.end ());
  for (uint32_t i = 0;i < m_accesses.size ();++i)
  {
    // for (uint32_t j = i + 1;j < m_accesses.size ();++j)
    // {
    //   Ptr<QkdStation> _dst = m_accesses[j].dst->GetNode ()->GetObject<QkdStation> ();
    //   m_accesses[j].wValue = m_accesses[j].value * m_weights[_dst];
    // }
    // // sort the access descending by their value
    // std::sort (m_accesses.begin () + i + 1, m_accesses.end ());
    Ptr<ns3::Turntable> src = NULL, dst = NULL;
    AccessData& access = m_accesses[i];
    src = access.src;
    dst = access.dst;
    if (m_turntableStateList.find (src) == m_turntableStateList.end ())
    {
      m_turntableStateList[src].push_back (TurntableState {Now (), src->GetPointing ()});
    }
    if (m_turntableStateList.find (dst) == m_turntableStateList.end ())
    {
      m_turntableStateList[dst].push_back (TurntableState {Now (), dst->GetPointing ()});
    }
    Time start = ToTime (access.fsoStart->time);
    Time stop = ToTime (access.fsoStop->time);
    TurntableState srcStartState {start, access.fsoStart->fromSrc};
    TurntableState srcStopState {stop, access.fsoStop->fromSrc};
    TurntableState dstStartState {start, access.fsoStart->fromDst};
    TurntableState dstStopState {stop, access.fsoStop->fromDst};
    if (true == TryInsert (
      src, srcStartState, srcStopState,
      dst, dstStartState, dstStopState))
    {
      access.selected = true;
    }
    else
    {
      access.selected = false;
    }
  }
}

void
AccessManager:: AssignTurntableTask (void)
{
  typedef TurntableStateList::iterator IT;
  for (IT it = m_turntableStateList.begin ();it != m_turntableStateList.end ();++it)
  {
    for (uint32_t i = 1;i < it->second.size ();i += 2)
    {
      Simulator::Schedule (it->second[i - 1].time - Now (), &Turntable::SetTargetPointing, it->first, it->second[i].time, it->second[i].pointing);
    }
  }
}

void
AccessManager::AddAccessData (const adi::LinkInfoList& datas, const std::vector<bool>& satisfied)
{
  for (uint32_t i = 0;i < datas.size ();++i)
  {
    if (satisfied[i])
    {
      AddAccessData (datas[i], i);
    }
  }
}

void
AccessManager::AddAccessData (const adi::LinkInfo& data, uint32_t i)
{
  typedef adi::LinkDatas::const_iterator CI;
  typedef adi::LinkDatas::const_reverse_iterator CRI;
  Ptr<Turntable> src = Ptr<Turntable> ((Turntable*) data.src->GetNs3 ());
  Ptr<Turntable> dst = Ptr<Turntable> ((Turntable*) data.dst->GetNs3 ());
  Ptr<QkdSatellite> sat = src->GetNode ()->GetObject<QkdSatellite> ();
  Ptr<QkdStation> sta = dst->GetNode ()->GetObject<QkdStation> ();
  NS_ASSERT (sat);
  NS_ASSERT (sta);
  Box satAzBound = src->GetAzimuthLimiter ();
  Box satPtBound = src->GetPitchLimiter ();
  Box staAzBound = dst->GetAzimuthLimiter ();
  Box staPtBound = dst->GetPitchLimiter ();
  AccessData access;
  access.i = i;
  access.src = src;
  access.dst = dst;
  bool srcInLimiter = false;
  bool dstInLimiter = false;
  bool findStart = false;
  bool findStop = false;
  CI it_start = data.linkDatas.cbegin ();
  access.fsoStart = data.linkDatas.cbegin ();
  // find the first time that the pointings of source and destination are all in limiter
  for (CI it = data.linkDatas.cbegin ();it != data.linkDatas.cend ();++it)
  {
    // considering the angular rate limitter
    double tmp1 = (it->fromSrc.rate.azimuth - satAzBound.yMax) * (it->fromSrc.rate.azimuth - satAzBound.yMin);
    double tmp2 = (it->fromSrc.rate.pitch - satPtBound.yMax) * (it->fromSrc.rate.pitch - satPtBound.yMin);
    double tmp3 = (it->fromDst.rate.azimuth - staAzBound.yMax) * (it->fromDst.rate.azimuth - staAzBound.yMin);
    double tmp4 = (it->fromDst.rate.pitch - staPtBound.yMax) * (it->fromDst.rate.pitch - staPtBound.yMin);
    // considering the angle limitter
    uint8_t state = it->state;
    srcInLimiter = tmp1 <= 0 && tmp2 <= 0 && (state & SRC2DST);
    dstInLimiter = tmp3 <= 0 && tmp4 <= 0 && (state & DST2SRC);
    if (srcInLimiter && dstInLimiter && it->distance <= adi::K_MAX_DISTANCE)
    {
      findStart = true;
      it_start = std::max (it - 1, data.linkDatas.cbegin ());
      break;
    }
  }
  CI it_stop = data.linkDatas.cend () - 1;
  access.fsoStop = data.linkDatas.cend () - 1;
  // find the last time that the pointings of source and destination are all in limiter
  for (CRI it = data.linkDatas.crbegin () + 1;it != data.linkDatas.crend ();++it)
  {
    // considering the angular rate limitter
    double tmp1 = (it->fromSrc.rate.azimuth - satAzBound.yMax) * (it->fromSrc.rate.azimuth - satAzBound.yMin);
    double tmp2 = (it->fromSrc.rate.pitch - satPtBound.yMax) * (it->fromSrc.rate.pitch - satPtBound.yMin);
    double tmp3 = (it->fromDst.rate.azimuth - staAzBound.yMax) * (it->fromDst.rate.azimuth - staAzBound.yMin);
    double tmp4 = (it->fromDst.rate.pitch - staPtBound.yMax) * (it->fromDst.rate.pitch - staPtBound.yMin);
    // considering the angle limitter
    uint8_t state = it->state;
    srcInLimiter = tmp1 <= 0 && tmp2 <= 0 && (state & SRC2DST);
    dstInLimiter = tmp3 <= 0 && tmp4 <= 0 && (state & DST2SRC);
    if (srcInLimiter && dstInLimiter && it->distance <= adi::K_MAX_DISTANCE)
    {
      findStop = true;
      it_stop = it.base ();
      break;
    }
  }
  if (it_stop->time <= it_start->time || (it_stop->time - it_start->time) < adi::TimeSpan (0, 1, 0))
  {
    return;
  }
  if (!findStart || !findStop)
  {
    return;
  }
  access.fsoStart = it_start;
  access.netStart = it_start;
  if (data.linkDatas.cend () - it_stop < 10)
  {
    it_stop = data.linkDatas.cend () - 10;
  }
  access.fsoStop = it_stop;
  access.netStop = it_stop;
  access.value = 0;
  access.selected = false;
  BangBangController::State srcAzInitState, srcAzLastState;
  BangBangController::State srcPtInitState, srcPtLastState;
  BangBangController::State dstAzInitState, dstAzLastState;
  BangBangController::State dstPtInitState, dstPtLastState;
  // assume the turntables always track the targets at first time
  bool srcTracking = true;
  bool dstTracking = true;
  srcInLimiter = false;
  dstInLimiter = false;
  Time srcLostTime, dstLostTime;
  Time srcRelinkTime, dstRelinkTime;
  for (CI it = it_start;it != it_stop + 1;++it)
  {
    // considering the angular rate limitter
    double tmp1 = (it->fromSrc.rate.azimuth - satAzBound.yMax) * (it->fromSrc.rate.azimuth - satAzBound.yMin);
    double tmp2 = (it->fromSrc.rate.pitch - satPtBound.yMax) * (it->fromSrc.rate.pitch - satPtBound.yMin);
    double tmp3 = (it->fromDst.rate.azimuth - staAzBound.yMax) * (it->fromDst.rate.azimuth - staAzBound.yMin);
    double tmp4 = (it->fromDst.rate.pitch - staPtBound.yMax) * (it->fromDst.rate.pitch - staPtBound.yMin);
    // considering the angle limitter
    uint8_t state = it->state;
    srcInLimiter = tmp1 <= 0 && tmp2 <= 0 && (state & SRC2DST);
    dstInLimiter = tmp3 <= 0 && tmp4 <= 0 && (state & DST2SRC);
    if (srcInLimiter && dstInLimiter)
    {
      if (!srcTracking || !dstTracking)
      {
        Time now = ToTime (it->time);
        // if source's turntable can not track the target
        // then calculate the transition time from loss state to current state
        if (!srcTracking)
        {
          srcAzLastState = BangBangController::State(ToTime (it->time), it->fromSrc.angle.azimuth, it->fromSrc.rate.azimuth);
          srcPtLastState = BangBangController::State(ToTime (it->time), it->fromSrc.angle.pitch, it->fromSrc.rate.pitch);
          Time t1 = BangBangController::SwingTime (srcAzInitState, srcAzLastState, satAzBound);
          Time t2 = BangBangController::SwingTime (srcPtInitState, srcPtLastState, satPtBound);
          Time t = std::max (t1, t2);
          if ((now - srcLostTime) >= t)
          {
            srcTracking = true;
            srcRelinkTime = now;
          }
        }
        // same with source'turntable
        if (!dstTracking)
        {
          dstAzLastState = BangBangController::State(ToTime (it->time), it->fromDst.angle.azimuth, it->fromDst.rate.azimuth);
          dstPtLastState = BangBangController::State(ToTime (it->time), it->fromDst.angle.pitch, it->fromDst.rate.pitch);
          Time t1 = BangBangController::SwingTime (dstAzInitState, dstAzLastState, staAzBound);
          Time t2 = BangBangController::SwingTime (dstPtInitState, dstPtLastState, staPtBound);
          Time t = std::max (t1, t2);
          if ((now - dstLostTime) >= t)
          {
            dstTracking = true;
            dstRelinkTime = now;
          }
        }
      }
      if (srcTracking && dstTracking && (state & BEYOND_DISTANCE) == 0)
      {
        double distance = it->distance;
        double value = 1.0 / (distance * distance);
        access.value += value;
      }
    }
    else
    {
      if (!srcInLimiter && srcTracking)
      {
        srcTracking = false;
        CI _it = std::max (it - 1, data.linkDatas.cbegin ());
        srcLostTime = ToTime (it->time);
        srcAzInitState = BangBangController::State(
          ToTime (_it->time), 
          _it->fromSrc.angle.azimuth,
          _it->fromSrc.rate.azimuth);
        srcPtInitState = BangBangController::State(
          ToTime (_it->time), 
          _it->fromSrc.angle.pitch,
          _it->fromSrc.rate.pitch);
      }
      if (!dstInLimiter && dstTracking)
      {
        dstTracking = false;
        CI _it = std::max (it - 1, data.linkDatas.cbegin ());
        dstLostTime = ToTime (it->time);
        dstAzInitState = BangBangController::State(
          ToTime (_it->time), 
          _it->fromDst.angle.azimuth,
          _it->fromDst.rate.azimuth);
        dstPtInitState = BangBangController::State(
          ToTime (_it->time), 
          _it->fromDst.angle.pitch,
          _it->fromDst.rate.pitch);
      }
    }
  }
  access.wValue = access.value;
  m_accesses.push_back (access);
}

std::string
AccessManager::ToString (void)
{
  std::stringstream ss;
  ss.setf (std::ios::right);
  struct _AccessData
  {
    Ptr<Turntable> src;
    Ptr<Turntable> dst;
    Time start;
    Time stop;
    _AccessData (const AccessData& data)
    : src   (data.src)
    , dst   (data.dst)
    , start (ToTime (data.fsoStart->time))
    , stop  (ToTime (data.fsoStop->time))
    {
    }
    bool operator< (const _AccessData& data)
    {
      return start < data.start && stop <= data.start;
    }
  };
  typedef std::map<Ptr<Turntable>, std::vector<_AccessData>> _Task;
  _Task _task;
  for (uint32_t i = 0;i < m_accesses.size ();++i)
  {
    if (!m_accesses[i].selected)
    {
      continue;
    }
    _AccessData _access = _AccessData (m_accesses[i]);
    Ptr<Turntable> src = _access.src;
    Ptr<Turntable> dst = _access.dst;
    _task[dst].push_back (_access);
    
  }
  for (_Task::iterator i = _task.begin ();i != _task.end ();++i)
  {
    std::sort (i->second.begin (), i->second.end ());
    ss << std::setw (15) << "Time"   << std::setw (32) << "Destination" << std::endl
       << std::setw (15) << "Source" << std::setw (32) << i->first->GetName () << std::endl;
    for (uint32_t j = 0;j < i->second.size ();++j)
    {
      _AccessData& _access = i->second[j];
      ss << std::setw (15) << _access.src->GetName () << std::setw (32) << ToTime (_access.start) << std::setw (32) << ToTime (_access.stop) << std::endl;
    }
  }
  return ss.str ();
}

}