# Set OGRE-next includes and library

message(STATUS "Ogre3:  Setting fixed paths, one up")

get_filename_component(DIR_ONE_ABOVE ../ ABSOLUTE)
message(STATUS ${DIR_ONE_ABOVE})

set(OGRE_NEXT ${DIR_ONE_ABOVE}/Ogre/ogre-next)
#set(OGRE_NEXT "../Ogre/ogre-next")

set(OGRE_CONFIG_INCLUDE_DIR ${OGRE_NEXT}/build/${CMAKE_BUILD_TYPE}/include)
set(OGRE_INCLUDE_DIR ${OGRE_NEXT}/OgreMain/include)
set(OGRE_LIBRARIES ${OGRE_NEXT}/build/${CMAKE_BUILD_TYPE}/lib/libOgreMain.so.3.0)
set(OGRE_LIB_DIR ${OGRE_NEXT}/build/${CMAKE_BUILD_TYPE}/lib)

message(STATUS "Ogre3    config: " ${OGRE_CONFIG_INCLUDE_DIR})
message(STATUS "Ogre3   include: " ${OGRE_INCLUDE_DIR})
#message(STATUS "Ogre3    source: " ${OGRE_SOURCE_DIR})
message(STATUS "Ogre3 libraries: " ${OGRE_LIBRARIES})
message(STATUS "Ogre3   lib dir: " ${OGRE_LIB_DIR})

SET(OGRE_FOUND TRUE)
