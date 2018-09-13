PROJECT(IntelTBB_Test)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# support C++11
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# support C++14, when gcc version > 5.1, use -std=c++14 instead of c++1y
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")

MESSAGE(STATUS "cmake current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
SET(PATH_TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../demo/IntelTBB_Test)
SET(PATH_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/./../../src/Intel_TBB)
MESSAGE(STATUS "path test files: ${PATH_TEST_FILES}")

SET(PATH_OPENCV /opt/opencv3.4.2)
IF(EXISTS ${PATH_OPENCV})
	MESSAGE(STATUS "Found OpenCV: ${PATH_OPENCV}")
ELSE()
	MESSAGE(FATAL_ERROR "Can not find OpenCV in ${PATH_OPENCV}")
ENDIF()

# head file search path
INCLUDE_DIRECTORIES(
	${PATH_TEST_FILES}
	${PATH_OPENCV}/include
	${PATH_SRC_FILES}/include
)

# find opencv library
FIND_LIBRARY(opencv_core NAMES opencv_core PATHS ${PATH_OPENCV}/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_imgproc NAMES opencv_imgproc PATHS ${PATH_OPENCV}/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_highgui NAMES opencv_highgui PATHS ${PATH_OPENCV}/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_imgcodecs NAMES opencv_imgcodecs PATHS ${PATH_OPENCV}/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_video NAMES opencv_video PATHS ${PATH_OPENCV}/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_videoio NAMES opencv_videoio PATHS ${PATH_OPENCV}/lib NO_DEFAULT_PATH)
FIND_LIBRARY(opencv_objdetect NAMES opencv_objdetect PATHS ${PATH_OPENCV}/lib NO_DEFAULT_PATH)
MESSAGE(STATUS "opencv libraries: ${opencv_core} ${opencv_imgproc} ${opencv_highgui} ${opencv_imgcodecs} ${opencv_video}" ${opencv_videoio} ${opencv_objdetect})

# find intel tbb library
SET(TBB_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build CACHE PATH "intel tbb library path")
MESSAGE(STATUS "tbb library dir: ${TBB_LIB_DIR}")
LINK_DIRECTORIES(${TBB_LIB_DIR})

# recursive query match files :*.cpp
FILE(GLOB_RECURSE TEST_CPP_LIST ${PATH_TEST_FILES}/*.cpp)
#MESSAGE(STATUS "test cpp list: ${TEST_CPP_LIST} ${TEST_C_LIST}")

# build executable program
ADD_EXECUTABLE(IntelTBB_Test ${TEST_CPP_LIST})
# add dependent library: static and dynamic
TARGET_LINK_LIBRARIES(IntelTBB_Test ${TBB_LIB_DIR}/libtbb.so ${TBB_LIB_DIR}/libtbbmalloc.so ${TBB_LIB_DIR}/libtbbmalloc_proxy.so ${opencv_core} ${opencv_imgproc} ${opencv_highgui} ${opencv_imgcodecs} ${opencv_video} ${opencv_videoio} ${opencv_objdetect})

