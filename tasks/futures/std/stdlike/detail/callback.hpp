#pragma once

#include "result.hpp"
#include <function2/function2.hpp>

namespace stdlike::detail {

template <typename T>
using Callback = fu2::function<void(Result<T>)>;

}  // namespace stdlike::detail