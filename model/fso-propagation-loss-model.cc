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

#include "ns3/log.h"
#include "ns3/math.h"
#include "ns3/double.h"
#include "ns3/mobility-model.h"
#include "fso-propagation-loss-model.h"
#include "fso-channel.h"
#include "fso-device.h"
#include "turntable.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("FsoPropagationLossModel");

NS_OBJECT_ENSURE_REGISTERED (FsoPropagationLossModel);

TypeId
FsoPropagationLossModel::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::FsoPropagationLossModel")
  .SetParent<PropagationLossModel> ()
  .SetGroupName ("Fso")
  .AddConstructor<FsoPropagationLossModel> ()
  ;
  return tid;
}

FsoPropagationLossModel::FsoPropagationLossModel ()
: PropagationLossModel ()
{
  NS_LOG_FUNCTION (this);
}

FsoPropagationLossModel::FsoPropagationLossModel (Ptr<FsoChannel> channel)
: PropagationLossModel ()
, m_channel (channel)
{
  NS_LOG_FUNCTION (this);
}

FsoPropagationLossModel::~FsoPropagationLossModel ()
{
  NS_LOG_FUNCTION (this);
}

void
FsoPropagationLossModel::SetChannel (Ptr<FsoChannel> channel)
{
  NS_LOG_FUNCTION (this << channel);
  NS_ASSERT (channel != 0);
  m_channel = channel;
}

void
FsoPropagationLossModel::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
  PropagationLossModel::DoInitialize ();
}

void
FsoPropagationLossModel::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  PropagationLossModel::DoDispose ();
}

int64_t
FsoPropagationLossModel::DoAssignStreams (int64_t streams)
{
  return 1;
}

double
FsoPropagationLossModel::DoCalcGeometricalLoss (Ptr<FsoTxDevice> tx, Ptr<FsoRxDevice> rx, double distance) const
{
  double divergence = tx->GetBeamDivergence ();
  double diameter = rx->GetTelescopeDiameter ();

  double diameter2 = diameter * diameter;
  double tmp = divergence * distance * 1e3;
  double tmp2 = tmp * tmp;
  //precise
  // DoubleValue waist;
  // lens->GetAttribute ("BeamWaist", waist);
  // double waist2 = waist * waist;
  // double lossGeom = 1.0 - exp (-0.5 * diameter2 / (waist2 + tmp2));
  //approx
  double lossGeom = 0.5 * diameter2 / tmp2;
  return lossGeom;
}

double
FsoPropagationLossModel::DoCalcMisalignmentLoss (Ptr<FsoTxDevice> tx, Ptr<FsoRxDevice> rx) const
{
  Ptr<Turntable> turntable;
  turntable = tx->GetObject<Turntable> ();
  NS_ASSERT (turntable);
  double pointingBias = turntable->GetPointingBias ();
  double pointingStd = turntable->GetPointingStd ();
  double divergence = tx->GetBeamDivergence ();
  turntable = rx->GetObject<Turntable> ();
  NS_ASSERT (turntable);
  double pointingBias2 = pointingBias * pointingBias;
  double pointingStd2 = pointingStd * pointingStd;
  double divergence2 = divergence * divergence;

  double tmp1 = divergence2 + 4 * pointingStd2;
  double lossMis = divergence2 / tmp1 * exp (-2 * pointingBias2 / tmp1);
  return lossMis;
}

double
FsoPropagationLossModel::DoCalcTimeSyncLoss (Ptr<FsoTxDevice> tx, Ptr<FsoRxDevice> rx) const
{
  return 1.0;
}

double
FsoPropagationLossModel::DoCalcAtmosphericLoss (Ptr<FsoTxDevice> tx, Ptr<FsoRxDevice> rx) const
{
  return 1.0;
}

double
FsoPropagationLossModel::DoCalcOpticalLoss (Ptr<FsoTxDevice> tx, Ptr<FsoRxDevice> rx) const
{
  double lensEffy = tx->GetLensEfficiency ();
  double telescopeEffy = rx->GetTelescopeEfficiency ();
  double detectorEffy = rx->GetDetectorEfficiency ();

  double lossOptics = lensEffy * telescopeEffy * detectorEffy;
  return lossOptics;
}

double
FsoPropagationLossModel::DoCalcRxPower (double distance,
                                        Ptr<MobilityModel> a,
                                        Ptr<MobilityModel> b) const
{
  Ptr<FsoTxDevice> txDevice= m_channel->GetTxDevice ();
  Ptr<FsoRxDevice> rxDevice= m_channel->GetRxDevice ();
  double rxPowerDb =  1.0
                    * DoCalcGeometricalLoss (txDevice, rxDevice, distance)
                    * DoCalcMisalignmentLoss (txDevice, rxDevice)
                    * DoCalcTimeSyncLoss (txDevice, rxDevice)
                    * DoCalcAtmosphericLoss (txDevice, rxDevice)
                    * DoCalcOpticalLoss (txDevice, rxDevice);
  return rxPowerDb;
}

}