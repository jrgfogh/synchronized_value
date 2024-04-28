# synchronized_value

`std::synchronized_value` is a proposed new library type for the C++ standard.

This repo contains an example implementation of the current proposal.

The current proposal is described in documents number [N4033](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4033.html) and [p0290r4](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p0290r4.html).

[Slides from the lightning talk at CppCon '23](https://github.com/jrgfogh/synchronized_value/blob/main/Adding%20Thread%20Safety.pdf)

## Ideas for extensions

* Const should propagate, so `synchronized_value<T> const` is equivalent to `synchronized_value<T const>`.
* Make update_guard movable/swappable, to facility hand-over-hand locking.
* Support for alternative mutex types, e.g. `std::shared_mutex`, `std::timed_mutex`, etc.
* Support for [conditional critical sections](https://abseil.io/docs/cpp/guides/synchronization#conditional-critical-sections)
* Hierarchies of synchronized values, for working with arena allocators:
  
  To read, only lock the value in read mode. To write, lock both the value and the arena in write mode.

## Related work:

Ansel Sermersheim talked about a similar construct with some extra features at CppCon 2017. The relevant bits are in part 2.

[CppCon 2017: Ansel Sermersheim “Multithreading is the answer. What is the question? (part 1 of 2)”](https://www.youtube.com/watch?v=GNw3RXr-VJk)

[CppCon 2017: Ansel Sermersheim “Multithreading is the answer. What is the question? (part 2 of 2)”](https://www.youtube.com/watch?v=sDLQWivf1-I)

[Boost's synchronized_value](https://www.boost.org/doc/libs/1_83_0/doc/html/thread/sds.html#thread.sds.synchronized_valuesxxx)

[`Synchronized<T>` from folly](https://github.com/facebook/folly/blob/main/folly/docs/Synchronized.md)
