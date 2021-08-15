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

#include "fso-tx-device.h"
#include "fso-channel.h"
#include "turntable.h"
#include "qkd-node.h"
#include "util.h"
#include "constant.h"
#include "ns3/double.h"
#include "ns3/box.h"
#include "ns3/pointer.h"
#include "ns3/random-variable-stream.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("FsoTxDevice");

NS_OBJECT_ENSURE_REGISTERED (FsoTxDevice);

TypeId
FsoTxDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::FsoTxDevice")
  .SetParent<FsoDevice> ()
  .SetGroupName ("Fso")
  .AddConstructor<FsoTxDevice> ()
  .AddAttribute ("TxBeamWavelength", "The wavelength of transmitter's Gaussian beam, in meter",
                 DoubleValue (848.6e-9),
                 MakeDoubleAccessor (&FsoTxDevice::GetBeamWavelength,
                                     &FsoTxDevice::SetBeamWavelength),
                 MakeDoubleChecker<double> (250.0e-9, 2500.0e-9)
                )
  .AddAttribute ("TxBeamWaist", "The radius of transmitter's Gaussian beam waist, in meter",
                 DoubleValue (27e-3),
                 MakeDoubleAccessor (&FsoTxDevice::GetBeamWaist,
                                     &FsoTxDevice::SetBeamWaist),
                 MakeDoubleChecker<double> (0.0)
                )
  .AddAttribute ("TxBeamDivergence", "The divergence half-angle (1/e^2) of transmitter's Gaussian beam, in radian",
                 DoubleValue (11e-6),
                 MakeDoubleAccessor (&FsoTxDevice::GetBeamDivergence,
                                     &FsoTxDevice::SetBeamDivergence),
                 MakeDoubleChecker<double> (0.0)
                )
  .AddAttribute ("TxBeamMinDivergence", "The theoretical minimum divergence half-angle (1/e^2) of transmitter's Gaussian beam, in radian",
                 TypeId::ATTR_GET,
                 DoubleValue (1),
                 MakeDoubleAccessor (&FsoTxDevice::GetBeamMinDivergence),
                 MakeDoubleChecker<double> (0.0)
                )
  .AddAttribute ("TxLensDiameter", "The diameter of transmitter's lens, in meter",
                 DoubleValue (0.3),
                 MakeDoubleAccessor (&FsoTxDevice::GetLensDiameter,
                                     &FsoTxDevice::SetLensDiameter),
                 MakeDoubleChecker<double> (0.0)
                )
  .AddAttribute ("TxLensEfficiency", "The efficiency of transmitter's lens",
                 TypeId::ATTR_GET,
                 DoubleValue (1.0),
                 MakeDoubleAccessor (&FsoTxDevice::GetLensEfficiency),
                 MakeDoubleChecker<double> ()
                )
  ;
  return tid;
}

FsoTxDevice::FsoTxDevice ()
: FsoDevice ()
{
  NS_LOG_FUNCTION (this);
}

FsoTxDevice::~FsoTxDevice ()
{
  NS_LOG_FUNCTION (this);
}

void
FsoTxDevice::SetBeamWavelength (double wavelength)
{
  m_txBeamWavelength = wavelength;
  DoCalcBeamMinDivergence ();
  DoCalcLensEfficiency ();
}

double
FsoTxDevice::GetBeamWavelength () const
{
  return m_txBeamWavelength;
}

void
FsoTxDevice::SetBeamWaist (double waist)
{
  m_txBeamWaist = waist;
  DoCalcBeamMinDivergence ();
  DoCalcLensEfficiency ();
}

double
FsoTxDevice::GetBeamWaist () const
{
  return m_txBeamWaist;
}

double
FsoTxDevice::GetBeamMinDivergence () const
{
  return m_txBeamMinDivergence;
}

void
FsoTxDevice::SetBeamDivergence (double div)
{
  m_txBeamDivergence = div;
  DoCalcBeamMinDivergence ();
  DoCalcLensEfficiency ();
}

double
FsoTxDevice::GetBeamDivergence () const
{
  return m_txBeamDivergence;
}

void
FsoTxDevice::SetLensDiameter (double dia)
{
  m_txLensDiameter = dia;
  DoCalcBeamMinDivergence ();
  DoCalcLensEfficiency ();
}

double
FsoTxDevice::GetLensDiameter () const
{
  return m_txLensDiameter;
}

double
FsoTxDevice::GetLensEfficiency () const
{
  return m_txLensEfficiency;
}

void
FsoTxDevice::SetChannel (Ptr<FsoChannel> channel)
{
  // NS_ASSERT (channel);
  if (!channel)
  {
    NS_LOG_LOGIC ("The fso-channel is deactive.");
  }
  {
    NS_LOG_LOGIC ("The fso-channel is active.");
  }
  m_channel = channel;
}

void
FsoTxDevice::NotifyConnectionStarted (void)
{
  NS_LOG_FUNCTION (this);
  m_connectionStartedTrace (this, Now ());
  m_timeStart = Now ();
  if (!m_connectionStarted.IsNull ())
  {
    m_connectionStarted ();
  }
}

void
FsoTxDevice::NotifyConnectionFinished (void)
{
  NS_LOG_FUNCTION (this);
  m_connectionFinishedTrace (this, Now ());
  m_timeStop = Now ();
  m_channel->StopSending (this);
  if (!m_connectionFinished.IsNull ())
  {
    m_connectionFinished ();
  }
}

void 
FsoTxDevice::NotifyConnectionSucceeded (void)
{
  NS_LOG_FUNCTION (this);
  m_connectionSucceededTrace (this, Now ());
  m_channel->StartSending (this);
  if (!m_connectionSucceeded.IsNull ())
  {
    m_connectionSucceeded ();
  }
}

void 
FsoTxDevice::NotifyConnectionFailed (void)
{
  NS_LOG_FUNCTION (this);
  m_connectionFailedTrace (this, Now ());
  m_channel->StopSending (this);
  if (!m_connectionFailed.IsNull ())
  {
    m_connectionFailed ();
  }
}

void
FsoTxDevice::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
  FsoDevice::DoInitialize ();
}

void
FsoTxDevice::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  FsoDevice::DoDispose ();
}

void
FsoTxDevice::DoCalcBeamMinDivergence ()
{
  NS_LOG_FUNCTION (this);
  m_txBeamMinDivergence = m_txBeamWavelength / M_PI / m_txBeamWaist;
  if (m_txBeamDivergence < m_txBeamMinDivergence)
  {
    NS_LOG_INFO ("The present divergence of beam is less than theoretical value and has been set to be theoretical value.");
    m_txBeamDivergence = m_txBeamMinDivergence;
  }
}

void
FsoTxDevice::DoCalcLensEfficiency ()
{
  NS_LOG_FUNCTION (this);
  double radius = m_txLensDiameter / 2;
  double radius2 = radius * radius;
  double waist2 = m_txBeamWaist * m_txBeamWaist;
  m_txLensEfficiency = 1 - exp (- 2 * radius2 / waist2);
}

}