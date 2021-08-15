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

#ifndef CONSTANT_H
#define CONSTANT_H

#include "ns3/nstime.h"
#include "adi-date-time.h"
#include "adi-time-span.h"

namespace ns3 {

extern const double K_MU;               //!< Gravitational constant
extern const double K_RE;               //!< Earth radius
extern const double K_sma;              //!< Earth radius
extern const double K_smb;              //!< Earth radius
extern const double K_J2;               //!< J2 constant
extern const double K_EAV;              //!< Earth rotation angle per sidereal day
extern const double K_AU;               //!< Astronomical Unit
extern const double K_F;                //!< Earth flattening
extern const double K_LIGHT_SPEED;      //!< Speed of light in vacuum
extern const double k_SECONDS_PER_DAY;  //!< 
extern const double K_MAX_RATE;

extern Time simStep;

extern adi::DateTime SimulationEpoch;
extern adi::DateTime SimulationStart;
extern adi::DateTime SimulationStop;

extern bool EnableISL;
extern bool EnableS2G;

}

#endif  /* CONSTANT_H */