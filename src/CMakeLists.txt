set(NN_SOURCES
    nn.h
    nn.c
    utils/alloc.h
    utils/alloc.c
    utils/atomic.h
    utils/atomic.c
    utils/attr.h
    utils/chunk.h
    utils/chunk.c
    utils/chunkref.h
    utils/chunkref.c
    utils/clock.h
    utils/clock.c
    utils/closefd.h
    utils/closefd.c
    utils/cont.h
    utils/err.h
    utils/err.c
    utils/fast.h
    utils/fd.h
    utils/hash.h
    utils/hash.c
    utils/list.h
    utils/list.c
    utils/msg.h
    utils/msg.c
    utils/condvar.h
    utils/condvar.c
    utils/mutex.h
    utils/mutex.c
    utils/once.h
    utils/once.c
    utils/queue.h
    utils/queue.c
    utils/random.h
    utils/random.c
    utils/sem.h
    utils/sem.c
    utils/sleep.h
    utils/sleep.c
    utils/strcasecmp.h
    utils/strcasecmp.c
    utils/strcasestr.h
    utils/strcasestr.c
    utils/strncasecmp.h
    utils/strncasecmp.c
    utils/thread.h
    utils/thread.c
    utils/wire.h
    utils/wire.c)

if(WIN32)
  list(APPEND NN_SOURCES utils/thread_win.h utils/thread_win.inc utils/win.h)
elseif(UNIX)
  list(APPEND NN_SOURCES utils/thread_posix.h utils/thread_posix.inc)
else()
  message(FATAL_ERROR "Assertion failed; this path is unreachable.")
endif()

foreach(f ${NN_SOURCES})
  if(IS_ABSOLUTE "${f}")
    file(RELATIVE_PATH f ${CMAKE_CURRENT_SOURCE_DIR} ${f})
  endif()
  set(SRC_GROUP "${f}")
  set(f "${CMAKE_CURRENT_SOURCE_DIR}/${f}")

  string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRC_GROUP ${SRC_GROUP})

  string(REPLACE / \\ SRC_GROUP ${SRC_GROUP})
  source_group("${SRC_GROUP}" FILES ${f})
endforeach()

if(NN_STATIC_LIB)
  add_library(${PROJECT_NAME} STATIC ${NN_SOURCES})
  target_compile_definitions(${PROJECT_NAME} PUBLIC NN_STATIC_LIB)
else()
  add_library(${PROJECT_NAME} SHARED ${NN_SOURCES})
  add_definitions(-DNN_SHARED_LIB)
  set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION "${NN_ABI_VERSION}"
                                                   VERSION "${NN_LIB_VERSION}")
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                                 ${PROJECT_BINARY_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                                 ${PROJECT_BINARY_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY
                                                 ${PROJECT_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} ${NN_REQUIRED_LIBRARIES})
if(THREADS_HAVE_PTHREAD_ARG)
  add_definitions(-pthread)
endif()

if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(${PROJECT_NAME} "${CMAKE_THREAD_LIBS_INIT}")
endif()

if(NN_REQUIRED_LIBRARIES)
  foreach(lib ${NN_REQUIRED_LIBRARIES})
    set(NN_REQUIRED_LIBRARIES "${NN_REQUIRED_LFLAGS} -l${lib}")
  endforeach()
endif()
configure_file(pkgconfig.in ${PROJECT_NAME}.pc @ONLY)
target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:include>)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}-target
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# include(CMakePackageConfigHelpers) set(PACKAGE_INSTALL_DESTINATION
# ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}-${NN_PACKAGE_VERSION})
# install(EXPORT ${PROJECT_NAME}-target DESTINATION
# ${PACKAGE_INSTALL_DESTINATION}) write_basic_package_version_file(
# ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake VERSION
# ${NN_PACKAGE_VERSION} COMPATIBILITY SameMajorVersion)
# configure_package_config_file(
# ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in
# ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake INSTALL_DESTINATION
# ${PACKAGE_INSTALL_DESTINATION} PATH_VARS CMAKE_INSTALL_PREFIX) install(FILES
# ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
# ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake DESTINATION
# ${PACKAGE_INSTALL_DESTINATION})
