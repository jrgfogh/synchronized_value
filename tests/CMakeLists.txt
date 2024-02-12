enable_testing()

add_executable(
  synchronized_value_tests
  synchronized_value_tests.cpp
  doubly_linked_list_tests.cpp)
target_link_libraries(
  synchronized_value_tests
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(synchronized_value_tests)