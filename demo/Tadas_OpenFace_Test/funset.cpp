#include "funset.hpp"
#include <vector>
#include <string>
#include <fstream>

#include <filesystem.hpp>
#include <filesystem/fstream.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <tbb/tbb.h>
#include <opencv2/opencv.hpp>

#include <LandmarkCoreIncludes.h>
#include <FaceAnalyser.h>
#include <GazeEstimation.h>

#define CONFIG_DIR "E:/GitCode/Face_Test/src/TadasBaltrusaitis_OpenFace/lib/local/LandmarkDetector/"

int test_face_landmark()
{
	// Note: ./lib/local/LandmarkDetector/src/PDM.cpp: lines 550, function cv::vconcat crash
	return -1;
	/*std::vector<std::string> arguments{ "", "-wild", "-fdir", "E:/GitCode/Face_Test/testdata/",
		"-ofdir", "E:/GitCode/Face_Test/testdata/ret1/", "-oidir", "E:/GitCode/Face_Test/testdata/ret2/" };

	std::vector<std::string> files, depth_files, output_images, output_landmark_locations, output_pose_locations;
	std::vector<cv::Rect_<double> > bounding_boxes; // Bounding boxes for a face in each image (optional)

	LandmarkDetector::get_image_input_output_params(files, depth_files, output_landmark_locations, output_pose_locations, output_images, bounding_boxes, arguments);
	LandmarkDetector::FaceModelParameters det_parameters(arguments);

	// The modules that are being used for tracking
	LandmarkDetector::CLNF clnf_model(det_parameters.model_location);

	dlib::frontal_face_detector face_detector_hog = dlib::get_frontal_face_detector();

	for (auto file : files) {
		cv::Mat grayscale_image = cv::imread(file, 0);
		if (grayscale_image.empty()) {
			fprintf(stderr, "Could not read the input image: %s\n", file.c_str());
			return -1;
		}

		int pos = file.find_last_of("\\");
		std::string image_name = file.substr(pos + 1);
		std::vector<cv::Rect_<double> > face_detections; // Detect faces in an image
		std::vector<double> confidences;
		LandmarkDetector::DetectFacesHOG(face_detections, grayscale_image, face_detector_hog, confidences);

		std::string image_path = file.substr(0, pos);
		std::string save_result = image_path + "/ret2/_" + image_name;
		cv::Mat bgr = cv::imread(file, 1);

		fprintf(stderr, "%s face count: %d\n", image_name.c_str(), face_detections.size());
		for (int i = 0; i < face_detections.size(); ++i) {
			// perform landmark detection for every face detected
			// if there are multiple detections go through them
			bool success = LandmarkDetector::DetectLandmarksInImage(grayscale_image, face_detections[i], clnf_model, det_parameters);
			if (success) {
				cv::Mat_<double> mat = clnf_model.detected_landmarks;
				fprintf(stderr, "**********landmarks: %d, rows: %d\n", mat.rows * mat.cols, mat.rows);
			}

			cv::Rect_<double> rect{ face_detections[i] };
			fprintf(stderr, "    x: %.2f, y: %.2f, width: %.2f, height: %.2f, confidence: %.2f\n",
				rect.x, rect.y, rect.width, rect.height, confidences[i]);

			cv::rectangle(bgr, cv::Rect(rect.x, rect.y, rect.width, rect.height), cv::Scalar(0, 255, 0), 2);
		}

		cv::imwrite(save_result, bgr);
	}

	int width = 200;
	int height = 200;
	cv::Mat dst(height * 5, width * 4, CV_8UC3);
	int pos = files[0].find_last_of("\\");
	std::string image_path = files[0].substr(0, pos);
	for (int i = 0; i < files.size(); i++) {
		std::string image_name = files[i].substr(pos + 1);
		std::string input_image = image_path + "/ret2/_" + image_name;

		cv::Mat src = cv::imread(input_image, 1);
		if (src.empty()) {
			fprintf(stderr, "read image error: %s\n", input_image.c_str());
			return -1;
		}

		cv::resize(src, src, cv::Size(width, height), 0, 0, 4);
		int x = (i * width) % (width * 4);
		int y = (i / 4) * height;
		cv::Mat part = dst(cv::Rect(x, y, width, height));
		src.copyTo(part);
	}
	std::string output_image = image_path + "/ret2/result.png";
	cv::imwrite(output_image, dst);

	return 0;*/
}

