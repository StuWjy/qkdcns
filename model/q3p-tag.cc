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

#include "q3p-tag.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Q3pTag");

/**************************************
 *           Q3pTagTimeSync
 **************************************/

NS_OBJECT_ENSURE_REGISTERED (Q3pTagTimeSync);

void
Q3pTagTimeSync::SetStartTime (const Time &time)
{
  m_start = time.GetMicroSeconds ();
}

void
Q3pTagTimeSync::SetStopTime (const Time &time)
{
  m_stop = time.GetMicroSeconds ();
}

void
Q3pTagTimeSync::SetPulses (const uint32_t &pulses)
{
  m_pulses = pulses;
}

Time
Q3pTagTimeSync::GetStartTime (void) const
{
  return MicroSeconds (m_start);
}

Time
Q3pTagTimeSync::GetStopTime (void) const
{
  return MicroSeconds (m_stop);
}

uint32_t
Q3pTagTimeSync::GetPulses (void) const
{
  return m_pulses;
}

Q3pTagTimeSync::Q3pTagTimeSync ()
{
  NS_LOG_FUNCTION (this);
}

Q3pTagTimeSync::~Q3pTagTimeSync ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
Q3pTagTimeSync::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Q3pTagTimeSync")
    .SetParent<Tag> ()
    .SetGroupName ("qkd")
    .AddConstructor<Q3pTagTimeSync> ()
  ;
  return tid;
}

TypeId
Q3pTagTimeSync::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);
  return GetTypeId ();
}

uint32_t
Q3pTagTimeSync::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return 20;
}

void
Q3pTagTimeSync::Serialize (TagBuffer i) const
{
  NS_LOG_FUNCTION (this);
  i.WriteU64 (m_start);
  i.WriteU64 (m_stop);
  i.WriteU32 (m_pulses);
}

void
Q3pTagTimeSync::Deserialize (TagBuffer i)
{
  NS_LOG_FUNCTION (this);
  m_start = i.ReadU64 ();
  m_stop = i.ReadU64 ();
  m_pulses = i.ReadU32 ();
}

void
Q3pTagTimeSync::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this);
  os << "Time Synchronization Tagging"
     << " starts at " << MicroSeconds (m_start).As (Time::S)
     << " stops at " << MicroSeconds (m_stop).As (Time::S)
     << ", there are " << m_pulses << " synchronization pulses";
}

/**************************************
 *          Q3pTagPulseLocating
 **************************************/

NS_OBJECT_ENSURE_REGISTERED (Q3pTagPulseLocating);

void
Q3pTagPulseLocating::SetPulseNumber (uint32_t pulses)
{
  NS_LOG_FUNCTION (this << pulses);
  m_pulseNumber = pulses;
}

void
Q3pTagPulseLocating::SetMs (const double& Ms)
{
  NS_LOG_FUNCTION (this << Ms);
  m_Ms = Ms;
}

void
Q3pTagPulseLocating::SetEsMs (const double& EsMs)
{
  NS_LOG_FUNCTION (this << EsMs);
  m_EsMs = EsMs;
}

void
Q3pTagPulseLocating::SetMw (const double& Mw)
{
  NS_LOG_FUNCTION (this << Mw);
  m_Mw = Mw;
}
void
Q3pTagPulseLocating::SetEwMw (const double& EwMw)
{
  NS_LOG_FUNCTION (this << EwMw);
  m_EwMw = EwMw;
}
void
Q3pTagPulseLocating::SetMv (const double& Mv)
{
  NS_LOG_FUNCTION (this << Mv);
  m_Mv = Mv;
}
void
Q3pTagPulseLocating::SetEvMv (const double& EvMv)
{
  NS_LOG_FUNCTION (this << EvMv);
  m_EvMv = EvMv;
}

// void
// Q3pTagPulseLocating::SetSiftedBits (uint32_t bits)
// {
//   NS_LOG_FUNCTION(this << bits);
//   m_siftedBits = bits;
// }

// void
// Q3pTagPulseLocating::SetSecureBits (uint32_t bits)
// {
//   NS_LOG_FUNCTION(this << bits);
//   m_secureBits = bits;
// }

// void
// Q3pTagPulseLocating::SetTimeSyncTaggingBegin (uint32_t begin)
// {
//   NS_LOG_FUNCTION (this << begin);
//   m_beginIdentifier = begin;
// }

// void
// Q3pTagPulseLocating::SetTimeSyncTaggingEnd (uint32_t end)
// {
//   NS_LOG_FUNCTION (this << end);
//   m_endIdentifier = end;
// }

uint32_t
Q3pTagPulseLocating::GetPulseNumber (void) const
{
  NS_LOG_FUNCTION (this);
  return m_pulseNumber;
}

