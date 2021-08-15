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

#ifndef FSO_RX_DEVICE_H
#define FSO_RX_DEVICE_H

#include "fso-device.h"

namespace ns3 {

class NormalRandomVariable;

class FsoRxDevice : public FsoDevice
{
public:
  static TypeId GetTypeId ();

  /**
   * \brief Default constructor
   */
  FsoRxDevice ();

  /**
   * \brief Deconstructor
   */
  ~FsoRxDevice ();

  virtual void SetBeamWavelength (double wavelength);
  virtual double GetBeamWavelength () const;
  virtual void SetChannel (Ptr<FsoChannel> channel);
  
  /**
   * \brief Set the detector efficiency
   * \param effcy the detector efficiency
   */
  void SetDetectorEfficiency (double effcy);

  /**
   * \return the detector efficiency
   */
  double GetDetectorEfficiency () const;

  /**
   * \brief Set the detector error probability
   * \param div the detector error probability
   */
  void SetDetectorError (double err);

  /**
   * \return the detector error probability
   */
  double GetDetectorError () const;

  /**
   * \brief Set the detector threshold
   * \param th the threshold, in second
   */
  void SetDetectorThreshold (double th);

  /**
   * \brief Get the detector threshold
   * \return the threshold, in second
   */
  double GetDetectorThreshold () const;
  
  /**
   * \brief Set the diameter of telescope
   * \param dia the diameter of telescope, in meter
   */
  void SetTelescopeDiameter (double dia);
  
  /**
   * \return the diameter of lens, in meter
   */
  double GetTelescopeDiameter () const;
  
  /**
   * \brief Set the telescope efficiency
   * \param dia the telescope efficiency
   */
  void SetTelescopeEfficiency (double effcy);
  
  /**
   * \return the telescope efficiency
   */
  double GetTelescopeEfficiency () const;

  /**
   * \brief Set the mean dark count of detector
   * \param mean the mean dark count of detector, in Hz
   */
  void SetDetectorMeanDarkCount (double mean);

  /**
   * \brief Get the mean dark count of detector
   * \return the mean dark count of detector, in Hz
   */
  double GetDetectorMeanDarkCount () const;


  /**
   * \brief Set the variance dark count of detector
   * \param var the variance dark count of detector, in square of Hz
   */
  void SetDetectorVarDarkCount (double var);

  /**
   * \brief Get the variance dark count of detector
   * \return the variance dark count of detector, in square of Hz
   */
  double GetDetectorVarDarkCount () const;

  /**
   * \brief Set the maximum dark count of detector
   * \param offset the maximum dark count of detector, in Hz
   */
  void SetDetectorMaxDarkCount (double max);
  
  /**
   * \brief Get the maximum dark count of detector
   * \return the maximum dark count of detector, in Hz
   */
  double GetDetectorMaxDarkCount () const;

  /**
   * \brief Set the minimum dark count of detector
   * \param offset the minimum dark count of detector, in Hz
   */
  void SetDetectorMinDarkCount (double max);
  
  /**
   * \brief Get the minimum dark count of detector
   * \return the minimum dark count of detector, in Hz
   */
  double GetDetectorMinDarkCount () const;
  
  /**
   * \return the dark count rate
   */
  double GetDarkCountRate () const;
  virtual void NotifyConnectionStarted (void);
  virtual void NotifyConnectionFinished (void);
  virtual void NotifyConnectionSucceeded (void);
  virtual void NotifyConnectionFailed (void);
protected:
  virtual void DoInitialize ();
  virtual void DoDispose ();
  int64_t DoAssignStream (int64_t stream);
private:
  // parameters
  double            m_rxBeamWavelength;         //!< The beam wavelength received, in meter
  double            m_rxBeamMaxRange;           //!< The beam maximum range, in kilometer
  double            m_rxDetectorEfficiency;     //!< The efficiency of detector
  double            m_rxDetectorError;          //!< The error count ratio of detector
  double            m_rxDetectorThreshold;      //!< Threshold of detector, in sec
  double            m_rxTelescopeDiameter;      //!< The diameter of telescope, in meter
  double            m_rxTelescopeEfficiency;    //!< The efficiency of telescope
  Ptr<NormalRandomVariable> m_darkCountRate;    //!< dark count rate, in count per second
  double            m_minDarkCount;             //!< minimum dark count, in count per second
  double            m_maxDarkCount;             //!< maximum dark count, in count per second
};

}

#endif /* FSO_RX_DEVICE_H */