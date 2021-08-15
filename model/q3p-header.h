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

#ifndef Q3P_HEADER_H
#define Q3P_HEADER_H

#include "ns3/header.h"

namespace ns3 {

class Q3pHeader : public Header
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  Q3pHeader ();
  virtual ~Q3pHeader ();
  virtual TypeId GetInstanceTypeId () const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;

  /**
   * Qkd-Post-processing Protocol type code
   */
  enum Type_e {
    Q3P_TIME_SYNC_TAGGING = 1,
    Q3P_PULSE_LOCATING,
    Q3P_BASIS_SIFTING,
    Q3P_KEY_SIFTING,
    Q3P_ERROR_CORRECTION,
    Q3P_PRIVACY_AMPLIFICATION
  };

  /**
   * \brief Set Qkd-Post-Processing Protocol type
   * \param[in] type the Qkd-Post-Processing Protocol type
   */
  void SetType (uint8_t type);

  /**
   * \brief Get Qkd-Post-Processing Protocol type
   * \returns the Qkd-Post-Processing Protocol type
   */
  uint8_t GetType (void) const;

  /**
   * \brief Get the string of Qkd-Post-Processing Protocol type
   * 
   */
  std::string GetTypeString (void) const;
private:
  uint8_t m_type;         //!< Qkd-Post-Processing Protocol type
};

}

#endif /* Q3P_HEADER_H */