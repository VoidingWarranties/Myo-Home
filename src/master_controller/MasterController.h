#ifndef MYO_INTELLIGESTURE_MASTERCONTROLLER_H_
#define MYO_INTELLIGESTURE_MASTERCONTROLLER_H_

#include <myo/myo.hpp>
#include "../../../Myo-Intelligesture/src/PoseGestures.h"

template <class BaseClass = PoseGestures<>, class PoseClass = PoseGestures<>::Pose>
class MasterController : public BaseClass {
 public:
  void onPose(myo::Myo* myo, PoseClass pose) {
    std::cout << "detected pose: " << pose << std::endl;
  }
};

#endif
