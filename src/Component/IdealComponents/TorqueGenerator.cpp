/*
 * @file TorqueGenerator.cpp
 * @brief Ideal component which can generate torque for control algorithm test
 */

#include "TorqueGenerator.hpp"

#include <cfloat>

// Constructor
TorqueGenerator::TorqueGenerator(const int prescaler, ClockGenerator* clock_gen, const double magnitude_error_standard_deviation_Nm,
                                 const double direction_error_standard_deviation_rad, const Dynamics* dynamics)
    : ComponentBase(prescaler, clock_gen),
      magnitude_noise_(0.0, magnitude_error_standard_deviation_Nm),
      direction_error_standard_deviation_rad_(direction_error_standard_deviation_rad),
      dynamics_(dynamics) {
  direction_noise_.set_param(0.0, 1.0);
}

TorqueGenerator::~TorqueGenerator() {}

void TorqueGenerator::MainRoutine(int count) {
  UNUSED(count);

  generated_torque_b_Nm_ = ordered_torque_b_Nm_;

  // Add noise
  double norm_ordered_torque = norm(ordered_torque_b_Nm_);
  if (norm_ordered_torque > 0.0 + DBL_EPSILON) {
    // Add noise only when the torque is generated
    libra::Vector<3> true_direction = normalize(generated_torque_b_Nm_);
    libra::Quaternion error_quaternion = GenerateDirectionNoiseQuaternion(true_direction, direction_error_standard_deviation_rad_);
    libra::Vector<3> converted_direction = error_quaternion.frame_conv(generated_torque_b_Nm_);
    double torque_norm_with_error = norm_ordered_torque + magnitude_noise_;
    generated_torque_b_Nm_ = torque_norm_with_error * converted_direction;
  }
}

void TorqueGenerator::PowerOffRoutine() { generated_torque_b_Nm_ *= 0.0; }

std::string TorqueGenerator::GetLogHeader() const {
  std::string str_tmp = "";

  std::string head = "IdealTorqueGenerator_";
  str_tmp += WriteVector(head + "ordered_torque", "b", "Nm", 3);
  str_tmp += WriteVector(head + "generated_torque", "b", "Nm", 3);

  return str_tmp;
}

std::string TorqueGenerator::GetLogValue() const {
  std::string str_tmp = "";

  str_tmp += WriteVector(ordered_torque_b_Nm_);
  str_tmp += WriteVector(generated_torque_b_Nm_);

  return str_tmp;
}

libra::Quaternion TorqueGenerator::GenerateDirectionNoiseQuaternion(libra::Vector<3> true_direction, const double error_standard_deviation_rad) {
  libra::Vector<3> random_direction;
  random_direction[0] = direction_noise_;
  random_direction[1] = direction_noise_;
  random_direction[2] = direction_noise_;
  random_direction = normalize(random_direction);

  libra::Vector<3> rotation_axis;
  rotation_axis = outer_product(true_direction, random_direction);
  double norm_rotation_axis = norm(rotation_axis);
  if (norm_rotation_axis < 0.0 + DBL_EPSILON) {
    // No rotation error if the randomized direction is parallel to the true direction
    rotation_axis = true_direction;
  }

  double error_angle_rad = direction_noise_ * error_standard_deviation_rad;
  libra::Quaternion error_quaternion(rotation_axis, error_angle_rad);
  return error_quaternion;
}
