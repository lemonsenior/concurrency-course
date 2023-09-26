#pragma once

#include <atomic>
#include "twist/ed/stdlike/atomic.hpp"
#include "twist/ed/wait/futex.hpp"
#include "twist/rt/layer/facade/wait/futex.hpp"

namespace stdlike::detail {

class Event {
 public:
  void Wait() {
    twist::ed::futex::Wait(ready_, 0);
  }

  void Set() {
    auto wake_key = twist::ed::futex::PrepareWake(ready_);
    ready_.store(1);
    twist::ed::futex::WakeOne(wake_key);
  }

 private:
  twist::ed::stdlike::atomic<uint32_t> ready_{0};
};

}  // namespace stdlike::detail