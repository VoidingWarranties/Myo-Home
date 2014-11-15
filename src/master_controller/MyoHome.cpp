#include <iostream>
#include <myo/myo.hpp>
#include "MasterController.h"

int main() {
  try {
    myo::Hub hub("com.voidingwarranties.myo-intelligesture");
    myo::Myo* myo = hub.waitForMyo(10000);
    if (!myo) {
      throw std::runtime_error("Unable to find a Myo!");
    }

    MasterController master_controller(myo);
    hub.addListener(&master_controller);

    while (true) {
      hub.run(1000 / 20);
      master_controller.onPeriodic(myo);
    }
  } catch (const std::exception& ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
