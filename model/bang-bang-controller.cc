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

#include <iomanip>
#include "ns3/util.h"
#include "bang-bang-controller.h"
#include "turntable.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("BangBangController");

bool
BangBangController::State::PosInBound (const Box& bound) const
{
  return (x <= bound.xMax && x >= bound.xMin);
}

bool
BangBangController::State::VelInBound (const Box& bound) const
{
  return (v <= bound.yMax && v >= bound.yMin);
}

bool
BangBangController::State::AccInBound (const Box& bound) const
{
  return (a <= bound.zMax && a >= bound.zMin);
}

bool
BangBangController::State::InBound (const Box& bound) const
{
  return PosInBound (bound) && VelInBound (bound) && AccInBound (bound);
}

std::string
BangBangController::State::ToString () const
{
  std::stringstream ss;
  ss << "     angle      rate       acc" << std::endl;
  ss << std::setiosflags (std::ios::right | std::ios::fixed) << std::setprecision(4) << std::setw (10);
  ss << RadianToDegree (x);
  ss << std::setiosflags (std::ios::right | std::ios::fixed) << std::setprecision(4) << std::setw (10);
  ss << RadianToDegree (v);
  ss << std::setiosflags (std::ios::right | std::ios::fixed) << std::setprecision(4) << std::setw (10);
  ss << RadianToDegree (a);
  return ss.str ();
}

BangBangController::BangBangController ()
: m_turntable (NULL)
, m_get       (MakeNullCallback<void, Time&, double&, double&> ())
, m_set       (MakeNullCallback<void, const Time&, const double&, const double&> ())
{
}

BangBangController::BangBangController (
  Ptr<Turntable> turntable,
  CbGet get,
  CbSet set
)
: m_turntable (turntable)
, m_get       (get)
, m_set       (set)
, m_initState (State (Seconds (0.0), 0.0, 0.0, 0.0))
, m_currState (State (Seconds (0.0), 0.0, 0.0, 0.0))
, m_lastState (State (Seconds (0.0), 0.0, 0.0, 0.0))
, m_states    (States ())
{
}

void
BangBangController::Update ()
{
  DoCalcState ();
}

void
BangBangController::NotifyTarget ()
{
  // get the last state as the target pointing
  NS_ASSERT (!m_get.IsNull ());
  m_get (m_lastState.t, m_lastState.x, m_lastState.v);
  // limiting the last state
  DoLimit ();
  DoMakeScheme ();
}

void
BangBangController::SetBound (const Box& bound)
{
  m_bound = bound;
}

Box
BangBangController::GetBound () const
{
  return m_bound;
}

Time
BangBangController::SwingTime (
  const CoordTurntable& initPointing,
  const CoordTurntable& lastPointing,
  const Box& azLimiter,
  const Box& ptLimiter)
{
  State initState, lastState;
  initState = State (Seconds (0.0), initPointing.GetAzimuth (), initPointing.GetAzimuthRate (), 0.0);
  lastState = State (Seconds (0.0), lastPointing.GetAzimuth (), lastPointing.GetAzimuthRate (), 0.0);
  Time tAz = SwingTime (initState, lastState, azLimiter);
  initState = State (Seconds (0.0), initPointing.GetPitch (), initPointing.GetPitchRate (), 0.0);
  lastState = State (Seconds (0.0), lastPointing.GetPitch (), lastPointing.GetPitchRate (), 0.0);
  Time tPt = SwingTime (initState, lastState, ptLimiter);
  Time t = std::max (tAz, tPt);
  return t;
}

Time
BangBangController::SwingTime (
  const State& initState,
  const State& lastState,
  const Box& limiter)
{
  double vm = limiter.yMax;
  double am = limiter.zMax;
  double k1 = initState.x - initState.v * initState.v / (2 * am);
  double k2 = initState.x + initState.v * initState.v / (2 * am);
  double c0 = lastState.v - initState.v;
  double c1 = lastState.x - lastState.v * lastState.v / (2 * am) - k1;
  double c2 = lastState.x + lastState.v * lastState.v / (2 * am) - k2;
  double v0 = initState.v;
  double x0 = initState.x;
  double vf = lastState.v;
  double xf = lastState.x;
  if ((c0 >= 0 && c1 > 0) || (c0 < 0 && c2 > 0))
  {
    double vs = sqrt (0.5 * (vf * vf + v0 * v0) + am * (xf - x0));
    if (vs > vm)
    {
      double xs1 = x0 + (vm * vm - v0 * v0) / (2 * am);
      double ts1 = (vm - v0) / am;
      double xs2 = xf + (vf * vf - vm * vm) / (2 * am);
      double ts2 = ts1 + (xs2 - xs1) / vm;
      double tf = ts2 - (vf - vm) / am;
      return Seconds (tf);
    }
    else
    {
      double ts = (vs - v0) / am;
      double tf = 2 * ts + (v0 - vf) / am;
      return  Seconds (tf);
    }
  }
  if ((c0 >= 0 && c1 < 0) || (c0 < 0 && c2 < 0))
  {
    double vs = -sqrt (0.5 * (vf * vf + v0 * v0) + am * (x0 - xf));
    if (vs < -vm)
    {
      double xs1 = x0 - (vm * vm - v0 * v0) / (2 * am);
      double ts1 = (v0 + vm) / am;
      double xs2 = xf - (vf * vf - vm * vm) / (2 * am);
      double ts2 = ts1 - (xs2 - xs1) / vm;
      double tf = ts2 + (vf + vm) / am;
      return Seconds (tf);
    }
    else
    {
      double ts = -(vs - v0) / am;
      double tf = 2 * ts + (vf - v0) / am;
      return Seconds (tf);
    }
  }
  if (c0 >= 0 && c1 == 0)
  {
    double tf = (vf - v0) / am;
    return Seconds (tf);
  }
  if (c0 >= 0 && c2 == 0)
  {
    double tf = -(vf - v0) / am;
    return Seconds (tf);
  }
  NS_ASSERT (false);
}

