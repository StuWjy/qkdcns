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

#include "ns3/node.h"
#include "ns3/simulator.h"
#include "ns3/net-device.h"
#include "ns3/adi-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/output-stream-wrapper.h"
#include "qkd-node.h"
#include "fso-device.h"
#include "fso-channel.h"
#include "fso-channel-list.h"
#include "fso-propagation-loss-model.h"
#include "fso-propagation-delay-model.h"
#include "turntable.h"
#include "constant.h"
#include "util.h"
#include "adi-constant.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("FsoChannel");

NS_OBJECT_ENSURE_REGISTERED (FsoChannel);

std::string FsoLinkStateToString (const FsoLinkState& state)
{
  switch (state)
  {
    case UNCONNECTED:     return "UNCONNECTED";
    case CONNECTING:      return "CONNECTING";
    case RECONNECTING:    return "RECONNECTING";
    case DISCONNECTED:    return "DISCONNECTED";
    case CONNECTED:       return "CONNECTED";
    case CONNECTION_DONE: return "CONNECTION_DONE";
    default:              return "UNCORRECT_STATE";
  }
}

TypeId
FsoChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::FsoChannel")
    .SetParent<Channel> ()
    .SetGroupName ("Fso")
    .AddConstructor<FsoChannel> ()
    .AddAttribute ("SimulationStep", "The time step of simulation",
                   TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&FsoChannel::m_step),
                   MakeTimeChecker (Seconds (0.01), Seconds (1.0)))
  ;
  return tid;
}

FsoChannel::FsoChannel ()
// : m_active    (false)
: m_index     (0)
, m_link      (Link ())
, m_nDevices  (0)
, m_loss      (CreateObject<FsoPropagationLossModel> (this))
, m_delay     (CreateObject<FsoPropagationDelayModel> ())
, m_updateEvent(EventId ())
{
  NS_LOG_FUNCTION (this << Now ());
}

FsoChannel::FsoChannel (const adi::LinkInfo& data)
: m_index     (0)
, m_link      (Link ())
, m_nDevices  (0)
, m_loss      (CreateObject<FsoPropagationLossModel> (this))
, m_delay     (CreateObject<FsoPropagationDelayModel> ())
, m_updateEvent(EventId ())
{
  ;
}

FsoChannel::~FsoChannel ()
{
  NS_LOG_FUNCTION (this << Now ());
}

// void
// FsoChannel::DoEnable (void)
// {
//   NS_LOG_FUNCTION (this);
//   uint32_t src = m_link.m_tx->GetTurntable ()->GetAdiTurntable ()->GetId ();
//   uint32_t dst = m_link.m_rx->GetTurntable ()->GetAdiTurntable ()->GetId ();
//   NS_LOG_INFO (ToTime (Now ()) << ": enable fso channel, tx: " << src << " rx: " << dst);
//   // m_active = true;
// }

// void
// FsoChannel::DoDisable (void)
// {
//   NS_LOG_FUNCTION (this);
//   uint32_t src = m_link.m_tx->GetTurntable ()->GetAdiTurntable ()->GetId ();
//   uint32_t dst = m_link.m_rx->GetTurntable ()->GetAdiTurntable ()->GetId ();
//   NS_LOG_INFO (ToTime (Now ()) << ": disable fso channel, tx: " << src << " rx: " << dst);
//   // m_active = false;
// }

void
FsoChannel::DoSetTxDevice (Ptr<FsoDevice> fsoDevice)
{
  NS_LOG_FUNCTION (this << fsoDevice);
  if (m_link.m_tx == 0)
  {
    m_nDevices++;
    NS_ASSERT (m_nDevices <= N_DEVICES);
  }
  NS_ASSERT (fsoDevice->GetObject<FsoTxDevice> ());
  m_link.m_tx = fsoDevice->GetObject<FsoTxDevice> ();
  m_link.m_tx->SetChannel (this);
}

void
FsoChannel::DoSetRxDevice (Ptr<FsoDevice> fsoDevice)
{
  NS_LOG_FUNCTION (this << fsoDevice);
  if (m_link.m_rx == 0)
  {
    m_nDevices++;
    NS_ASSERT (m_nDevices <= N_DEVICES);
  }
  NS_ASSERT (fsoDevice->GetObject<FsoRxDevice> ());
  m_link.m_rx = fsoDevice->GetObject<FsoRxDevice> ();
  m_link.m_rx->SetChannel (this);
}

