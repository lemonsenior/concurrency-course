#pragma once

#include <mutex>
#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>

#include <cstdlib>

class CyclicBarrier {
 public:
  explicit CyclicBarrier(size_t participants)
      : all_participantes_(participants),
        curr_participantes_(0) {
  }

  void ArriveAndWait() {
    WaitForOthers();
    GoThrow();
  }

 private:
  void WaitForOthers() {
    std::unique_lock lock(mutex_);
    ++curr_participantes_;
    if (curr_participantes_ == all_participantes_) {
      flag_is_opened_ = true;
      condvar_is_opened_.notify_all();
    } else {
      condvar_is_opened_.wait(lock, [this] {
        return flag_is_opened_;
      });
    }
  }

  void GoThrow() {
    std::unique_lock lock(mutex_);
    --curr_participantes_;
    if (curr_participantes_ == 0) {
      flag_is_opened_ = false;
      condvar_is_closed_.notify_all();
    } else {
      condvar_is_closed_.wait(lock, [this] {
        return !flag_is_opened_;
      });
    }
  }

  size_t all_participantes_;
  size_t curr_participantes_;
  twist::ed::stdlike::mutex mutex_{};
  bool flag_is_opened_{false};
  twist::ed::stdlike::condition_variable condvar_is_opened_;
  twist::ed::stdlike::condition_variable condvar_is_closed_;
};
