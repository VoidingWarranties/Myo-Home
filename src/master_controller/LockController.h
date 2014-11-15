/* Manages the locking and unlocking for the UI. It will automatically re-lock
 * after the specified timeout.
 */

#ifndef MYO_HOME_LOCKCONTROLLER_H_
#define MYO_HOME_LOCKCONTROLLER_H_

#include <ctime>
#include "../../../Basic-Timer/BasicTimer.h"

class LockController {
 public:
  LockController(myo::Myo* myo, int unlock_time = SUGGESTED_UNLOCK_TIME)
      : myo_(myo), unlock_time_(unlock_time), locked_(true) {}

  bool locked() const { return locked_; }

  void lock() {
    locked_ = true;
    myo_->vibrate(myo::Myo::vibrationMedium);
    std::cout << "Locked!" << std::endl;
  }
  void unlock() {
    locked_ = false;
    unlock_timer_.tick();
    myo_->vibrate(myo::Myo::vibrationShort);
    myo_->vibrate(myo::Myo::vibrationShort);
    std::cout << "Unlocked" << std::endl;
  }
  void extendUnlock() { unlock_timer_.tick(); }

  void onPeriodic() {
    if (!locked_ && unlock_timer_.millisecondsSinceTick() > unlock_time_) {
      lock();
    }
  }

  static const int SUGGESTED_UNLOCK_TIME = 5000;

 private:
  myo::Myo* myo_;
  int unlock_time_;
  std::function<void(void)> onLockCallback_, onUnlockCallback_;
  bool locked_;
  BasicTimer unlock_timer_;
};

#endif
