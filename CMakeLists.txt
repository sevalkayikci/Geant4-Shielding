#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 3.16...3.27)
project(B1)

#----------------------------------------------------------------------------
# Find Geant4 and ROOT
#
find_package(Geant4 REQUIRED ui_all vis_all)
find_package(ROOT    REQUIRED COMPONENTS Core Tree)

#----------------------------------------------------------------------------
# Locate sources and headers
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Build executable
#
add_executable(exampleB1 exampleB1.cc ${sources} ${headers})

target_compile_features(exampleB1 PRIVATE cxx_std_17)

target_include_directories(exampleB1 PRIVATE
  include
  ${ROOT_INCLUDE_DIRS}
)

target_link_libraries(exampleB1 PRIVATE
  ${Geant4_LIBRARIES}
  ROOT::Core
  ROOT::Tree
)

#----------------------------------------------------------------------------
# Copy macro scripts to build directory
#
set(EXAMPLEB1_SCRIPTS
  exampleB1.in
  exampleB1.out
  init_vis.mac
  run1.mac
  run2.mac
  vis.mac
)

foreach(_script ${EXAMPLEB1_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
  )
endforeach()
