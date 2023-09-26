#pragma once

#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>

#include <mutex>

namespace tp {

class WaitGroup {
 public:
  void Add(size_t count) {
    std::lock_guard guard(mutex_);
    workers_count_ += count;
  }

  void Done() {
    std::lock_guard guard(mutex_);
    workers_count_ -= 1;
    if (workers_count_ == 0) {
      all_threads_.notify_all();
    }
  }

  void Wait() {
    std::unique_lock lock(mutex_);
    while (workers_count_ != 0) {
      all_threads_.wait(lock);
    }
  }

 private:
  size_t workers_count_{0};
  twist::ed::stdlike::mutex mutex_;
  twist::ed::stdlike::condition_variable all_threads_;
};

}  // namespace tp