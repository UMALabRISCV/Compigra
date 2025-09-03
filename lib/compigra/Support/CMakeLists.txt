add_compigra_library(MLIRCompigraSupport
  Utils.cpp
  OpenEdgeASM.cpp
  InterferenceGraphCreation.cpp
  PolyhedraScheduler.cpp

  LINK_LIBS PUBLIC
  MLIRIR
  )