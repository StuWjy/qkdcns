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

#ifndef Q3P_RX_CACHE_H
#define Q3P_RX_CACHE_H

#include "q3p-cache.h"

namespace ns3 {

class Q3pRxCache : public Q3pCache
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  Q3pRxCache ();
  ~Q3pRxCache ();
  virtual void Flush (void);
  virtual void NotifyConnectionStarted (void);
  virtual void NotifyConnectionFinished (void);
  virtual void NotifyConnectionSucceeded (void);
  virtual void NotifyConnectionFailed (void);
  virtual void NotifyNewPacket (Ptr<Packet> packet);
  void NotifyNewDetectionEvent (
    const Time &begin,
    const Time &end,
    const double &dark,
    const double &loss);
protected:
  virtual void DoInitialize (void);
  virtual void DoDispose (void);
  virtual void NotifyNewAggregate (void);
  virtual void DoUpdate (void);
private:
  void DoHandleTimeSynchronization (Ptr<Packet> packet);
  void DoSendPulseLocating ();
  void DoHandleBasisSifting (Ptr<Packet> packet);
  void DoSendKeySifting ();
  void DoHandleErrorCorrection (Ptr<Packet> packet);
  void DoHandlePrivacyAmplification (Ptr<Packet> packet);
  uint32_t m_recvCountTimeSync;       //!< the count of received time synchronization tagging packet
  uint32_t m_sentCountPulseLocating;  //!< the count of sent pulse locating packet
  uint32_t m_recvCountBasisSifting;   //!< the count of sent basis sifting packet
  uint32_t m_sentCountKeySifting;     //!< the count of sent key sifting packet
  uint32_t m_recvCountErrorCorrection;//!< the count of received error correction packet

  /**
   * \brief Copy constructor
   * Defined and unimplemented to avoid misuse
   */
  Q3pRxCache (const Q3pRxCache &q3p);
  
  /**
   * \brief Copy constructor
   *
   * Defined and unimplemented to avoid misuse
   */
  Q3pRxCache &operator= (const Q3pRxCache &o);
};

}

#endif /* Q3P_RX_CACHE_H */