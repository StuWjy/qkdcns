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

#ifndef Q3P_TX_CACHE_H
#define Q3P_TX_CACHE_H

#include "q3p-cache.h"

namespace ns3 {

class Q3pTxCache : public Q3pCache
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  Q3pTxCache ();
  ~Q3pTxCache ();
  virtual void Flush (void);
  virtual void NotifyConnectionStarted (void);
  virtual void NotifyConnectionFinished (void);
  virtual void NotifyConnectionSucceeded (void);
  virtual void NotifyConnectionFailed (void);
  virtual void NotifyNewPacket (Ptr<Packet> packet);
protected:
  virtual void DoInitialize (void);
  virtual void DoDispose (void);
  virtual void NotifyNewAggregate (void);
  virtual void DoUpdate (void);
  void DoSendTimeSyncTagging (void);
  void DoHandlePulseLocating (Ptr<Packet> packet);
  void DoSendBasisSifting (void);
  void DoHandleKeySifting (Ptr<Packet> packet);
  void DoSendErrorCorrection (void);
  void DoSendPrivacyAmplification (void);
  uint32_t  m_sentCountTimeSync;       //!< the count of sent time synchronization tagging packet
  uint32_t  m_recvCountPulseLocating;  //!< the count of received pulse locating packet
  uint32_t  m_sentCountBasisSifting;   //!< the count of received basis sifting packet
  uint32_t  m_recvCountKeySifting;     //!< the count of received key sifting packet
  uint32_t  m_sentCountErrorCorrection;//!< the count of sent error correction packet
  uint32_t  m_syncPulses;
  Time      m_syncPulseCycle;          //!< The cycle of synchronization pulse
  uint32_t  m_pulsesTimeSyncTag;       //!< Indicate how many time sync pulse is in the time sync tagging
  Time      m_lastUpdated;             //!< 
  EventId   m_syncEvent;               //!< The event of time synchronization
  /**
   * \brief Copy constructor
   * Defined and unimplemented to avoid misuse
   */
  Q3pTxCache (const Q3pTxCache &q3p);
  
  /**
   * \brief Copy constructor
   *
   * Defined and unimplemented to avoid misuse
   */
  Q3pTxCache &operator= (const Q3pTxCache &o);
};

}

#endif /* Q3P_TX_CACHE_H */