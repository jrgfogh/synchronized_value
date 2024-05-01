# synchronized_value

`synchronized_value` is a utility class meant to almost completely replace manual usage of mutexes and condition variables in C++,
in the same way that smart pointers replace owning raw pointers.

The project originally grew out of a [lightning talk](https://www.youtube.com/watch?v=F0jaGcCUpOo) at CppCon 2023 about
a version of `synchronized_value`, which has been proposed for the C++ standard. Based on the feedback and inspiration from similar
projects, I have decided to redesign the abstraction.

## Design Goals

* Make it easy to fix broken concurrent code / safely add concurrency to sequential code.
* Be a [zero cost abstraction](https://stackoverflow.com/questions/69178380/what-does-zero-cost-abstraction-mean).
* Support enough features that users have no need to use mutexes directly.

## Ideas for Extensions

* Const should propagate, so `synchronized_value<T> const` is equivalent to `synchronized_value<T const>`.
* Timed mutex types.
* Sharing a mutex between several update guards, analogously to how several `std::shared_ptr`s can share a count.
  Use case: Locking head/tail in the `concurrent_doubly_linked_list` example. 
* Make update_guard movable/swappable, to facility hand-over-hand locking.
* Support for alternative mutex types, e.g. `std::shared_mutex`, `std::timed_mutex`, etc.
* Support for [conditional critical sections](https://abseil.io/docs/cpp/guides/synchronization#conditional-critical-sections)
* Hierarchies of synchronized values, for working with arena allocators:
  
  To read, only lock the value in read mode. To write, lock both the value and the arena in write mode.

## Related Work:

Ansel Sermersheim talked about a similar construct with some extra features at CppCon 2017. The relevant bits are in part 2.

[CppCon 2017: Ansel Sermersheim “Multithreading is the answer. What is the question? (part 1 of 2)”](https://www.youtube.com/watch?v=GNw3RXr-VJk)

[CppCon 2017: Ansel Sermersheim “Multithreading is the answer. What is the question? (part 2 of 2)”](https://www.youtube.com/watch?v=sDLQWivf1-I)

[Boost's synchronized_value](https://www.boost.org/doc/libs/1_83_0/doc/html/thread/sds.html#thread.sds.synchronized_valuesxxx)

[`Synchronized<T>` from folly](https://github.com/facebook/folly/blob/main/folly/docs/Synchronized.md)

The current standard proposal is described in documents number [N4033](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4033.html) and [p0290r4](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p0290r4.html).