Ptr<FsoTxDevice>
FsoChannel::GetTxDevice (void) const
{
  return m_link.m_tx;
}

Ptr<FsoRxDevice>
FsoChannel::GetRxDevice (void) const
{
  return m_link.m_rx;
}

void
FsoChannel::SetPropagationLossModel (const Ptr<FsoPropagationLossModel> loss)
{
  NS_LOG_FUNCTION (this << loss);
  m_loss = loss;
}

void
FsoChannel::SetPropagationDelayModel (const Ptr<FsoPropagationDelayModel> delay)
{
  NS_LOG_FUNCTION (this << delay);
  m_delay = delay;
}

std::size_t
FsoChannel::GetNDevices (void) const
{
  NS_LOG_FUNCTION (this);
  return m_nDevices;
}

Ptr<NetDevice>
FsoChannel::GetDevice (std::size_t i) const
{
  NS_LOG_FUNCTION (this << i);
  NS_ASSERT_MSG(i < N_DEVICES, "Only two devices permitted");
  if (i == 0)
  {
    return GetTxDevice ()->GetNetDevice ();
  }
  else
  {
    return GetRxDevice ()->GetNetDevice ();
  }
}

int64_t
FsoChannel::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  int64_t currentStream = stream;
  currentStream += m_loss->AssignStreams (stream);
  return (currentStream - stream);
}

double
FsoChannel::CalcChannelLoss ()
{
  return m_loss->CalcRxPower (m_link.m_currDistance, NULL, NULL);
}

double
FsoChannel::GetDistance (void) const
{
  return m_link.m_currDistance;
}

void
FsoChannel::Attach (Ptr<FsoDevice> tx, Ptr<FsoDevice> rx, const adi::LinkDatas& data)
{
  NS_LOG_FUNCTION (this << tx << rx);
  NS_ASSERT (tx->GetObject<NetDevice> ());
  NS_ASSERT (rx->GetObject<NetDevice> ());
  DoSetTxDevice (tx);
  DoSetRxDevice (rx);
  DoAppend (data);
}

void
FsoChannel::StartSending (Ptr<FsoTxDevice> txFso)
{
  NS_LOG_FUNCTION (this << txFso);
  m_sendStart = Now ();
  m_link.m_state = CONNECTED;
  m_sendingEvent = Simulator::Schedule (
    m_delay->GetDelay (m_link.m_currDistance),
    &FsoRxDevice::NotifyConnectionSucceeded,
    m_link.m_rx
    );
}

void
FsoChannel::StopSending (Ptr<FsoTxDevice> txFso)
{
  NS_LOG_FUNCTION (this << txFso);
  m_sendStop = Now ();
}

void
FsoChannel::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
  Channel::DoInitialize ();
}

void
FsoChannel::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  // m_active = false;
  m_link = Link ();
  m_nDevices = 0;
  m_loss = 0;
  m_delay = 0;
  m_updateEvent = EventId ();
  Channel::DoDispose ();
}

