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

#include "ns3/packet.h"
#include "ns3/node.h"
#include "q3p-header.h"
#include "q3p-tx-cache.h"
#include "q3p-tag.h"
#include "q3p-l3-protocol.h"
#include "q3p-calc.h"
#include "qkd-key-pool.h"
#include "fso-tx-device.h"
#include "fso-channel.h"
#include "turntable.h"
#include "util.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Q3pTxCache);

NS_LOG_COMPONENT_DEFINE ("Q3pTxCache");

TypeId
Q3pTxCache::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Q3pTxCache")
    .SetParent<Q3pCache> ()
    .SetGroupName ("Qkd")
    .AddConstructor<Q3pTxCache> ()
    .AddAttribute ("TimeSyncTagPulses", "How many time sync pulse is in the time sync tagging",
                   UintegerValue (10000u),
                   MakeUintegerAccessor (&Q3pTxCache::m_pulsesTimeSyncTag),
                   MakeUintegerChecker<uint32_t> (0)
                  )
    .AddAttribute ("SyncPulseCycle", "The cycle of sync pulse",
                   TimeValue (MicroSeconds (100ul)),
                   MakeTimeAccessor (&Q3pTxCache::m_syncPulseCycle),
                   MakeTimeChecker (MicroSeconds (1ul))
                  );
  ;
  return tid;
}

Q3pTxCache::Q3pTxCache ()
: Q3pCache ()
{
  NS_LOG_FUNCTION (this);
  Flush ();
}

Q3pTxCache::~Q3pTxCache ()
{
  NS_LOG_FUNCTION (this);
}

void
Q3pTxCache::Flush (void)
{
  NS_LOG_FUNCTION (this);
  m_sentCountTimeSync = 0;
  m_recvCountPulseLocating = 0;
  m_sentCountBasisSifting = 0;
  m_recvCountKeySifting = 0;
  m_sentCountErrorCorrection = 0;
  Q3pCache::Flush ();
}

void
Q3pTxCache::NotifyConnectionStarted (void)
{
  NS_LOG_FUNCTION (this);
  m_processing = true;
}

void
Q3pTxCache::NotifyConnectionFinished (void)
{
  NS_LOG_FUNCTION (this);
  if (m_syncEvent.IsRunning ())
  {
    m_syncEvent.Cancel ();
  }
  DoSendPrivacyAmplification ();
  Flush ();
}

void
Q3pTxCache::NotifyConnectionSucceeded (void)
{
  NS_LOG_FUNCTION (this);
  m_start = Now ();
  /**
   * The first sync-pulse will be sent immediately,
   * therefore, the i-th sync-pulse will be sent after
   * (i - 1) time interval between sync-pulses.
   */
  Time delay = MicroSeconds (m_syncPulseCycle.GetMicroSeconds () * m_pulsesTimeSyncTag);
  m_syncEvent = Simulator::Schedule (delay, &Q3pTxCache::DoSendTimeSyncTagging, this);
}

void
Q3pTxCache::NotifyConnectionFailed (void)
{
  /**
   * If connection failed, then send the time-sync-tagging instantly
   * and cancel the event.
   */
  NS_LOG_FUNCTION (this);
  // DoSendTimeSyncTagging ();
  if (m_syncEvent.IsRunning ())
  {
    m_syncEvent.Cancel ();
  }
}

void
Q3pTxCache::NotifyNewPacket (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this);
  Q3pHeader hdr;
  packet->RemoveHeader (hdr);
  switch (hdr.GetType ())
  {
    case Q3pHeader::Q3P_PULSE_LOCATING: DoHandlePulseLocating (packet);break;
    case Q3pHeader::Q3P_KEY_SIFTING:    DoHandleKeySifting (packet);   break;
    default:  NS_ASSERT (false);break;
  }
}

void
Q3pTxCache::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  Q3pCache::DoInitialize ();
}

void
Q3pTxCache::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Q3pCache::DoDispose ();
}

void
Q3pTxCache::NotifyNewAggregate (void)
{
  NS_LOG_FUNCTION (this);
}

void
Q3pTxCache::DoUpdate (void)
{
  NS_LOG_FUNCTION (this);
}

