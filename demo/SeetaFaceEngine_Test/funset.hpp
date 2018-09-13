#ifndef FBC_FACE_TEST_SEETAFACEENGINE_HPP_
#define FBC_FACE_TEST_SEETAFACEENGINE_HPP_

#include <string>

int test_detection();
int test_alignment();
int test_recognize();
int test_identification_CropFace();
int test_identification_ExtractFeature();
//int test_identification_ExtractFeatureWithCrop();
std::string get_image_name(std::string name);

#endif // FBC_FACE_TEST_SEETAFACEENGINE_HPP_
