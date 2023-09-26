#pragma once

#include "tagged_semaphore.hpp"
#include "twist/ed/stdlike/condition_variable.hpp"
#include "twist/ed/stdlike/mutex.hpp"

#include <deque>
#include <mutex>
#include <utility>

// Bounded Blocking Multi-Producer/Multi-Consumer (MPMC) Queue

template <typename T>
class BoundedBlockingQueue {
 public:
  explicit BoundedBlockingQueue(size_t capacity)
      : free_semaphore_(capacity),
        wasted_semaphore_(0),
        buffer_() {
  }

  void Put(T value) {
    auto new_token = free_semaphore_.Acquire();
    auto lock = mutex_.Acquire();

    buffer_.push_back(std::move(value));

    mutex_.Release(std::move(lock));
    wasted_semaphore_.Release(std::move(new_token));
  }

  T Take() {
    auto token = wasted_semaphore_.Acquire();
    auto lock = mutex_.Acquire();
    T value = std::move(buffer_.front());
    buffer_.pop_front();
    mutex_.Release(std::move(lock));
    free_semaphore_.Release(std::move(token));
    return value;
  }

 private:
  struct Tag {};

  TaggedSemaphore<Tag> free_semaphore_;
  TaggedSemaphore<Tag> wasted_semaphore_;

  struct MutexTag {};
  TaggedSemaphore<MutexTag> mutex_{1};

 private:
  std::deque<T> buffer_;
};