double
Q3pTagPulseLocating::GetMs () const
{
  NS_LOG_FUNCTION (this);
  return m_Ms;
}

double
Q3pTagPulseLocating::GetEsMs () const
{
  NS_LOG_FUNCTION (this);
  return m_EsMs;
}

double
Q3pTagPulseLocating::GetMw () const
{
  NS_LOG_FUNCTION (this);
  return m_Mw;
}

double
Q3pTagPulseLocating::GetEwMw () const
{
  NS_LOG_FUNCTION (this);
  return m_EwMw;
}

double
Q3pTagPulseLocating::GetMv () const
{
  NS_LOG_FUNCTION (this);
  return m_Mv;
}

double
Q3pTagPulseLocating::GetEvMv () const
{
  NS_LOG_FUNCTION (this);
  return m_EvMv;
}

// uint32_t
// Q3pTagPulseLocating::GetSiftedBits (void) const
// {
//   NS_LOG_FUNCTION (this);
//   return m_siftedBits;
// }

// uint32_t
// Q3pTagPulseLocating::GetSecureBits (void) const
// {
//   NS_LOG_FUNCTION (this);
//   return m_secureBits;
// }

// uint32_t
// Q3pTagPulseLocating::GetTimeSyncTaggingBegin (void) const
// {
//   NS_LOG_FUNCTION (this);
//   return m_beginIdentifier;
// }

// uint32_t
// Q3pTagPulseLocating::GetTimeSyncTaggingEnd (void) const
// {
//   NS_LOG_FUNCTION (this);
//   return m_endIdentifier;
// }

Q3pTagPulseLocating::Q3pTagPulseLocating ()
{
  NS_LOG_FUNCTION (this);
}

Q3pTagPulseLocating::~Q3pTagPulseLocating ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
Q3pTagPulseLocating::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Q3pTagPulseLocating")
    .SetParent<Tag> ()
    .SetGroupName ("qkd")
    .AddConstructor<Q3pTagPulseLocating> ()
  ;
  return tid;
}

TypeId
Q3pTagPulseLocating::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);
  return GetTypeId ();
}

uint32_t
Q3pTagPulseLocating::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return 52;
}

void
Q3pTagPulseLocating::Serialize (TagBuffer i) const
{
  NS_LOG_FUNCTION (this);
  // i.WriteU32 (m_beginIdentifier);
  // i.WriteU32 (m_endIdentifier);
  i.WriteU32 (m_pulseNumber);
  i.WriteDouble (m_Ms);
  i.WriteDouble (m_Mw);
  i.WriteDouble (m_Mv);
  i.WriteDouble (m_EsMs);
  i.WriteDouble (m_EwMw);
  i.WriteDouble (m_EvMv);
}

void
Q3pTagPulseLocating::Deserialize (TagBuffer i)
{
  NS_LOG_FUNCTION (this);
  // m_beginIdentifier = i.ReadU32 ();
  // m_endIdentifier = i.ReadU32 ();
  m_pulseNumber = i.ReadU32 ();
  m_Ms = i.ReadDouble ();
  m_Mw = i.ReadDouble ();
  m_Mv = i.ReadDouble ();
  m_EsMs = i.ReadDouble ();
  m_EwMw = i.ReadDouble ();
  m_EvMv = i.ReadDouble ();
}

void
Q3pTagPulseLocating::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this);
  os << "Pulse Locating Tag reports "
     << m_pulseNumber << " pulses detected"
     << m_Ms << " signal detected events"
     << m_Mw << " decoy detected events"
     << m_Mv << " vacuum detected events"
     << m_EsMs << " signal erroneous detected events"
     << m_EwMw << " decoy erroneous detected events"
     << m_EvMv << " vacuum erroneous detected events";
}

/**************************************
 *          Q3pTagBasisSifting
 **************************************/

NS_OBJECT_ENSURE_REGISTERED (Q3pTagBasisSifting);

Q3pTagBasisSifting::Q3pTagBasisSifting ()
{
  NS_LOG_FUNCTION (this);
}

Q3pTagBasisSifting::~Q3pTagBasisSifting ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
Q3pTagBasisSifting::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Q3pTagBasisSifting")
    .SetParent<Tag> ()
    .SetGroupName ("qkd")
    .AddConstructor<Q3pTagBasisSifting> ()
  ;
  return tid;
}

TypeId
Q3pTagBasisSifting::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);
  return GetTypeId ();
}

uint32_t
Q3pTagBasisSifting::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return 0;
}

void
Q3pTagBasisSifting::Serialize (TagBuffer i) const
{
  NS_LOG_FUNCTION (this);
}

