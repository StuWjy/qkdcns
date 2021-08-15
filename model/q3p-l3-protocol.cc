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

#include "fso-tx-device.h"
#include "fso-rx-device.h"
#include "q3p-l3-protocol.h"
#include "q3p-header.h"
#include "q3p-tag.h"
#include "qkd-node.h"
#include "qkd-satellite.h"
#include "qkd-station.h"
#include "qnet-ipv4-l3-protocol.h"
#include "ns3/node.h"
#include "ns3/simulator.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/ipv4-interface.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/traffic-control-layer.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Q3pL3Protocol");

NS_OBJECT_ENSURE_REGISTERED (Q3pL3Protocol);

const uint16_t Q3pL3Protocol::PROT_NUMBER = 0x0806;

TypeId
Q3pL3Protocol::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Q3pL3Protocol")
    .SetParent<Object> ()
    .SetGroupName ("Qkd")
    .AddConstructor<Q3pL3Protocol> ()
    .AddAttribute ("TimeSyncTagPulses", "How many time sync pulse is in the time sync tagging",
                   UintegerValue (250u),
                   MakeUintegerAccessor (&Q3pL3Protocol::m_pulsesTimeSyncTag),
                   MakeUintegerChecker<uint32_t> (0)
                  )
    .AddAttribute ("SyncPulseCycle", "The cycle of sync pulse",
                   TimeValue (MicroSeconds (100ul)),
                   MakeTimeAccessor (&Q3pL3Protocol::m_syncPulseCycle),
                   MakeTimeChecker (MicroSeconds (1ul))
                  )
    .AddAttribute ("Frequency", "The frequency of photon pulses",
                   UintegerValue (100000000ul),  //100M
                   MakeUintegerAccessor (&Q3pL3Protocol::m_freq),
                   MakeUintegerChecker<uint64_t> (0)
                  )
    .AddAttribute ("BasisRatio", "The ratio of basis",
                   DoubleValue (0.5),
                   MakeDoubleAccessor (&Q3pL3Protocol::m_q),
                   MakeDoubleChecker<double> (0.0, 1.0)
                  )
    .AddAttribute ("ErrorRate", "The error rate of vacuum state",
                   DoubleValue (0.5),
                   MakeDoubleAccessor (&Q3pL3Protocol::m_e),
                   MakeDoubleChecker<double> (0.0, 1.0)
                  )
    .AddAttribute ("SignalPhotons", "The mean-photons of signal state",
                   DoubleValue (0.8),
                   MakeDoubleAccessor (&Q3pL3Protocol::GetSignalPhotons,
                                       &Q3pL3Protocol::DoSetSignalPhotons),
                   MakeDoubleChecker<double> (0.0)
                  )
    .AddAttribute ("DecoyPhotons", "The mean-photons of decoy state",
                   DoubleValue (0.2),
                   MakeDoubleAccessor (&Q3pL3Protocol::GetDecoyPhotons,
                                       &Q3pL3Protocol::DoSetDecoyPhotons),
                   MakeDoubleChecker<double> (0.0)
                  )
    // The sum of following probabilities should be exactly 1.0.
    .AddAttribute ("SignalProbability", "The probability of signal state",
                   DoubleValue (0.5),
                   MakeDoubleAccessor (&Q3pL3Protocol::GetSignalProbability,
                                       &Q3pL3Protocol::DoSetSignalProbability),
                   MakeDoubleChecker<double> (0.0, 1.0)
                  )
    .AddAttribute ("DecoyProbability", "The probability of decoy state",
                   DoubleValue (0.25),
                   MakeDoubleAccessor (&Q3pL3Protocol::GetDecoyProbability,
                                       &Q3pL3Protocol::DoSetDecoyProbability),
                   MakeDoubleChecker<double> (0.0, 1.0)
                  )
    .AddAttribute ("VacuumProbability", "The probability of vacuum state",
                   DoubleValue (0.25),
                   MakeDoubleAccessor (&Q3pL3Protocol::GetVacuumProbability,
                                       &Q3pL3Protocol::DoSetVacuumProbability),
                   MakeDoubleChecker<double> (0.0, 1.0)
                  )
  ;
  return tid;
}

Q3pL3Protocol::Q3pL3Protocol ()
: m_node  (0)
{
  NS_LOG_FUNCTION (this);
}

Q3pL3Protocol::~Q3pL3Protocol ()
{
  NS_LOG_FUNCTION (this);
}

void
Q3pL3Protocol::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);
  NS_ASSERT (node->GetObject<QkdNode> ());
  m_node = node;
}

void
Q3pL3Protocol::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  if (m_node == 0)
  {
    Ptr<Node> node = this->GetObject<Node>();
    // verify that it's a valid node and that
    // the node has not been set before
    if (node != 0)
    {
      this->SetNode (node);
    }
  }
  else
  {
    Ptr<QkdNode> node = m_node->GetObject<QkdNode> ();
    for (size_t i = 0;i < node->GetNQkdDevice ();++i)
    {
      Ptr<QkdDevice> qkd = node->GetQkdDevice (i);
      Ptr<NetDevice> net = qkd->GetNetDevice ();
      Ptr<FsoDevice> fso = qkd->GetFsoDevice ();
      Ptr<QnetIpv4L3Protocol> qnet = node->GetObject<QnetIpv4L3Protocol> ();
      NS_ASSERT (qkd);
      NS_ASSERT (net);
      NS_ASSERT (fso);
      NS_ASSERT (qnet);
      int32_t id = qnet->GetInterfaceForDevice (net);
      NS_ASSERT (id >= 0);
      Ptr<Ipv4Interface> interface = qnet->GetInterface (id);
      Ptr<Q3pCache> cache = CreateCache (net, interface);
      cache->SetQkdDevice (qkd);
      cache->SetNode (m_node);
      qkd->SetCache (cache);
      fso->SetConnectCallback (
        MakeCallback (&Q3pCache::NotifyConnectionStarted, cache),
        MakeCallback (&Q3pCache::NotifyConnectionFinished, cache),
        MakeCallback (&Q3pCache::NotifyConnectionSucceeded, cache),
        MakeCallback (&Q3pCache::NotifyConnectionFailed, cache)
      );
    }
  }
}

