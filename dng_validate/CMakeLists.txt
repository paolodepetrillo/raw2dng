# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

FIND_PACKAGE( ZLIB )
INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} )
ADD_DEFINITIONS( ${ZLIB_DEFINITIONS} )

FIND_PACKAGE( JPEG )
INCLUDE_DIRECTORIES( ${JPEG_INCLUDE_DIR} )
ADD_DEFINITIONS( ${JPEG_DEFINITIONS} )

INCLUDE_DIRECTORIES( ${raw2dng_SOURCE_DIR}/libdng 
                     ${raw2dng_SOURCE_DIR}/libdng/dng-sdk/source 
                     ${CMAKE_CURRENT_BINARY_DIR} )

ADD_EXECUTABLE( dng_validate
                ${raw2dng_SOURCE_DIR}/libdng/dng-sdk/source/dng_validate.cpp )

TARGET_LINK_LIBRARIES( dng_validate dng ${ZLIB_LIBRARIES} ${LIBRAW_LIBRARIES} ${EXIV2_LIBRARIES} ${JPEG_LIBRARIES})
TARGET_COMPILE_OPTIONS( dng_validate PRIVATE -fexceptions -std=c++11 )

INSTALL(TARGETS dng_validate DESTINATION bin)