void
Q3pTagBasisSifting::Deserialize (TagBuffer i)
{
  NS_LOG_FUNCTION (this);
}

void
Q3pTagBasisSifting::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this);
  os << "Basis Sifting Tag";
}

/**************************************
 *           Q3pTagKeySifting
 **************************************/

NS_OBJECT_ENSURE_REGISTERED (Q3pTagKeySifting);

Q3pTagKeySifting::Q3pTagKeySifting ()
{
  NS_LOG_FUNCTION (this);
}

Q3pTagKeySifting::~Q3pTagKeySifting ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
Q3pTagKeySifting::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Q3pTagKeySifting")
    .SetParent<Tag> ()
    .SetGroupName ("qkd")
    .AddConstructor<Q3pTagBasisSifting> ()
  ;
  return tid;
}

TypeId
Q3pTagKeySifting::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);
  return GetTypeId ();
}

uint32_t
Q3pTagKeySifting::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return 0;
}

void
Q3pTagKeySifting::Serialize (TagBuffer i) const
{
  NS_LOG_FUNCTION (this);
}

void
Q3pTagKeySifting::Deserialize (TagBuffer i)
{
  NS_LOG_FUNCTION (this);
}

void
Q3pTagKeySifting::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this);
  os << "Key Sifting Tag";
}

/**************************************
 *        Q3pTagErrorCorrection
 **************************************/

NS_OBJECT_ENSURE_REGISTERED (Q3pTagErrorCorrection);

Q3pTagErrorCorrection::Q3pTagErrorCorrection ()
{
  NS_LOG_FUNCTION (this);
}

Q3pTagErrorCorrection::~Q3pTagErrorCorrection ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
Q3pTagErrorCorrection::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Q3pTagErrorCorrection")
    .SetParent<Tag> ()
    .SetGroupName ("qkd")
    .AddConstructor<Q3pTagErrorCorrection> ()
  ;
  return tid;
}

TypeId
Q3pTagErrorCorrection::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);
  return GetTypeId ();
}

uint32_t
Q3pTagErrorCorrection::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return 0;
}

void
Q3pTagErrorCorrection::Serialize (TagBuffer i) const
{
  NS_LOG_FUNCTION (this);
}

void
Q3pTagErrorCorrection::Deserialize (TagBuffer i)
{
  NS_LOG_FUNCTION (this);
}

void
Q3pTagErrorCorrection::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this);
  os << "Error Correction Tag";
}

/**************************************
 *      Q3pTagPrivacyAmplification
 **************************************/

NS_OBJECT_ENSURE_REGISTERED (Q3pTagPrivacyAmplification);

Q3pTagPrivacyAmplification::Q3pTagPrivacyAmplification ()
{
  NS_LOG_FUNCTION (this);
}

Q3pTagPrivacyAmplification::~Q3pTagPrivacyAmplification ()
{
  NS_LOG_FUNCTION (this);
}

void
Q3pTagPrivacyAmplification::SetSecureBits (uint64_t bits)
{
  NS_LOG_FUNCTION (this << bits);
  m_secureBits = bits;
}

uint64_t
Q3pTagPrivacyAmplification::GetSecureBits (void) const
{
  return m_secureBits;
}

void
Q3pTagPrivacyAmplification::SetTotalBytes (uint64_t bytes)
{
  NS_LOG_FUNCTION (this << bytes);
  m_totalBytes = bytes;
}

uint64_t
Q3pTagPrivacyAmplification::GetTotalBytes (void) const
{
  return m_totalBytes;
}

TypeId
Q3pTagPrivacyAmplification::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Q3pTagPrivacyAmplification")
    .SetParent<Tag> ()
    .SetGroupName ("qkd")
    .AddConstructor<Q3pTagPrivacyAmplification> ()
  ;
  return tid;
}

TypeId
Q3pTagPrivacyAmplification::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);
  return GetTypeId ();
}

uint32_t
Q3pTagPrivacyAmplification::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return 16;
}

void
Q3pTagPrivacyAmplification::Serialize (TagBuffer i) const
{
  NS_LOG_FUNCTION (this);
  i.WriteU64 (m_secureBits);
  i.WriteU64 (m_totalBytes);
}

void
Q3pTagPrivacyAmplification::Deserialize (TagBuffer i)
{
  NS_LOG_FUNCTION (this);
  m_secureBits = i.ReadU64 ();
  m_totalBytes = i.ReadU64 ();
}

void
Q3pTagPrivacyAmplification::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this);
  os << "Privacy Amplification Tag:"
     << " secure bits " << m_secureBits
     << " total bytes " << m_totalBytes;
}

} // namespace ns3