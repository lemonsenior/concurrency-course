#pragma once

#include <optional>
#include "rendezvous.hpp"

namespace stdlike::detail {

template <typename T>
class SharedState {
 public:
  void Produce(Result<T> result) {
    result_.emplace(std::move(result));
    if (bool both = state_.Produce()) {
      Rendezvous();
    }
  }

  void Consume(Callback<T> cb) {
    cb_.emplace(std::move(cb));
    if (bool both = state_.Consume()) {
      Rendezvous();
    }
  }

 private:
  void Rendezvous() {
    (*cb_)(std::move(*result_));
    delete this;
  }

  std::optional<Result<T>> result_;
  std::optional<Callback<T>> cb_;
  RendezvousStateMachine state_;
};

}  // namespace stdlike::detail