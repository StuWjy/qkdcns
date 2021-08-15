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

#ifndef BANG_BANG_CONTROLLER_H
#define BANG_BANG_CONTROLLER_H

#include <queue>
#include "ns3/object.h"
#include "ns3/box.h"
#include "ns3/simulator.h"
#include "ns3/callback.h"
#include "coordinate-turntable.h"

namespace ns3 {

class Turntable;

class BangBangController
{
public:
  enum Scheme
  {
    KEEPING,    //!< keep speed
    SPEEDING,   //!< speed up
    BRAKING,    //!< speed down
    TRACKING    //!< tracking, maintain angle and speed
  };

  struct State
  {
    // EventId event;
    Time   t;
    double x;
    double v;
    double a;
    State ()
    // : event (EventId ())
    : t (0.0)
    , x (0.0)
    , v (0.0)
    , a (0.0)
    {
    }
    State (const Time& _t, const double& _x, const double& _v, const double& _a = 0.0)
    // : event (EventId ())
    : t (_t)
    , x (_x)
    , v (_v)
    , a (_a)
    {
    }
    std::string ToString () const;
    bool PosInBound (const Box& bound) const;
    bool VelInBound (const Box& bound) const;
    bool AccInBound (const Box& bound) const;
    bool InBound (const Box& bound) const;
  };
  typedef Callback<void, Time&, double&, double&> CbGet;
  typedef Callback<void, const Time&, const double&, const double&> CbSet;
  BangBangController ();
  BangBangController (Ptr<Turntable> turntable, CbGet get, CbSet set);
  void Update ();
  void NotifyTarget ();
  void SetBound (const Box& bound);
  Box GetBound () const;
  static Time SwingTime (
    const CoordTurntable& initPointing,
    const CoordTurntable& lastPointing,
    const Box& azLimiter,
    const Box& ptLimiter);
  static Time SwingTime (
    const State& initState,
    const State& lastState,
    const Box& limiter);
private:
  void DoCalcState ();
  void DoMakeScheme ();
  void Speeding (double t);
  void Braking (double t);
  void Keeping (double t);
  void Tracking ();
  void Staying ();
  void DoLimit ();
  void DoGet (State& state);
  void DoSet (const State& state);
private:
  typedef std::queue<State> States;
  Ptr<Turntable>  m_turntable;
  Box     m_bound;
  CbGet   m_get;
  CbSet   m_set;
  State   m_initState;
  State   m_currState;
  State   m_lastState;
  States  m_states;
};

std::ostream& operator<< (std::ostream& os, const BangBangController::State& state);

} // namespace BANG_BANG_CONTROLLER_H

#endif /* BANG_BANG_CONTROLLER_H */