cmake_minimum_required(VERSION 3.16)
project(scope_guard CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Runner не передаёт тип сборки: без этого получится -O0.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
add_executable(solution main.cpp)   # target ДОЛЖЕН называться solution
