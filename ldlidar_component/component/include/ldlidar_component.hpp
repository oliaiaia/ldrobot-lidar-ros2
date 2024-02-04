//  Copyright 2022 Walter Lucetti
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
////////////////////////////////////////////////////////////////////////////////

#ifndef LDLIDAR_COMPONENT_HPP_
#define LDLIDAR_COMPONENT_HPP_

#include <rcutils/logging_macros.h>

#include <string>
#include <memory>

#include <lifecycle_msgs/msg/transition.hpp>
#include <rclcpp/publisher.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_lifecycle/lifecycle_node.hpp>
#include <rclcpp_lifecycle/lifecycle_publisher.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <diagnostic_updater/diagnostic_updater.hpp>
#include <diagnostic_msgs/msg/diagnostic_status.hpp>

#include <nav2_util/lifecycle_node.hpp>

#include "visibility_control.hpp"
#include "ldlidar_driver.h"

namespace lc = rclcpp_lifecycle;

namespace ldlidar
{
class LdLidarComponent : public nav2_util::LifecycleNode
{
public:
  /// \brief Default constructor
  LDLIDAR_COMPONENTS_EXPORT
  explicit LdLidarComponent(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());

  virtual ~LdLidarComponent();

  /// \brief Callback from transition to "configuring" state.
  /// \param[in] state The current state that the node is in.
  nav2_util::CallbackReturn on_configure(const lc::State & prev_state) override;

  /// \brief Callback from transition to "activating" state.
  /// \param[in] state The current state that the node is in.
  nav2_util::CallbackReturn on_activate(const lc::State & prev_state) override;

  /// \brief Callback from transition to "deactivating" state.
  /// \param[in] state The current state that the node is in.
  nav2_util::CallbackReturn on_deactivate(const lc::State & prev_state) override;

  /// \brief Callback from transition to "unconfigured" state.
  /// \param[in] state The current state that the node is in.
  nav2_util::CallbackReturn on_cleanup(const lc::State & prev_state) override;

  /// \brief Callback from transition to "shutdown" state.
  /// \param[in] state The current state that the node is in.
  nav2_util::CallbackReturn on_shutdown(const lc::State & prev_state) override;

  /// \brief Callback from transition to "error" state.
  /// \param[in] state The current state that the node is in.
  nav2_util::CallbackReturn on_error(const lc::State & prev_state) override;

  /// \brief Callback for diagnostic updater
  /// \param[in] stat The current diagnostic status
  void callback_updateDiagnostic(diagnostic_updater::DiagnosticStatusWrapper & stat);

protected:
  // ----> Node Parameters
  template<typename T>
  void getParam(
    std::string paramName, T defValue, T & outVal,
    const std::string & description = "",
    bool read_only = true,
    std::string log_info = std::string());

  void getParam(
    std::string paramName, int defValue, int & outVal,
    const nav2_util::LifecycleNode::integer_range & range,
    const std::string & description = "",
    bool read_only = true,
    std::string log_info = std::string());

  void getParam(
    std::string paramName, float defValue, float & outVal,
    const nav2_util::LifecycleNode::floating_point_range & range,
    const std::string & description = "",
    bool read_only = true,
    std::string log_info = std::string());

  void getParameters();
  void getDebugParams();
  void getCommParams();
  void getLidarParams();
  // <---- Node Parameters

  // ----> Log functions
  void info(const std::string & msg);
  void debug(const std::string & msg);
  void warning(const std::string & msg);
  void error(const std::string & msg);
  // <---- Log functions

  bool initLidar();
  bool initLidarComm();
  void startLidarThread();
  void stopLidarThread();
  void lidarThreadFunc();

  void publishLaserScan(ldlidar::Points2D & src, double lidar_spin_freq);

private:
  // ----> Parameters
  bool _debugMode = true;
  std::string _lidarModel;        // Lidar Model: LDLiDAR_LD06, LDLiDAR_LD19, LDLiDAR_STL27L
  std::string _serialPort;        // Serial port name
  int _baudrate = 230400;     // Serial baudrate
  bool _counterlockwise = true;   // TODO ADD NODE PARAMETER
  int _readTimeOut_msec = 1500;   // TODO ADD NODE PARAMETER
  bool _enableAngleCrop = false;  // TODO ADD NODE PARAMETER
  double _angleCropMin = 0;       // TODO ADD NODE PARAMETER
  double _angleCropMax = 180;     // TODO ADD NODE PARAMETER

  //UNITS _units = UNITS::METERS;
  //ROTATION _rotVerse = ROTATION::CLOCKWISE;
  std::string _frameId = "ldlidar_link";
  // <---- Parameters

  // Publisher
  std::shared_ptr<rclcpp_lifecycle::LifecyclePublisher<sensor_msgs::msg::LaserScan>> _scanPub;

  // ----> Topics
  std::string _topicRoot = "~/";
  std::string _scanTopic = "scan";
  // <---- Topics

  // Diagnostic updater
  diagnostic_updater::Updater _diagUpdater;

  // Lidar communication
  std::unique_ptr<ldlidar::LDLidarDriver> _lidar;
  ldlidar::LDType _lidarType;

  // Lidar Thread
  std::thread _lidarThread;
  bool _threadStop = false;

  // Diagnostic
  double _pubFreq;
  bool _publishing;
};

}  // namespace ldlidar

#endif  // LDLIDAR_COMPONENT_HPP_
