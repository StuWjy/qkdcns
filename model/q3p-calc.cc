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

#include "ns3/callback.h"
#include "q3p-calc.h"
#include "q3p-cache.h"
#include "q3p-l3-protocol.h"
#include "qkd-util.h"

namespace ns3 {

const double probFail = 1e-10;
const double beta     = -log (probFail / 2.0);
double nx = 0;
double nz = 0;
double qx = 0;
double e1U = 0;

void GetBound (Q3pCalc::Bound &bound)
{
  double x = bound.val;
  NS_ASSERT (x >= 0);
  if (x >= 6 * beta)
  {
    double y = (3 * beta + sqrt (8 * beta * x + beta * beta)) / (2 * (x - beta));
    bound.LB = x / (1 + y);
    bound.UB = x / (1 - y);
    return;
  }
  else if (x == 0)
  {
    bound.LB = 0;
    bound.UB = beta;
    return;
  }
  else// if (x > 0 && x < 6 * beta)
  {
    double p = x / beta;
    if (x < 0.3 * beta)
    {
      bound.LB = 0;
    }
    else
    {
      bound.LB = std::max(-0.015895 * p * p + 0.18151 * p - 0.01158, 0.0) * x;
    }
    double temp = -0.018461 * p * p + 0.3216 * p + 1.1536;
    temp = std::max (temp, 1.0);
    bound.UB = 1 / (1 - 1 / temp) * x;
    return;
  }
  NS_ASSERT (false);
}

double
DoCalcXi (double x)
{
  NS_ASSERT (x >= 0 && x <= 1);
  double c = log (probFail * sqrt (nx * nz * e1U * (1 - e1U)) / sqrt (nx + nz)) / (nx + nz);
  double y = entropy (e1U + x - qx * x) - qx * entropy (e1U) - (1.0 - qx) * entropy (e1U + x) + c;
  return y;
}

double
Q3pCalc::CalcSecureBits (Ptr<Q3pCache> cache)
{
  Bound sigEventBound;
  Bound decEventBound;
  Bound vacEventBound;
  Bound sigErrorEventBound;
  Bound decErrorEventBound;
  Bound vacErrorEventBound;

  sigEventBound.val = cache->GetSignalEvents ();
  decEventBound.val = cache->GetDecoyEvents ();
  vacEventBound.val = cache->GetVacuumEvents ();
  sigErrorEventBound.val = cache->GetSignalErrorEvents ();
  decErrorEventBound.val = cache->GetDecoyErrorEvents ();
  vacErrorEventBound.val = cache->GetVacuumErrorEvents ();
  if (sigEventBound.val == 0
  ||  decEventBound.val == 0
  ||  vacEventBound.val == 0
  ||  sigErrorEventBound.val == 0
  ||  decErrorEventBound.val == 0
  ||  vacErrorEventBound.val == 0)
  {
    return 0;
  }
  GetBound (sigEventBound);
  GetBound (decEventBound);
  GetBound (vacEventBound);
  GetBound (sigErrorEventBound);
  GetBound (decErrorEventBound);
  GetBound (vacErrorEventBound);

  uint64_t totalEvents = cache->GetTotalEvents ();
  if (totalEvents == 0)
  {
    return 0;
  }
  Ptr<Q3pL3Protocol> q3p = cache->GetProtocol ();
  double q = q3p->GetBasisRatio ();
  double ps = q3p->GetSignalProbability ();
  double pw = q3p->GetDecoyProbability ();
  double pv = q3p->GetVacuumProbability ();
  Bound Qs;
  Bound Qw;
  Bound Qv;
  // Bound EsQs;
  Bound EwQw;
  Bound EvQv;
  Qs.LB = sigEventBound.LB / (totalEvents * ps);
  Qs.UB = sigEventBound.UB / (totalEvents * ps);
  Qw.LB = decEventBound.LB / (totalEvents * pw);
  Qw.UB = decEventBound.UB / (totalEvents * pw);
  Qv.LB = vacEventBound.LB / (totalEvents * pv);
  Qv.UB = vacEventBound.UB / (totalEvents * pv);

  // EsQs.LB = sigErrorEventBound.LB / totalEvents;
  // EsQs.UB = sigErrorEventBound.UB / totalEvents;
  EwQw.LB = decErrorEventBound.LB / (totalEvents * pw);
  EwQw.UB = decErrorEventBound.UB / (totalEvents * pw);
  EvQv.LB = vacErrorEventBound.LB / (totalEvents * pv);
  EvQv.UB = vacErrorEventBound.UB / (totalEvents * pv);

  double u = q3p->GetSignalPhotons ();
  double v = q3p->GetDecoyPhotons ();
  double w = v * v / (u * u);
  double Y1L = u / (u * v - v * v)
            * ( Qw.LB * exp (v)
              - w * Qs.UB * exp (u)
              - (1 - w) * Qv.UB
              );
  double e1Y1U = (EwQw.UB * exp (v) - EvQv.LB) / v;
  if (e1Y1U < 0)
  {
    return 0;
  }
  e1U = e1Y1U / Y1L;
  if (e1U > 0.11)
  {
    return 0;
  }
  double M1L = Y1L * totalEvents * (exp (-u) * u * ps + exp (-v) * v * pw);
  double tmp = (beta + sqrt (beta * beta + 8 * beta * M1L * ps))
             / (2 * M1L * ps);
  double M1sL = (1 - tmp) * M1L * ps;
  nx = q * M1L;
  nz = q * M1sL;
  qx = nx / (nx + nz);

  double lx = 0;
  double rx = 1 - e1U;
  double mx = BisectionMethod (MakeCallback (&DoCalcXi), lx, rx);
  double e1psU = e1U + mx;
  double Es = sigErrorEventBound.val / sigEventBound.val;
  double secureBits = M1sL * (1.0 - entropy (e1psU)) - 1.4742 * sigEventBound.val * entropy (Es);
  if (secureBits < 0)
  {
    secureBits = 0;
  }
  return secureBits;
}

}