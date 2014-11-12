#ifndef MYO_INTELLIGESTURE_MASTERCONTROLLER_H_
#define MYO_INTELLIGESTURE_MASTERCONTROLLER_H_

#include <myo/myo.hpp>
#include "../../../Myo-Intelligesture/src/PoseGestures.h"
#include "LockController.h"
#include "LightsController.h"
#include "MediaController.h"

template <class BaseClass = PoseGestures<>,
          class PoseClass = PoseGestures<>::Pose>
class MasterController : public BaseClass {
 public:
  MasterController(myo::Myo* myo)
      : locker_(myo),
        current_appliance_(Appliance::lights),
        lights_controller_(&locker_),
        media_controller_(&locker_) {}

  void onPose(myo::Myo* myo, PoseClass pose) {
    std::cout << "detected pose: " << pose << std::endl;

    if (locker_.locked()) {
      if (pose.gesture() == PoseClass::Gesture::doubleClick) {
        if (pose.pose() == PoseClass::fingersSpread) {
          std::cout << "Lights mode" << std::endl;
          lights_controller_.markStartingRotation();
          current_appliance_ = Appliance::lights;
          this->calibrateOrientation();
          locker_.unlock();
        } else if (pose.pose() == PoseClass::fist) {
          std::cout << "Media mode" << std::endl;
          current_appliance_ = Appliance::media;
          this->calibrateOrientation();
          locker_.unlock();
        }
      }
    } else {
      switch (current_appliance_) {
        case Appliance::lights:
          lights_controller_.onPose(myo, pose);
          break;
        case Appliance::media:
          media_controller_.onPose(myo, pose);
          break;
      }
    }
  }

  void onOrientationData(myo::Myo* myo, uint64_t timestamp,
                         const myo::Quaternion<float>& quat) {
    BaseClass::onOrientationData(myo, timestamp, quat);

    lights_controller_.onOrientationData(myo, quat);
    media_controller_.onOrientationData(myo, quat);
  }

  void onPeriodic(myo::Myo* myo) {
    BaseClass::onPeriodic(myo);

    locker_.onPeriodic();
    if (current_appliance_ == Appliance::media) media_controller_.onPeriodic();
  }

  virtual void onArmRecognized(myo::Myo* myo, uint64_t timestamp, myo::Arm arm,
                       myo::XDirection x_direction) {
    BaseClass::onArmRecognized(myo, timestamp, arm, x_direction);

    media_controller_.onArmRecognized(myo, arm, x_direction);
  }

 private:
  enum class Appliance { lights, media };

  LockController locker_;
  Appliance current_appliance_;

  LightsController lights_controller_;
  MediaController media_controller_;
};

#endif
