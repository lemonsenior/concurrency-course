#pragma once

#include <tp/task.hpp>

#include <optional>
#include <iostream>

namespace tp::support {

static inline void TryBlock(std::optional<Task> task) {
  try {
    (*task)();
  } catch (const std::exception& e) {
    std::cout << "task faild, because " << e.what() << '\n';
  } catch (...) {
    std::cout << "unknown error in task\n";
  }
}

}  // namespace tp::support