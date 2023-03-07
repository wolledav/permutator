cmake_minimum_required(VERSION 3.10)

# set the project name
project(MetaOpt VERSION 0.1)
option(CXX "enable C++ compilation" ON)
if(CXX)
    enable_language(CXX)
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(GCC_COVERAGE_COMPILE_FLAGS "-fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

# turn of json build tests
set(JSON_BuildTests OFF CACHE INTERNAL "")

# ************************************
#       Metaheuristics
# ************************************

add_executable(ROADEF_meta
        src/problem/ROADEF/ROADEF_meta.cpp
        src/problem/ROADEF/ROADEF.cpp
        src/problem/ROADEF/code/hashing.cpp
        src/problem/ROADEF/code/instance.cpp
        src/problem/ROADEF/code/objective.cpp
        src/problem/ROADEF/code/params.cpp
        src/problem/ROADEF/code/parser.cpp
        src/problem/ROADEF/code/solution.cpp
        src/problem/ROADEF/code/util.cpp
        src/config.cpp
        src/generic/EA.cpp
        src/generic/optimizer.cpp
        src/generic/instance.cpp
        src/generic/solution.cpp
        src/generic/operator.cpp
        src/problem/EVRP/EVRP.cpp
        lib/getopt/getopt.c
        )

target_include_directories(ROADEF_meta PUBLIC
        "${PROJECT_BINARY_DIR}/../"
        "${PROJECT_BINARY_DIR}/../src"
        "${PROJECT_BINARY_DIR}/../lib/nlohmann"
        )

add_executable(CVRP_meta
        src/problem/CVRP/CVRP_meta.cpp
        src/config.cpp
        src/generic/EA.cpp
        src/generic/optimizer.cpp
        src/generic/instance.cpp
        src/generic/solution.cpp
        src/generic/operator.cpp
        src/problem/CVRP/CVRP.cpp
        lib/tinyxml/tinyxml.cpp
        lib/tinyxml/tinyxmlerror.cpp
        lib/tinyxml/tinyxmlparser.cpp
        lib/tinyxml/tinystr.cpp
        lib/getopt/getopt.c
        )

target_include_directories(CVRP_meta PUBLIC
        "${PROJECT_BINARY_DIR}/../"
        "${PROJECT_BINARY_DIR}/../src"
        "${PROJECT_BINARY_DIR}/../lib/nlohmann"
        )



add_executable(EVRP_meta
        src/problem/EVRP/EVRP_meta.cpp
        src/config.cpp
        src/generic/EA.cpp
        src/generic/optimizer.cpp
        src/generic/instance.cpp
        src/generic/solution.cpp
        src/generic/operator.cpp
        src/problem/EVRP/EVRP.cpp
        lib/getopt/getopt.c
        )

target_include_directories(EVRP_meta PUBLIC
        "${PROJECT_BINARY_DIR}/../"
        "${PROJECT_BINARY_DIR}/../src"
        "${PROJECT_BINARY_DIR}/../lib/nlohmann"
        )



add_executable(QAP_meta
        src/problem/QAP/QAP_meta.cpp
        src/config.cpp
        src/generic/EA.cpp
        src/generic/optimizer.cpp
        src/generic/instance.cpp
        src/generic/solution.cpp
        src/generic/operator.cpp
        src/problem/QAP/QAP.cpp
        lib/getopt/getopt.c
        )

target_include_directories(QAP_meta PUBLIC
        "${PROJECT_BINARY_DIR}/../"
        "${PROJECT_BINARY_DIR}/../src"
        "${PROJECT_BINARY_DIR}/../lib/nlohmann"
        )



add_executable(NPFS_meta
        src/problem/NPFS/NPFS_meta.cpp
        src/config.cpp
        src/generic/EA.cpp
        src/generic/optimizer.cpp
        src/generic/instance.cpp
        src/generic/solution.cpp
        src/generic/operator.cpp
        src/problem/NPFS/NPFS.cpp
        lib/getopt/getopt.c
        )

target_include_directories(NPFS_meta PUBLIC
        "${PROJECT_BINARY_DIR}/../"
        "${PROJECT_BINARY_DIR}/../src"
        "${PROJECT_BINARY_DIR}/../lib/nlohmann"
        )


add_executable(TSPSD_meta
        src/problem/TSPSD/TSPSD_meta.cpp
        src/config.cpp
        src/generic/EA.cpp
        src/generic/optimizer.cpp
        src/generic/instance.cpp
        src/generic/solution.cpp
        src/generic/operator.cpp
        src/problem/TSPSD/TSPSD.cpp
        lib/getopt/getopt.c
        )

target_include_directories(TSPSD_meta PUBLIC
        "${PROJECT_BINARY_DIR}/../"
        "${PROJECT_BINARY_DIR}/../src"
        "${PROJECT_BINARY_DIR}/../lib/nlohmann"
        )


add_executable(wTSPSD_meta
        src/problem/wTSPSD/wTSPSD_meta.cpp
        src/config.cpp
        src/generic/EA.cpp
        src/generic/optimizer.cpp
        src/generic/instance.cpp
        src/generic/solution.cpp
        src/generic/operator.cpp
        src/problem/wTSPSD/wTSPSD.cpp
        lib/getopt/getopt.c
        )

target_include_directories(wTSPSD_meta PUBLIC
        "${PROJECT_BINARY_DIR}/../"
        "${PROJECT_BINARY_DIR}/../src"
        "${PROJECT_BINARY_DIR}/../lib/nlohmann"
        )



# add_executable(SUDOKU_meta
#         src/problem/SUDOKU/SUDOKU_meta.cpp
#         src/config.cpp
#         src/generic/EA.cpp
#         src/generic/optimizer.cpp
#         src/generic/instance.cpp
#         src/generic/solution.cpp
#         src/generic/operator.cpp
#         src/problem/SUDOKU/SUDOKU.cpp
#         lib/getopt/getopt.c
#         )


# target_include_directories(SUDOKU_meta PUBLIC
#         "${PROJECT_BINARY_DIR}/../"
#         "${PROJECT_BINARY_DIR}/../src"
#         "${PROJECT_BINARY_DIR}/../lib/nlohmann"
#         )


## ************************************
##       ILP EXECS
## ************************************
#
#add_executable(CVRP_ilp
#        tools/CVRP_ilp.cpp
#        src/config.cpp
#        src/generic/LS_optimizer.cpp
#        src/generic/instance.cpp
#        src/generic/solution.cpp
#        src/problem/CVRP.cpp
#        lib/tinyxml/tinyxml.cpp
#        lib/tinyxml/tinyxmlerror.cpp
#        lib/tinyxml/tinyxmlparser.cpp
#        lib/tinyxml/tinystr.cpp
#        )
#
#add_executable(QAP_ilp
#        tools/QAP_ilp.cpp
#        src/config.cpp
#        src/generic/LS_optimizer.cpp
#        src/generic/instance.cpp
#        src/generic/solution.cpp
#        src/problem/QAP.cpp
#        )
#
#add_executable(NPFS_ilp
#        src/problem/QAP.cpp
#        tools/NPFS_ilp.cpp
#        src/config.cpp
#        src/generic/LS_optimizer.cpp
#        src/generic/instance.cpp
#        src/generic/solution.cpp
#        src/problem/NPFS.cpp
#        )
#
#add_executable(SUDOKU_ilp
#        tools/SUDOKU_ilp.cpp
#        src/problem/SUDOKU.cpp
#        src/config.cpp
#        src/generic/instance.cpp
#        src/generic/solution.cpp
#        )
#
## gurobi
#find_package(GUROBI REQUIRED)
#target_link_libraries(CVRP_ilp optimized ${GUROBI_CXX_LIBRARY} debug ${GUROBI_CXX_DEBUG_LIBRARY})
#target_link_libraries(CVRP_ilp ${GUROBI_LIBRARY})
#target_link_libraries(QAP_ilp optimized ${GUROBI_CXX_LIBRARY} debug ${GUROBI_CXX_DEBUG_LIBRARY})
#target_link_libraries(QAP_ilp ${GUROBI_LIBRARY})
#target_link_libraries(NPFS_ilp optimized ${GUROBI_CXX_LIBRARY} debug ${GUROBI_CXX_DEBUG_LIBRARY})
#target_link_libraries(NPFS_ilp ${GUROBI_LIBRARY})
#target_link_libraries(SUDOKU_ilp optimized ${GUROBI_CXX_LIBRARY} debug ${GUROBI_CXX_DEBUG_LIBRARY})
#target_link_libraries(SUDOKU_ilp ${GUROBI_LIBRARY})
#
#message(STATUS "gurobi include dir: ${GUROBI_LIBRARY}")
#
#target_include_directories(CVRP_ilp PUBLIC
#        "${PROJECT_BINARY_DIR}/../"
#        "${PROJECT_BINARY_DIR}/../src"
#        "${GUROBI_INCLUDE_DIRS}"
#        )
#target_include_directories(QAP_ilp PUBLIC
#        "${PROJECT_BINARY_DIR}/../"
#        "${PROJECT_BINARY_DIR}/../src"
#        "${GUROBI_INCLUDE_DIRS}"
#        )
#
#target_include_directories(NPFS_ilp PUBLIC
#        "${PROJECT_BINARY_DIR}/../"
#        "${PROJECT_BINARY_DIR}/../src"
#        "${GUROBI_INCLUDE_DIRS}"
#        )
#
#target_include_directories(SUDOKU_ilp PUBLIC
#        "${PROJECT_BINARY_DIR}/../"
#        "${PROJECT_BINARY_DIR}/../src"
#        "${GUROBI_INCLUDE_DIRS}"
#        )
#
#if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
#    include(FeatureSummary)
#    feature_summary(WHAT ALL)
#endif()