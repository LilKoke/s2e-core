/**
 * @file AirDrag.h
 * @brief Class to calculate the air drag disturbance force and torque
 */

#ifndef __AirDrag_H__
#define __AirDrag_H__

#include <string>
#include <vector>

#include "../Environment/Local/Atmosphere.h"
#include "../Interface/LogOutput/ILoggable.h"
#include "../Library/math/Quaternion.hpp"
#include "../Library/math/Vector.hpp"
#include "SurfaceForce.h"
using libra::Quaternion;
using libra::Vector;

#pragma once
/**
 * @class AirDrag
 * @brief Class to calculate the air drag disturbance force and torque
 */
class AirDrag : public SurfaceForce {
 public:
  /**
   * @fn AirDrag
   * @brief Constructor
   */
  AirDrag(const vector<Surface>& surfaces, const Vector<3>& cg_b, const double t_w, const double t_m, const double molecular);

  /**
   * @fn Update
   * @brief Override Updates function of SimpleDisturbance
   */
  virtual void Update(const LocalEnvironment& local_env, const Dynamics& dynamics);

  // Override ILoggable
  /**
   * @fn GetLogHeader
   * @brief Override GetLogHeader function of ILoggable
   */
  virtual std::string GetLogHeader() const;
  /**
   * @fn GetLogValue
   * @brief Override GetLogValue function of ILoggable
   */
  virtual std::string GetLogValue() const;

  // for debug TODO: remove?
  void PrintParams(void);
  std::vector<double> cnct;

 private:
  vector<double> Cn_;  //!< Coefficients for out-plane force
  vector<double> Ct_;  //!< Coefficients for in-plane force
  double rho_;         //!< Air density [kg/m^3]
  double Tw_;          //!< Temperature of surface [K]
  double Tm_;          //!< Temperature of atmosphere [K]
  double M_;           //!< Molecular weight [g/mol]

  /**
   * @fn CalcCoef
   * @brief Override CalcCoef function of SurfaceForce
   * @param [in] vel_b: Spacecraft's velocity vector in the body frame [m/s]
   * @param [in] air_dens: Air density around the spacecraft [kg/m^3]
   */
  void CalcCoef(Vector<3>& vel_b, double air_dens);

  // internal function for calculation
  /**
   * @fn CalCnCt
   * @brief Calculate the Cn and Ct
   * @param [in] vel_b: Spacecraft's velocity vector in the body frame [m/s]
   */
  void CalCnCt(Vector<3>& vel_b);
  /**
   * @fn funcPi
   * @brief Calculate The Pi function in the algorithm
   */
  double funcPi(double s);
  /**
   * @fn funcChi
   * @brief Calculate The Chi function in the algorithm
   */
  double funcChi(double s);
};
#endif
