#pragma once

#include <mutex>
#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/sys.hpp>
#include "mutex.hpp"

#include <cstdint>

namespace stdlike {

class CondVar {
 public:
  // Mutex - BasicLockable
  // https://en.cppreference.com/w/cpp/named_req/BasicLockable
  template <class Mutex>
  void Wait(Mutex& mutex) {
    uint32_t old_condvar = condvar_.load();
    mutex.unlock();
    twist::ed::Wait(condvar_, old_condvar);
    mutex.lock();
  }

  void NotifyOne() {
    auto key = twist::ed::PrepareWake(condvar_);
    condvar_.fetch_add(1);
    twist::ed::WakeOne(key);
  }

  void NotifyAll() {
    auto key = twist::ed::PrepareWake(condvar_);
    condvar_.fetch_add(1);
    twist::ed::WakeAll(key);
  }

 private:
  twist::ed::stdlike::atomic<uint32_t> condvar_{0};
};

}  // namespace stdlike
