#include "funset.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>

namespace {

#ifdef _MSC_VER
	const std::string images_path_detect{ "E:/GitCode/Face_Test/testdata/detection/" };
#else
	const std::string images_path_detect{ "testdata/detection/" };
#endif
const std::vector<std::string> images_name_detect{ "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg",
	"11.jpg", "12.jpg", "13.jpg", "14.jpg", "15.jpg", "16.jpg", "17.jpg", "18.jpg", "19.jpg", "20.jpg" };

void save_mats()
{
	const int width = 200, height = 200;
	cv::Mat dst(height * 5, width * 4, CV_8UC3);
	for (int i = 0; i < images_name_detect.size(); ++i) {
		std::string input_image = images_path_detect + "_" + images_name_detect[i];
		cv::Mat src = cv::imread(input_image, 1);
		if (src.empty()) {
			fprintf(stderr, "read image error: %s\n", input_image.c_str());
			return;
		}

		cv::resize(src, src, cv::Size(width, height), 0, 0, 4);
		int x = (i * width) % (width * 4);
		int y = (i / 4) * height;
		cv::Mat part = dst(cv::Rect(x, y, width, height));
		src.copyTo(part);
	}
	std::string output_image = images_path_detect + "result.png";
	cv::imwrite(output_image, dst);
}

} // namespace

int test_face_detect_LBP()
{
	// Blog: https://blog.csdn.net/fengbingchun/article/details/79867667
#ifdef _MSC_VER
	const std::string lbp_files_path{ "E:/GitCode/Face_Test/testdata/lbpcascades/" };
#else
	const std::string lbp_files_path{ "testdata/lbpcascades/" };
#endif
	const std::vector<std::string> lbpcascades_files{ "lbpcascade_frontalface.xml", "lbpcascade_frontalface_improved.xml", "lbpcascade_profileface.xml" };

	cv::CascadeClassifier face_cascade(lbp_files_path+lbpcascades_files[0]);
	if (face_cascade.empty()) {
		fprintf(stderr, "classifier hasn't been loaded\n");
		return -1;
	}

	// Search for many objects in the one image.
	const int flags = cv::CASCADE_SCALE_IMAGE;
	// Smallest object size.
	const cv::Size min_feature_size = cv::Size(10, 10);
	// How detailed should the search be. Must be larger than 1.0.
	const float search_scale_factor = 1.1f;
	// How much the detections should be filtered out. This should depend on how bad false detections are to your system.
	// min_neighbors=2 means lots of good+bad detections, and min_neighbors=6 means only good detections are given but some are missed.
	const int min_neighbors = 2;
	const int scaled_width = 320;

	for (int i = 0; i < images_name_detect.size(); ++i) {
		std::string name = images_path_detect + images_name_detect[i];
		cv::Mat bgr = cv::imread(name, 1);
		cv::Mat gray = cv::imread(name, 0);
		if (!bgr.data || bgr.channels() != 3 || !gray.data || gray.channels() != 1) {
			fprintf(stderr, "read image fail: %s\n", name.c_str());
			return -1;
		}
		fprintf(stdout, "image name: %s: size(width, height): (%d, %d)\n", images_name_detect[i].c_str(), gray.cols, gray.rows);
		// possibly shrink the image to run much faster.
		cv::Mat resized, equalized;
		float scale = gray.cols / (float)scaled_width;
		if (gray.cols > scaled_width) {
			// Shrink the image while keeping the same aspect ratio.
			int scaled_height = cvRound(gray.rows / scale);
			cv::resize(gray, resized, cv::Size(scaled_width, scaled_height));
		} else {
			// Access the input image directly, since it is already small.
			resized = gray;
		}

		// standardize the brightness and contrast to improve dark images.
		cv::equalizeHist(resized, equalized);

		std::vector<cv::Rect> objects;
		// Detect objects in the small grayscale image.
		face_cascade.detectMultiScale(equalized, objects, search_scale_factor, min_neighbors, flags, min_feature_size);
		fprintf(stdout, "image name: %s: detect face count: %d\n", images_name_detect[i].c_str(), objects.size());

		// Enlarge the results if the image was temporarily shrunk before detection.
		if (gray.cols > scaled_width) {
			for (int j = 0; j < objects.size(); ++j) {
				objects[j].x = cvRound(objects[j].x * scale);
				objects[j].y = cvRound(objects[j].y * scale);
				objects[j].width = cvRound(objects[j].width * scale);
				objects[j].height = cvRound(objects[j].height * scale);
			}
		}

		for (int j = 0; j < objects.size(); ++j) {
			// Make sure the object is completely within the image, in case it was on a border.
			objects[j].x = std::max(objects[j].x, 0);
			objects[j].y = std::max(objects[j].y, 0);
			if (objects[j].x + objects[j].width > gray.cols) {
				objects[j].x = gray.cols - objects[j].width;
			}
			if (objects[j].y + objects[j].height > gray.rows) {
				objects[j].y = gray.rows - objects[j].height;
			}

			cv::rectangle(bgr, objects[j], cv::Scalar(0, 255, 0), 2);
		}

		std::string save_result = images_path_detect + "_" + images_name_detect[i];
		cv::imwrite(save_result, bgr);
	}

	save_mats();

	return 0;
}

