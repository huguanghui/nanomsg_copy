set(NN_SOURCES nn.h nn.c)

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
