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

#include "fso-channel-list.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("FsoChannelList");

FsoChannelList::FsoChannels FsoChannelList::m_fsoChannels   = FsoChannels ();
uint32_t                    FsoChannelList::m_channelCount  = 0;

uint32_t
FsoChannelList::Add (Ptr<FsoChannel> channel)
{
  Ptr<FsoTxDevice> tx = channel->GetTxDevice ();
  Ptr<FsoRxDevice> rx = channel->GetRxDevice ();
  NS_ASSERT (tx);
  NS_ASSERT (rx);
  if (m_fsoChannels[tx][rx] == NULL)
  {
    m_channelCount++;
    m_fsoChannels[tx][rx] = channel;
  }
  else
  {
    NS_ASSERT_MSG (false, "This fso-channel " << channel << " has been added once");
  }
  return m_channelCount;
}

Ptr<FsoChannel>
FsoChannelList::Get (Ptr<FsoTxDevice> tx, Ptr<FsoRxDevice> rx)
{
  NS_ASSERT (tx);
  NS_ASSERT (rx);
  if (m_fsoChannels.find (tx) != m_fsoChannels.end ())
  {
    if (m_fsoChannels[tx].find (rx) != m_fsoChannels[tx].end ())
    {
      return m_fsoChannels[tx][rx];
    }
  }
  NS_LOG_LOGIC ("Cannot find associated fso-channel");
  return NULL;
}

}