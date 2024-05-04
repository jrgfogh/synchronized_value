#pragma once

#include "../synchronized_value/synchronized.h"

#include <optional>
#include <stack>

namespace example {
/// A simple example to show the basic usage of `synchronized`.
template <typename Element> class synchronized_stack {
  sv::synchronized<std::stack<int>> data_;

public:
  using value_type = Element;

  void push(int d) { data_.wlock()->push(d); }
  std::optional<int> pop() {
    auto lock = data_.wlock();
    if (lock->empty())
      return {};
    auto result = lock->top();
    lock->pop();
    return result;
  }
  auto empty() { return data_.rlock()->empty(); }
  auto size() { return data_.rlock()->size(); }
};
} // namespace example