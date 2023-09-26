#pragma once

#include <__concepts/arithmetic.h>
#include <mutex>
#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>

#include <cstdlib>

class Semaphore {
 public:
  explicit Semaphore(size_t tokens)
      : tokens_(tokens) {
  }

  void Acquire() {
    std::unique_lock<twist::ed::stdlike::mutex> lock(mutex_);
    has_tokens_.wait(lock, [=]() {
      return tokens_ > 0;
    });
    --tokens_;
  }

  void Release() {
    std::unique_lock<twist::ed::stdlike::mutex> lock(mutex_);
    ++tokens_;
    if (tokens_ == 1) {
      has_tokens_.notify_all();
    }
  }

 private:
  twist::ed::stdlike::mutex mutex_;
  size_t tokens_;
  twist::ed::stdlike::condition_variable has_tokens_;
};
