/**
 * @file LocalEnvironment.cpp
 * @brief Class to manage local environments
 */
#include "LocalEnvironment.h"

#include <Dynamics/Attitude/Attitude.h>
#include <Dynamics/Orbit/Orbit.h>
#include <Interface/InitInput/IniAccess.h>

#include "InitLocalEnvironment.hpp"

LocalEnvironment::LocalEnvironment(SimulationConfig* sim_config, const GlobalEnvironment* glo_env, const int sat_id) {
  Initialize(sim_config, glo_env, sat_id);
}

LocalEnvironment::~LocalEnvironment() {
  delete mag_;
  delete srp_;
  delete atmosphere_;
  delete celes_info_;
}

void LocalEnvironment::Initialize(SimulationConfig* sim_config, const GlobalEnvironment* glo_env, const int sat_id) {
  // Read file name
  IniAccess iniAccess = IniAccess(sim_config->sat_file_[sat_id]);
  std::string ini_fname = iniAccess.ReadString("LOCAL_ENVIRONMENT", "local_env_file");
  // Save ini file
  sim_config->main_logger_->CopyFileToLogDir(ini_fname);
  // Initialize
  mag_ = new MagEnvironment(InitMagEnvironment(ini_fname));
  atmosphere_ = new Atmosphere(InitAtmosphere(ini_fname));
  celes_info_ = new LocalCelestialInformation(&(glo_env->GetCelesInfo()));
  srp_ = new SRPEnvironment(InitSRPEnvironment(ini_fname, celes_info_));
  // Force to disable when the center body is not the Earth
  if (glo_env->GetCelesInfo().GetCenterBodyName() != "EARTH") {
    mag_->IsCalcEnabled = false;
    atmosphere_->IsCalcEnabled = false;
  }

  // Log setting for Local celestial information
  IniAccess conf = IniAccess(ini_fname);
  celes_info_->IsLogEnabled = conf.ReadEnable("LOCAL_CELESTIAL_INFORMATION", "logging");
}

void LocalEnvironment::Update(const Dynamics* dynamics, const SimTime* sim_time) {
  auto& orbit = dynamics->GetOrbit();
  auto& attitude = dynamics->GetAttitude();

  // Update local environments that depend on the attitude (and the position)
  if (sim_time->GetAttitudePropagateFlag()) {
    celes_info_->UpdateAllObjectsInfo(orbit.GetSatPosition_i(), orbit.GetSatVelocity_i(), attitude.GetQuaternion_i2b(), attitude.GetOmega_b());
    mag_->CalcMag(sim_time->GetCurrentDecyear(), sim_time->GetCurrentSidereal(), orbit.GetLatLonAlt(), attitude.GetQuaternion_i2b());
  }

  // Update local environments that depend only on the position
  if (sim_time->GetOrbitPropagateFlag()) {
    srp_->UpdateAllStates();
    atmosphere_->CalcAirDensity(sim_time->GetCurrentDecyear(), sim_time->GetEndSec(), orbit.GetLatLonAlt());
  }
}

void LocalEnvironment::LogSetup(Logger& logger) {
  logger.AddLoggable(mag_);
  logger.AddLoggable(srp_);
  logger.AddLoggable(atmosphere_);
  logger.AddLoggable(celes_info_);
}
