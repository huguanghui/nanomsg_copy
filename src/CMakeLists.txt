set(NN_SOURCES
    nn.h
    nn.c
    utils/alloc.h
    utils/alloc.c
    utils/err.h
    utils/err.c
    utils/fast.h
    utils/mutex.h
    utils/mutex.c)

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
