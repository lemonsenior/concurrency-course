#pragma once

#include <mutex>
#include <stdlike/future.hpp>
// #include <stdlike/helpers.hpp>
#include "stdlike/detail/shared_state.hpp"
#include "tl/expected.hpp"
#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>

#include <memory>

namespace stdlike {

template <typename T>
class Promise {
  using State = detail::SharedState<T>;

 public:
  Promise() {
    state_ = new detail::SharedState<T>{};
  }

  // Non-copyable
  Promise(const Promise&) = delete;
  Promise& operator=(const Promise&) = delete;

  // Movable
  Promise(Promise&& that)
      : state_(that.Release()) {
  }
  Promise& operator=(Promise&&) = default;

  // One-shot
  Future<T> MakeFuture() {
    return Future(state_);
  }

  // One-shot
  // Fulfill promise with value
  void SetValue(T value) {
    Release()->Produce(std::move(value));
  }

  // void SetValue(T& value) {
  //   content_ptr_->SetValue(value);
  // }

  // void SetValue(T&& value) {
  //   content_ptr_->SetValue(std::move(value));
  // }

  // One-shot
  // Fulfill promise with exception
  void SetException(std::exception_ptr exception_ptr) {
    Release()->Produce(tl::unexpected(exception_ptr));
  }

  ~Promise() {
    delete state_;
  }

 private:
  // std::shared_ptr<detail::StaredState<T>> content_ptr_;
  State* Release() {
    return std::exchange(state_, nullptr);
  }

  State* state_;
};

}  // namespace stdlike
