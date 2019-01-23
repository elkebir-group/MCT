cmake_minimum_required( VERSION 2.8 )

project( mCTP )

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

option(CPLEX "Use CPLEX" OFF)

if (CPLEX)
  ADD_DEFINITIONS(-DCPLEX -DIL_STD)
  if( NOT CPLEX_INC_DIR OR NOT CPLEX_LIB_DIR OR NOT CONCERT_INC_DIR OR NOT CONCERT_LIB_DIR)
    set( CPLEX_AUTODETECTION_PREFIXES "$ENV{HOME}/ILOG;/opt/ibm/ILOG;$ENV{HOME}/Applications/IBM/ILOG" CACHE STRING "CPLEX automatic lookup directories" )
    foreach( CplexInstallPrefix ${CPLEX_AUTODETECTION_PREFIXES} )
      file( GLOB_RECURSE CplexLibraryPaths ${CplexInstallPrefix}/*/cplex/lib/*/libcplex.a )
      if( CplexLibraryPaths )
        list( LENGTH CplexLibraryPaths NumPath )
        if( NumPath GREATER 1 )
          message( WARNING "Found multiple CPLEX installations in ${CplexInstallPrefix}, automatically selecting the first one, please set {CPLEX|CONCERT}_{INC|LIB}_DIR manually if this is not the right selection" )
        else()
          message( "Found CPLEX's installation in ${CPlexInstallPrefix}" )
        endif()
        list( GET CplexLibraryPaths 0 CplexLibraryPath )

        set( MyRegex "${CplexInstallPrefix}/(.*)/cplex/?/lib/(.*)/libcplex.a" )
        string( REGEX MATCH "${MyRegex}" ValidatedCplexLibPath ${CplexLibraryPath} )
        if( NOT ValidatedCplexLibPath )
          message( WARNING "Unable to recognize ${CplexLibraryPath} as a valid CPLEX path" )
        else()
          string( REGEX REPLACE "${MyRegex}" "\\1" CplexHome ${ValidatedCplexLibPath} )
          string( REGEX REPLACE "${MyRegex}" "\\2" CplexArch ${ValidatedCplexLibPath} )

          set( CPLEX_INC_DIR   "${CplexInstallPrefix}/${CplexHome}/cplex/include"            CACHE PATH "CPLEX's include dir" )
          set( CPLEX_LIB_DIR   "${CplexInstallPrefix}/${CplexHome}/cplex/lib/${CplexArch}"   CACHE PATH "CPLEX's library dir" )
          set( CONCERT_INC_DIR "${CplexInstallPrefix}/${CplexHome}/concert/include"          CACHE PATH "Concert's include dir" )
          set( CONCERT_LIB_DIR "${CplexInstallPrefix}/${CplexHome}/concert/lib/${CplexArch}" CACHE PATH "Concert's library dir" )

          break()
        endif()
      else()
        message( "Unable to find CPLEX and Concert installation in ${CplexInstallPrefix}" )
      endif()
    endforeach()

    if( NOT CPLEX_INC_DIR OR NOT CPLEX_LIB_DIR OR NOT CONCERT_INC_DIR OR NOT CONCERT_LIB_DIR )
      message( "Unable to autodeted CPLEX's and Concert's installation directories, please set {CPLEX|CONCERT}_{INC|LIB}_DIR manually" )
    else()
      message( "Autodetected CPLEX's include dir   : ${CPLEX_INC_DIR}" )
      message( "Autodetected CPLEX's library dir   : ${CPLEX_LIB_DIR}" )
      message( "Autodetected Concert's include dir : ${CONCERT_INC_DIR}" )
      message( "Autodetected Concert's library dir : ${CONCERT_LIB_DIR}" )
    endif()
  endif()
endif()

set (visualize_src
  src/utils.cpp
  src/mainvisualize.cpp
  src/clonetree.cpp
  src/basetree.cpp
  src/parentchildgraph.cpp
  src/visualize.cpp
)

