cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME "Cas_za_obracun")

project(${PROJECT_NAME} VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#################################################################################################
#                                                                                               #
# ################################## - LIBRARY PATHS - ######################################## #
#                                                                                               #
#################################################################################################
#                                                                                               #
## (enter the folder containing the include, bin and lib subfolders)                            #
set(SDL2_PATH "C:/ProgLangs/cpp_libs/SDL2-2.28.5/x86_64-w64-mingw32")                           #
set(SDL2_IMAGE_PATH "C:/ProgLangs/cpp_libs/SDL2_image-2.6.3/x86_64-w64-mingw32")                #
set(SDL2_TTF_PATH "C:/ProgLangs/cpp_libs/SDL2_ttf-2.20.0/x86_64-w64-mingw32")                   #
set(SDL2_NET_PATH "C:/ProgLangs/cpp_libs/SDL2_net-2.2.0/x86_64-w64-mingw32")                    #
set(INTERNAL_LIBS_PATH "libs")                                                                  #
#                                                                                               #
#################################################################################################





set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/include)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

set(DEBUG_OUTPUT_DIR ${PROJECT_BINARY_DIR}/debug)
set(RELEASE_OUTPUT_DIR ${PROJECT_BINARY_DIR}/release)

message(STATUS "")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")

message(STATUS "")
message(STATUS "Searching for packages...")


# find SDL2
find_package(SDL2 REQUIRED)
# find SDL2_image
find_package(SDL2_image REQUIRED)
# find SDL2_ttf
find_package(SDL2_ttf REQUIRED)
# find SDL2_net
find_package(SDL2_net REQUIRED)
# find spdlog
find_package(spdlog REQUIRED)
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

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_INCLUDE_PATH}
    # includes without the need of the full path
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIR}
    ${SDL2_TTF_INCLUDE_DIR}
    ${SDL2_NET_INCLUDE_DIR}
    ${SPDLOG_INCLUDE_DIR}
    # includes that end with SDL2/...
    ${SDL2_INCLUDE_DIR}/..
    ${SDL2_IMAGE_INCLUDE_DIR}/..
    ${SDL2_TTF_INCLUDE_DIR}/..
    ${SDL2_NET_INCLUDE_DIR}/..
)

#message(STATUS "SDL2 include: " ${SDL2_INCLUDE_DIR})
#message(STATUS "SDL2_image include: " ${SDL2_IMAGE_INCLUDE_DIR})
#message(STATUS "SDL2_ttf include: " ${SDL2_TTF_INCLUDE_DIR})
#message(STATUS "SDL2_net include: " ${SDL2_NET_INCLUDE_DIR})


# LINK PROGRAMS
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    mingw32
    SDL2::SDL2main
    SDL2::SDL2
    SDL2_IMAGE::SDL2_IMAGE
    SDL2_TTF::SDL2_TTF
    SDL2_NET::SDL2_NET
    #spdlog::spdlog
)
# !!! LINKING ORDER:
# - SDL2main mora bit pred vsemi ostalimi SDL2 zadevami

message(STATUS "Copying all dependencies to the output directory...")
file(GLOB DLL_DEPENDENCIES ${SDL2_PATH}/bin/*.dll ${SDL2_IMAGE_PATH}/bin/*.dll ${SDL2_TTF_PATH}/bin/*.dll ${SDL2_NET_PATH}/bin/*.dll ${INTERNAL_LIBS_PATH}/lib/*.dll ${INTERNAL_LIBS_PATH}/lib/*.lib)
foreach(DLL_FILE IN LISTS DLL_DEPENDENCIES)
    message(STATUS " -> ${DLL_FILE}")
    file(
        COPY ${DLL_FILE} 
        DESTINATION ${DEST_DIR}
    )
endforeach()

message(STATUS "")