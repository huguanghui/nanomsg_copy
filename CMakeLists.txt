cmake_minimum_required(VERSION 2.8.12)

project(nanomsg C)
include(CheckFunctionExists)
include(CheckSymbolExists)
include(CheckStructHasMember)
include(CheckLibraryExists)
include(CheckCSourceCompiles)
include(GNUInstallDirs)

if(POLICY CMP0042)
  # Newer cmake on MacOS should use @rpath
  cmake_policy(SET CMP0042 NEW)
endif()

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(NN_DESCRIPTION "High-Performance Scalability Protocols")
set(ISSUE_REPORT_MSG
    "Please consider opening an issue at https://github.com/nanomsg/nanomsg")

# Determine library versions.

file(READ src/nn.h NN_HDR_STR)
string(REGEX REPLACE ".*#define +NN_VERSION_CURRENT +([0-9]+).*" "\\1"
                     NN_VERSION_CURRENT "${NN_HDR_STR}")
string(REGEX REPLACE ".*#define +NN_VERSION_REVISION +([0-9]+).*" "\\1"
                     NN_VERSION_REVISION "${NN_HDR_STR}")
string(REGEX REPLACE ".*#define +NN_VERSION_AGE +([0-9]+).*" "\\1"
                     NN_VERSION_AGE "${NN_HDR_STR}")

if((NN_VERSION_CURRENT STREQUAL "")
   OR (NN_VERSION_REVISION STREQUAL "")
   OR (NN_VERSION_AGE STREQUAL ""))
  message(FATAL_ERROR "Could not read ABI version from nn.h")
else()
  set(NN_ABI_VERSION "${NN_VERSION_CURRENT}")
  set(NN_LIB_VERSION
      "${NN_VERSION_CURRENT}.${NN_VERSION_REVISION}.${NN_VERSION_AGE}")
  message(STATUS "Detected nanomsg ABI v${NN_ABI_VERSION} (v${NN_LIB_VERSION})")
endif()

find_package(Git QUIET)
if(DEFINED ENV{TRAVIS_TAG})
  set(NN_PACKAGE_VERSION "$ENV{TRAVIS_TAG}")
elseif(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" describe --always # | sed -e "s:v::"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    OUTPUT_VARIABLE NN_PACKAGE_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(
    COMMAND "${GIT_EXECUTABLE}" diff --quiet
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    RESULT_VARIABLE res)
  if(res EQUAL 1)
    set(NN_PACKAGE_VERSION "${NN_PACKAGE_VERSION}"-dirty)
  endif()
elseif(EXISTS ${PROJECT_SOURCE_DIR}/.version)
  file(STRINGS .version NN_PACKAGE_VERSION)
else()
  set(NN_PACKAGE_VERSION "Unknown")
endif()

option(NN_STATIC_LIB "Build static library instead of shared library." OFF)
option(NN_ENABLE_DOC "Enable building documentation." ON)
option(NN_ENABLE_COVERAGE "Enable coverage reporting." OFF)
option(NN_ENABLE_GETADDRINFO_A
       "Enable/disable use of getaddrinfo_a in place of getaddrinfo." ON)
