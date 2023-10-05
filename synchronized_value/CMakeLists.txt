# CMakeList.txt : CMake project for synchronized_value, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (synchronized_value "synchronized_value.cpp" "synchronized_value.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET synchronized_value PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
