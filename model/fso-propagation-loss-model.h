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

#ifndef FSO_PROPAGATION_LOSS_MODEL_H
#define FSO_PROPAGATION_LOSS_MODEL_H

#include "ns3/propagation-loss-model.h"

namespace ns3 {

class FsoChannel;
class FsoTxDevice;
class FsoRxDevice;

class FsoPropagationLossModel : public PropagationLossModel
{
public:
  static TypeId GetTypeId (void);
  FsoPropagationLossModel ();
  FsoPropagationLossModel (Ptr<FsoChannel> channel);
  virtual ~FsoPropagationLossModel ();
  void SetChannel (Ptr<FsoChannel> channel);
protected:
  virtual void DoInitialize ();
  virtual void DoDispose ();
  int64_t DoAssignStreams (int64_t stream);
private:
  FsoPropagationLossModel (const FsoPropagationLossModel &);
  FsoPropagationLossModel & operator = (const FsoPropagationLossModel &);

  /**
   * \brief Calculate the geometrical loss, Approximate method is used here
   * \param [in] tx transmitter device
   * \param [in] rx receiver device
   * \param [in] distance link distance between transmitter and receiver
   * \param [out] the geometrical loss
   */
  double DoCalcGeometricalLoss (Ptr<FsoTxDevice> tx, Ptr<FsoRxDevice> rx, double distance) const;

  /**
   * \brief Calculate the misalignment loss
   * \param [in] tx transmitter device
   * \param [in] rx receiver device
   * \param [out] the misalignment loss
   */
  double DoCalcMisalignmentLoss (Ptr<FsoTxDevice> tx, Ptr<FsoRxDevice> rx) const;

  /**
   * \brief Calculate the time synchronization loss
   * \param [in] tx transmitter device
   * \param [in] rx receiver device
   * \param [out] the time synchronization loss
   */
  double DoCalcTimeSyncLoss (Ptr<FsoTxDevice> tx, Ptr<FsoRxDevice> rx) const;

  /**
   * \brief Calculate the Atmospheric loss
   * \param [in] tx transmitter device
   * \param [in] rx receiver device
   * \param [out] the Atmospheric loss
   */
  double DoCalcAtmosphericLoss (Ptr<FsoTxDevice> tx, Ptr<FsoRxDevice> rx) const;

  /**
   * \brief Calculate the Optical loss
   * \param [in] tx transmitter device
   * \param [in] rx receiver device
   * \param [out] the Optical loss
   */
  double DoCalcOpticalLoss (Ptr<FsoTxDevice> tx, Ptr<FsoRxDevice> rx) const;

  /**
   * \brief Calculate the channel loss by given distance
   * \param [in] distance the distance between two parties
   *                      (here, it is different with Inherited function)
   * \param [in] a        mobility model of transmitter, not used
   * \param [in] b        mobility model of receiver, not used
   */
  virtual double DoCalcRxPower (double distance, Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;
  Ptr<FsoChannel> m_channel;
};

}

#endif