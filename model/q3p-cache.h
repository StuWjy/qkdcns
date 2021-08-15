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

#ifndef Q3P_CACHE_H
#define Q3P_CACHE_H

#include "ns3/object.h"
#include "ns3/nstime.h"
#include "ns3/callback.h"
#include "ns3/simulator.h"
#include "ns3/qkd-satellite.h"
#include "ns3/qkd-station.h"

namespace ns3 {

class Node;
class Packet;
class FsoDevice;
class NetDevice;
class QkdDevice;
class Q3pL3Protocol;
class Ipv4Interface;

class Q3pCache : public Object
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Default constructor
   */
  Q3pCache ();

  /**
   * \brief Deconstructor
   */
  ~Q3pCache ();

  /**
   * \brief Set the node of q3p cache
   * \param[in] node the node
   */
  void SetNode (Ptr<Node> node);

  /**
   * \return the node
   */
  Ptr<Node> GetNode (void) const;

  /**
   * \brief Set the Q3pProtocol of q3p cache
   * \param[in] node the Q3pProtocol
   */
  void SetProtocol (Ptr<Q3pL3Protocol> protocol);

  /**
   * \return the Q3pProtocol
   */
  Ptr<Q3pL3Protocol> GetProtocol (void) const;

  /**
   * \brief Set the qkd device of q3p cache
   * \param[in] qkd the qkd device
   */
  void SetQkdDevice (Ptr<QkdDevice> qkd);

  /**
   * \return the qkd device
   */
  Ptr<QkdDevice> GetQkdDevice (void) const;

  /**
   * \return the fso device
   */
  Ptr<FsoDevice> GetFsoDevice (void) const;

  /**
   * \return the net device
   */
  Ptr<NetDevice> GetNetDevice (void) const;

  /**
   * \brief Set the ipv4 interface of q3p cache
   * \param[in] interface the ipv4 interface
   */
  void SetInterface (Ptr<Ipv4Interface> interface);

  /**
   * \return the ipv4 interface
   */
  Ptr<Ipv4Interface> GetInterface (void) const;

  const uint64_t& GetTotalEvents () const;
  const double& GetSignalEvents () const;
  const double& GetDecoyEvents () const;
  const double& GetVacuumEvents () const;
  const double& GetSignalErrorEvents () const;
  const double& GetDecoyErrorEvents () const;
  const double& GetVacuumErrorEvents () const;
  const uint64_t& GetSecureBits () const;
  /**
   * \brief Clear the datas of Q3pCache
   */
  virtual void Flush (void);

  /**
   * \brief Notified when connection start
   */
  virtual void NotifyConnectionStarted (void) = 0;

  /**
   * \brief Notified when connection finished
   */
  virtual void NotifyConnectionFinished (void) = 0;

  /**
   * \brief Notified when connnection succeeded
   */
  virtual void NotifyConnectionSucceeded (void) = 0;

  /**
   * \brief Notified when connection failed
   */
  virtual void NotifyConnectionFailed (void) = 0;

  /**
   * \brief Notified when new packet arrived
   */
  virtual void NotifyNewPacket (Ptr<Packet> packet) = 0;

protected:
  virtual void DoInitialize (void);
  virtual void DoDispose (void);
  virtual void NotifyNewAggregates (void);
  Ptr<Node>           m_node;       //!< Node associated with the cache
  Ptr<Q3pL3Protocol>  m_q3p;        //!< Q3p protocol associated with the cache
  Ptr<QkdDevice>      m_qkd;        //!< QkdDevice associated with the cache
  // Ptr<QkdConfig>      m_config;     //!< QkdConfig associated with the cache
  Ptr<Ipv4Interface>  m_interface;  //!< Ipv4Interface associated with the cache
  Time   m_start;
  Time   m_stop;
  enum Type_e
  {
    Total,
    Negotiating,
    Processing
  };
  uint64_t m_events[3];
  uint64_t m_detectionEvents[3];
  uint64_t m_correctEvents[3];
  uint64_t m_secureBits;
  double m_eventSig[3];  //!< signal pulse events
  double m_errorSig[3];  //!< signal pulse error events
  double m_eventDec[3];  //!< decoy pulse events
  double m_errorDec[3];  //!< decoy pulse error events
  double m_eventVac[3];  //!< vacuum pulse events
  double m_errorVac[3];  //!< vacuum pulse error events
  bool   m_processing;//!< whether is processing
  uint64_t m_totalBytes;
private:

  /**
   * \brief Copy constructor
   * Defined and unimplemented to avoid misuse
   */
  Q3pCache (const Q3pCache &q3p);
  
  /**
   * \brief Copy constructor
   *
   * Defined and unimplemented to avoid misuse
   */
  Q3pCache &operator= (const Q3pCache &o);

};

}

#endif /* Q3P_CACHE_H */