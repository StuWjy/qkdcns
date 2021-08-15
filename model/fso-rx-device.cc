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

#include "fso-rx-device.h"
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

NS_LOG_COMPONENT_DEFINE ("FsoRxDevice");

NS_OBJECT_ENSURE_REGISTERED (FsoRxDevice);

TypeId
FsoRxDevice::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::FsoRxDevice")
  .SetParent<FsoDevice> ()
  .SetGroupName ("Fso")
  .AddConstructor<FsoRxDevice> ()
  .AddAttribute ("RxMeanDarkCountRate", "The mean dark count rate of receiver's detector, in Hz",
                 DoubleValue (300.0),
                 MakeDoubleAccessor (&FsoRxDevice::GetDetectorMeanDarkCount,
                                     &FsoRxDevice::SetDetectorMeanDarkCount),
                 MakeDoubleChecker<double> (0.0)
                )
  .AddAttribute ("RxVarDarkCountRate", "The variance dark count rate of receiver's detector, in square Hz",
                 DoubleValue (50.0),
                 MakeDoubleAccessor (&FsoRxDevice::GetDetectorVarDarkCount,
                                     &FsoRxDevice::SetDetectorVarDarkCount),
                 MakeDoubleChecker<double> (0.0)
                )
  .AddAttribute ("RxMaxDarkCountRate", "The maximum dark count rate of receiver's detector, in Hz",
                 DoubleValue (1000.0),
                 MakeDoubleAccessor (&FsoRxDevice::GetDetectorMaxDarkCount,
                                     &FsoRxDevice::SetDetectorMaxDarkCount),
                 MakeDoubleChecker<double> (0.0)
                )
  .AddAttribute ("RxMinDarkCountRate", "The minimum dark count rate of receiver's detector, in Hz",
                 DoubleValue (1.0),
                 MakeDoubleAccessor (&FsoRxDevice::GetDetectorMinDarkCount,
                                     &FsoRxDevice::SetDetectorMinDarkCount),
                 MakeDoubleChecker<double> (0.0)
                )
  .AddAttribute ("RxBeamWavelength", "The wavelength of receiver's Gaussian beam, in meter",
                 DoubleValue (848.6e-9),
                 MakeDoubleAccessor (&FsoRxDevice::GetBeamWavelength,
                                     &FsoRxDevice::SetBeamWavelength),
                 MakeDoubleChecker<double> (250.0e-9, 2500.0e-9)
                )
  .AddAttribute ("RxDetectorEfficiency", "The efficiency of receiver's detector",
                 DoubleValue (0.5),
                 MakeDoubleAccessor (&FsoRxDevice::GetDetectorEfficiency,
                                     &FsoRxDevice::SetDetectorEfficiency),
                 MakeDoubleChecker<double> (0.0, 1.0)
                )
  .AddAttribute ("RxDetectorError", "The error rate of receiver's detector",
                 DoubleValue (0.003),
                 MakeDoubleAccessor (&FsoRxDevice::GetDetectorError,
                                     &FsoRxDevice::SetDetectorError),
                 MakeDoubleChecker<double> (0.0, 1.0)
                )
  .AddAttribute ("RxDetectorThreshold", "The threshold of receiver's detector",
                 DoubleValue (2.0e-9),
                 MakeDoubleAccessor (&FsoRxDevice::GetDetectorThreshold,
                                     &FsoRxDevice::SetDetectorThreshold),
                 MakeDoubleChecker<double> (0.0, 1.0)
                )
  .AddAttribute ("RxTelescopeDiameter", "The diameter of receiver's telescope, in meter",
                 DoubleValue (1.0),
                 MakeDoubleAccessor (&FsoRxDevice::GetTelescopeDiameter,
                                     &FsoRxDevice::SetTelescopeDiameter),
                 MakeDoubleChecker<double> (0.0)
                )
  .AddAttribute ("RxTelescopeEfficiency", "The efficiency of receiver's telescope",
                 DoubleValue (0.16),
                 MakeDoubleAccessor (&FsoRxDevice::GetTelescopeEfficiency,
                                     &FsoRxDevice::SetTelescopeEfficiency),
                 MakeDoubleChecker<double> (0.0, 1.0)
                )
  ;
  return tid;
}

FsoRxDevice::FsoRxDevice ()
: FsoDevice ()
{
  NS_LOG_FUNCTION (this);
  m_darkCountRate = CreateObject<NormalRandomVariable> ();
}

