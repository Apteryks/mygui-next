# Set OGRE-next includes and library

message(STATUS "Freetype:  Setting fixed paths, one up")

get_filename_component(DIR_ONE_ABOVE ../ ABSOLUTE)
message(STATUS ${DIR_ONE_ABOVE})

set(OGRE_DEPS ${DIR_ONE_ABOVE}/Ogre/ogre-next-deps)

set(FREETYPE_INCLUDE_DIR ${OGRE_DEPS}/src/freetype/include)
set(FREETYPE_FT2BUILD_INCLUDE_DIR ${OGRE_DEPS}/src/freetype/include)
set(FREETYPE_LIBRARIES ${OGRE_DEPS}/build/src/freetype/Release/freetype.lib)
set(FREETYPE_LIBRARY_DBG ${OGRE_DEPS}/build/src/freetype/Debug/freetype_d.lib)
#set(FREETYPE_LIBRARY_FWK 
set(FREETYPE_LIBRARY_REL ${OGRE_DEPS}/build/src/freetype/Release/freetype.lib)

message(STATUS "freetype   include: " ${FREETYPE_INCLUDE_DIR})
message(STATUS "freetype libraries: " ${FREETYPE_LIBRARY_REL})

SET(FREETYPE_FOUND TRUE)
