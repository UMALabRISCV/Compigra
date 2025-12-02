add_compigra_library(MLIRCompigraKernelSchedule
  ModuloScheduleAdapter.cpp
  TemporalCGRAScheduler.cpp
  BasicBlockILPModel.cpp
  BasicBlockOpAssignment.cpp

  LINK_LIBS PUBLIC
  MLIRIR
)

# Enable exceptions for BasicBlockILPModel.cpp since Gurobi uses exceptions
set_source_files_properties(BasicBlockILPModel.cpp PROPERTIES COMPILE_FLAGS "-fexceptions")

if(GUROBI_INSTALL_DIR)
  message(STATUS "GUROBI_INSTALL_DIR is set via CMake parameter: ${GUROBI_INSTALL_DIR}")
  add_definitions(-DHAVE_GUROBI)
  include_directories(${GUROBI_INSTALL_DIR}/include)
  include_directories(${GUROBI_INSTALL_DIR}/src/cpp)
  if(APPLE)
    target_link_libraries(MLIRCompigraASMGen PUBLIC
      ${GUROBI_INSTALL_DIR}/lib/libgurobi_c++.a
      ${GUROBI_INSTALL_DIR}/lib/libgurobi110.dylib)
  else()
    target_link_libraries(MLIRCompigraASMGen PUBLIC
      ${GUROBI_INSTALL_DIR}/lib/libgurobi_c++.a
      ${GUROBI_INSTALL_DIR}/lib/libgurobi110.so)
  endif()
  message(STATUS "Including Gurobi include directory: ${GUROBI_INSTALL_DIR}/include")
  message(STATUS "Linking Gurobi library: ${GUROBI_INSTALL_DIR}/lib")
else()
  message(WARNING "GUROBI_INSTALL_DIR is not set. Gurobi will not be included.")
endif()
