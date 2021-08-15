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

#ifndef Q3P_L3_PROTOCOL_H
#define Q3P_L3_PROTOCOL_H

#include <vector>
#include <queue>
#include "ns3/net-device.h"
#include "ns3/ptr.h"
#include "q3p-tx-cache.h"
#include "q3p-rx-cache.h"

namespace ns3 {

class Node;
class Packet;
class NetDevice;
class FsoDevice;
class QkdDevice;
class Q3pHeader;
class Ipv4Interface;

/**
 * Qkd Post-Processing Protocol (Qppp, or simply Q3p) 
 */
class Q3pL3Protocol : public Object
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  static const uint16_t PROT_NUMBER;  //!< Qkd protocol number (0x0806)
  Q3pL3Protocol ();
  ~Q3pL3Protocol ();

  /**
   * \brief Set the node the Qkd L3 protocol is associated with
   * \param node the node
   */
  void SetNode (Ptr<Node> node);

  /**
   * \return the node
   */
  Ptr<Node> GetNode (void) const;

  Ptr<Q3pCache> CreateCache (Ptr<NetDevice> device, Ptr<Ipv4Interface> interface);

  Ptr<Q3pCache> FindCache (Ptr<NetDevice> device);

  /**
   * \brief Send qkd post-processing protocol message
   * \param[in] cache  the q3p cache
   * \param[in] packet the packet
   */
  void SendMessage (Ptr<Q3pCache> cache, Ptr<Packet> packet);

  /**
   * \brief Receive a packet
   * \param device the source NetDevice
   * \param p the packet
   * \param protocol the protocol
   * \param from the source address
   * \param to the destination address
   * \param packetType type of packet (i.e., unicast, multicast, etc.)
   */
  void Receive (
    Ptr<NetDevice>        device,
    Ptr<const Packet>     p,
    uint16_t              protocol,
    const Address         &from,
    const Address         &to,
    NetDevice::PacketType packetType);

private:
  typedef std::list<Ptr<Q3pCache>> CacheList;
  virtual void DoInitialize (void);
  virtual void DoDispose (void);
  /*
   * This function will notify other components connected to the node that a new stack member is now connected
   * This will be used to notify Layer 3 protocol of layer 4 protocol stack to connect them together.
   */
  virtual void NotifyNewAggregate ();

  /**
   * \brief Store QkdKey into associated QkdKeyPool
   * \param[in] partyNode party node
   * \param[in] size the size of QkdKey
   * \return true if succeeded, false otherwise
   */
  bool StoreQkdKeys (Ptr<Node> partyNode, uint32_t size);
  Ptr<Node> m_node;   //!< The associated node
  CacheList m_cacheList;
  Time      m_syncPulseCycle;          //!< The cycle of synchronization pulse
  uint32_t  m_pulsesTimeSyncTag;       //!< Indicate how many time sync pulse is in the time sync tagging
public:

  double GetFrequency () const;
  double GetBasisRatio () const;
  double GetErrorRate () const;
  double GetSignalPhotons () const;
  double GetDecoyPhotons () const;
  double GetSignalProbability () const;
  double GetDecoyProbability () const;
  double GetVacuumProbability ()const;
private:
  struct State
  {
    double m; //!< mean photons of state
    double p; //!< probability of state
  };
  void DoSetFrequency (const double& freq);
  void DoSetBasisRatio (const double& q);
  void DoSetErrorRate (const double& e);
  void DoSetSignalPhotons (const double& m);
  void DoSetDecoyPhotons (const double& m);
  void DoSetSignalProbability (const double& p);
  void DoSetDecoyProbability (const double& p);
  void DoSetVacuumProbability (const double& p);
  double m_freq;
  double m_q;
  double m_e;
  State  m_sig;
  State  m_dec;
  State  m_vac;
};

} // namespace ns3

#endif /* Q3P_L3_PROTOCOL_H */