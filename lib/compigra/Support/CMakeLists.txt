add_compigra_library(MLIRCompigraSupport
  Utils.cpp
  OpenEdgeASM.cpp
  InterferenceGraphCreation.cpp
  PolyhedraScheduler.cpp

  LINK_LIBS PUBLIC
  MLIRIR
)

if(Z3_INSTALL_DIR)
  message(STATUS "Z3_INSTALL_DIR is set via CMake parameter: ${Z3_INSTALL_DIR}")
  add_definitions(-DHAVE_Z3)

  include_directories(${Z3_INSTALL_DIR}/include)
  target_link_libraries(MLIRCompigraSupport PUBLIC ${Z3_INSTALL_DIR}/lib/libz3.so)

  message(STATUS "Including Z3 include directory: ${Z3_INSTALL_DIR}/include")
  message(STATUS "Linking Z3 library from: ${Z3_INSTALL_DIR}/lib")
else()
  message(WARNING "Z3 INSTALL DIR is not set. SMT will not be included.")
endif()