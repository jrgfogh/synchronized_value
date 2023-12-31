# synchronized_value

`std::synchronized_value` is a proposed new library type for the C++ standard.

This repo contains an example implementation of the current proposal.

The current proposal is described in documents number N4033 and p0290r4.

[Slides from the lightning talk at CppCon '23](https://github.com/jrgfogh/synchronized_value/blob/main/Adding%20Thread%20Safety.pdf)

## Ideas for extensions

* Support for alternative mutex types, e.g. `std::shared_mutex`, `std::timed_mutex`, etc.
* Compatibility with `std::condition_variable` and `std::condition_variable_any`
* Hierarchies of synchronized values, for working with arena allocators:
  
  To read, only lock the value in read mode. To write, lock both the value and the arena in write mode.

## Related work:

[Boost's synchronized_value](https://www.boost.org/doc/libs/1_83_0/doc/html/thread/sds.html#thread.sds.synchronized_valuesxxx)

[`Synchronized<T>` from folly](https://github.com/facebook/folly/blob/main/folly/docs/Synchronized.md)
