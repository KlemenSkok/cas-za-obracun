
###########################################################################
#                                                                         #
# Find SDL2_image library                                                 #
#                                                                         #
###########################################################################


# SDL2 headers
find_path(SDL2_IMAGE_INCLUDE_DIR SDL_image.h
    HINTS ${SDL2_IMAGE_PATH}/include/SDL2
)

# SDL2 library
find_library(SDL2_IMAGE_LIBRARY
    NAMES SDL2_image
    HINTS ${SDL2_IMAGE_PATH}/lib
)



# If SDL2 is found, define the target for linking
if(SDL2_IMAGE_INCLUDE_DIR AND SDL2_IMAGE_LIBRARY)
set(SDL2_IMAGE_FOUND TRUE)
message(STATUS " -> SDL2_image found")

add_library(SDL2_IMAGE::SDL2_IMAGE INTERFACE IMPORTED)
set_target_properties(SDL2_IMAGE::SDL2_IMAGE PROPERTIES
INTERFACE_INCLUDE_DIRECTORIES "${SDL2_IMAGE_INCLUDE_DIR}"
INTERFACE_LINK_LIBRARIES "${SDL2_IMAGE_LIBRARY}"
)
else()
set(SDL2_IMAGE_FOUND FALSE)
message(WARNING " -> SDL2_image not found!")
endif()


# PREVERJANJE CE JE NAJDU VSAKO STVAR

#if(SDL2_IMAGE_LIBRARY)
#    message(STATUS "NAJDU SDL2_image lib: ${SDL2_IMAGE_LIBRARY}")
#endif()
#if(SDL2_IMAGE_INCLUDE_DIR)
#    message(STATUS "NAJDU SDL2_image include: ${SDL2_IMAGE_INCLUDE_DIR}")
#endif()