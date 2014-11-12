#include "../../../Myo-Media/src/ApplicationControlManager.h"

class MediaController {
 public:
  MediaController(LockController* locker_p)
      : locker_p_(locker_p),
        media_manager_(ApplicationControlManager::supported_apps_t::VLC),
        roll_(0),
        roll_mid_(0),
        controlling_volume_(false),
        controlling_position_(false),
        last_pose_(myo::Pose::rest),
        last_pattern_(PosePatterns::Pattern::nothing),
        arm_(myo::armRight),
        x_direction_(myo::xDirectionUnknown) {}

  void togglePlay() {
    media_manager_.togglePlay();
  }

  void onPose(myo::Myo* myo, myo::Pose pose, PosePatterns::Pattern pattern) {
    if (arm_ == myo::armLeft) {
      if (pose == myo::Pose::waveIn) {
        pose = myo::Pose::waveOut;
      } else if (pose == myo::Pose::waveOut) {
        pose = myo::Pose::waveIn;
      }
    }
    if (pattern == PosePatterns::singleClick) {
      switch (pose.type()) {
        case myo::Pose::fingersSpread:
          media_manager_.togglePlay();
          locker_p_->extendUnlock();
          break;
        case myo::Pose::waveIn:
          media_manager_.previousTrack();
          locker_p_->extendUnlock();
          break;
        case myo::Pose::waveOut:
          media_manager_.nextTrack();
          locker_p_->extendUnlock();
          break;
      }
    }
    if (pattern == PosePatterns::hold && pose == myo::Pose::fist) {
      controlling_volume_ = true;
      roll_mid_ = roll_;
      std::cout << "Adjusting volume..." << std::endl;
      locker_p_->extendUnlock();
      myo->vibrate(myo::Myo::vibrationShort);
    } else if (last_pattern_ == PosePatterns::hold &&
               last_pose_ == myo::Pose::fist) {
      controlling_volume_ = false;
      myo->vibrate(myo::Myo::vibrationShort);
    }
    if (pattern == PosePatterns::hold && (pose == myo::Pose::waveIn || pose == myo::Pose::waveOut)) {
      controlling_position_ = true;
      std::cout << "Seeking VLC" << std::endl;
      locker_p_->extendUnlock();
    } else {
      controlling_position_ = false;
    }
    last_pose_ = pose;
    last_pattern_ = pattern;
  }

  void onPeriodic() {
    if (controlling_volume_) {
      locker_p_->extendUnlock();
      float roll_diff = roll_ - roll_mid_;
      if (x_direction_ != myo::xDirectionTowardWrist) {
        roll_diff *= -1;
      }
      if (roll_diff > M_PI) {
        roll_diff -= (2 * M_PI);
      }
      if (roll_diff < -M_PI) {
        roll_diff += (2 * M_PI);
      }
      int volume_diff = roll_diff * 300; // Change this to your preference.
      media_manager_.incrementVolumeBy(volume_diff);
    }
    if (controlling_position_) {
      locker_p_->extendUnlock();
      if (last_pose_ == myo::Pose::waveIn) {
        media_manager_.stepBackward();
      } else {
        media_manager_.stepForward();
      }
    }
  }

  void setRoll(float roll) {
    roll_mid_ = roll_;
    roll_ = roll;
  }
  void onArmRecognized(myo::Myo* myo, myo::Arm arm, myo::XDirection x_direction) {
    arm_ = arm;
    x_direction_ = x_direction;
  }

 private:
  LockController* locker_p_;
  ApplicationControlManager media_manager_;
  float roll_, roll_mid_;
  bool controlling_volume_;
  bool controlling_position_;
  myo::Pose last_pose_;
  PosePatterns::Pattern last_pattern_;
  myo::Arm arm_;
  myo::XDirection x_direction_;
};
