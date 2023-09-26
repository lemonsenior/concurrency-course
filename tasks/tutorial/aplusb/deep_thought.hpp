#pragma once

namespace galaxy {

struct UltimateQuestion {};

class DeepThought {
 public:
  int ComputeAnswer(UltimateQuestion) {
    return 42;
  }
};

}  // namespace galaxy
