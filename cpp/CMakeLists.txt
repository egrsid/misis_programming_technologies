cmake_minimum_required(VERSION 3.16)
project(catalog_dedup CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Runner не передаёт тип сборки: без этого получится -O0 и честное решение не уложится в TL.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
add_executable(solution main.cpp)   # target ДОЛЖЕН называться solution
