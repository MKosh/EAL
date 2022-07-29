cmake_minimum_required(VERSION 3.13)

project(EAL VERSION 0.1.0 LANGUAGES CXX)

find_package(ROOT REQUIRED)
#find_package(nlohmann_json 3.10.5)

find_package(Doxygen)
if (DOXYGEN_FOUND)
  set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/docs)
  set(DOXYGEN_EXTRACT_PRIVATE YES)
  set(DOXYGEN_RECURSIVE YES)
  set(DOXYGEN_CLASS_DIAGRAMS NO)
  set(DOXYGEN_HAVE_DOT NO)
  set(DOXYGEN_PROJECT_NAME "Exploratory Analysis Library")
  set(DOXYGEN_GENERATE_LATEX YES)
  doxygen_add_docs(doxygen "src" "include" ALL)
  message("Doxygen is set up")
else()
  message("Doxygen needs to be installed to generate the documention")
endif()

find_library(XML_LIBRARY NAMES libXMLParser.so HINTS ${ROOTSYS}/lib)

add_library(EAL SHARED src/Sample.cc src/Sample.h src/Analysis.h src/Analysis.cc src/Process.h src/Process.cc src/Training.h)
target_link_libraries(EAL PUBLIC ROOT::Core ROOT::Tree ROOT::ROOTDataFrame ${XML_LIBRARY})

add_subdirectory(app)

# Compile command:
#   g++ test.cc $(root-config --glibs --cflags --libs) -o test --std=c++20

