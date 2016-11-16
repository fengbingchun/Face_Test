#include"funset.hpp"
#include <string>
#include <face_detection.h>
#include <face_alignment.h>
#include <opencv2/opencv.hpp>

int test_detection()
{
	std::vector<std::string> images{ "1.jpg", "2.jpg", "3.jpg", "4.jpeg", "5.jpeg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg",
		"11.jpeg", "12.jpg", "13.jpeg", "14.jpg", "15.jpeg", "16.jpg", "17.jpg", "18.jpg", "19.jpg", "20.jpg" };
	std::vector<int> count_faces{ 1, 2, 6, 0, 1, 1, 1, 2, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 0, 8, 2 };

	const std::string path_images{ "E:/GitCode/Face_Test/testdata/" };

	seeta::FaceDetection detector("E:/GitCode/Face_Test/src/SeetaFaceEngine/FaceDetection/model/seeta_fd_frontal_v1.0.bin");

	detector.SetMinFaceSize(20);
	detector.SetMaxFaceSize(200);
	detector.SetScoreThresh(2.f);
	detector.SetImagePyramidScaleFactor(0.8f);
	detector.SetWindowStep(4, 4);

	for (int i = 0; i < images.size(); i++) {
		cv::Mat src_ = cv::imread(path_images + images[i], 1);
		if (src_.empty()) {
			fprintf(stderr, "read image error: %s\n", images[i].c_str());
			continue;
		}

		cv::Mat src;
		cv::cvtColor(src_, src, CV_BGR2GRAY);

		seeta::ImageData img_data;
		img_data.data = src.data;
		img_data.width = src.cols;
		img_data.height = src.rows;
		img_data.num_channels = 1;

		std::vector<seeta::FaceInfo> faces = detector.Detect(img_data);

		fprintf(stderr, "image_name: %s, faces_num: %d\n", images[i].c_str(), faces.size());
		for (int num = 0; num < faces.size(); num++) {
			fprintf(stderr, "    score = %f\n",/*, roll = %f, pitch = %f, yaw = %f*/
				faces[num].score/*, faces[num].roll, faces[num].pitch, faces[num].yaw*/);

			cv::rectangle(src_, cv::Rect(faces[num].bbox.x, faces[num].bbox.y,
				faces[num].bbox.width, faces[num].bbox.height), cv::Scalar(0, 255, 0), 2);
		}

		std::string save_result = path_images + "_" + images[i];
		cv::imwrite(save_result, src_);
	}

	int width = 200;
	int height = 200;
	cv::Mat dst(height * 5, width * 4, CV_8UC3);
	for (int i = 0; i < images.size(); i++) {
		std::string input_image = path_images + "_" + images[i];
		cv::Mat src = cv::imread(input_image, 1);
		if (src.empty()) {
			fprintf(stderr, "read image error: %s\n", images[i].c_str());
			return -1;
		}

		cv::resize(src, src, cv::Size(width, height), 0, 0, 4);
		int x = (i * width) % (width * 4);
		int y = (i / 4) * height;
		cv::Mat part = dst(cv::Rect(x, y, width, height));
		src.copyTo(part);
	}
	std::string output_image = path_images + "result.png";
	cv::imwrite(output_image, dst);

	return 0;
}

int test_alignment()
{
	std::vector<std::string> images{ "1.jpg", "2.jpg", "3.jpg", "4.jpeg", "5.jpeg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg",
		"11.jpeg", "12.jpg", "13.jpeg", "14.jpg", "15.jpeg", "16.jpg", "17.jpg", "18.jpg", "19.jpg", "20.jpg" };
	std::vector<int> count_faces{ 1, 2, 6, 0, 1, 1, 1, 2, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 0, 8, 2 };

	const std::string path_images{ "E:/GitCode/Face_Test/testdata/" };

	seeta::FaceDetection detector("E:/GitCode/Face_Test/src/SeetaFaceEngine/FaceDetection/model/seeta_fd_frontal_v1.0.bin");

	detector.SetMinFaceSize(20);
	detector.SetMaxFaceSize(200);
	detector.SetScoreThresh(2.f);
	detector.SetImagePyramidScaleFactor(0.8f);
	detector.SetWindowStep(4, 4);

	seeta::FaceAlignment point_detector("E:/GitCode/Face_Test/src/SeetaFaceEngine/FaceAlignment/model/seeta_fa_v1.1.bin");

	for (auto name : images) {
		fprintf(stderr, "start detect image: %s\n", name.c_str());

		cv::Mat src_ = cv::imread(path_images + name, 1);
		if (src_.empty()) {
			fprintf(stderr, "read image error: %s\n", name.c_str());
			continue;
		}

		cv::Mat src;
		cv::cvtColor(src_, src, CV_BGR2GRAY);

		seeta::ImageData img_data;
		img_data.data = src.data;
		img_data.width = src.cols;
		img_data.height = src.rows;
		img_data.num_channels = 1;

		std::vector<seeta::FaceInfo> faces = detector.Detect(img_data);

		for (auto face : faces) {
			// Detect 5 facial landmarks: two eye centers, nose tip and two mouth corners
			seeta::FacialLandmark points[5];
			point_detector.PointDetectLandmarks(img_data, face, points);

			cv::rectangle(src_, cv::Rect(face.bbox.x, face.bbox.y,
				face.bbox.width, face.bbox.height), cv::Scalar(0, 255, 0), 2);

			for (auto point : points) {
				cv::circle(src_, cv::Point(point.x, point.y), 2, cv::Scalar(0, 0, 255), 2);
			}
		}

		std::string save_result = path_images + "_" + name;
		cv::imwrite(save_result, src_);
	}

	int width = 200;
	int height = 200;
	cv::Mat dst(height * 5, width * 4, CV_8UC3);
	for (int i = 0; i < images.size(); i++) {
		std::string input_image = path_images + "_" + images[i];
		cv::Mat src = cv::imread(input_image, 1);
		if (src.empty()) {
			fprintf(stderr, "read image error: %s\n", images[i].c_str());
			return -1;
		}

		cv::resize(src, src, cv::Size(width, height), 0, 0, 4);
		int x = (i * width) % (width * 4);
		int y = (i / 4) * height;
		cv::Mat part = dst(cv::Rect(x, y, width, height));
		src.copyTo(part);
	}
	std::string output_image = path_images + "result.png";
	cv::imwrite(output_image, dst);

	return 0;
}
