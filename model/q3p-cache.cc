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
#include "ns3/net-device.h"
#include "ns3/ipv4-interface.h"
#include "q3p-cache.h"
#include "q3p-calc.h"
#include "q3p-l3-protocol.h"
#include "qkd-device.h"
#include "fso-tx-device.h"
#include "fso-rx-device.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Q3pCache");

NS_OBJECT_ENSURE_REGISTERED (Q3pCache);

TypeId
Q3pCache::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Q3pCache")
    .SetParent<Object> ()
    .SetGroupName ("Qkd")
    // .AddConstructor<Q3pCache> ()
  ;
  return tid;
}

Q3pCache::Q3pCache ()
: m_node      (0)
, m_qkd       (0)
, m_interface (0)
, m_processing(false)
{
  NS_LOG_FUNCTION (this);
  Flush ();
}

Q3pCache::~Q3pCache ()
{
  NS_LOG_FUNCTION (this);
}

void
Q3pCache::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);
  m_node = node;
}

Ptr<Node>
Q3pCache::GetNode (void) const
{
  return m_node;
}

void
Q3pCache::SetProtocol (Ptr<Q3pL3Protocol> q3p)
{
  NS_LOG_FUNCTION (this << q3p);
  NS_ASSERT (q3p);
  m_q3p = q3p;
}

Ptr<Q3pL3Protocol>
Q3pCache::GetProtocol (void) const
{
  return m_q3p;
}

void
Q3pCache::SetQkdDevice (Ptr<QkdDevice> qkd)
{
  NS_LOG_FUNCTION (this << qkd);
  NS_ASSERT (qkd);
  m_qkd = qkd;
}

Ptr<QkdDevice>
Q3pCache::GetQkdDevice (void) const
{
  return m_qkd;
}

void
Q3pCache::SetInterface (Ptr<Ipv4Interface> interface)
{
  NS_LOG_FUNCTION (this << interface);
  NS_ASSERT (interface);
  m_interface = interface;
}

Ptr<Ipv4Interface>
Q3pCache::GetInterface (void) const
{
  return m_interface;
}

Ptr<FsoDevice>
Q3pCache::GetFsoDevice (void) const
{
  return m_qkd->GetFsoDevice ();
}

Ptr<NetDevice>
Q3pCache::GetNetDevice (void) const
{
  return m_qkd->GetNetDevice ();
}

void
Q3pCache::Flush (void)
{
  NS_LOG_FUNCTION (this);
  memset (m_events, 0, sizeof (double) * 3);
  memset (m_detectionEvents, 0, sizeof (double) * 3);
  memset (m_correctEvents, 0, sizeof (double) * 3);
  m_secureBits = 0;
  m_eventSig[Total] = 0;
  m_errorSig[Total] = 0;
  m_eventDec[Total] = 0;
  m_errorDec[Total] = 0;
  m_eventVac[Total] = 0;
  m_errorVac[Total] = 0;
  m_eventSig[Negotiating] = 0;
  m_errorSig[Negotiating] = 0;
  m_eventDec[Negotiating] = 0;
  m_errorDec[Negotiating] = 0;
  m_eventVac[Negotiating] = 0;
  m_errorVac[Negotiating] = 0;
  m_eventSig[Processing] = 0;
  m_errorSig[Processing] = 0;
  m_eventDec[Processing] = 0;
  m_errorDec[Processing] = 0;
  m_eventVac[Processing] = 0;
  m_errorVac[Processing] = 0;

  m_totalBytes = 0;
}

const uint64_t&
Q3pCache::GetTotalEvents () const
{
  return m_events[Total];
}

const double&
Q3pCache::GetSignalEvents () const
{
  return m_eventSig[Total];
}

const double&
Q3pCache::GetDecoyEvents () const
{
  return m_eventDec[Total];
}

const double&
Q3pCache::GetVacuumEvents () const
{
  return m_eventVac[Total];
}

const double&
Q3pCache::GetSignalErrorEvents () const
{
  return m_errorSig[Total];
}

const double&
Q3pCache::GetDecoyErrorEvents () const
{
  return m_errorDec[Total];
}

const double&
Q3pCache::GetVacuumErrorEvents () const
{
  return m_errorVac[Total];
}

const uint64_t&
Q3pCache::GetSecureBits () const
{
  return m_secureBits;
}

void
Q3pCache::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (m_node);
  NS_ASSERT (m_q3p);
  NS_ASSERT (m_qkd);
  NS_ASSERT (m_interface);
  Object::DoInitialize ();
}

void
Q3pCache::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_node = 0;
  m_q3p = 0;
  m_qkd = 0;
  m_interface = 0;
  m_processing = false;
  Object::DoDispose ();
}

void
Q3pCache::NotifyNewAggregates (void)
{
  NS_LOG_FUNCTION (this);
}

}