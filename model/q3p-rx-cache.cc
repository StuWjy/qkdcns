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
#include "ns3/packet.h"
#include "q3p-header.h"
#include "q3p-rx-cache.h"
#include "q3p-tag.h"
#include "q3p-l3-protocol.h"
#include "qkd-key-pool.h"
#include "fso-rx-device.h"
#include "fso-channel.h"
#include "turntable.h"
#include "util.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Q3pRxCache);

NS_LOG_COMPONENT_DEFINE ("Q3pRxCache");

TypeId
Q3pRxCache::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Q3pRxCache")
    .SetParent<Q3pCache> ()
    .SetGroupName ("Qkd")
    .AddConstructor<Q3pRxCache> ()
  ;
  return tid;
}

Q3pRxCache::Q3pRxCache ()
: Q3pCache ()
{
  NS_LOG_FUNCTION (this);
  Flush ();
}

Q3pRxCache::~Q3pRxCache ()
{
  NS_LOG_FUNCTION (this);
}

void
Q3pRxCache::Flush (void)
{
  NS_LOG_FUNCTION (this);
  m_recvCountTimeSync = 0;
  m_sentCountPulseLocating = 0;
  m_recvCountBasisSifting = 0;
  m_sentCountKeySifting = 0;
  m_recvCountErrorCorrection = 0;
  Q3pCache::Flush ();
}

void
Q3pRxCache::NotifyConnectionStarted (void)
{
  NS_LOG_FUNCTION (this);
  m_processing = true;
}

void
Q3pRxCache::NotifyConnectionFinished (void)
{
  NS_LOG_FUNCTION (this);
  m_processing = false;
}

void
Q3pRxCache::NotifyConnectionSucceeded (void)
{
  NS_LOG_FUNCTION (this);
}

void
Q3pRxCache::NotifyConnectionFailed (void)
{
  NS_LOG_FUNCTION (this);
}

void
Q3pRxCache::NotifyNewPacket (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this);
  Q3pHeader hdr;
  packet->RemoveHeader (hdr);
  switch (hdr.GetType ())
  {
    case Q3pHeader::Q3P_TIME_SYNC_TAGGING:      DoHandleTimeSynchronization (packet); break;
    case Q3pHeader::Q3P_BASIS_SIFTING:          DoHandleBasisSifting (packet);        break;
    case Q3pHeader::Q3P_ERROR_CORRECTION:       DoHandleErrorCorrection (packet);     break;
    case Q3pHeader::Q3P_PRIVACY_AMPLIFICATION:  DoHandlePrivacyAmplification (packet);break;
    default:                                    NS_ASSERT (false);                    break;
  }
}

void
Q3pRxCache::NotifyNewDetectionEvent (
  const Time &begin,
  const Time &end,
  const double &dark,
  const double &loss)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (GetFsoDevice ()->GetObject<FsoRxDevice> ());
  m_start = begin;
  m_stop = end;
  Ptr<FsoRxDevice> rx = GetFsoDevice ()->GetObject<FsoRxDevice> ();
  NS_ASSERT (m_stop > m_start);
  double seconds = (m_stop - m_start).GetSeconds ();
  double events = seconds * m_q3p->GetFrequency ();
  double gate = rx->GetDetectorThreshold ();
  double Qv = dark * gate;
  double err = rx->GetDetectorError ();
  double Qs = Qv + 1 - exp (-loss * m_q3p->GetSignalPhotons ());
  double EsQs = m_q3p->GetErrorRate () * Qv + err * (1 - exp (-loss * m_q3p->GetSignalPhotons ()));
  double Qw = Qv + 1 - exp (-loss * m_q3p->GetDecoyPhotons ());
  double EwQw = m_q3p->GetErrorRate () * Qv + err * (1 - exp (-loss * m_q3p->GetDecoyPhotons ()));
  double tmp = events * m_q3p->GetVacuumProbability () * Qv;
  m_eventVac[Processing] += tmp;
  m_errorVac[Processing] += tmp * m_q3p->GetErrorRate ();
  tmp = events * m_q3p->GetSignalProbability ();
  m_eventSig[Processing] += tmp * Qs;
  m_errorSig[Processing] += tmp * EsQs;
  tmp = events * m_q3p->GetDecoyProbability ();
  m_eventDec[Processing] += tmp * Qw;
  m_errorDec[Processing] += tmp * EwQw;
  m_events[Processing] += events;
  m_detectionEvents[Processing] += m_eventSig[Processing] + m_eventDec[Processing] + m_eventVac[Processing];
  m_correctEvents[Processing] += m_q3p->GetBasisRatio () * (m_eventSig[Processing] - m_errorSig[Processing]);

}

