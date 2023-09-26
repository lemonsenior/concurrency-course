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
    auto free_state = static_cast<uint32_t>(MutexStates::Free);
    if(phase_.compare_exchange_strong(free_state, MutexStates::LockedWithoutContention)) {
      return;
    }

    while(phase_.exchange(MutexStates::LockedWithContention) != MutexStates::Free) {
      twist::ed::Wait(phase_, MutexStates::LockedWithContention);
    }
  }

  void Unlock() {
    auto key = twist::ed::PrepareWake(phase_);
    if (phase_.exchange(MutexStates::Free) == LockedWithContention) {
      twist::ed::WakeOne(key);
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
  twist::ed::stdlike::atomic<uint32_t> phase_{0};
  //twist::ed::stdlike::atomic<uint32_t> cnt_{0};
};

}  // namespace stdlike
