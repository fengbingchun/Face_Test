#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <facedetect-dll.h>
#include <opencv2/opencv.hpp>

namespace {

//define the buffer size. Do not change the size!
#define DETECT_BUFFER_SIZE 0x20000

const std::vector<std::string> images{ "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg",
	"11.jpg", "12.jpg", "13.jpg", "14.jpg", "15.jpg", "16.jpg", "17.jpg", "18.jpg", "19.jpg", "20.jpg" };
const std::vector<int> count_faces{ 1, 2, 6, 0, 1, 1, 1, 2, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 0, 8, 2 };
const std::string path_images{ "E:/GitCode/Face_Test/testdata/detection/" };

typedef int* (*detect_face)(unsigned char* result_buffer, unsigned char * gray_image_data, int width, int height, int step,
	float scale, int min_neighbors, int min_object_width, int max_object_width, int doLandmark);

detect_face detect_methods[]{
	&facedetect_frontal, // frontal face detection / 68 landmark detection, it's fast, but cannot detect side view faces
	&facedetect_multiview, // multiview face detection / 68 landmark detection, it can detect side view faces, but slower than facedetect_frontal()
	&facedetect_multiview_reinforce, // reinforced multiview face detection / 68 landmark detection, it can detect side view faces, better but slower than facedetect_multiview().
	&facedetect_frontal_surveillance //frontal face detection designed for video surveillance / 68 landmark detection, it can detect faces with bad illumination
};

const std::string detect_type[4] {"face frontal", "face multiview", "face multiview reinforce", "face surveillance"};

} // namespace

int test_face_detect_align(int do_landmark, int method);

int main()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/52964163

	int do_landmark = 0; // 1: landmark; 0: no landmark
	int method = 1; // 0: facedetect_frontal; 1: facedetect_multiview; 2: facedetect_multiview_reinforce; 3: facedetect_frontal_surveillance
	int ret = test_face_detect_align(do_landmark, method);

	if (ret == 0) fprintf(stdout, "========== test success ==========\n");
	else fprintf(stderr, "########## test fail ##########\n");
}

int test_face_detect_align(int do_landmark, int method)
{
	if (images.size() != count_faces.size()) {
		fprintf(stderr, "their size that images and count_faces are mismatch\n");
		return -1;
	}
	if (method < 0 || method > 3) {
		fprintf(stderr, "detect method is no support\n");
		return -1;
	}

	detect_face detect = detect_methods[method];
	fprintf(stderr, "detect type: %s\n", detect_type[method].c_str());

	for (int i = 0; i < images.size(); ++i) {
		cv::Mat src_ = cv::imread(path_images + images[i], 1);
		if (src_.empty()) {
			fprintf(stderr, "read image error: %s\n", images[i].c_str());
			return -1;
		}

		cv::Mat src;
		cv::cvtColor(src_, src, CV_BGR2GRAY);

		//buffer is used in the detection functions.
		//If you call functions in multiple threads, please create one buffer for each thread!
		std::unique_ptr<unsigned char[]> buffer(new unsigned char[DETECT_BUFFER_SIZE]);

		int* results = detect(buffer.get(), src.data, src.cols, src.rows, src.step, 1.2f, 2, 10, 0, do_landmark);
		std::string save_result = path_images + std::to_string(method) + "_" + images[i];
		//fprintf(stdout, "save result: %s\n", save_result.c_str());

		for (int faces = 0; faces < (results ? *results : 0); ++faces) {
			short* p = ((short*)(results + 1)) + 142 * faces;
			int x = p[0];
			int y = p[1];
			int w = p[2];
			int h = p[3];
			int neighbors = p[4];
			int angle = p[5];

			fprintf(stderr, "image_name: %s, faces_num: %d, face_rect=[%d, %d, %d, %d], neighbors=%d, angle=%d\n",
				images[i].c_str(), *results, x, y, w, h, neighbors, angle);

			cv::rectangle(src_, cv::Rect(x, y, w, h), cv::Scalar(0, 0, 255), 2);
			if (do_landmark) {
				for (int j = 0; j < 68; ++j)
					cv::circle(src_, cv::Point((int)p[6 + 2 * j], (int)p[6 + 2 * j + 1]), 1, cv::Scalar(0, 255, 0));
			}
		}

		cv::imwrite(save_result, src_);
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

	return 0;
}
