#include <ctime>

class LockController {
 public:
  LockController(int unlock_time,
                 const std::function<void(void)>& onLockCallback,
                 const std::function<void(void)>& onUnlockCallback)
      : unlock_time_(unlock_time),
        onLockCallback_(onLockCallback),
        onUnlockCallback_(onUnlockCallback),
        locked_(true),
        unlocked_at_(0) {}

  bool locked() const { return locked_; }

  void lock() {
    locked_ = true;
    onLockCallback_();
  }
  void unlock() {
    locked_ = false;
    unlocked_at_ = std::clock();
    onUnlockCallback_();
  }
  void extendUnlock() {
    unlocked_at_ = std::clock();
  }

  void onPeriodic() {
    if (!locked_) {
      std::clock_t current_time = std::clock();
      int passed_milliseconds =
          float(current_time - unlocked_at_) / CLOCKS_PER_SEC * 100000;
      if (passed_milliseconds > unlock_time_) {
        lock();
      }
    }
  }

  static const int SUGGESTED_UNLOCK_TIME = 3000;

 private:
  int unlock_time_;
  std::function<void(void)> onLockCallback_, onUnlockCallback_;
  bool locked_;
  std::clock_t unlocked_at_;
};
