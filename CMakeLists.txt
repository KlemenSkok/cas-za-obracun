cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME "cas-za-obracun")

project(${PROJECT_NAME} VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/include)

set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
set(DEBUG_OUTPUT_DIR ${PROJECT_BINARY_DIR}/debug)
set(RELEASE_OUTPUT_DIR ${PROJECT_BINARY_DIR}/release)


# find SDL2
find_package(SDL2 CONFIG REQUIRED)
# find SDL2_image
find_package(SDL2_image CONFIG REQUIRED)
# find SDL2_ttf
find_package(SDL2_ttf CONFIG REQUIRED)
# find SDL2_net
find_package(SDL2_net CONFIG REQUIRED)
# find spdlog
find_package(spdlog CONFIG REQUIRED)
message(STATUS "")

# Files that should be compiled
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "./main.cpp" "src/*.cpp" "src/*.hpp" "src/*.h")

message(STATUS "Adding source files...")
foreach(SRC_FILE IN LISTS SRC)
    message(STATUS " -> ${SRC_FILE}")
    endforeach()
    message(STATUS "")


# Add project executable
add_executable(${PROJECT_NAME} main.cpp ${SRC})


# Save it to:
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(DEST_DIR ${RELEASE_OUTPUT_DIR})
else()
    set(DEST_DIR ${DEBUG_OUTPUT_DIR})
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${DEST_DIR}
)


# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_INCLUDE_PATH}
)


# LINK PROGRAMS
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    mingw32
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
    $<IF:$<TARGET_EXISTS:SDL2_net::SDL2_net>,SDL2_net::SDL2_net,SDL2_net::SDL2_net-static>
    spdlog::spdlog
)