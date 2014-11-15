/* Provides a user interface for controlling lights. Currently this class
 * communicates with the hardware by sending serial commands to the device over
 * USB. The serial commands are sent by directly writing to the hardware device
 * file in /dev. Eventually this library will use Bluetooth or another wireless
 * technology.
 *
 * Currently the lights controller directly communicates with the lights.
 * Instead it should simply store Light objects and simply turn on/off the
 * appropriate light object.
 */

#ifndef MYO_HOME_LIGHTSCONTROLLER_H_
#define MYO_HOME_LIGHTSCONTROLLER_H_

#include <unistd.h>
#include <myo/myo.hpp>
#include "../../../Myo-Intelligesture/src/PoseGestures.h"
#include "../../../Myo-Intelligesture/src/OrientationUtility.h"

class LightsController {
 public:
  LightsController(LockController* locker_p)
      : locker_p_(locker_p),
        light_states_{false},
        rotation_(),
        starting_rotation_() {}

  void turnOn(size_t light) {
    // assert light < num_lights_
    if (!light_states_[light]) {
      std::cout << "turning light " << light << " on" << std::endl;
      std::string cmd = "echo -n " + std::to_string(light + 1) +
                        " >> /dev/cu.usbserial-AH01L4DJ";
      std::system(cmd.c_str());
      light_states_[light] = true;
    }
  }
  void turnOff(size_t light) {
    // assert light < num_lights_
    if (light_states_[light]) {
      std::cout << "turning light " << light << " off" << std::endl;
      std::string cmd = "echo -n " + std::to_string(light + 1) +
                        " >> /dev/cu.usbserial-AH01L4DJ";
      std::system(cmd.c_str());
      light_states_[light] = false;
    }
  }

  void partyMode() {
    int i = 0;
    while (i < 3) {
      turnOn(0);
      turnOff(1);
      usleep(250000);
      turnOn(1);
      turnOff(0);
      usleep(250000);
      ++i;
    }
  }

  void onPose(myo::Myo* myo, PoseGestures<>::Pose pose) {
    // Use the relative yaw angle to determine which light to control.
    float yaw_diff = OrientationUtility::RelativeOrientation(
        starting_rotation_, rotation_, OrientationUtility::QuaternionToYaw);
    size_t light = (yaw_diff < 0 ? 0 : 1);
    if (pose.pose() == PoseGestures<>::Pose::fingersSpread) {
      turnOn(light);
      locker_p_->extendUnlock();
    } else if (pose.pose() == PoseGestures<>::Pose::fist) {
      turnOff(light);
      locker_p_->extendUnlock();
    } else if (pose.pose() == PoseGestures<>::Pose::waveUp) {
      for (size_t i = 0; i < num_lights_; ++i) {
        turnOn(i);
      }
      locker_p_->extendUnlock();
    } else if (pose.pose() == PoseGestures<>::Pose::waveDown) {
      for (size_t i = 0; i < num_lights_; ++i) {
        turnOff(i);
      }
      locker_p_->extendUnlock();
    }
  }

  void onOrientationData(myo::Myo* myo, const myo::Quaternion<float>& quat) {
    rotation_ = quat;
  }

  // Set the reference quaternion to use when calculating relative orientation
  // angles. This should be set when the user is pointing in between the two
  // lights.
  void markStartingRotation() { starting_rotation_ = rotation_; }

 private:
  LockController* locker_p_;
  bool light_states_[2];
  size_t num_lights_ = 2;
  myo::Quaternion<float> rotation_, starting_rotation_;
};

#endif