void
Q3pRxCache::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  Q3pCache::DoInitialize ();
}

void
Q3pRxCache::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Q3pCache::DoDispose ();
}

void
Q3pRxCache::NotifyNewAggregate (void)
{
  NS_LOG_FUNCTION (this);
}

void
Q3pRxCache::DoUpdate (void)
{
  NS_LOG_FUNCTION (this);
}

void
Q3pRxCache::DoHandleTimeSynchronization (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);
  if (!m_processing)
  {
    return;
  }
  NS_ASSERT (GetFsoDevice ()->GetObject<FsoRxDevice> ());
  NS_ASSERT (GetFsoDevice ()->GetChannel ());
  m_recvCountTimeSync++;
  /**
   * Here, we assume the time-sync tagging range of each packet
   * is short enough. Therefore, the free-space-optics link
   * can be considered as a link with fixed loss.
   * What we do is to update loss and other parameters.
   */
  Q3pTagTimeSync tag;
  NS_ASSERT (packet->RemovePacketTag (tag));
  /**
   * We assume the time-sync tags are consistent.
   */
  Time start = tag.GetStartTime ();
  Time stop = tag.GetStopTime ();
  uint32_t pulses = tag.GetPulses ();
  Time period = stop - start;
  NS_LOG_LOGIC (
    "Received the time-sync-tag"
    " from " << start.As (Time::S) <<
    " to " << stop.As (Time::S) <<
    " contains " << pulses << "-pulses"
    ", total " << period.As (Time::US)
  );
  //
  // Check whether the packet size is right.
  // The size of each time tag is 8 bytes (double).
  //
  NS_ASSERT (packet->GetSize () == pulses * 8);

  NotifyNewDetectionEvent (
    start,
    stop,
    300.0,//GetFsoDevice ()->GetObject<FsoRxDevice> ()->GetDarkCountRate (),
    GetFsoDevice ()->GetChannel ()->CalcChannelLoss ());
  if (m_detectionEvents[Processing] > 250)
  {
    m_eventSig[Negotiating] = m_eventSig[Processing];
    m_eventDec[Negotiating] = m_eventDec[Processing];
    m_eventVac[Negotiating] = m_eventVac[Processing];
    m_errorSig[Negotiating] = m_errorSig[Processing];
    m_errorDec[Negotiating] = m_errorDec[Processing];
    m_errorVac[Negotiating] = m_errorVac[Processing];
    m_eventSig[Total] += m_eventSig[Processing];
    m_eventDec[Total] += m_eventDec[Processing];
    m_eventVac[Total] += m_eventVac[Processing];
    m_errorSig[Total] += m_errorSig[Processing];
    m_errorDec[Total] += m_errorDec[Processing];
    m_errorVac[Total] += m_errorVac[Processing];
    m_eventSig[Processing] = 0;
    m_eventDec[Processing] = 0;
    m_eventVac[Processing] = 0;
    m_errorSig[Processing] = 0;
    m_errorDec[Processing] = 0;
    m_errorVac[Processing] = 0;
    m_events[Negotiating] = m_events[Processing];
    m_events[Total] += m_events[Processing];
    m_events[Processing] = 0;
    m_detectionEvents[Negotiating] = m_detectionEvents[Processing];
    m_detectionEvents[Total] += m_detectionEvents[Processing];
    m_detectionEvents[Processing] = 0;
    m_correctEvents[Negotiating] = m_correctEvents[Processing];
    m_correctEvents[Total] += m_correctEvents[Processing];
    m_correctEvents[Processing] = 0;
    DoSendPulseLocating ();
  }
}

void
Q3pRxCache::DoSendPulseLocating ()
{
  NS_LOG_FUNCTION (this);
  if (!m_processing)
  {
    return;
  }
  Q3pHeader hdr;
  Q3pTagPulseLocating tag;
  Ptr<Packet> pkt = Create<Packet> ();
  hdr.SetType (Q3pHeader::Q3P_PULSE_LOCATING);
  tag.SetMs (m_eventSig[Negotiating]);
  tag.SetMw (m_eventDec[Negotiating]);
  tag.SetMv (m_eventVac[Negotiating]);
  tag.SetEsMs (m_errorSig[Negotiating]);
  tag.SetEwMw (m_errorDec[Negotiating]);
  tag.SetEvMv (m_errorVac[Negotiating]);
  tag.SetPulseNumber (m_detectionEvents[Negotiating]);
  pkt->AddHeader (hdr);
  pkt->AddPacketTag (tag);
  pkt->AddPaddingAtEnd (m_detectionEvents[Negotiating] * 8);
  m_totalBytes += pkt->GetSize ();
  m_q3p->SendMessage (this, pkt);
  m_sentCountPulseLocating++;
}