void
Q3pTxCache::DoSendTimeSyncTagging (void)
{
  NS_LOG_FUNCTION (this);
  if (!m_processing)
  {
    return;
  }
  /**
   * Get the current time and calculate the number of sync-pulses between the interval
   */
  m_stop = Now ();
  m_syncPulses = (m_stop - m_start).GetMicroSeconds () / m_syncPulseCycle.GetMicroSeconds ();

  Q3pHeader hdr;
  Q3pTagTimeSync tag;
  Ptr<Packet> pkt = Create<Packet> ();
  hdr.SetType (Q3pHeader::Q3P_TIME_SYNC_TAGGING);
  tag.SetStartTime (m_start);
  tag.SetStopTime (m_stop);
  tag.SetPulses (m_syncPulses);
  pkt->AddHeader (hdr);
  pkt->AddPacketTag (tag);
  pkt->AddPaddingAtEnd (m_syncPulses * 8);
  m_totalBytes += pkt->GetSize ();
  m_q3p->SendMessage (this, pkt);

  m_events[Processing] = (m_stop - m_start).GetSeconds () * m_q3p->GetFrequency ();
  m_events[Total] += m_events[Processing];
  m_sentCountTimeSync++;
  /**
   * the next start time of time tagging is the subsequent sync pulse of the last stop time
   */
  m_start = m_stop;
  /**
   * It has to wait an additional sync-pulse interval,
   * otherwise there is overlapping sync-pulse
   */
  Time delay = MicroSeconds (m_pulsesTimeSyncTag * m_syncPulseCycle.GetMicroSeconds ());
  m_syncEvent = Simulator::Schedule (delay, &Q3pTxCache::DoSendTimeSyncTagging, this);
}

void
Q3pTxCache::DoHandlePulseLocating (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);
  if (!m_processing)
  {
    return;
  }
  m_recvCountPulseLocating++;
  Q3pTagPulseLocating tag;
  NS_ASSERT (packet->RemovePacketTag (tag));
  m_eventSig[Total] += tag.GetMs ();
  m_eventDec[Total] += tag.GetMw ();
  m_eventVac[Total] += tag.GetMv ();
  m_errorSig[Total] += tag.GetEsMs ();
  m_errorDec[Total] += tag.GetEwMw ();
  m_errorVac[Total] += tag.GetEvMv ();
  m_detectionEvents[Negotiating] = tag.GetPulseNumber ();
  m_correctEvents[Negotiating] = m_q3p->GetBasisRatio () * (tag.GetMs () - tag.GetEsMs ());
  DoSendBasisSifting ();
}

void
Q3pTxCache::DoSendBasisSifting (void)
{
  NS_LOG_FUNCTION (this);
  if (!m_processing)
  {
    return;
  }
  Q3pHeader hdr;
  Q3pTagBasisSifting tag;
  Ptr<Packet> pkt = Create<Packet> ();
  hdr.SetType (Q3pHeader::Q3P_BASIS_SIFTING);
  pkt->AddHeader (hdr);
  pkt->AddPacketTag (tag);
  pkt->AddPaddingAtEnd (m_correctEvents[Negotiating]);
  m_totalBytes += pkt->GetSize ();
  m_q3p->SendMessage (this, pkt);
  m_sentCountBasisSifting++;
}

void
Q3pTxCache::DoHandleKeySifting (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);
  if (!m_processing)
  {
    return;
  }
  m_recvCountKeySifting++;
  Q3pTagKeySifting tag;
  NS_ASSERT (packet->RemovePacketTag (tag));
  DoSendErrorCorrection ();
}

void
Q3pTxCache::DoSendErrorCorrection (void)
{
  NS_LOG_FUNCTION (this);
  if (!m_processing)
  {
    return;
  }
  Q3pHeader hdr;
  Q3pTagErrorCorrection tag;
  Ptr<Packet> pkt = Create<Packet> ();
  hdr.SetType (Q3pHeader::Q3P_ERROR_CORRECTION);
  pkt->AddHeader (hdr);
  pkt->AddPacketTag (tag);
  pkt->AddPaddingAtEnd (m_correctEvents[Negotiating]);
  m_q3p->SendMessage (this, pkt);
  m_correctEvents[Total] += m_correctEvents[Negotiating];
}

void
Q3pTxCache::DoSendPrivacyAmplification (void)
{
  NS_LOG_FUNCTION (this);
  if (!m_processing)
  {
    return;
  }
  Q3pHeader hdr;
  Q3pTagPrivacyAmplification tag;
  m_secureBits = Q3pCalc::CalcSecureBits (this);
  tag.SetSecureBits (m_secureBits);
  Ptr<Packet> pkt = Create<Packet> ();
  hdr.SetType (Q3pHeader::Q3P_PRIVACY_AMPLIFICATION);
  pkt->AddHeader (hdr);
  uint32_t size = m_secureBits >> 3;
  pkt->AddPaddingAtEnd (size);
  m_totalBytes += pkt->GetSize ();
  tag.SetTotalBytes (m_totalBytes + tag.GetSerializedSize ());
  pkt->AddPacketTag (tag);
  m_q3p->SendMessage (this, pkt);
  Ptr<QkdNode> peer = GetFsoDevice ()->GetChannel ()->GetRxDevice ()->GetNode ()->GetObject<QkdNode> ();
  m_node->GetObject<QkdKeyPool> ()->Store (this, peer);
  m_processing = false;
}

}