int test_FaceDetect_HaarCascade()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/70147435
	std::vector<std::string> arguments{ "", "-wild", "-fdir", "E:/GitCode/Face_Test/testdata/detection/",
		"-ofdir", "E:/GitCode/Face_Test/testdata/ret1/", "-oidir", "E:/GitCode/Face_Test/testdata/ret2/" };

	std::vector<std::string> files, depth_files, output_images, output_landmark_locations, output_pose_locations;
	std::vector<cv::Rect_<double> > bounding_boxes; // Bounding boxes for a face in each image (optional)

	LandmarkDetector::get_image_input_output_params(files, depth_files, output_landmark_locations, output_pose_locations, output_images, bounding_boxes, arguments);
	LandmarkDetector::FaceModelParameters det_parameters(arguments);

	cv::CascadeClassifier classifier(det_parameters.face_detector_location);

	for (auto file : files) {
		cv::Mat grayscale_image = cv::imread(file, 0);
		if (grayscale_image.empty()) {
			fprintf(stderr, "Could not read the input image: %s\n", file.c_str());
			return -1;
		}

		int pos = file.find_last_of("\\");
		std::string image_name = file.substr(pos + 1);
		std::vector<cv::Rect_<double> > face_detections; // Detect faces in an image
		LandmarkDetector::DetectFaces(face_detections, grayscale_image, classifier);

		std::string image_path = file.substr(0, pos);
		std::string save_result = image_path + "/ret2/_" + image_name;
		cv::Mat bgr = cv::imread(file, 1);

		fprintf(stderr, "%s face count: %d\n", image_name.c_str(), face_detections.size());
		for (int i = 0; i < face_detections.size(); ++i) {
			cv::Rect_<double> rect{ face_detections[i] };
			fprintf(stderr, "    x: %.2f, y: %.2f, width: %.2f, height: %.2f\n",
				rect.x, rect.y, rect.width, rect.height);

			cv::rectangle(bgr, cv::Rect(rect.x, rect.y, rect.width, rect.height), cv::Scalar(0, 255, 0), 2);
		}

		cv::imwrite(save_result, bgr);
	}

	int width = 200;
	int height = 200;
	cv::Mat dst(height * 5, width * 4, CV_8UC3);
	int pos = files[0].find_last_of("\\");
	std::string image_path = files[0].substr(0, pos);
	for (int i = 0; i < files.size(); i++) {
		std::string image_name = files[i].substr(pos + 1);
		std::string input_image = image_path + "/ret2/_" + image_name;

		cv::Mat src = cv::imread(input_image, 1);
		if (src.empty()) {
			fprintf(stderr, "read image error: %s\n", input_image.c_str());
			return -1;
		}

		cv::resize(src, src, cv::Size(width, height), 0, 0, 4);
		int x = (i * width) % (width * 4);
		int y = (i / 4) * height;
		cv::Mat part = dst(cv::Rect(x, y, width, height));
		src.copyTo(part);
	}
	std::string output_image = image_path + "/ret2/result.png";
	cv::imwrite(output_image, dst);

	return 0;
}

int test_FaceDetect_HOG()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/70146734
	std::vector<std::string> arguments{ "", "-wild", "-fdir", "E:/GitCode/Face_Test/testdata/detection/",
		"-ofdir", "E:/GitCode/Face_Test/testdata/ret1/", "-oidir", "E:/GitCode/Face_Test/testdata/ret2/" };

	std::vector<std::string> files, depth_files, output_images, output_landmark_locations, output_pose_locations;
	std::vector<cv::Rect_<double> > bounding_boxes; // Bounding boxes for a face in each image (optional)

	LandmarkDetector::get_image_input_output_params(files, depth_files, output_landmark_locations, output_pose_locations, output_images, bounding_boxes, arguments);
	LandmarkDetector::FaceModelParameters det_parameters(arguments);

	dlib::frontal_face_detector face_detector_hog = dlib::get_frontal_face_detector();

	for (auto file : files) {
		cv::Mat grayscale_image = cv::imread(file, 0);
		if (grayscale_image.empty()) {
			fprintf(stderr, "Could not read the input image: %s\n", file.c_str());
			return -1;
		}

		int pos = file.find_last_of("\\");
		std::string image_name = file.substr(pos+1);
		std::vector<cv::Rect_<double> > face_detections; // Detect faces in an image
		std::vector<double> confidences;
		LandmarkDetector::DetectFacesHOG(face_detections, grayscale_image, face_detector_hog, confidences);

		std::string image_path = file.substr(0, pos);
		std::string save_result = image_path + "/ret2/_" + image_name;
		cv::Mat bgr = cv::imread(file, 1);

		fprintf(stderr, "%s face count: %d\n", image_name.c_str(), face_detections.size());
		for (int i = 0; i < face_detections.size(); ++i) {
			cv::Rect_<double> rect{ face_detections[i] };
			fprintf(stderr, "    x: %.2f, y: %.2f, width: %.2f, height: %.2f, confidence: %.2f\n",
				rect.x, rect.y, rect.width, rect.height, confidences[i]);

			cv::rectangle(bgr, cv::Rect(rect.x, rect.y, rect.width, rect.height), cv::Scalar(0, 255, 0), 2);
		}

		cv::imwrite(save_result, bgr);
	}

	int width = 200;
	int height = 200;
	cv::Mat dst(height * 5, width * 4, CV_8UC3);
	int pos = files[0].find_last_of("\\");
	std::string image_path = files[0].substr(0, pos);
	for (int i = 0; i < files.size(); i++) {
		std::string image_name = files[i].substr(pos + 1);
		std::string input_image = image_path + "/ret2/_" + image_name;

		cv::Mat src = cv::imread(input_image, 1);
		if (src.empty()) {
			fprintf(stderr, "read image error: %s\n", input_image.c_str());
			return -1;
		}

		cv::resize(src, src, cv::Size(width, height), 0, 0, 4);
		int x = (i * width) % (width * 4);
		int y = (i / 4) * height;
		cv::Mat part = dst(cv::Rect(x, y, width, height));
		src.copyTo(part);
	}
	std::string output_image = image_path + "/ret2/result.png";
	cv::imwrite(output_image, dst);

	return 0;
}