void
FsoChannel::DoUpdate ()
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (m_link.m_tx);
  NS_ASSERT (m_link.m_rx);
  NS_ASSERT (m_loss);
  NS_ASSERT (m_delay);
  NS_ASSERT (m_nDevices == N_DEVICES);
  Time now = Now ();
  DoSwing ();
  // cannot exceeds the simulation stop time
  NS_ASSERT (now < SimulationStop);
  double a1 = m_link.m_txCurrTarget.GetIncludedAngle (m_link.m_tx->GetTurntable ()->GetPointing ());
  double a2 = m_link.m_rxCurrTarget.GetIncludedAngle (m_link.m_rx->GetTurntable ()->GetPointing ());
  //
  // Decide the current state of fso link
  //
  bool tracking = false;
  if (a1 < 1e-6 && a2 < 1e-6)
  {
    tracking = true;
  }
  if (tracking == true && m_link.m_currDistance <= adi::K_MAX_DISTANCE)
  {
    if (m_link.m_state == UNCONNECTED || m_link.m_state == DISCONNECTED)
    {
      switch (m_link.m_state)
      {
        case UNCONNECTED: m_link.m_state = CONNECTING;  break;
        case DISCONNECTED:m_link.m_state = RECONNECTING;break;
        default:  NS_ASSERT (false);  break;
      }
      Time period = Seconds (5.0);//std::max (m_link.m_tx->GetTurntable ()->GetAcqTime (),m_link.m_rx->GetTurntable ()->GetAcqTime ());
      if (m_link.m_state == CONNECTING)
      {
        Simulator::Schedule (Seconds (0.0), &FsoTxDevice::NotifyConnectionStarted, m_link.m_tx);
        Simulator::Schedule (Seconds (0.0), &FsoRxDevice::NotifyConnectionStarted, m_link.m_rx);
      }
      m_connectingEvent = Simulator::Schedule (period, &FsoTxDevice::NotifyConnectionSucceeded, m_link.m_tx);
    }
  }
  else
  {
    if (m_link.m_state == CONNECTING || m_link.m_state == RECONNECTING)
    {
      m_connectingEvent.Cancel ();
      if (m_link.m_state == CONNECTING)
      {
        m_link.m_state = UNCONNECTED;
      }
      else
      {
        m_link.m_state = DISCONNECTED;
      }
    }
    else if (m_link.m_state == CONNECTED)
    {
      m_link.m_state = DISCONNECTED;
      m_link.m_tx->NotifyConnectionFailed ();
    }
  }
  if (m_linkDatas.empty ())
  {
    Simulator::Schedule (m_step, &FsoTxDevice::NotifyConnectionFinished, m_link.m_tx);
    m_link.m_state = CONNECTION_DONE;
  }
  switch (m_link.m_state)
  {
    case UNCONNECTED:     NS_LOG_INFO (Now ().As (Time::S) << " Unconnected "     << m_link.m_currDistance);  break;
    case DISCONNECTED:    NS_LOG_INFO (Now ().As (Time::S) << " Disconnected "    << m_link.m_currDistance);  break;
    case CONNECTING:      NS_LOG_INFO (Now ().As (Time::S) << " Connecting "      << m_link.m_currDistance);  break;
    case RECONNECTING:    NS_LOG_INFO (Now ().As (Time::S) << " Reconnecting "    << m_link.m_currDistance);  break;
    case CONNECTED:       NS_LOG_INFO (Now ().As (Time::S) << " Connected "       << m_link.m_currDistance);   break;
    case CONNECTION_DONE: NS_LOG_INFO (Now ().As (Time::S) << " Connection done " << m_link.m_currDistance);   break;
    default:              break;
  }
  NS_LOG_INFO (m_link.m_txCurrTarget);
  NS_LOG_INFO (m_link.m_tx->GetTurntable ()->GetPointing ());
  NS_LOG_INFO (m_link.m_rxCurrTarget);
  NS_LOG_INFO (m_link.m_rx->GetTurntable ()->GetPointing ());
  m_link.m_tx->GetTurntable ()->NotifyToRecord (m_link.m_state, m_link.m_currDistance);
  m_link.m_rx->GetTurntable ()->NotifyToRecord (m_link.m_state, m_link.m_currDistance);
  if (m_linkDatas.empty ())
  {
    return;
  }
  Simulator::Schedule (m_linkDatas.front ().t - now, &FsoChannel::DoUpdate, this);
}

void
FsoChannel::DoAppend (const adi::LinkDatas& data)
{ 
  NS_LOG_FUNCTION (this);
  Time now = Now ();
  for (uint32_t i = 0;i < data.size ();++i)
  {
    NS_ASSERT (data[i].time > now);
    Time t = ToTime (data[i].time);
    if (i > 0)
    {
      NS_ASSERT (data[i].time > data[i - 1].time);
    }
    m_linkDatas.push (LinkData {t, data[i].distance, data[i].fromSrc, data[i].fromDst});
  }
  DoUpdate ();
}

void
FsoChannel::DoSwing ()
{
  NS_LOG_FUNCTION (this);
  m_link.m_txCurrTarget = m_linkDatas.front ().fromSrc;
  m_link.m_rxCurrTarget = m_linkDatas.front ().fromDst;
  m_link.m_currDistance = m_linkDatas.front ().distance;
  m_linkDatas.pop ();
  if (!m_linkDatas.empty ())
  {
    m_link.m_txPredTarget = m_linkDatas.front ().fromSrc;
    m_link.m_rxPredTarget = m_linkDatas.front ().fromDst;
    m_link.m_predDistance = m_linkDatas.front ().distance;
    m_link.m_tx->GetTurntable ()->SetTargetPointing (m_linkDatas.front ().t, m_link.m_txPredTarget);
    m_link.m_rx->GetTurntable ()->SetTargetPointing (m_linkDatas.front ().t, m_link.m_rxPredTarget);
  }
}

} //namespace ns3