void 
Q3pL3Protocol::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  for (CacheList::iterator i = m_cacheList.begin (); i != m_cacheList.end (); ++i)
  {
    Ptr<Q3pCache> cache = *i;
    cache->Dispose ();
  }
  m_cacheList.clear ();
  m_node = 0;
  Object::DoDispose ();
}

void
Q3pL3Protocol::NotifyNewAggregate ()
{
  NS_LOG_FUNCTION (this);
  if (m_node == 0)
  {
    Ptr<Node> node = this->GetObject<Node> ();
    //verify that it's a valid node and that
    //the node was not set before
    if (node != 0)
    {
      this->SetNode (node);
    }
  }
  Object::NotifyNewAggregate ();
}

Ptr<Q3pCache> 
Q3pL3Protocol::CreateCache (Ptr<NetDevice> device, Ptr<Ipv4Interface> interface)
{
  NS_LOG_FUNCTION (this << device);
  Ptr<Q3pCache> cache;
  if (device->GetObject<FsoTxDevice> ())
  {
    cache = CreateObject<Q3pTxCache> ();
  }
  else if (device->GetObject<FsoRxDevice> ())
  {
    cache = CreateObject<Q3pRxCache> ();
  }
  else
  {
    NS_ASSERT (false);
  }
  cache->SetInterface (interface);
  cache->SetProtocol (this);
  m_cacheList.push_back (cache);
  return cache;
}

Ptr<Q3pCache>
Q3pL3Protocol::FindCache (Ptr<NetDevice> device)
{
  NS_LOG_FUNCTION (this << device);
  for (CacheList::const_iterator i = m_cacheList.begin (); i != m_cacheList.end (); i++)
  {
    if ((*i)->GetNetDevice () == device)
    {
      return *i;
    }
  }
  NS_ASSERT (false);
  return 0;
}

void
Q3pL3Protocol::Receive (
  Ptr<NetDevice>        device,
  Ptr<const Packet>     p,
  uint16_t              protocol,
  const Address&        from,
  const Address&        to,
  NetDevice::PacketType packetType)
{
  NS_LOG_FUNCTION (this << device << p->GetSize () << protocol << from << to << packetType);
  Ptr<Packet> packet = p->Copy ();
  NS_LOG_LOGIC ("QPP: received packet of size "<< packet->GetSize ());
  Q3pHeader hdr;
  NS_ASSERT (packet->PeekHeader (hdr));
  NS_LOG_LOGIC (
    "QPP: received " << hdr.GetTypeString () <<
    " " << (m_node->GetObject<QkdNode> ()->GetObject<QkdSatellite> () ? "satellite" : "station") <<
    "=" << m_node->GetId () <<
    ", from " << from <<
    " to " << to 
  );
  Ptr<Q3pCache> cache = FindCache (device);
  cache->NotifyNewPacket (packet);
}

void
Q3pL3Protocol::SendMessage (Ptr<Q3pCache> cache, Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << cache);
  NS_ASSERT (cache->GetInterface ()->GetNAddresses () == 1);
  cache->GetNetDevice ()->Send (
    packet,
    cache->GetInterface ()->GetAddress (0).GetLocal (),
    PROT_NUMBER);
}

double 
Q3pL3Protocol::GetFrequency () const
{
  return m_freq;
}

double 
Q3pL3Protocol::GetBasisRatio () const
{
  return m_q;
}

double 
Q3pL3Protocol::GetErrorRate () const
{
  return m_e;
}

double 
Q3pL3Protocol::GetSignalPhotons () const
{
  return m_sig.m;
}

double 
Q3pL3Protocol::GetDecoyPhotons () const
{
  return m_dec.m;
}

double 
Q3pL3Protocol::GetSignalProbability () const
{
  return m_sig.p;
}

double 
Q3pL3Protocol::GetDecoyProbability () const
{
  return m_dec.p;
}

double 
Q3pL3Protocol::GetVacuumProbability ()const
{
  return m_vac.p;
}

void
Q3pL3Protocol::DoSetFrequency (const double& freq)
{
  NS_LOG_FUNCTION (this << freq);
  m_freq = freq;
}

void
Q3pL3Protocol::DoSetBasisRatio (const double& q)
{
  NS_LOG_FUNCTION (this << q);
  m_q = q;
}

void
Q3pL3Protocol::DoSetErrorRate (const double& e)
{
  NS_LOG_FUNCTION (this << e);
  m_e = e;
}

void
Q3pL3Protocol::DoSetSignalPhotons (const double& m)
{
  NS_LOG_FUNCTION (this << m);
  m_sig.m = m;
}

void
Q3pL3Protocol::DoSetDecoyPhotons (const double& m)
{
  NS_LOG_FUNCTION (this << m);
  m_dec.m = m;
}

void
Q3pL3Protocol::DoSetSignalProbability (const double& p)
{
  NS_LOG_FUNCTION (this << p);
  m_sig.p = p;
}

void
Q3pL3Protocol::DoSetDecoyProbability (const double& p)
{
  NS_LOG_FUNCTION (this << p);
  m_dec.p = p;
}

void
Q3pL3Protocol::DoSetVacuumProbability (const double& p)
{
  NS_LOG_FUNCTION (this << p);
  m_vac.p = p;
}

}