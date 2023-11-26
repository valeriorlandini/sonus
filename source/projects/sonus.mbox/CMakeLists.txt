
# Copyright 2018 The Min-DevKit Authors. All rights reserved.
# Use of this source code is governed by the MIT License found in the License.md file.

cmake_minimum_required(VERSION 3.5)

set(C74_MIN_API_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../min-api)
set(SOUTEL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../soutel/include/soutel")
include(${C74_MIN_API_DIR}/script/min-pretarget.cmake)


#############################################################
# MAX EXTERNAL
#############################################################


include_directories( 
	"${C74_INCLUDES}"
	"${SOUTEL_INCLUDE_DIR}"
)


set( SOURCE_FILES
	${PROJECT_NAME}.cpp
)


add_library( 
	${PROJECT_NAME} 
	MODULE
	${SOURCE_FILES}
)

if (APPLE)
	find_library(COCOA_LIBRARY "Cocoa")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -x objective-c++")
	target_link_libraries(${PROJECT_NAME} PUBLIC ${COCOA_LIBRARY})
endif()


include(${C74_MIN_API_DIR}/script/min-posttarget.cmake)


#############################################################
# UNIT TEST
#############################################################

include(${C74_MIN_API_DIR}/test/min-object-unittest.cmake)
