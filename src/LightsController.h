class LightsController {
 public:
  LightsController(LockController* locker_p)
      : locker_p_(locker_p), roll_(0), roll_mid_(0), yaw_(0), yaw_mid_(0), light_states_{false} {}

  void turnOn(size_t light) {
    // assert light < num_lights_
    if (!light_states_[light]) {
      std::cout << "turning light " << light << " on" << std::endl;
      std::string cmd =
          "echo -n " + std::to_string(light + 1) + " >> /dev/cu.usbmodem1421";
      std::system(cmd.c_str());
      light_states_[light] = true;
    }
  }
  void turnOff(size_t light) {
    // assert light < num_lights_
    if (light_states_[light]) {
      std::cout << "turning light " << light << " off" << std::endl;
      std::string cmd =
          "echo -n " + std::to_string(light + 1) + " >> /dev/cu.usbmodem1421";
      std::system(cmd.c_str());
      light_states_[light] = false;
    }
  }

  void onPose(myo::Myo* myo, myo::Pose pose, PosePatterns::Pattern pattern) {
    float yaw_diff = yaw_mid_ - yaw_;
    if (yaw_diff > M_PI) {
      yaw_diff -= (2 * M_PI);
    }
    if (yaw_diff < -M_PI) {
      yaw_diff += (2 * M_PI);
    }

    float roll_diff = roll_ - roll_mid_;
    if (roll_diff > M_PI) {
      roll_diff -= (2 * M_PI);
    }
    if (roll_diff < -M_PI) {
      roll_diff += (2 * M_PI);
    }

    size_t light = (yaw_diff < 0 ? 0 : 1);
    if (pose == myo::Pose::fingersSpread) {
      turnOn(light);
      locker_p_->extendUnlock();
    } else if (pose == myo::Pose::fist) {
      turnOff(light);
      locker_p_->extendUnlock();
    } else if (pose == myo::Pose::waveIn) {
      if (roll_diff < -0.2) {
        for (size_t i = 0; i < num_lights_; ++i) {
          turnOff(i);
        }
      } else {
        turnOn(0);
        turnOff(1);
      }
      locker_p_->extendUnlock();
    } else if (pose == myo::Pose::waveOut) {
      if (roll_diff < -0.2) {
        for (size_t i = 0; i < num_lights_; ++i) {
          turnOn(i);
        }
      } else {
        turnOff(0);
        turnOn(1);
      }
      locker_p_->extendUnlock();
    }
  }

  void setRoll(float roll) { roll_ = roll; }
  void setRollMid(float mid) { roll_mid_ = mid; }

  void setYaw(float yaw) { yaw_ = yaw; }
  void setYawMid(float mid) { yaw_mid_ = mid; }

 private:
  LockController* locker_p_;
  bool light_states_[2];
  size_t num_lights_ = 2;
  float roll_, roll_mid_, yaw_, yaw_mid_;
};