FsoRxDevice::~FsoRxDevice ()
{
  NS_LOG_FUNCTION (this);
}

double
FsoRxDevice::GetDarkCountRate () const
{
  return std::min (m_maxDarkCount, std::max (m_minDarkCount, m_darkCountRate->GetValue ()));
}

void
FsoRxDevice::SetBeamWavelength (double wavelength)
{
  m_rxBeamWavelength = wavelength;
}

double
FsoRxDevice::GetBeamWavelength () const
{
  return m_rxBeamWavelength;
}

void
FsoRxDevice::SetDetectorEfficiency (double effcy)
{
  m_rxDetectorEfficiency = effcy;
}

double
FsoRxDevice::GetDetectorEfficiency () const
{
  return m_rxDetectorEfficiency;
}

void
FsoRxDevice::SetDetectorError (double err)
{
  m_rxDetectorError = err;
}

double
FsoRxDevice::GetDetectorError () const
{
  return m_rxDetectorError;
}

void
FsoRxDevice::SetDetectorThreshold (double th)
{
  m_rxDetectorThreshold = th;
}

double
FsoRxDevice::GetDetectorThreshold () const
{
  return m_rxDetectorThreshold;
}

void
FsoRxDevice::SetTelescopeDiameter (double dia)
{
  m_rxTelescopeDiameter = dia;
}

double
FsoRxDevice::GetTelescopeDiameter () const
{
  return m_rxTelescopeDiameter;
}

void
FsoRxDevice::SetTelescopeEfficiency (double effcy)
{
  m_rxTelescopeEfficiency = effcy;
}

double
FsoRxDevice::GetTelescopeEfficiency () const
{
  return m_rxTelescopeEfficiency;
}

void
FsoRxDevice::SetDetectorMeanDarkCount (double mean)
{
  m_darkCountRate->SetAttribute ("Mean", DoubleValue (mean));
}

double
FsoRxDevice::GetDetectorMeanDarkCount () const
{
  return m_darkCountRate->GetMean ();
}

void
FsoRxDevice::SetDetectorVarDarkCount (double var)
{
  m_darkCountRate->SetAttribute ("Variance", DoubleValue (var));
}

double
FsoRxDevice::GetDetectorVarDarkCount () const
{
  return m_darkCountRate->GetVariance ();
}

void
FsoRxDevice::SetDetectorMaxDarkCount (double max)
{
  NS_ASSERT (max > m_darkCountRate->GetMean ());
  m_maxDarkCount = max;
}

double
FsoRxDevice::GetDetectorMaxDarkCount () const
{
  return m_maxDarkCount;
}

void
FsoRxDevice::SetDetectorMinDarkCount (double min)
{
  NS_ASSERT (min < m_darkCountRate->GetMean () && min > 0);
  m_minDarkCount = min;
}

double
FsoRxDevice::GetDetectorMinDarkCount () const
{
  return m_minDarkCount;
}

void
FsoRxDevice::SetChannel (Ptr<FsoChannel> channel)
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
FsoRxDevice::NotifyConnectionStarted (void)
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
FsoRxDevice::NotifyConnectionFinished (void)
{
  NS_LOG_FUNCTION (this);
  m_connectionFinishedTrace (this, Now ());
  m_timeStop = Now ();
  if (!m_connectionFinished.IsNull ())
  {
    m_connectionFinished ();
  }
}

void
FsoRxDevice::NotifyConnectionSucceeded (void)
{
  NS_LOG_FUNCTION (this);
  m_connectionSucceededTrace (this, Now ());
  if (!m_connectionSucceeded.IsNull ())
  {
    m_connectionSucceeded ();
  }
}

void
FsoRxDevice::NotifyConnectionFailed (void)
{
  NS_LOG_FUNCTION (this);
  m_connectionFailedTrace (this, Now ());
  if (!m_connectionFailed.IsNull ())
  {
    m_connectionFailed ();
  }
}


void
FsoRxDevice::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
  m_darkCountRate->Initialize ();
  FsoDevice::DoInitialize ();
}

void
FsoRxDevice::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_darkCountRate = 0;
  FsoDevice::DoDispose ();
}

int64_t
FsoRxDevice::DoAssignStream (int64_t stream)
{
  m_darkCountRate->SetStream (stream + 1);
  return 1;
}

}