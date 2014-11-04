#include <cmath>

#include "LightsController.h"
#include "MediaController.h"

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
        lights_controller_(&locker_),
        roll_(0),
        yaw_(0),
        pitch_(0),
        media_controller_(&locker_) {}

  void onPose(myo::Myo* myo, myo::Pose pose, PosePatterns::Pattern pattern) {
    if (!locker_.locked()) {
      if (pitch_ <= -1) {
        if (pose == myo::Pose::waveOut && pattern == PosePatterns::nothing) {
          std::cout << "PARTY MODE" << std::endl;
          media_controller_.togglePlay();
          lights_controller_.partyMode();
        }
      }
      switch (current_appliance_) {
        case lights:
          lights_controller_.setRoll(roll_);
          lights_controller_.setYaw(yaw_);
          lights_controller_.setPitch(pitch_);
          lights_controller_.onPose(myo, pose, pattern);
          break;
        case media:
          media_controller_.onPose(myo, pose, pattern);
          break;
      }
    } else {
      if (pattern == PosePatterns::doubleClick) {
        if (pose == myo::Pose::fingersSpread) {
          lights_controller_.setRollMid(roll_);
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
    roll_ = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
                  1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
    yaw_ = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
                 1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));
    pitch_ = asin(std::max(
        -1.0f, std::min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
    if (current_appliance_ == media) media_controller_.setRoll(roll_);
  }

  void onPeriodic() {
    locker_.onPeriodic();
    if (current_appliance_ == media) media_controller_.onPeriodic();
  }

  void onArmRecognized(myo::Myo* myo, myo::Arm arm, myo::XDirection x_direction) {
    media_controller_.onArmRecognized(myo, arm, x_direction);
  }

 private:
  enum Appliance { lights, media };

  Appliance current_appliance_;
  myo::Myo* myo_;
  LockController locker_;
  float roll_, yaw_, pitch_;

  LightsController lights_controller_;
  MediaController media_controller_;
};