void
BangBangController::DoCalcState ()
{
  if (m_states.empty ())
    return;
  Time now = Simulator::Now ();
  while (m_states.front ().t <= now)
  {
    m_currState = m_states.front ();
    m_states.pop ();
    if (m_states.empty ())
    {
      break;
    }
  }
  NS_ASSERT (now >= m_currState.t);
  double dt = (now - m_currState.t).GetSeconds ();
  NS_ASSERT (dt >= 0);
  if (dt > 0)
  {
    m_currState.t = now;
    m_currState.x = m_currState.x + m_currState.v * dt + 0.5 * m_currState.a * dt * dt;
    if (m_currState.x > m_bound.xMax)
    {
      m_currState.x = m_bound.xMax;
    }
    if (m_currState.x < m_bound.xMin)
    {
      m_currState.x = m_bound.xMin;
    }
    m_currState.v = m_currState.v + m_currState.a * dt;
    if (m_currState.v > m_bound.yMax)
    {
      m_currState.v = m_bound.yMax;
    }
    if (m_currState.v < m_bound.yMin)
    {
      m_currState.v = m_bound.yMin;
    }
  }
  m_set (m_currState.t, m_currState.x, m_currState.v);
  m_initState = m_currState;
}

void
BangBangController::DoMakeScheme ()
{
  NS_LOG_LOGIC (this << ": Scheme will be calculated.");
  // clear the state's queue
  while (!m_states.empty ())
  {
    m_states.pop ();
  }
  m_states.push (m_initState);
  /*
  * Here is a completed scheme calculation process
  * which is based on Bang-bang control
  * with considering the constraint of velocity
  */
  double vm = m_bound.yMax;
  double am = m_bound.zMax;
  double k1 = m_currState.x - m_currState.v * m_currState.v / (2 * am);
  double k2 = m_currState.x + m_currState.v * m_currState.v / (2 * am);
  double c0 = m_lastState.v - m_currState.v;
  double c1 = m_lastState.x - m_lastState.v * m_lastState.v / (2 * am) - k1;
  double c2 = m_lastState.x + m_lastState.v * m_lastState.v / (2 * am) - k2;
  double v0 = m_currState.v;
  double x0 = m_currState.x;
  double vf = m_lastState.v;
  double xf = m_lastState.x;
  Time dt   = m_lastState.t - m_initState.t;
  if ((c0 >= 0 && c1 > 0) || (c0 < 0 && c2 > 0))
  {
    double vs = sqrt (0.5 * (vf * vf + v0 * v0) + am * (xf - x0));
    if (vs > vm)
    {
      double xs1 = x0 + (vm * vm - v0 * v0) / (2 * am);
      double ts1 = (vm - v0) / am;
      double xs2 = xf + (vf * vf - vm * vm) / (2 * am);
      double ts2 = ts1 + (xs2 - xs1) / vm;
      double tf = ts2 - (vf - vm) / am;
      if (Seconds (tf) > dt)
      {
        Speeding (ts1);
        Keeping (ts2 - ts1);
        Braking (tf - ts2);
      }
      Tracking ();
      Staying ();
      return;
    }
    else
    {
      double ts = (vs - v0) / am;
      double tf = 2 * ts + (v0 - vf) / am;
      if (Seconds (tf) > dt)
      {
        Speeding (ts);
        Braking (tf - ts);
      }
      Tracking ();
      Staying ();
      return;
    }
  }
  if ((c0 >= 0 && c1 < 0) || (c0 < 0 && c2 < 0))
  {
    double vs = -sqrt (0.5 * (vf * vf + v0 * v0) + am * (x0 - xf));
    if (vs < -vm)
    {
      double xs1 = x0 - (vm * vm - v0 * v0) / (2 * am);
      double ts1 = (v0 + vm) / am;
      double xs2 = xf - (vf * vf - vm * vm) / (2 * am);
      double ts2 = ts1 - (xs2 - xs1) / vm;
      double tf = ts2 + (vf + vm) / am;
      if (Seconds (tf) > dt)
      {
        Braking (ts1);
        Keeping (ts2 - ts1);
        Speeding (tf - ts2);
      }
      Tracking ();
      Staying ();
      return;
    }
    else
    {
      double ts = -(vs - v0) / am;
      double tf = 2 * ts + (vf - v0) / am;
      if (Seconds (tf) > dt)
      {
        Braking (ts);
        Speeding (tf - ts);
      }
      Tracking ();
      Staying ();
      return;
    }
  }
  if (c0 >= 0 && c1 == 0)
  {
    double tf = (vf - v0) / am;
    if (Seconds (tf) > dt)
    {
      Speeding (tf);
    }
    Tracking ();
    Staying ();
  }
  if (c0 >= 0 && c2 == 0)
  {
    double tf = -(vf - v0) / am;
    if (Seconds (tf) > dt)
    {
      Braking (tf);
    }
    Tracking ();
    Staying ();
    return;
  }
  Tracking ();
  Staying ();
  return;
}

