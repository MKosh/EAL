cmake_minimum_required(VERSION 3.13)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(EAL VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
find_package(ROOT REQUIRED)

#find_package(nlohmann_json 3.10.5)

#find_package(Doxygen)
#if (DOXYGEN_FOUND)
#  set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/docs)
#  set(DOXYGEN_EXTRACT_PRIVATE YES)
#  set(DOXYGEN_RECURSIVE YES)
#  set(DOXYGEN_CLASS_DIAGRAMS NO)
#  set(DOXYGEN_HAVE_DOT NO)
#  set(DOXYGEN_PROJECT_NAME "Exploratory Analysis Library")
#  set(DOXYGEN_GENERATE_LATEX YES)
#  doxygen_add_docs(doxygen "src" "include" ALL)
#  message("Doxygen is set up")
#else()
#  message("Doxygen needs to be installed to generate the documention")
#endif()

include_directories(./external)
set(CMAKE_CONFIG_PARSER "json" CACHE STRING "Config file parser: json or yaml")

if ("${CMAKE_CONFIG_PARSER}" STREQUAL "json")
  message("Using nlohmann_json parser for config files")
elseif ("${CMAKE_CONFIG_PARSER}" STREQUAL "yaml")
  message("Using yaml-cpp parser for config files")
else()
  message("No config parser specified")
endif()

#find_library(XML_LIBRARY NAMES libXMLParser.so HINTS ${ROOTSYS}/lib)
find_library(TMVA_LIBRARY NAMES libTMVA.so HINTS /usr/lib/root/libTMVA.so)
find_library(TMVAGUI_LIBRARY NAMES libTMVAGui.so HINTS /usr/lib/root/libTMVAGui.so)

add_library(EAL SHARED src/EAL/Analysis.cc src/EAL/Analysis.h src/EAL/Training.h)
target_link_libraries(EAL PUBLIC ROOT::Core ROOT::Tree ROOT::ROOTDataFrame ${TMVA_LIBRARY} ${TMVAGUI_LIBRARY})
# set_target_properties(EAL PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

add_subdirectory(app)

# Compile command:
#   g++ test.cc $(root-config --glibs --cflags --libs) -o test --std=c++20

