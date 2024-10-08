
###########################################################################
#                                                                         #
# Find SDL2_ttf library                                                   #
#                                                                         #
###########################################################################


# SDL2_ttf headers
find_path(SDL2_TTF_INCLUDE_DIR SDL_ttf.h
    HINTS ${SDL2_TTF_PATH}/include/SDL2
)

# SDL2_ttf library
find_library(SDL2_TTF_LIBRARY
    NAMES SDL2_ttf
    HINTS ${SDL2_TTF_PATH}/lib
)

# If SDL2_ttf is found, define the target for linking
if(SDL2_TTF_INCLUDE_DIR AND SDL2_TTF_LIBRARY)
set(SDL2_TTF_FOUND TRUE)
message(STATUS " -> SDL2_ttf found")

add_library(SDL2_TTF::SDL2_TTF INTERFACE IMPORTED)
set_target_properties(SDL2_TTF::SDL2_TTF PROPERTIES
INTERFACE_INCLUDE_DIRECTORIES "${SDL2_TTF_INCLUDE_DIR}"
INTERFACE_LINK_LIBRARIES "${SDL2_TTF_LIBRARY}"
)
else()
set(SDL2_TTF_FOUND FALSE)
message(WARNING " -> SDL2_ttf not found!")
endif()