void
BangBangController::Speeding (double t)
{
  if (t < 0)
  {
    t = 0;
  }
  if (t == 0)
  {
    return;
  }
  State _state = m_states.back ();
  _state.a = m_bound.zMax;
  m_states.push (_state);
  State state;
  state.t = _state.t + Seconds (t);
  state.x = _state.x + _state.v * t + 0.5 * m_bound.zMax * t * t;
  state.v = _state.v + m_bound.zMax * t;
  state.a = 0.0;
  m_states.push (state);
}

void
BangBangController::Braking (double t)
{
  if (t < 0)
  {
    t = 0;
  }
  if (t == 0)
  {
    return;
  }
  State _state = m_states.back ();
  _state.a = m_bound.zMin;
  m_states.push (_state);
  State state;
  state.t = _state.t + Seconds (t);
  state.x = _state.x + _state.v * t + 0.5 * m_bound.zMin * t * t;
  state.v = _state.v + m_bound.zMin * t;
  state.a = m_bound.zMin;
  // state.event = Simulator::Schedule (state.t - Now (), &BangBangController::DoSet, this, state);
  m_states.push (state);
}

void
BangBangController::Keeping (double t)
{
  if (t < 0)
  {
    t = 0;
  }
  if (t == 0)
  {
    return;
  }
  State _state = m_states.back ();
  _state.a = 0.0;
  m_states.push (_state);
  State state;
  state.t = _state.t + Seconds (t);
  state.x = _state.x + _state.v * t;
  state.v = _state.v;
  state.a = 0.0;
  // state.event = Simulator::Schedule (state.t - Now (), &BangBangController::DoSet, this, state);
  m_states.push (state);
}

void
BangBangController::Tracking ()
{
  // m_lastState.event = Simulator::Schedule (m_lastState.t - Now (), &BangBangController::DoSet, this, m_lastState);
  if (m_lastState.t < m_states.back ().t)
  {
    m_lastState.t = m_states.back ().t;
  }
  m_states.push (m_lastState);
}

void
BangBangController::Staying ()
{
  State _state = m_states.back ();
  State state;
  state.v = 0.0;
  double t;
  double a;
  if (_state.v > 0)
  {
    a = m_bound.zMin;
    t = - _state.v / a;
  }
  else if (_state.v < 0)
  {
    a = m_bound.zMax;
    t = - _state.v / a;
  }
  else
  {
    a = 0;
    t = 0;
  }
  _state.a = a;
  m_states.push (_state);
  state.t = _state.t + Seconds (t);
  state.x = _state.x + _state.v * t + 0.5 * a * t * t;
  state.v = 0.0;
  state.a = 0.0;
  m_states.push (state);
}

void
BangBangController::DoLimit ()
{
  if (!m_lastState.PosInBound (m_bound))
  {
    NS_LOG_WARN ("The last state's position is out of limiter.");
    m_lastState.x = m_lastState.x > 0 ? m_bound.xMax : m_bound.xMin;
  }
  if (!m_lastState.VelInBound (m_bound))
  {
    NS_LOG_WARN ("The last state's velocity is out of limiter.");
    m_lastState.v = m_lastState.v > 0 ? m_bound.yMax : m_bound.yMin;
  }
  double x = m_lastState.x > 0 ? (m_bound.xMax - m_lastState.x) : (m_bound.xMin - m_lastState.x);
  double a = m_lastState.x > 0 ? m_bound.zMax : m_bound.zMin;
  NS_ASSERT (a * x >= 0);
  double v = sqrt (2 * a * x);
  if (m_lastState.v > 0)
  {
    if (m_lastState.v > std::min (v, m_bound.yMax))
    {
      NS_LOG_WARN ("The last state's position and velocity are in out of limiter");
      m_lastState.v = std::min (v, m_bound.yMax);
    }
  }
  else if (m_lastState.v < 0)
  {
    if (m_lastState.v < std::max (-v, m_bound.yMin))
    {
      NS_LOG_WARN ("The last state's position and velocity are in out of limiter");
      m_lastState.v = std::max (-v, m_bound.yMin);
    }
  }
}

void
BangBangController::DoGet (State& state)
{
  m_get (state.t, state.x, state.v);
}

void
BangBangController::DoSet (const State& state)
{
  m_set (state.t, state.x, state.v);
}

std::ostream&
operator<< (std::ostream& os, const BangBangController::State& state)
{
  os << state.ToString ();
  return os;
}

}