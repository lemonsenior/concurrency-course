#pragma once

#include <exception>
#include <memory>
#include <cassert>
#include <mutex>
#include <variant>

#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>

namespace detail {
template <typename T>
class StaredState {
 public:
  void SetVatue(T& value) {
    std::lock_guard lock(mutex_);
    content_ = std::make_unique<T>(value);
    is_error_ = false;
    is_completed_ = true;
    condvar_.notify_one();
  }

  void SetValue(T&& value) {
    std::lock_guard lock(mutex_);
    content_ = std::make_unique<T>(std::move(value));
    is_completed_ = true;
    condvar_.notify_one();
  }

  void SetException(std::exception_ptr exc_ptr) {
    std::lock_guard lock(mutex_);
    content_ = exc_ptr;
    is_completed_ = true;
    condvar_.notify_one();
  }

  T Get() {
    std::unique_lock ul(mutex_);
    while (!is_completed_) {
      condvar_.wait(ul);
    }
    try {
      return std::move(*std::get<std::unique_ptr<T>>(content_));
    } catch (const std::bad_variant_access& e) {
      std::rethrow_exception(std::get<std::exception_ptr>(content_));
    }
  }

 private:
  bool is_error_{false};
  bool is_completed_{false};
  twist::ed::stdlike::mutex mutex_;
  twist::ed::stdlike::condition_variable condvar_;
  std::variant<std::monostate, std::exception_ptr, std::unique_ptr<T>> content_;
};
}  // namespace detail