option(NN_TESTS "Build and run nanomsg tests" ON)
option(NN_TOOLS "Build nanomsg tools" ON)
option(NN_ENABLE_NANOCAT "Enable building nanocat utility." ${NN_TOOLS})
set(NN_MAX_SOCKETS
    512
    CACHE STRING "max number of nanomsg sockets than can be created")

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  set(NN_WARN_FLAGS "-Wall -Wextra")
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  set(NN_WARN_FLAGS "-Wall -Wextra")
elseif(CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
  set(NN_WARN_FLAGS "-Wall -Wextra")
endif()

if(NN_ENABLE_COVERAGE)
  if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(NN_COVERAGE_FLAGS "-g -O0 --coverage")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(NN_COVERAGE_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
    set(NN_COVERAGE_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
  else()
    message(FATAL_ERROR "Unable to enable coverage for your compiler.")
  endif()
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${NN_WARN_FLAGS} ${NN_COVERAGE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${NN_WARN_FLAGS} ${NN_COVERAGE_FLAGS}")

find_package(Threads REQUIRED)

message(STATUS "OS System is ${CMAKE_SYSTEM_NAME}")
message(STATUS "OS Version is ${CMAKE_SYSTEM_VERSION}")
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  add_definitions(-DNN_HAVE_LINUX)
  if(CMAKE_SYSTEM_VERSION MATCHES "Miscrosoft")
    add_definitions(-DNN_HAVE_WSL)
  endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Android")
  add_definitions(-DNN_HAVE_ANDROID)
  add_definitions(-DNN_HAVE_LINUX)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  add_definitions(-DNN_HAVE_OSX)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(NN_HAVE_WINSOCK 1)
  add_definitions(-DNN_HAVE_WINDOWS)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)

  add_definitions(-D_WIN32_WINNT=0x0600)
  list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_WIN32_WINNT=0x0600)
elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
  add_definitions(-DNN_HAVE_FREEBSD)
elseif(CMAKE_SYSTEM_NAME MATCHES "NetBSD")
  add_definitions(-DNN_HAVE_NETBSD)
elseif(CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
  add_definitions(-DNN_HAVE_OPENBSD)
elseif(CMAKE_SYSTEM_NAME MATCHES "Solaris|SunOS")
  add_definitions(-DNN_HAVE_SOLARIS)
elseif(CMAKE_SYSTEM_NAME MATCHES "HP-UX")
  add_definitions(-DNN_HAVE_HPUX)
elseif(CMAKE_SYSTEM_NAME MATCHES "QNX")
  add_definitions(-DNN_HAVE_QNX)
else()
  message(
    AUTHOR_WARNING
      "WARNING: This platform may or may not be supported: ${CMAKE_SYSTEM_NAME}"
  )
  message(AUTHOR_WARNING "${ISSUE_REPORT_MSG}")
endif()

if(NN_STATIC_LIB)
  add_definitions(-DNN_STATIC_LIB)
endif()

macro(nn_check_func SYM DEF)
  check_function_exists(${SYM} ${DEF})
  if(${DEF})
    add_definitions(-D${DEF}=1)
  endif()
endmacro(nn_check_func)

macro(nn_check_sym SYM HDR DEF)
  check_symbol_exists(${SYM} ${HDR} ${DEF})
  if(${DEF})
    add_definitions(-D${DEF}=1)
  endif()
endmacro(nn_check_sym)

macro(nn_check_lib LIB SYM DEF)
  check_library_exists(${LIB} ${SYM} "" ${DEF})
  if(${DEF})
    add_definitions(-D${DEF}=1)
    set(NN_REQUIRED_LIBRARIES ${NN_REQUIRED_LIBRARIES} ${LIB})
  endif()
endmacro(nn_check_lib)

macro(nn_check_struct_member STR MEM HDR DEF)
  check_struct_has_member("struct ${STR}" ${MEM} ${HDR} ${DEF})
  if(${DEF})
    add_definitions(-D${DEF}=1)
  endif()
endmacro(nn_check_struct_member)

if(WIN32)
  set(NN_REQUIRED_LIBRARIES ${NN_REQUIRED_LIBRARIES} ws2_32)
  set(NN_REQUIRED_LIBRARIES ${NN_REQUIRED_LIBRARIES} mswsock)
  set(NN_REQUIRED_LIBRARIES ${NN_REQUIRED_LIBRARIES} advapi32)
  nn_check_sym(InitializeConditionVariable windows.h NN_HAVE_CONDVAR)
  if(NOT NN_HAVE_CONDVAR)
    message(
      FATAL_ERROR
        "Modern Windows API support is missing. "
        "Versions of Windows prior to Vista are not supported.  "
        "Further, the 32-bit MinGW environment is not supported. "
        "Ensure you have at least Windows Vista or newer, and are "
        "using either Visual Studio 2010 or newer or MinGW-W64.")
  endif()
else()
  add_definitions(-D_GNU_SOURCE)
  add_definitions(-D_REENTRANT)
  add_definitions(-D_THREAD_SAFE)
  add_definitions(-D_POSIX_PTHREAD_SEMANTICS)

  nn_check_func(gethrtime NN_HAVE_GETHRTIME)
  nn_check_func(socketpair NN_HAVE_SOCKETPAIR)
  nn_check_func(eventfd NN_HAVE_EVENTFD)
  nn_check_func(pipe NN_HAVE_PIPE)
  nn_check_func(pipe2 NN_HAVE_PIPE2)
  nn_check_func(accept4 NN_HAVE_ACCEPT4)
  nn_check_func(epoll_create NN_HAVE_EPOLL)
  nn_check_func(kqueue NN_HAVE_EQUEUE)
  nn_check_func(poll NN_HAVE_POLL)

  nn_check_lib(anl getaddrinfo_a NN_HAVE_GETADDRINFO_A)
  nn_check_lib(rt clock_gettime NN_HAVE_CLOCK_GETTIME)
  nn_check_lib(rt sem_wait NN_HAVE_SEMAPHORE_RT)
  nn_check_lib(pthread sem_wait NN_HAVE_SEMAPHORE_RT)
  nn_check_lib(nsl gethostbyname NN_HAVE_LIBNSL)
  nn_check_lib(socket socket NN_HAVE_LIBSOCKET)

  nn_check_sym(CLOCK_MONOTONIC time.h NN_HAVE_CLOCK_MONOTONIC)
  nn_check_sym(atomic_cas_32 atomic.h NN_HAVE_ATOMIC_SOLARIS)
  nn_check_sym(AF_UNIX sys/socket.h NN_HAVE_UNIX_SOCKETS)
  nn_check_sym(backtrace_symbols_fd execinfo.h NN_HAVE_BACKTRACE)
  nn_check_struct_member(msghdr msg_control sys/socket.h NN_HAVE_MSG_CONTROL)
  if(NN_HAVE_SEMAPHORE_RT OR NN_HAVE_SEMAPHORE_PTHREAD)
    if(NOT CMAKE_SYSTEM_NAME MATCHES "Darwin")
      add_definitions(-DNN_HAVE_SEMAPHORE)
    endif()
  endif()
endif()

if(NOT NN_ENABLE_GETADDRINFO_A)
  add_definitions(-DNN_DISABLE_GETADDRINFO_A)
endif()

check_c_source_compiles(
  "
    #include <stdint.h>
    int main()
    {
      volatile uint32_t n = 0;
      __sync_fetch_and_add (&n, 1);
      __sync_fetch_and_sub (&n, 1);
      return 0;
    }
  "
  NN_HAVE_GCC_ATOMIC_BUILTINS)
if(NN_HAVE_GCC_ATOMIC_BUILTINS)
  add_definitions(-DNN_HAVE_GCC_ATOMIC_BUILTINS)
endif()

add_definitions(-DNN_MAX_SOCKETS=${NN_MAX_SOCKETS})

add_subdirectory(src)
