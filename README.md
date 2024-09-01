# synchronized

`synchronized` is a utility class meant to almost completely replace manual usage of mutexes and condition variables in C++,
in the same way that smart pointers replace owning raw pointers.

The project originally grew out of a [lightning talk](https://www.youtube.com/watch?v=F0jaGcCUpOo) at CppCon 2023 about
a version of `synchronized_value`, which has been proposed for the C++ standard. Based on the feedback and inspiration from similar
projects, I have decided to redesign the abstraction.

## Usage

A simple but complete example can be found in `examples/synchronized_stack.h`:

```c++
class synchronized_stack {
  sv::synchronized<std::stack<int>> data_;

public:
  using value_type = Element;

  void push(int d) { data_.wlock()->push(d); }
  std::optional<int> pop() {
    auto lock = data_.wlock();
    if (lock->empty()) return {};
    auto result = lock->top();
    lock->pop();
    return result;
  }
  auto empty() { return data_.rlock()->empty(); }
  auto size() { return data_.rlock()->size(); }
};
```

## Design Goals

* Make it easy to fix broken concurrent code / safely add concurrency to sequential code.
* Be a [zero cost abstraction](https://stackoverflow.com/questions/69178380/what-does-zero-cost-abstraction-mean).
* Support enough features that users have no need to use mutexes directly.

## Ideas for Extensions

* Const should propagate, so `synchronized<T> const` is equivalent to `synchronized<T const>`.
* Timed mutex types.
* Sharing a mutex between several update guards, analogously to how several `std::shared_ptr`s can share a count.
  Use case: Locking head/tail in the `concurrent_doubly_linked_list` example. 
* Make update_guard movable/swappable, to facility hand-over-hand locking.
* Support for alternative mutex types, e.g. `std::shared_mutex`, `std::timed_mutex`, etc.
* Support for [conditional critical sections](https://abseil.io/docs/cpp/guides/synchronization#conditional-critical-sections)
* Hierarchies of synchronized values, for working with arena allocators:
  
  To read, only lock the value in read mode. To write, lock both the value and the arena in write mode.

## Related Work:

[CsLibGuarded](https://github.com/copperspice/cs_libguarded): "... a standalone header only library for multithreaded programming."

A very mature library, which is part of [CopperSpice](https://www.copperspice.com/).

[`Synchronized<T>` from folly](https://github.com/facebook/folly/blob/main/folly/docs/Synchronized.md): Very well-designed abstraction. Inspired `synchronized`'s interface.

[data_mutex](https://www.pera-software.com/html/blog/compile-time-guaranteed-thread-synchronization/compile-time-guaranteed-thread-synchronization-in-cpp.html) by [Peter Most](https://github.com/petermost): 
Very similar to the `synchronized_value` proposed for the C++ standard, but with different names for the classes.
Includes support for const access. Part of [CppAidKit](https://github.com/petermost/CppAidKit)

[Boost's synchronized_value](https://www.boost.org/doc/libs/1_83_0/doc/html/thread/sds.html#thread.sds.synchronized_valuesxxx):
Essentially the same interface as the one proposed for the standard, with a few features added.

The current standard proposal is described in documents number [N4033](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4033.html) and [p0290r4](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p0290r4.html).
