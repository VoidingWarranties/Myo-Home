#ifndef MYO_HOME_MEDIACONTROLLER_H_
#define MYO_HOME_MEDIACONTROLLER_H_

#include "../../../Myo-Media/src/ApplicationControlManager.h"
#include "../../../Myo-Intelligesture/src/PoseGestures.h"
#include "../../../Myo-Intelligesture/src/OrientationUtility.h"

class MediaController {
 public:
  MediaController(LockController* locker_p)
      : locker_p_(locker_p),
        media_manager_(ApplicationControlManager::supported_apps_t::ITUNES),
        rotation_(),
        starting_rotation_(),
        controlling_volume_(false),
        previous_controlling_position_(false),
        controlling_position_(false),
        last_pose_(),
        arm_(myo::armRight),
        x_direction_(myo::xDirectionUnknown) {}

  void togglePlay() {
    media_manager_.togglePlay();
  }

  void onPose(myo::Myo* myo, PoseGestures<>::Pose pose) {
    if (pose.gesture() == PoseGestures<>::Pose::Gesture::singleClick ||
        pose.gesture() == PoseGestures<>::Pose::Gesture::doubleClick) {
      if (pose.pose() == PoseGestures<>::Pose::fingersSpread) {
        media_manager_.togglePlay();
        locker_p_->extendUnlock();
      } else if (pose.pose() == PoseGestures<>::Pose::waveIn) {
        media_manager_.previousTrack();
        locker_p_->extendUnlock();
      } else if (pose.pose() == PoseGestures<>::Pose::waveOut) {
        media_manager_.nextTrack();
        locker_p_->extendUnlock();
      }
    } else if (pose.gesture() == PoseGestures<>::Pose::Gesture::hold &&
               pose.pose() == PoseGestures<>::Pose::fist) {
      controlling_volume_ = true;
      starting_rotation_ = rotation_;
      std::cout << "Adjusting volume..." << std::endl;
      locker_p_->extendUnlock();
      myo->vibrate(myo::Myo::vibrationShort);
    } else if (last_pose_.gesture() == PoseGestures<>::Pose::Gesture::hold &&
               last_pose_.pose() == PoseGestures<>::Pose::fist) {
      controlling_volume_ = false;
      myo->vibrate(myo::Myo::vibrationShort);
    } else if (pose.gesture() == PoseGestures<>::Pose::Gesture::hold &&
               (pose.pose() == PoseGestures<>::Pose::waveIn ||
                pose.pose() == PoseGestures<>::Pose::waveOut)) {
      controlling_position_ = true;
      std::cout << "Seeking iTunes" << std::endl;
      locker_p_->extendUnlock();
    } else {
      controlling_position_ = false;
    }
    last_pose_ = pose;
  }

  void onPeriodic() {
    if (controlling_volume_) {
      locker_p_->extendUnlock();
      float roll_diff = OrientationUtility::RelativeOrientation(
          starting_rotation_, rotation_, OrientationUtility::QuaternionToRoll);
      if (x_direction_ != myo::xDirectionTowardWrist) {
        roll_diff *= -1;
      }
      int volume_diff = roll_diff * 50; // Change this to your preference.
      media_manager_.incrementVolumeBy(volume_diff);
      starting_rotation_ = rotation_;
    } else if (controlling_position_) {
      locker_p_->extendUnlock();
      if (last_pose_ == myo::Pose::waveIn) {
        media_manager_.stepBackward();
      } else {
        media_manager_.stepForward();
      }
      previous_controlling_position_ = true;
    } else if (previous_controlling_position_) {
      media_manager_.resume();
      previous_controlling_position_ = false;
    }
  }

  void onOrientationData(myo::Myo* myo, const myo::Quaternion<float>& quat) {
    rotation_ = quat;
  }

  void onArmRecognized(myo::Myo* myo, myo::Arm arm, myo::XDirection x_direction) {
    arm_ = arm;
    x_direction_ = x_direction;
  }

 private:
  LockController* locker_p_;
  ApplicationControlManager media_manager_;
  myo::Quaternion<float> rotation_, starting_rotation_;
  bool controlling_volume_, controlling_position_, previous_controlling_position_;
  PoseGestures<>::Pose last_pose_;
  myo::Arm arm_;
  myo::XDirection x_direction_;
};

#endif