set (visualize_hdr
  src/utils.h
  src/clonetree.h
  src/basetree.h
  src/parentchildgraph.h
  src/visualize.h
)

set (mctpca_src
  src/mainca.cpp
  src/utils.cpp
  src/clonetree.cpp
  src/basetree.cpp
  src/parentchildgraph.cpp
  src/mctsolver.cpp
  src/mctsolverca.cpp
)

set (mctpca_hdr
  src/utils.h
  src/clonetree.h
  src/basetree.h
  src/parentchildgraph.h
  src/mctsolverca.h
  src/mctsolver.h
)

set (mctpbf_src
  src/mainbf.cpp
  src/utils.cpp
  src/clonetree.cpp
  src/basetree.cpp
  src/parentchildgraph.cpp
  src/mctsolver.cpp
  src/mctsolverbf.cpp
)

set (mctpbf_hdr
  src/utils.h
  src/clonetree.h
  src/basetree.h
  src/parentchildgraph.h
  src/mctsolverbf.h
  src/mctsolver.h
)

if (CPLEX)
  set (mctpilp_src
    src/mainilp.cpp
    src/utils.cpp
    src/clonetree.cpp
    src/basetree.cpp
    src/parentchildgraph.cpp
    src/mctsolver.cpp
    src/mctsolverilp.cpp
    src/mctsolverilpcallback.cpp
    src/mctsolverilpcallbackuser.cpp
    src/mctsolverilpcallbacklazy.cpp
  )

  set (mctpilp_hdr
    src/utils.h
    src/clonetree.h
    src/basetree.h
    src/parentchildgraph.h
    src/mctsolverilp.h
    src/mctsolverilpcallback.h
    src/mctsolverilpcallbackuser.h
    src/mctsolverilpcallbacklazy.h
    src/mctsolver.h
  )
endif()

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" OFF)

if (BUILD_DOC)
  if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
    # check if Doxygen is installed
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        # note the option ALL which allows to build the docs together with the application
        add_custom_target( doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
    else (DOXYGEN_FOUND)
      message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
  endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: Debug Release Profile" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set( Boost_USE_STATIC_LIBS ON )
find_package( Boost REQUIRED )
MESSAGE( STATUS "found boost:"
"\nINCLUDE: ${Boost_INCLUDE_DIRS}"
"\nLIB: ${Boost_LIBRARIES}"
)

set( LIBLEMON_ROOT "$ENV{HOME}/lemon" CACHE PATH "Additional search directory for lemon library" )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-unused-local-typedef")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wextra -Wno-long-long -Wno-unused-parameter -Wno-unknown-pragmas -g -ggdb")

include_directories( ${CPLEX_INC_DIR} ${CONCERT_INC_DIR} "${LIBLEMON_ROOT}/include" "src" ${Boost_INCLUDE_DIRS} )
link_directories( ${CPLEX_LIB_DIR} ${CONCERT_LIB_DIR} "${LIBLEMON_ROOT}/lib" )

set( CommonLibs
  m
  pthread
  emon
  dl
  ${Boost_LIBRARIES}
)

add_executable( mCTPCA ${mctpca_src} ${mctpca_hdr} )
target_link_libraries( mCTPCA ${CommonLibs} )

add_executable( mCTPBF ${mctpbf_src} ${mctpbf_hdr} )
target_link_libraries( mCTPBF ${CommonLibs} )

add_executable( visualize ${visualize_src} ${visualize_hdr} )
target_link_libraries( visualize ${CommonLibs} )

if (CPLEX)
  add_executable( mCTPILP ${mctpilp_src} ${mctpilp_hdr} )
  target_include_directories( mCTPILP PUBLIC "${LIBLEMON_ROOT}/include" "src" ${Boost_INCLUDE_DIRS} ${CPLEX_INC_DIR} ${CONCERT_INC_DIR} )
  target_link_libraries( mCTPILP ${CommonLibs} ilocplex cplex concert dl )
endif()
