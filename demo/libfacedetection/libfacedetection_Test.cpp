#include <iostream>
#include <string>
#include <vector>
#include <facedetect-dll.h>
#include <opencv2/opencv.hpp>

int test_facedetect_frontal();
int test_facedetect_multiview();
int test_facedetect_multiview_reinforce();
int test_facedetect_frontal_surveillance();

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

	for (int method = 0; method < 4; method++) {
		detect_face detect = detect_methods[method];

		for (int i = 0; i < images.size(); i++) {
			cv::Mat src = cv::imread(path_images + images[i], 1);
			if (src.empty()) {
				fprintf(stderr, "read image error: %s\n", images[i].c_str());
				return -1;
			}
			cv::cvtColor(src, src, CV_BGR2GRAY);

			int* results = nullptr;
			results = detect(src.data, src.cols, src.rows, src.step, 1.2f, 5, 24, 0);

			for (int faces = 0; faces < (results ? *results : 0); faces++) {
				short* p = ((short*)(results + 1)) + 6 * faces;
				int x = p[0];
				int y = p[1];
				int w = p[2];
				int h = p[3];
				int neighbors = p[4];
				int angle = p[5];

				fprintf(stderr, "face_rect=[%d, %d, %d, %d], neighbors=%d, angle=%d\n", x, y, w, h, neighbors, angle);
			}
		}
	}

	fprintf(stderr, "ok\n");
	return 0;
}
