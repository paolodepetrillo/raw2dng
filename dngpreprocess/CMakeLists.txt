# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

FIND_PACKAGE( ZLIB )
INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} )
ADD_DEFINITIONS( ${ZLIB_DEFINITIONS} )

INCLUDE_DIRECTORIES( ${raw2dng_SOURCE_DIR}/libdng 
                     ${raw2dng_SOURCE_DIR}/libdng/dng-sdk/source 
                     ${CMAKE_CURRENT_BINARY_DIR} )

ADD_EXECUTABLE( dngpreprocess
                ${CMAKE_CURRENT_SOURCE_DIR}/dngpreprocess.cpp )

TARGET_LINK_LIBRARIES( dngpreprocess dng ${ZLIB_LIBRARIES} )
TARGET_COMPILE_OPTIONS( dngpreprocess PRIVATE -fexceptions -std=c++11 )

INSTALL(TARGETS dngpreprocess DESTINATION bin)

