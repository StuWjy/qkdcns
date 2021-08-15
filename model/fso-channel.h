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
#ifndef FSO_CHANNEL_H
#define FSO_CHANNEL_H

#include <queue>
#include "ns3/channel.h"
#include "ns3/event-id.h"
#include "ns3/callback.h"
#include "coordinate-turntable.h"
#include "fso-tx-device.h"
#include "fso-rx-device.h"
#include "adi-type-define.h"

namespace ns3 {

class FsoPropagationLossModel;
class FsoPropagationDelayModel;

/**
 * \brief Free space optical link states
 */
enum FsoLinkState
{
  /* Unconnected state, never connected */
  UNCONNECTED,
  /* Connecting state, channel is establishing link */
  CONNECTING,
  /* Re-connecting state, only after the disconnected link */
  RECONNECTING,
  /* Disconnected state */
  DISCONNECTED,
  /* Linking state (data is being transmitted from FsoNetDevice and received by FsoNetDevice). */
  CONNECTED,
  /* Connection finished */
  CONNECTION_DONE
};

std::string FsoLinkStateToString (const FsoLinkState& state);

class FsoChannel : public Channel
{
public:
  static TypeId GetTypeId (void);
  FsoChannel ();
  FsoChannel (const adi::LinkInfo& data);
  virtual ~FsoChannel ();
  // declaration of inherited function start
public:
  virtual std::size_t GetNDevices (void) const;
  virtual Ptr<NetDevice> GetDevice (std::size_t i) const;
  // declaration of inherited function over
  int64_t AssignStreams (int64_t stream);
  Ptr<FsoTxDevice> GetTxDevice () const;
  Ptr<FsoRxDevice> GetRxDevice () const;
  void SetPropagationLossModel (const Ptr<FsoPropagationLossModel> loss);
  void SetPropagationDelayModel (const Ptr<FsoPropagationDelayModel> delay);
  double CalcChannelLoss ();
  double GetDistance (void) const;
  /**
   * \brief Attach the fso devices at given time
   * \param[in] tx    the fso tx device
   * \param[in] rx    the fso rx device
   * \param[in] data  the link datas of the whole linking time
   */
  void Attach (Ptr<FsoDevice> tx, Ptr<FsoDevice> rx, const adi::LinkDatas& data);
  void StartSending (Ptr<FsoTxDevice> txFso);
  void StopSending (Ptr<FsoTxDevice> txFso);
private:
  struct LinkData
  {
    Time t;
    double distance;
    adi::Pointing fromSrc;
    adi::Pointing fromDst;
  };
  typedef std::queue<LinkData> LinkDatas;
  /**
   * \brief Set the fso-tx-device
   * \param[in] fsoDevice the fso-tx-device 
   */
  void DoSetTxDevice (Ptr<FsoDevice> fsoDevice);

  /**
   * \brief Set the fso-rx-device
   * \param[in] fsoDevice the fso-rx-device 
   */
  void DoSetRxDevice (Ptr<FsoDevice> fsoDevice);

  // /**
  //  * \brief Enable the fso-channel
  //  */
  // void DoEnable (void);

  // /**
  //  * \brief Disable the fso-channel
  //  */
  // void DoDisable (void);

  /**
   * \brief Append the link data
   * \param[in] data the link data
   */
  void DoAppend (const adi::LinkDatas& data);

  void DoSwing ();
  virtual void DoInitialize ();
  virtual void DoDispose ();
  virtual void DoUpdate ();
  class Link
  {
  public:
    /**
     * \brief Create the link, it will be in DISCONNECTING state
     */
    Link()
    : m_state (UNCONNECTED), m_tx (0), m_rx (0) {}
    FsoLinkState      m_state;          //!< State of the link
    Ptr<FsoTxDevice>  m_tx;             //!< Tx net device
    Ptr<FsoRxDevice>  m_rx;             //!< Rx net device
    CoordTurntable    m_txCurrTarget;   //!< the current coordinate of tx's target
    CoordTurntable    m_rxCurrTarget;   //!< the current coordinate of rx's target
    CoordTurntable    m_txPredTarget;   //!< the predicted coordinate of tx's target after 1 second
    CoordTurntable    m_rxPredTarget;   //!< the predicted coordinate of rx's target after 1 second
    double            m_currDistance;   //!< the current distance between two parties
    double            m_predDistance;   //!< the predicted distance between two parties
  };
  /** Each fso link has exactly two net devices. */
  static const std::size_t          N_DEVICES = 2;
  uint32_t    m_index;
  Link        m_link;     //!< Link list, only first link is usable.
  std::size_t m_nDevices; //!< Devices of this channel
  Ptr<FsoPropagationLossModel>  m_loss;             //!< Loss model of this channel
  Ptr<FsoPropagationDelayModel> m_delay;            //!< Delay model of this channel
  Time      m_step;
  Time      m_sendStart;
  Time      m_sendStop;
  bool      m_isSending;
  EventId   m_enableEvent;
  EventId   m_updateEvent;
  EventId   m_connectingEvent;
  EventId   m_sendingEvent;
  EventId   m_finishedEvent;
  EventId   m_txStartedEvent;
  EventId   m_rxStartedEvent;
  LinkDatas m_linkDatas;
};

} //namespace ns3

#endif /* YANS_WIFI_CHANNEL_H */