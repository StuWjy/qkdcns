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

#include "q3p-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Q3pHeader");

NS_OBJECT_ENSURE_REGISTERED (Q3pHeader);

TypeId
Q3pHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Q3pHeader")
    .SetParent<Header> ()
    .SetGroupName ("Qkd")
    .AddConstructor<Q3pHeader> ()
  ;
  return tid;
}

Q3pHeader::Q3pHeader ()
: m_type (0)
{
  NS_LOG_FUNCTION (this);
}

Q3pHeader::~Q3pHeader ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
Q3pHeader::GetInstanceTypeId () const
{
  NS_LOG_FUNCTION (this);
  return GetTypeId ();
}

uint32_t
Q3pHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return 1;
}

void
Q3pHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_type);
}

uint32_t
Q3pHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  m_type = start.ReadU8 ();
  return 1;
}

void
Q3pHeader::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this << &os);
  os << "type: " << (uint32_t)m_type;
}

void
Q3pHeader::SetType (uint8_t type)
{
  NS_LOG_FUNCTION (this << static_cast<uint32_t> (type));
  m_type = type;
}

uint8_t
Q3pHeader::GetType () const
{
  return m_type;
}

std::string
Q3pHeader::GetTypeString (void) const
{
  switch (m_type)
  {
    case Q3P_TIME_SYNC_TAGGING:     return "Time synchronization tagging";
    case Q3P_PULSE_LOCATING:        return "Pulse locating";
    case Q3P_BASIS_SIFTING:         return "Basis sifting";
    case Q3P_KEY_SIFTING:           return "Key sifting";
    case Q3P_ERROR_CORRECTION:      return "Error correction";
    case Q3P_PRIVACY_AMPLIFICATION: return "Privacy amplification";
    default:                        NS_ASSERT (false);
  }
  return "";
}

std::ostream & operator<< (std::ostream & os, Q3pHeader const & h)
{
  h.Print (os);
  return os;
}

}