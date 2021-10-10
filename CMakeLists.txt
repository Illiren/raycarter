cmake_minimum_required(VERSION 2.8)

project(raycarter)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

find_package(OpenAL REQUIRED)

file(GLOB_RECURSE LibFiles
    "${SRC_DIR}/include/*.hpp")

file(GLOB_RECURSE SOURCES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/source/*.cpp")

add_custom_target(headers SOURCES ${LibFiles})
add_custom_target(sources SOURCES ${SrcFiles})

include_directories("${SRC_DIR}/include")

file(COPY resources/walltext.bmp DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY resources/monsters.bmp DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY resources/test.wav DESTINATION "${CMAKE_BINARY_DIR}")

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} omp openal)
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
