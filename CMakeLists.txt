cmake_minimum_required(VERSION 3.16)
project(AntineutrinoDetector)

# ── Find Geant4 (with UI and VIS optional) ──────────────────────────────────
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# ── Sources ──────────────────────────────────────────────────────────────────
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hh)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(antinu_sim antinu_sim.cc ${SOURCES})
target_link_libraries(antinu_sim ${Geant4_LIBRARIES})

# ── Copy macros to build dir ─────────────────────────────────────────────────
configure_file(run.mac ${CMAKE_CURRENT_BINARY_DIR}/run.mac COPYONLY)
configure_file(vis.mac ${CMAKE_CURRENT_BINARY_DIR}/vis.mac COPYONLY)