void
Q3pRxCache::DoHandleBasisSifting (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);
  if (!m_processing)
  {
    return;
  }
  m_recvCountBasisSifting++;
  Q3pTagBasisSifting tag;
  NS_ASSERT (packet->RemovePacketTag (tag));
  DoSendKeySifting ();
}

void
Q3pRxCache::DoSendKeySifting ()
{
  NS_LOG_FUNCTION (this);
  if (!m_processing)
  {
    return;
  }
  Q3pHeader hdr;
  Q3pTagKeySifting tag;
  Ptr<Packet> pkt = Create<Packet> ();
  hdr.SetType (Q3pHeader::Q3P_KEY_SIFTING);
  pkt->AddHeader (hdr);
  pkt->AddPacketTag (tag);
  pkt->AddPaddingAtEnd (m_correctEvents[Negotiating]);
  m_totalBytes += pkt->GetSize ();
  m_q3p->SendMessage (this, pkt);
  m_sentCountKeySifting++;
}

void
Q3pRxCache::DoHandleErrorCorrection (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);
  if (!m_processing)
  {
    return;
  }
  m_recvCountErrorCorrection++;
  Q3pTagErrorCorrection tag;
  NS_ASSERT (packet->RemovePacketTag (tag));
}

void
Q3pRxCache::DoHandlePrivacyAmplification (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);
  if (!m_processing)
  {
    return;
  }
  GetFsoDevice ()->NotifyConnectionFinished ();
  Q3pTagPrivacyAmplification tag;
  NS_ASSERT (packet->RemovePacketTag (tag));
  m_secureBits = tag.GetSecureBits ();
  Ptr<QkdSatellite> rxSat = GetFsoDevice ()->GetNode ()->GetObject<QkdSatellite> ();
  Ptr<QkdStation> rxSta = GetFsoDevice ()->GetNode ()->GetObject<QkdStation> ();
  Ptr<QkdNode> peer = GetFsoDevice ()->GetChannel ()->GetTxDevice ()->GetNode ()->GetObject<QkdNode> ();
  m_node->GetObject<QkdKeyPool> ()->Store (this, peer);
  Ptr<QkdSatellite> txSat = peer->GetObject<QkdSatellite> ();
  Ptr<QkdStation> txSta = peer->GetObject<QkdStation> ();
  std::string txName, rxName;
  if (txSat && !txSta)
  {
    txName = std::to_string (txSat->GetId ());
  }
  else if (txSta && !txSat)
  {
    txName = txSta->GetName ();
  }
  else
  {
    NS_ASSERT (false);
  }
  if (rxSat && !rxSta)
  {
    rxName = std::to_string (rxSat->GetId ());
  }
  else if (rxSta && !rxSat)
  {
    rxName = rxSta->GetName ();
  }
  else
  {
    NS_ASSERT (false);
  }
  std::stringstream ss;
  Time start, stop;
  GetFsoDevice ()->GetRoundInterval (start, stop);
  ss << std::setw (32) << (stop - start).As (Time::S) << std::endl
     << std::setw (32) << ToTime (start) << std::setw (32) << ToTime (stop) << std::endl
     << std::setw (32) << std::right << "Source"                        << " " << std::left << txName << std::endl
     << std::setw (32) << std::right << "Destination"                   << " " << std::left << rxName << std::endl
     << std::setw (32) << std::right << "Secure-key-bits(kb)"           << " " << std::left << (m_secureBits * 1.0 / (1 << 10)) << std::endl
     << std::setw (32) << std::right << "Tx-Network-traffic(MB)"        << " " << std::left << (tag.GetTotalBytes () * 1.0 / (1 << 20)) << std::endl
     << std::setw (32) << std::right << "Rx-Network-traffic(MB)"        << " " << std::left << (m_totalBytes * 1.0 / (1 << 20)) << std::endl
     << std::setw (32) << std::right << "Tx-Mean-network-traffic(Mbps)" << " " << std::left << tag.GetTotalBytes () / GetFsoDevice ()->GetRoundPeriod ().GetSeconds () * 8.0 / 1000.0 / 1000.0 << std::endl
     << std::setw (32) << std::right << "Rx-Mean-network-traffic(Mbps)" << " " << std::left << m_totalBytes / GetFsoDevice ()->GetRoundPeriod ().GetSeconds () * 8.0 / 1000.0 / 1000.0;
  NS_LOG_INFO (ss.str ());
  Flush ();
  NotifyConnectionFinished ();
}


}