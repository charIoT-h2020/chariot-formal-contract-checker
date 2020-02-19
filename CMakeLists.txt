cmake_minimum_required(VERSION 3.0)

project(contract_checker)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(libutils)
add_subdirectory(libalgorithms)

include_directories(utils)
include_directories(algorithms)
set(SOURCES
   src/Expression.cpp
   src/MemoryZone.cpp
   src/Contract.cpp
   src/MemoryState.cpp
   src/Processor.cpp
   src/dll.cpp
   src/contract_checker.cpp
   )
    
set(HEADERS
   src/domsec_callback.h
   src/target_address_decoder.h
   src/decsec_callback.h
   src/memsec_callback.h
   src/DomainValue.h
   src/Expression.h
   src/Contract.h
   src/MemoryZone.h
   src/MemoryState.h
   src/Processor.h
   src/dll.h
   src/contract_checker.h
   )

add_library(contract_checker SHARED ${SOURCES})
target_link_libraries(contract_checker utils numerics stdc++)

enable_testing()
# add_subdirectory(tests)
add_test(NAME TestPython COMMAND python3 ${CMAKE_SOURCE_DIR}/src/check_contract.py
        -arch ${CMAKE_SOURCE_DIR}/../chariot-formal-decoder-armv7/src/armsec_decoder.so
        -dom  ${CMAKE_SOURCE_DIR}/../chariot-semantic-domains/build/libapplications/libScalarInterface.so
        ${CMAKE_SOURCE_DIR}/tests/modbus.nobug.bin ${CMAKE_SOURCE_DIR}/tests/contracts.json)

install(TARGETS contract_checker
        LIBRARY DESTINATION bin)
