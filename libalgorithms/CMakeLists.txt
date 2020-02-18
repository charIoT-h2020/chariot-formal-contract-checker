project(numerics)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(../utils)
include_directories(../algorithms)

set(SOURCES
   ../algorithms/Numerics/Integer.cpp
   ../algorithms/Numerics/HostFloating.cpp
   )

add_library(numerics ${SOURCES})
set_property(TARGET numerics PROPERTY POSITION_INDEPENDENT_CODE ON)
