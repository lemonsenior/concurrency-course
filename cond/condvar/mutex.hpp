#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/sys.hpp>

#include <cstdint>

namespace stdlike {

enum MutexStates : uint32_t {
  Free,
  LockedWithoutContention,
  LockedWithContention
};

class Mutex {
 public:
  void Lock() {
    cnt_.fetch_add(1);
    while (flag_.exchange(1) != 0) {
      twist::ed::Wait(flag_, 1);
    }
  }

  void Unlock() {
    auto wake_key = twist::ed::PrepareWake(flag_);
    flag_.store(0);
    if (cnt_.fetch_sub(1) != 1) {
      twist::ed::WakeOne(wake_key);
    }
  }


  // BasicLockable
  // https://en.cppreference.com/w/cpp/named_req/BasicLockable

  void lock() {  // NOLINT
    Lock();
  }

  void unlock() {  // NOLINT
    Unlock();
  }

 private:
  //twist::ed::stdlike::atomic<uint32_t> phase_{0};
  twist::ed::stdlike::atomic<uint32_t> cnt_{0};
  twist::ed::stdlike::atomic<uint32_t> flag_{0};
};

}  // namespace stdlike
