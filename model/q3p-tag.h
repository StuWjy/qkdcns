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

#ifndef Q3P_TAG_H
#define Q3P_TAG_H

#include "ns3/tag.h"
#include "ns3/nstime.h"

namespace ns3 {

class Q3pTagTimeSync : public Tag
{
public:
  /**
   * 
   * 
   */
  void SetStartTime (const Time &time);
  void SetStopTime (const Time &time);
  void SetPulses (const uint32_t &pulses);
  Time GetStartTime (void) const;
  Time GetStopTime (void) const;
  uint32_t GetPulses (void) const;
  Q3pTagTimeSync ();
  ~Q3pTagTimeSync ();

  // from Tag class
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;
private:
  int64_t   m_start;
  int64_t   m_stop;
  uint32_t  m_pulses;
};

class Q3pTagPulseLocating : public Tag
{
public:
  /**
   * 
   * 
   */
  void SetPulseNumber (uint32_t pulses);
  void SetMs (const double& Ms);
  void SetEsMs (const double& EsMs);
  void SetMw (const double& Ms);
  void SetEwMw (const double& EsMs);
  void SetMv (const double& Ms);
  void SetEvMv (const double& EsMs);
  // void SetSiftedBits (uint32_t bits);
  // void SetSecureBits (uint32_t bits);
  // void SetTimeSyncTaggingBegin (uint32_t begin);
  // void SetTimeSyncTaggingEnd (uint32_t end);
  uint32_t GetPulseNumber (void) const;
  double GetMs () const;
  double GetEsMs () const;
  double GetMw () const;
  double GetEwMw () const;
  double GetMv () const;
  double GetEvMv () const;
  // uint32_t GetSiftedBits (void) const;
  // uint32_t GetSecureBits (void) const;
  // uint32_t GetTimeSyncTaggingBegin (void) const;
  // uint32_t GetTimeSyncTaggingEnd (void) const;

  Q3pTagPulseLocating ();
  ~Q3pTagPulseLocating ();

  // from Tag class
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;
private:
  // uint64_t m_beginIdentifier;
  // uint64_t m_endIdentifier;
  uint32_t m_pulseNumber;
  double   m_Ms;
  double   m_EsMs;
  double   m_Mw;
  double   m_EwMw;
  double   m_Mv;
  double   m_EvMv;
  // uint32_t m_siftedBits;
  // uint32_t m_secureBits;
};

class Q3pTagBasisSifting : public Tag
{
public:

  Q3pTagBasisSifting ();
  ~Q3pTagBasisSifting ();

  // from Tag class
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;
private:
};

class Q3pTagKeySifting : public Tag
{
public:
  /**
   * 
   * 
   */

  Q3pTagKeySifting ();
  ~Q3pTagKeySifting ();

  // from Tag class
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;
private:
};

class Q3pTagErrorCorrection : public Tag
{
public:
  /**
   * 
   * 
   */

  Q3pTagErrorCorrection ();
  ~Q3pTagErrorCorrection ();

  // from Tag class
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;
private:
};

class Q3pTagPrivacyAmplification : public Tag
{
public:
  /**
   * 
   * 
   */

  Q3pTagPrivacyAmplification ();
  ~Q3pTagPrivacyAmplification ();
  void SetSecureBits (uint64_t bits);
  uint64_t GetSecureBits (void) const;
  void SetTotalBytes (uint64_t bytes);
  uint64_t GetTotalBytes (void) const;
  // from Tag class
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;
private:
  uint64_t m_secureBits;
  uint64_t m_totalBytes;
};

}

#endif /* Q3P_TAG_H */