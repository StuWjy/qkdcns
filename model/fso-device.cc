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

#include "ns3/boolean.h"
#include "ns3/enum.h"
#include "ns3/double.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/node.h"
#include "coordinate-turntable.h"
#include "fso-device.h"
#include "qkd-node.h"
#include "turntable.h"
#include "fso-channel.h"
#include "q3p-l3-protocol.h"
#include "constant.h"
#include "util.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("FsoDevice");

NS_OBJECT_ENSURE_REGISTERED (FsoDevice);

TypeId
FsoDevice::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::FsoDevice")
    .SetParent<Object> ()
    .SetGroupName ("Fso")
    .AddTraceSource ("ConnectionStarted",
                     "Trace of started connection for this free-space-optics device",
                     MakeTraceSourceAccessor (&FsoDevice::m_connectionStartedTrace),
                     "ns3::FsoDevice::SentTracedCallback")
    .AddTraceSource ("ConnectionFinished",
                     "Trace of finished connection for this free-space-optics device",
                     MakeTraceSourceAccessor (&FsoDevice::m_connectionFinishedTrace),
                     "ns3::FsoDevice::SentTracedCallback")
    .AddTraceSource ("ConnectionSucceeded",
                     "Re-connection for this free-space-optics device",
                     MakeTraceSourceAccessor (&FsoDevice::m_connectionSucceededTrace),
                     "ns3::FsoDevice::SentTracedCallback")
    .AddTraceSource ("ConnectionFailed",
                     "Connection failed for this free-space-optics device",
                     MakeTraceSourceAccessor (&FsoDevice::m_connectionFailedTrace),
                     "ns3::FsoDevice::SentTracedCallback")
  ;
  return tid;
}

FsoDevice::FsoDevice ()
// : m_ifIndex   (0)
: m_node      (0)
, m_qkdDevice (0)
, m_netDevice (0)
, m_turntable (0)
, m_channel   (0)
, m_connectionStarted (MakeNullCallback<void> ())
, m_connectionFinished (MakeNullCallback<void> ())
, m_connectionSucceeded (MakeNullCallback<void> ())
, m_connectionFailed (MakeNullCallback<void> ())
, m_connectionEvent (EventId ())
, m_swingEvent (EventId ())
{
  NS_LOG_FUNCTION (this);
}

FsoDevice::~FsoDevice ()
{
  NS_LOG_FUNCTION (this);
}

int64_t
FsoDevice::DoAssignStream (int64_t stream)
{
  return 0;
}

void
FsoDevice::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);
  NS_ASSERT (node);
  m_node = node;
  // DoSetup ();
}

Ptr<Node>
FsoDevice::GetNode () const
{
  return m_node;
}

void
FsoDevice::SetQkdDevice (Ptr<QkdDevice> qkdDevice)
{
  NS_LOG_FUNCTION (this);
  m_qkdDevice = qkdDevice;
}

Ptr<QkdDevice>
FsoDevice::GetQkdDevice () const
{
  return m_qkdDevice;
}

void
FsoDevice::SetNetDevice (Ptr<NetDevice> netDevice)
{
  NS_LOG_FUNCTION (this << netDevice);
  NS_ASSERT (netDevice);
  m_netDevice = netDevice;
}

Ptr<NetDevice>
FsoDevice::GetNetDevice () const
{
  return m_netDevice;
}

void
FsoDevice::SetChannel (Ptr<FsoChannel> channel)
{
  NS_LOG_FUNCTION (this << channel);
  NS_ASSERT (channel);
  m_channel = channel;
}

Ptr<FsoChannel>
FsoDevice::GetChannel () const
{
  return m_channel;
}

void
FsoDevice::SetTurntable (Ptr<Turntable> turntable)
{
  NS_LOG_FUNCTION (this << turntable);
  NS_ASSERT (turntable);
  m_turntable = turntable;
}

Ptr<Turntable>
FsoDevice::GetTurntable () const
{
  return m_turntable;
}

void
FsoDevice::SetConnectCallback (
  Callback<void> connectionStarted,
  Callback<void> connectionFinished,
  Callback<void> connectionSucceeded,
  Callback<void> connectionFailed)
{
  NS_LOG_FUNCTION (this << &connectionStarted << &connectionFinished << &connectionSucceeded << &connectionFailed);
  m_connectionStarted   = connectionStarted;
  m_connectionFinished  = connectionFinished;
  m_connectionSucceeded = connectionSucceeded;
  m_connectionFailed    = connectionFailed;
}

Time
FsoDevice::GetRoundPeriod (void) const
{
  return m_timeStop - m_timeStart;
}

adi::Interval
FsoDevice::GetInterval () const
{
  return adi::Interval (ToTime (m_timeStart), ToTime (m_timeStop));
}

void
FsoDevice::GetRoundInterval (Time& start, Time& stop) const
{
  start = m_timeStart;
  stop  = m_timeStop;
}

// bool Compare (const std::pair<Time, CoordTurntable>& first, const std::pair<Time, CoordTurntable>& second)
// {
//   return first.first < second.first;
// }

// void
// FsoDevice::AppendTarget (const Time& time, const CoordTurntable& coord)
// {
//   m_targets.push_back (std::make_pair (time, coord));
//   // resort m_targets according to the time
//   std::sort (m_targets.begin (), m_targets.end (), &Compare);
//   // if there exists swing event, cancel it and re-schedule
//   if (m_swingEvent.IsRunning ())
//   {
//     m_swingEvent.Cancel ();
//   }
//   m_swingEvent = Simulator::Schedule (Seconds (0.0), &FsoDevice::SwingToTarget, this);
// }

// void
// FsoDevice::SwingToTarget (void)
// {
//   // if there is no target remains and target-swinging event
//   if (!m_targets.empty () && !m_swingEvent.IsRunning ())
//   {
//     m_turntable->SetTargetPointing (m_targets.front ().first, m_targets.front ().second);
//     Time delay = m_targets.front ().first - Now ();
//     m_swingEvent = Simulator::Schedule (delay, &FsoDevice::SwingToTarget, this);
//     m_targets.erase (m_targets.begin ());
//   }
// }

void
FsoDevice::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
  Object::DoInitialize ();
}

void
FsoDevice::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_node = 0;
  m_netDevice = 0;
  m_turntable = 0;
  m_channel = 0;
  m_connectionStarted = MakeNullCallback<void> ();
  m_connectionFinished = MakeNullCallback<void> ();
  m_connectionSucceeded = MakeNullCallback<void> ();
  m_connectionFailed = MakeNullCallback<void> ();
  m_connectionEvent = EventId ();
  Object::DoDispose ();
}

void
FsoDevice::NotifyNewAggregate ()
{
  NS_LOG_FUNCTION (this);
  Ptr<NetDevice> netDevice = GetObject<NetDevice> ();
  if (netDevice)
  {
    SetNetDevice (netDevice);
  }
  Ptr<Turntable> turntable = GetObject<Turntable> ();
  if (turntable)
  {
    SetTurntable (turntable);
  }
}

int64_t
FsoDevice::AssignStream (int64_t start)
{
  return DoAssignStream (start);
}

std::ostream & operator<< (std::ostream &os, const Ptr<FsoDevice> device)
{
  os << PeekPointer (device);
  return os;
}

}