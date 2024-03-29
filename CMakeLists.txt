cmake_minimum_required(VERSION 3.18.4)
project(ipc C CXX)


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/llvmir)
include(LLVMIRUtil)
include(CTest)
find_package(Boost 1.71 REQUIRED)
find_program(AWK awk mawk gawk)
find_program(DEFAULT_LLVM_NM llvm-nm-14)
find_program(CPP_FILT c++filt)

# Enable CTest
enable_testing()

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR )
  message (FATAL_ERROR
    "In-source builds are not allowed. Please clean your source tree and try again.")
endif()

# Default is release with debug info
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

set(SEAHORN_ROOT "/usr" CACHE PATH "Path to SeaHorn installation")
set(SEA_LINK "llvm-link" CACHE STRING "Path to llvm-link")
set(LLVMIR_LINK ${SEA_LINK})
set(SEA_OPT "${SEAHORN_ROOT}/bin/seaopt" CACHE STRING  "Path to seaopt binary")
set(SEA_PP "${SEAHORN_ROOT}/bin/seapp" CACHE STRING  "Path to seapp binary")
set(LLVMIR_OPT ${SEA_OPT})
set(SEAMOCK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src" CACHE PATH "Path to SEAMOCK src directory")

set(LLVM_NM ${DEFAULT_LLVM_NM} CACHE PATH "Path to llvm-nm")

set (CMAKE_CXX_STANDARD 17)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/examples/verify.py.in verify @ONLY)
set(VERIFY_CMD ${CMAKE_CURRENT_BINARY_DIR}/verify)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/examples/extract_tests.cmake.in extract_tests.cmake @ONLY)
set(EXTRACT_TEST_CMD ${CMAKE_CURRENT_BINARY_DIR}/extract_tests.cmake)


#include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/include)

#include_directories(seahorn/trusty_common/include)

include_directories(${SEAHORN_ROOT}/include)
include_directories(${SEAMOCK_ROOT}/include)

add_subdirectory(examples)
