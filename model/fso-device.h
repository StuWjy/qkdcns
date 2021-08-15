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

#ifndef FSO_DEVICE_H
#define FSO_DEVICE_H

#include <queue>
#include "ns3/object.h"
#include "ns3/callback.h"
#include "ns3/simulator.h"
#include "ns3/rectangle.h"
#include "ns3/traced-callback.h"
#include "qkd-device.h"
#include "coordinate-turntable.h"
#include "adi-interval.h"

namespace ns3 
{

class NetDevice;
class FsoChannel;
class Node;
class Turntable;

class FsoDevice : public Object
{
public:
  static TypeId GetTypeId ();

  /**
   * \brief Default constructor
   */
  FsoDevice ();

  /**
   * \brief Deconstructor
   */
  virtual ~FsoDevice ();

  /**
   * \brief Set the Gaussian beam wavelength
   * \param wavelength the Gaussian beam wavelength, in meter
   */
  virtual void SetBeamWavelength (double wavelength) = 0;

  /**
   * \return the Gaussian beam wavelength, in meter
   */
  virtual double GetBeamWavelength () const = 0;

  // /**
  //  * \brief Set the installed surface
  //  * \param[in] side the installed surface
  //  */
  // void SetInstalledSurface (adi::Side side);

  // /**
  //  * \return the installed surface
  //  */
  // adi::Side GetInstalledSurface (void) const;

  /**
   * \brief Set the node
   * \param node the node
   */
  void SetNode (Ptr<Node> node);

  /**
   * \return the node
   */
  Ptr<Node> GetNode () const;

  /**
   * \brief Set the qkd device
   * \param qkdDevice the qkd device
   */
  void SetQkdDevice (Ptr<QkdDevice> qkdDevice);

  /**
   * \return the qkd device
   */
  Ptr<QkdDevice> GetQkdDevice () const;

  /**
   * \brief Set the net device
   * \param netDevice the net device
   */
  void SetNetDevice (Ptr<NetDevice> netDevice);

  /**
   * \return the net device
   */
  Ptr<NetDevice> GetNetDevice () const;

  /**
   * \brief Set the turntable
   * \param[in] turntable the turntable
   */
  void SetTurntable (Ptr<Turntable> turntable);

  /**
   * \brief Get the turntable
   * \return the turntable
   */
  Ptr<Turntable> GetTurntable () const;

  /**
   * \brief Set the FsoChannel
   * \param channel the FsoChannel
   */
  virtual void SetChannel (Ptr<FsoChannel> channel) = 0;

  /**
   * \return the FsoChannel
   */
  Ptr<FsoChannel> GetChannel () const;


  /**
   * Assign a fixed random variable stream number to the random variables
   * used by this model. Return the number of streams (possibly zero) that
   * have been assigned.
   *
   * \param stream first stream index to use
   * \return the number of stream indices assigned by this model
   */
  int64_t AssignStream (int64_t start);

  /**
   * \brief Set the connect callback
   * \param[in] connectionStarted   the connection callback when started
   * \param[in] connectionFinished  the connection callback when finished
   * \param[in] connectionSucceeded the connection callback when succeeding
   * \param[in] connectionFailed    the connection callback when failing
   */
  void SetConnectCallback (
  Callback<void> connectionStarted,
  Callback<void> connectionFinished,
  Callback<void> connectionSucceeded,
  Callback<void> connectionFailed);

  /**
   * \brief Notify through the callback (if set) that the connection has been
   *        started
   */
  virtual void NotifyConnectionStarted (void) = 0;

  /**
   * \brief Notify through the callback (if set) that the connection has been
   *        finished
   */
  virtual void NotifyConnectionFinished (void) = 0;

  /**
   * \brief Notify through the callback (if set) that the connection has been
   *        established.
   */
  virtual void NotifyConnectionSucceeded (void) = 0;

  /**
   * \brief Notify through the callback (if set) that the connection has not been
   *        established due to an error.
   */
  virtual void NotifyConnectionFailed (void) = 0;

  /**
   * \brief Get the period of qkd round
   * \return the period
   */
  Time GetRoundPeriod (void) const;

  adi::Interval GetInterval () const;

  void GetRoundInterval (Time& start, Time& stop) const;
  // /**
  //  * \brief Append the target, for swinging the turntable before link established
  //  * \param[in] time the time that turntable should swing to target
  //  * \param[in] coord the turntable coordinate of target pointing
  //  */
  // void AppendTarget (const Time& time, const CoordTurntable& coord);

  // /**
  //  * \brief Swing the turntable to the next target
  //  */
  // void SwingToTarget (void);
protected:
  virtual void DoInitialize ();
  virtual void DoDispose ();
  virtual void NotifyNewAggregate ();
  virtual int64_t DoAssignStream (int64_t stream);
protected:
  Ptr<Node>       m_node;       //!< The node associated to this FsoDevice
  Ptr<QkdDevice>  m_qkdDevice;  //!< The qkd device associated to this FsoDevice
  Ptr<NetDevice>  m_netDevice;  //!< The net device associated to this FsoDevice
  Ptr<Turntable>  m_turntable;  //!< The turntable associated to this FsoDevice
  Ptr<FsoChannel> m_channel;    //!< The free-space-optical channel associated to this FsoDevice
  Callback<void> m_connectionStarted;    //!< connection started callback
  Callback<void> m_connectionFinished;   //!< connection finished callback
  Callback<void> m_connectionSucceeded;  //!< connection succeeded callback
  Callback<void> m_connectionFailed;     //!< connection failed callback
  EventId m_connectionEvent;      //!< the event of connection establishing
  typedef void (* SentTracedCallback) (Ptr<const FsoDevice>, const Time&);
  TracedCallback<Ptr<const FsoDevice>, const Time&> m_connectionStartedTrace;
  TracedCallback<Ptr<const FsoDevice>, const Time&> m_connectionFinishedTrace;
  TracedCallback<Ptr<const FsoDevice>, const Time&> m_connectionSucceededTrace;
  TracedCallback<Ptr<const FsoDevice>, const Time&> m_connectionFailedTrace;
  Time m_timeStart;              //!< time start
  Time m_timeStop;               //!< time stop
  std::vector<std::pair<Time, CoordTurntable>> m_targets;
  EventId m_swingEvent;
private:

  /**
   * \brief Copy constructor
   * 
   * Defined and unimplemented to avoid misuse
   */
  FsoDevice (const FsoDevice& device);
};

std::ostream& operator<< (std::ostream &os, const Ptr<FsoDevice> device);

}

#endif