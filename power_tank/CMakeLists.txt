cmake_minimum_required(VERSION 2.8)

project(power_tank)

include(FindFreetype)
include_directories(${FREETYPE_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES "*.cpp" "*.h" "*.hpp")
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    -pthread
    -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
    -lfreetype
    -lGL)
