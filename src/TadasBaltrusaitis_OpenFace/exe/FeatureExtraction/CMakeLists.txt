#TBB library
include_directories(${TBB_ROOT_DIR}/include)

add_executable(FeatureExtraction FeatureExtraction.cpp)

# Local libraries
include_directories(${LandmarkDetector_SOURCE_DIR}/include)

include_directories(../../lib/local/LandmarkDetector/include)
include_directories(../../lib/local/FaceAnalyser/include)		

target_link_libraries(FeatureExtraction LandmarkDetector)
target_link_libraries(FeatureExtraction FaceAnalyser)
target_link_libraries(FeatureExtraction dlib)

target_link_libraries(FeatureExtraction ${OpenCV_LIBS} ${Boost_LIBRARIES} ${TBB_LIBRARIES})

install (TARGETS FeatureExtraction DESTINATION bin)
