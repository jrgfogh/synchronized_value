add_executable(
  synchronized_tests
  synchronized_tests.cpp
  doubly_linked_list_tests.cpp)
target_link_libraries(
  synchronized_tests
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(synchronized_tests)