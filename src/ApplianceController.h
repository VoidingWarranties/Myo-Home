#include "LightsController.h"

#include <myo/myo.hpp>

class ApplianceController {
 public:
  ApplianceController(myo::Myo* myo)
      : myo_(myo),
        locker_(LockController::SUGGESTED_UNLOCK_TIME,
                [myo]() {
                  std::cout << "locked!" << std::endl;
                  myo->vibrate(myo::Myo::vibrationMedium);
                },
                [myo]() {
                  std::cout << "unlocked!" << std::endl;
                  myo->vibrate(myo::Myo::vibrationShort);
                  myo->vibrate(myo::Myo::vibrationShort);
                }),
        lights_controller_(&locker_), yaw_(0) {}

  void onPose(myo::Myo* myo, myo::Pose pose, PosePatterns::Pattern pattern) {
    if (!locker_.locked()) {
      switch (current_appliance_) {
        case lights:
          lights_controller_.setYaw(yaw_);
          lights_controller_.onPose(myo, pose, pattern);
          break;
        case media:
          break;
      }
    } else {
      if (pattern == PosePatterns::doubleClick) {
        if (pose == myo::Pose::fingersSpread) {
          lights_controller_.setYawMid(yaw_);
          current_appliance_ = lights;
          locker_.unlock();
        } else if (pose == myo::Pose::fist) {
          current_appliance_ = media;
          locker_.unlock();
        }
      }
    }
  }

  void onOrientationData(myo::Myo* myo, uint64_t timestamp,
                         const myo::Quaternion<float>& quat) {
    yaw_ = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
                 1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));
  }

  void onPeriodic() { locker_.onPeriodic(); }

 private:
  enum Appliance { lights, media };

  Appliance current_appliance_;
  myo::Myo* myo_;
  LockController locker_;
  float yaw_;

  LightsController lights_controller_;
};
