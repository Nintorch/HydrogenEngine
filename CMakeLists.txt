cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(PaletteTest VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(
        ${PROJECT_SOURCE_DIR}/include/game
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src

        ${PROJECT_SOURCE_DIR}/libs/SDL2/include
        ${PROJECT_SOURCE_DIR}/libs/SDL2_gfx/include
        ${PROJECT_SOURCE_DIR}/libs/SDL2_image
)

file(GLOB all_SRCS
        "${PROJECT_SOURCE_DIR}/src/game/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/game/*.c"
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.c"
        "${PROJECT_SOURCE_DIR}/libs/SDL2_gfx/*.c"
        )

add_executable(${PROJECT_NAME} ${all_SRCS})

# Libraries
target_link_libraries(${PROJECT_NAME}
       libSDL2.a
       libSDL2_image.a
       winmm.lib
       setupapi.lib
       imm32.lib
       version.lib
       )