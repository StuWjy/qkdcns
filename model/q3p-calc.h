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

#ifndef Q3P_CALC_H
#define Q3P_CALC_H

#include "ns3/ptr.h"

namespace ns3 {

class Q3pCache;

class Q3pCalc
{
public:
  Q3pCalc (){}
  ~Q3pCalc (){}
  static double CalcSecureBits (Ptr<Q3pCache> cache);
  struct Bound
  {
    double LB;
    double val;
    double UB;
  };
};

}

#endif /* Q3P_CALC_H */