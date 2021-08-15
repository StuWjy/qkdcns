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
#include "ns3/nstime.h"
#include "ns3/mobility-model.h"
#include "fso-propagation-delay-model.h"
#include "constant.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("FsoPropagationDelayModel");

NS_OBJECT_ENSURE_REGISTERED (FsoPropagationDelayModel);

TypeId
FsoPropagationDelayModel::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::FsoPropagationDelayModel")
  .SetParent<PropagationDelayModel> ()
  .SetGroupName ("Fso")
  .AddConstructor<FsoPropagationDelayModel> ()
  ;
  return tid;
}

FsoPropagationDelayModel::FsoPropagationDelayModel ()
{
  NS_LOG_FUNCTION (this);
}

FsoPropagationDelayModel::~FsoPropagationDelayModel ()
{
  NS_LOG_FUNCTION (this);
}

Time
FsoPropagationDelayModel::GetDelay (Ptr<MobilityModel> a,
                                    Ptr<MobilityModel> b) const
{
  NS_LOG_FUNCTION (this << a << b);
  // Defined but will not be called in our simulation
  NS_ASSERT (false);
  NS_ASSERT (a != 0 && b != 0);
  double distance = (a->GetPosition () - b->GetPosition ()).GetLength ();
  Time delay = Seconds (distance / K_LIGHT_SPEED);
  return delay;
}

Time
FsoPropagationDelayModel::GetDelay (double distance) const
{
  NS_LOG_FUNCTION (this << distance);
  Time delay = Seconds (distance / K_LIGHT_SPEED);
  return delay;
}

void
FsoPropagationDelayModel::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
  PropagationDelayModel::DoInitialize ();
}

void
FsoPropagationDelayModel::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  PropagationDelayModel::DoDispose ();
}

int64_t
FsoPropagationDelayModel::DoAssignStreams (int64_t stream)
{
  return 0;
}

}