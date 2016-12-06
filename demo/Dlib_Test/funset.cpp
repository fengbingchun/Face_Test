#include "funset.hpp"
#include <string>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <opencv2/opencv.hpp>

int test_face_detect()
{
	dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
	dlib::image_window win;

	std::vector<std::string> images{ "1.jpg", "2.jpg", "3.jpg", "4.jpeg", "5.jpeg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg",
		"11.jpeg", "12.jpg", "13.jpeg", "14.jpg", "15.jpeg", "16.jpg", "17.jpg", "18.jpg", "19.jpg", "20.jpg" };
	std::vector<int> count_faces{ 1, 2, 6, 0, 1, 1, 1, 2, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 0, 8, 2 };

	std::string path_images{ "E:/GitCode/Face_Test/testdata/" };

	if (images.size() != count_faces.size()) {
		fprintf(stderr, "their size that images and count_faces are mismatch\n");
		return -1;
	}

	for (int i = 0; i < images.size(); i++) {
		dlib::array2d<unsigned char> img;
		dlib::load_image(img, path_images + images[i]);

		// Make the image bigger by a factor of two.  This is useful since
		// the face detector looks for faces that are about 80 by 80 pixels
		// or larger.  Therefore, if you want to find faces that are smaller
		// than that then you need to upsample the image as we do here by
		// calling pyramid_up().  So this will allow it to detect faces that
		// are at least 40 by 40 pixels in size.  We could call pyramid_up()
		// again to find even smaller faces, but note that every time we
		// upsample the image we make the detector run slower since it must
		// process a larger image.
		pyramid_up(img);

		// Now tell the face detector to give us a list of bounding boxes
		// around all the faces it can find in the image.
		std::vector<dlib::rectangle> dets = detector(img);
		fprintf(stderr, "detect face count: %d, actual face count: %d\n", dets.size(), count_faces[i]);

		cv::Mat matSrc = cv::imread(path_images + images[i], 1);
		if (matSrc.empty()) {
			fprintf(stderr, "read image error: %s\n", images[i].c_str());
			return -1;
		}

		for (auto faces : dets) {
			int x = faces.left() / 2;
			int y = faces.top() / 2;
			int w = faces.width() / 2;
			int h = faces.height() / 2;

			cv::rectangle(matSrc, cv::Rect(x, y, w, h), cv::Scalar(0, 255, 0), 2);
		}

		std::string save_result = path_images + "_" + images[i];
		cv::imwrite(save_result, matSrc);
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

	fprintf(stderr, "ok\n");

	return 0;
}
