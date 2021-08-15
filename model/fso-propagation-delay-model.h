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

#ifndef FSO_PROPAGATION_DELAY_MODEL_H
#define FSO_PROPAGATION_DELAY_MODEL_H

#include "ns3/propagation-delay-model.h"

namespace ns3 {

class Time;
class MobilityModel;


class FsoPropagationDelayModel : public PropagationDelayModel
{
public:
  static TypeId GetTypeId (void);
  FsoPropagationDelayModel ();
  ~FsoPropagationDelayModel ();
  virtual Time GetDelay (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;
  Time GetDelay (double distance) const;
protected:
  virtual void DoInitialize ();
  virtual void DoDispose ();
  virtual int64_t DoAssignStreams (int64_t stream);
};

}

#endif