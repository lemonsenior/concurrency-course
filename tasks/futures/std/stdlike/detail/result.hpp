#pragma once

#include <exception>
#include <tl/expected.hpp>

namespace stdlike::detail {

template <typename T>
using Result = tl::expected<T, std::exception_ptr>;

}  // namespace stdlike::detail