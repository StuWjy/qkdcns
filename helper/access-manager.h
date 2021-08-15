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

#ifndef ACCESS_MANAGER_H
#define ACCESS_MANAGER_H

#include <map>
#include <vector>
#include <deque>
#include "ns3/nstime.h"
#include "adi-type-define.h"
#include "ns3/coordinate-turntable.h"

namespace ns3 {

class QkdSatellite;
class QkdStation;
class Turntable;

class AccessManager
{
public:
  struct AccessData
  {
    uint32_t i;
    Ptr<Turntable> src;
    Ptr<Turntable> dst;
    adi::LinkDatas::const_iterator fsoStart;
    adi::LinkDatas::const_iterator fsoStop;
    adi::LinkDatas::const_iterator netStart;
    adi::LinkDatas::const_iterator netStop;
    double value;
    double wValue;
    bool selected;
    bool operator< (const AccessData& access) const;
  };
  struct TurntableState
  {
    Time time;
    CoordTurntable pointing;
    bool operator< (const TurntableState& state) const;
  };
  typedef std::deque<AccessData> AccessList;
  typedef AccessList::iterator AccessIter;
  typedef std::vector<TurntableState> TurntableStates;
  typedef std::map<Ptr<Turntable>, TurntableStates> TurntableStateList;
  AccessManager (){}
  ~AccessManager (){}
  static AccessList& SelectTasks (const adi::LinkInfoList& datas, const std::vector<bool>& satisfied);
private:
  static void CalcScheme (void);
  static void AssignTurntableTask (void);
  static std::string ToString (void);
  static void AddAccessData (const adi::LinkInfoList& datas, const std::vector<bool>& satisfied);
  static void AddAccessData (const adi::LinkInfo& data, uint32_t i);
private:
  static TurntableStates::iterator TryInsert (
    Ptr<Turntable>        turntable,
    const TurntableState& startState,
    const TurntableState& stopState);
  static bool TryInsert (
    Ptr<Turntable>        src,
    const TurntableState& srcStartState,
    const TurntableState& srcStopState,
    Ptr<Turntable>        dst,
    const TurntableState& dstStartState,
    const TurntableState& dstStopState);
  static AccessList m_accesses;
  static TurntableStateList m_turntableStateList;
};

}

#endif /* ACCESS_MANAGER_H */