#pragma once

#include <mutex>
#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>
#include <queue>

#include <optional>
#include "twist/ed/stdlike/atomic.hpp"

namespace tp {

// Unbounded blocking multi-producers/multi-consumers (MPMC) queue

template <typename T>
class UnboundedBlockingQueue {
 public:
  bool Put(T value) {
    std::unique_lock ul(mutex_);
    if (is_stopped_.load()) {
      return false;
    }
    queue_.push(std::move(value));
    is_empty_.notify_one();
    return true;
  }

  std::optional<T> Take() {
    std::unique_lock ul(mutex_);

    is_empty_.wait(ul, [this]() {
      return !queue_.empty() || is_stopped_.load();
    });

    if (queue_.empty()) {
      return std::nullopt;
    }

    std::optional<T> value(std::move(queue_.front()));
    queue_.pop();

    return value;
  }

  void Close() {
    std::lock_guard guard(mutex_);
    is_stopped_.store(true);
    is_empty_.notify_all();
  }

 private:
  std::queue<T> queue_;
  twist::ed::stdlike::atomic<bool> is_stopped_{false};
  twist::ed::stdlike::mutex mutex_;
  twist::ed::stdlike::condition_variable is_empty_;
};

}  // namespace tp
