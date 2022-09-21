cmake_minimum_required(VERSION 3.10)

# set the project name
project(MetaOpt VERSION 0.1)
option(CXX "enable C++ compilation" ON)
if(CXX)
    enable_language(CXX)
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

# turn of json build tests
set(JSON_BuildTests OFF CACHE INTERNAL "")

# ************************************
#       Metaheuristics
# ************************************

# add the executables
add_executable(cvrp_meta
        tools/cvrp_meta.cpp
        src/config.cpp
        src/generic/LS_optimizer.cpp
        src/generic/instance.cpp
        src/generic/solution.cpp
        src/problem/cvrp.cpp
        lib/tinyxml/tinyxml.cpp
        lib/tinyxml/tinyxmlerror.cpp
        lib/tinyxml/tinyxmlparser.cpp
        lib/tinyxml/tinystr.cpp
        )

add_executable(qap_meta
        tools/qap_meta.cpp
        src/config.cpp
        src/generic/LS_optimizer.cpp
        src/generic/instance.cpp
        src/generic/solution.cpp
        src/problem/qap.cpp
        )

add_executable(npfs_meta
        tools/npfs_meta.cpp
        src/config.cpp
        src/generic/LS_optimizer.cpp
        src/generic/instance.cpp
        src/generic/solution.cpp
        src/problem/npfs.cpp
        )

add_executable(sudoku_meta
        tools/sudoku_meta.cpp
        src/config.cpp
        src/generic/LS_optimizer.cpp
        src/generic/instance.cpp
        src/generic/solution.cpp
        src/problem/sudoku.cpp
        )

add_executable(scp_meta
        tools/scp_meta.cpp
        src/config.cpp
        src/generic/LS_optimizer.cpp
        src/generic/instance.cpp
        src/generic/solution.cpp
        src/problem/scp.cpp)

add_executable(wcp_meta
        tools/wcp_meta.cpp
        src/config.cpp
        src/generic/LS_optimizer.cpp
        src/generic/instance.cpp
        src/generic/solution.cpp
        src/problem/wcp.cpp)

target_include_directories(cvrp_meta PUBLIC
                           "${PROJECT_BINARY_DIR}/../"
                           "${PROJECT_BINARY_DIR}/../src"
                           "${PROJECT_BINARY_DIR}/../lib/nlohmann"
                           )

target_include_directories(qap_meta PUBLIC
        "${PROJECT_BINARY_DIR}/../"
        "${PROJECT_BINARY_DIR}/../src"
        "${PROJECT_BINARY_DIR}/../lib/nlohmann"
        )

target_include_directories(npfs_meta PUBLIC
        "${PROJECT_BINARY_DIR}/../"
        "${PROJECT_BINARY_DIR}/../src"
        "${PROJECT_BINARY_DIR}/../lib/nlohmann"
        )

target_include_directories(sudoku_meta PUBLIC
        "${PROJECT_BINARY_DIR}/../"
        "${PROJECT_BINARY_DIR}/../src"
        "${PROJECT_BINARY_DIR}/../lib/nlohmann"
        )

target_include_directories(scp_meta PUBLIC
        "${PROJECT_BINARY_DIR}/../"
        "${PROJECT_BINARY_DIR}/../src"
        "${PROJECT_BINARY_DIR}/../lib/nlohmann"
        )

target_include_directories(wcp_meta PUBLIC
        "${PROJECT_BINARY_DIR}/../"
        "${PROJECT_BINARY_DIR}/../src"
        "${PROJECT_BINARY_DIR}/../lib/nlohmann"
        )

## ************************************
##       ILP EXECS
## ************************************
#
#add_executable(cvrp_ilp
#        tools/cvrp_ilp.cpp
#        src/config.cpp
#        src/generic/LS_optimizer.cpp
#        src/generic/instance.cpp
#        src/generic/solution.cpp
#        src/problem/cvrp.cpp
#        lib/tinyxml/tinyxml.cpp
#        lib/tinyxml/tinyxmlerror.cpp
#        lib/tinyxml/tinyxmlparser.cpp
#        lib/tinyxml/tinystr.cpp
#        )
#
#add_executable(qap_ilp
#        tools/qap_ilp.cpp
#        src/config.cpp
#        src/generic/LS_optimizer.cpp
#        src/generic/instance.cpp
#        src/generic/solution.cpp
#        src/problem/qap.cpp
#        )
#
#add_executable(npfs_ilp
#        src/problem/qap.cpp
#        tools/npfs_ilp.cpp
#        src/config.cpp
#        src/generic/LS_optimizer.cpp
#        src/generic/instance.cpp
#        src/generic/solution.cpp
#        src/problem/npfs.cpp
#        )
#
#add_executable(sudoku_ilp
#        tools/sudoku_ilp.cpp
#        src/problem/sudoku.cpp
#        src/config.cpp
#        src/generic/instance.cpp
#        src/generic/solution.cpp
#        )
#
## gurobi
#find_package(GUROBI REQUIRED)
#target_link_libraries(cvrp_ilp optimized ${GUROBI_CXX_LIBRARY} debug ${GUROBI_CXX_DEBUG_LIBRARY})
#target_link_libraries(cvrp_ilp ${GUROBI_LIBRARY})
#target_link_libraries(qap_ilp optimized ${GUROBI_CXX_LIBRARY} debug ${GUROBI_CXX_DEBUG_LIBRARY})
#target_link_libraries(qap_ilp ${GUROBI_LIBRARY})
#target_link_libraries(npfs_ilp optimized ${GUROBI_CXX_LIBRARY} debug ${GUROBI_CXX_DEBUG_LIBRARY})
#target_link_libraries(npfs_ilp ${GUROBI_LIBRARY})
#target_link_libraries(sudoku_ilp optimized ${GUROBI_CXX_LIBRARY} debug ${GUROBI_CXX_DEBUG_LIBRARY})
#target_link_libraries(sudoku_ilp ${GUROBI_LIBRARY})
#
#message(STATUS "gurobi include dir: ${GUROBI_LIBRARY}")
#
#target_include_directories(cvrp_ilp PUBLIC
#        "${PROJECT_BINARY_DIR}/../"
#        "${PROJECT_BINARY_DIR}/../src"
#        "${GUROBI_INCLUDE_DIRS}"
#        )
#target_include_directories(qap_ilp PUBLIC
#        "${PROJECT_BINARY_DIR}/../"
#        "${PROJECT_BINARY_DIR}/../src"
#        "${GUROBI_INCLUDE_DIRS}"
#        )
#
#target_include_directories(npfs_ilp PUBLIC
#        "${PROJECT_BINARY_DIR}/../"
#        "${PROJECT_BINARY_DIR}/../src"
#        "${GUROBI_INCLUDE_DIRS}"
#        )
#
#target_include_directories(sudoku_ilp PUBLIC
#        "${PROJECT_BINARY_DIR}/../"
#        "${PROJECT_BINARY_DIR}/../src"
#        "${GUROBI_INCLUDE_DIRS}"
#        )
#
#if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
#    include(FeatureSummary)
#    feature_summary(WHAT ALL)
#endif()