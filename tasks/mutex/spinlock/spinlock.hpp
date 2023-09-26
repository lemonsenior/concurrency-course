#pragma once

#include "atomic.hpp"
#include "atomic_ops.hpp"

#include <twist/ed/wait/spin.hpp>

// Naive Test-and-Set (TAS) spinlock

class TASSpinLock {
 public:
  void Lock() {
    // while (locked_.Exchange(1) == 1) {
    while (AtomicExchange(&locked_, 1) != 0) {
      twist::ed::CpuRelax();
    }
  }

  bool TryLock() {
    // if (AtomicLoad(&locked_) == 1) {
    //   return false;
    // }
    // Lock();
    // return true;
    return AtomicExchange(&locked_, 1) == 0;
  }

  void Unlock() {
    AtomicStore(&locked_, 0);
    // locked_.Store(0);
  }

 private:
  AtomicInt64 locked_ = 0;
  // stdlike::Atomic locked_{0};
};
