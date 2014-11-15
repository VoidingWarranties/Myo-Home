/* Controls and manages the UI and all of the appliances.
 */

#ifndef MYO_INTELLIGESTURE_MASTERCONTROLLER_H_
#define MYO_INTELLIGESTURE_MASTERCONTROLLER_H_

#include <myo/myo.hpp>
#include "../../../Myo-Intelligesture/src/PoseGestures.h"
#include "LockController.h"
#include "LightsController.h"
#include "MediaController.h"

class MasterController : public PoseGestures<> {
 public:
  MasterController(myo::Myo* myo)
      : locker_(myo),
        current_appliance_(Appliance::lights),
        lights_controller_(&locker_),
        media_controller_(&locker_) {}

  void onPose(myo::Myo* myo, PoseGestures<>::Pose pose) {
    if (pose.gesture() == PoseGestures<>::Pose::Gesture::doubleClick &&
        pose.pose() == PoseGestures<>::Pose::fingersSpread &&
        (locker_.locked() || current_appliance_ != Appliance::lights)) {
      std::cout << "Lights mode" << std::endl;
      lights_controller_.markStartingRotation();
      current_appliance_ = Appliance::lights;
      this->calibrateOrientation();
      locker_.unlock();
    } else if (pose.gesture() == PoseGestures<>::Pose::Gesture::doubleClick &&
               pose.pose() == PoseGestures<>::Pose::fist &&
               (locker_.locked() || current_appliance_ != Appliance::media)) {
      std::cout << "Media mode" << std::endl;
      current_appliance_ = Appliance::media;
      this->calibrateOrientation();
      locker_.unlock();
    } else if (!locker_.locked()) {
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
    PoseGestures<>::onOrientationData(myo, timestamp, quat);

    lights_controller_.onOrientationData(myo, quat);
    media_controller_.onOrientationData(myo, quat);
  }

  void onPeriodic(myo::Myo* myo) {
    PoseGestures<>::onPeriodic(myo);

    locker_.onPeriodic();
    if (current_appliance_ == Appliance::media) media_controller_.onPeriodic();
  }

  virtual void onArmRecognized(myo::Myo* myo, uint64_t timestamp, myo::Arm arm,
                               myo::XDirection x_direction) {
    PoseGestures<>::onArmRecognized(myo, timestamp, arm, x_direction);

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
