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

#include "constant.h"

namespace ns3 {

const double K_MU = 398600.4418;
const double K_RE = 6378.137;
const double K_sma = 6378.137;
const double K_smb = 6356.752314;
const double K_J2 = 1082.63e-6;
const double K_EAV = 1.00273779174;
const double K_AU = 1.49597870691e8;
const double K_F = 1.0 / 298.257223563;
const double K_LIGHT_SPEED = 299792458.0;
const double k_SECONDS_PER_DAY = 86400.0;

Time simStep = Seconds (1.0);

const double K_MAX_RATE = 0.5;

adi::DateTime SimulationEpoch = adi::DateTime (2022, 1, 1, 0, 0, 0);
adi::DateTime SimulationStart = adi::DateTime (2022, 1, 1, 10, 0, 0);
adi::DateTime SimulationStop  = adi::DateTime (2023, 1, 1, 10, 0, 0);

#define ISL 0
#define S2G 1

#if ISL
  bool EnableISL = true;
#else
  bool EnableISL = false;
#endif

#if S2G
  bool EnableS2G = true;
#else
  bool EnableS2G = false;
#endif

}