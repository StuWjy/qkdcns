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

#ifndef FSO_TX_DEVICE_H
#define FSO_TX_DEVICE_H

#include "fso-device.h"

namespace ns3 {

class FsoTxDevice : public FsoDevice
{
public:
  static TypeId GetTypeId ();
  FsoTxDevice ();
  virtual ~FsoTxDevice ();

  virtual void SetBeamWavelength (double wavelength);
  virtual double GetBeamWavelength () const;
  virtual void SetChannel (Ptr<FsoChannel> channel);
  
  /**
   * \brief Set the Gaussian beam waist
   * \param waist the Gaussian beam waist, in meter
   */
  void SetBeamWaist (double waist);

  /**
   * \return the Gaussian beam waist, in meter
   */
  double GetBeamWaist () const;

  /**
   * \return the theoretical minimum divergence half-angle (1/e^2), in radian
   */
  double GetBeamMinDivergence () const;

  /**
   * \brief Set the beam divergence half-angle (1/e^2)
   * \param div the beam divergence half-angle (1/e^2), in radian
   */
  void SetBeamDivergence (double div);

  /**
   * \return the beam divergence half-angle (1/e^2), in radian
   */
  double GetBeamDivergence () const;
  
  /**
   * \brief Set the diameter of lens
   * \param dia the diameter of lens, in meter
   */
  void SetLensDiameter (double dia);
  
  /**
   * \return the diameter of lens, in meter
   */
  double GetLensDiameter () const;
  
  /**
   * \return the proportion of Gaussian beam pass through lens
   */
  double GetLensEfficiency () const;

  virtual void NotifyConnectionStarted (void);
  virtual void NotifyConnectionFinished (void);
  virtual void NotifyConnectionSucceeded (void);
  virtual void NotifyConnectionFailed (void);
protected:
  virtual void DoInitialize ();
  virtual void DoDispose ();
private:

  /**
   * \brief Calculate theoretical minimum divergence
   */
  void DoCalcBeamMinDivergence ();

  /**
   * \brief Calculate the efficiency of lens
   * The Gaussian beam will loss power due to the radius of lens
   */
  void DoCalcLensEfficiency ();
  
  double            m_txBeamWavelength;         //!< The Gaussian beam wavelength, in meter
  double            m_txBeamWaist;              //!< The Gaussian beam waist, in meter
  double            m_txBeamMinDivergence;      //!< The theoretical minimum divergence half-angle (1/e^2), in radian
  double            m_txBeamDivergence;         //!< The actual divergence half-angle (1/e^2), in radian
  double            m_txLensDiameter;           //!< The diameter of lens, in meter
  double            m_txLensEfficiency;         //!< The proportion of Gaussian beam pass through lens

};

}

#endif  /* FSO_TX_DEVICE_H */