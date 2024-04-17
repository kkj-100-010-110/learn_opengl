# To learn more about CMake
# https://cmake.org/
# https://cmake.org/getting-started/ 

cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME LearnOpenGL)
set(CMAKE_CXX_STANDARD 17)
if (APPLE)
    set(CMAKE_CXX_FLAGS "-framework Cocoa -framework IOKit -framework OpenGL")
endif()

set(WINDOW_NAME "LearnOpenGL")
set(WINDOW_WIDTH 800)
set(WINDOW_HEIGHT 600)

project(${PROJECT_NAME})
add_executable(${PROJECT_NAME}
    src/main.cpp
    # src/main_01.cpp
    src/shader.h src/shader.cpp
    )

include(Dependency.cmake)

target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})

target_compile_definitions(${PROJECT_NAME} PUBLIC
    WINDOW_NAME="${WINDOW_NAME}"
    WINDOW_WIDTH=${WINDOW_WIDTH}
    WINDOW_HEIGHT=${WINDOW_HEIGHT}
    )

add_dependencies(${PROJECT_NAME} ${DEP_LIST})

#
# configure : cmake -Bbuild . -DCMAKE_BUILD_TYPE=[Debug|Release]
#
# build : cmake --build build --config Debug
#