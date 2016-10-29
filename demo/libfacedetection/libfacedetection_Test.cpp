#include <iostream>
#include <string>
#include <vector>
#include <facedetect-dll.h>
#include <opencv2/opencv.hpp>

int main()
{
	std::vector<std::string> images{ "1.jpg", "2.jpg", "3.jpg", "4.jpeg", "5.jpeg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg",
		"11.jpeg", "12.jpg", "13.jpeg", "14.jpg", "15.jpeg", "16.jpg", "17.jpg", "18.jpg", "19.jpg", "20.jpg" };
	std::vector<int> count_faces{1, 2, 6, 0, 1, 1, 1, 2, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 0, 8, 2};

	std::string path_images{ "E:/GitCode/Face_Test/testdata/" };

	if (images.size() != count_faces.size()) {
		fprintf(stderr, "their size that images and count_faces are mismatch\n");
		return -1;
	}

	typedef int* (*detect_face)(unsigned char * gray_image_data, int width, int height, int step,
		float scale, int min_neighbors, int min_object_width, int max_object_width);

	detect_face detect_methods[]{
		&facedetect_frontal,
		&facedetect_multiview,
		&facedetect_multiview_reinforce,
		&facedetect_frontal_surveillance
	};

	std::string detect_type[4] {"face frontal", "face multiview", "face multiview reinforce", "face surveillance"};

	for (int method = 0; method < 4; method++) {
		detect_face detect = detect_methods[method];
		fprintf(stderr, "detect type: %s\n", detect_type[method].c_str());

		for (int i = 0; i < images.size(); i++) {
			cv::Mat src_ = cv::imread(path_images + images[i], 1);
			if (src_.empty()) {
				fprintf(stderr, "read image error: %s\n", images[i].c_str());
				return -1;
			}

			cv::Mat src;
			cv::cvtColor(src_, src, CV_BGR2GRAY);

			int* results = nullptr;
			results = detect(src.data, src.cols, src.rows, src.step, 1.2f, 2, 10, 0);
			std::string save_result = path_images + std::to_string(method) + "_" + images[i];
			//fprintf(stderr, "save result: %s\n", save_result.c_str());

			for (int faces = 0; faces < (results ? *results : 0); faces++) {
				short* p = ((short*)(results + 1)) + 6 * faces;
				int x = p[0];
				int y = p[1];
				int w = p[2];
				int h = p[3];
				int neighbors = p[4];
				int angle = p[5];

				fprintf(stderr, "image_name: %s, faces_num: %d, face_rect=[%d, %d, %d, %d], neighbors=%d, angle=%d\n",
					images[i].c_str(), *results, x, y, w, h, neighbors, angle);

				cv::rectangle(src_, cv::Rect(x, y, w, h), cv::Scalar(0, 255, 0), 2);
			}

			cv::imwrite(save_result, src_);
		}
	}

	int width = 200;
	int height = 200;
	cv::Mat dst(height * 5, width * 4, CV_8UC3);
	for (int i = 0; i < images.size(); i++) {
		std::string input_image = path_images + "2_" + images[i];
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

	fprintf(stderr, "ok\n");
	return 0;
}
