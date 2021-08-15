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

#ifndef FSO_CHANNEL_LIST_H
#define FSO_CHANNEL_LIST_H

#include <map>
#include <vector>
#include "fso-channel.h"

namespace ns3 {

class FsoChannelList
{
public:
  FsoChannelList (){}
  ~FsoChannelList (){}
  static uint32_t Add (Ptr<FsoChannel> channel);
  static Ptr<FsoChannel> Get (Ptr<FsoTxDevice> tx, Ptr<FsoRxDevice> rx);
private:
  typedef std::map<Ptr<FsoTxDevice>, std::map<Ptr<FsoRxDevice>, Ptr<FsoChannel>>> FsoChannels;
  static FsoChannels m_fsoChannels;
  static uint32_t m_channelCount;
};

}

#endif /* FSO_CHANNEL_LIST_H */