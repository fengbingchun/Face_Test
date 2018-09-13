#include "funset.hpp"
#include <string>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/image_io.h>
#include <opencv2/opencv.hpp>

/* reference: dlib/examples/face_detection_ex.cpp
  This face detector is made using the now classic Histogram of Oriented
  Gradients (HOG) feature combined with a linear classifier, an image pyramid,
  and sliding window detection scheme.  This type of object detector is fairly
  general and capable of detecting many types of semi-rigid objects in
  addition to human faces.
*/
int test_face_detect()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/53493305
	dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();

	std::vector<std::string> images{ "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg",
		"11.jpg", "12.jpg", "13.jpg", "14.jpg", "15.jpg", "16.jpg", "17.jpg", "18.jpg", "19.jpg", "20.jpg" };
	std::vector<int> count_faces{ 1, 2, 6, 0, 1, 1, 1, 2, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 0, 8, 2 };

#ifdef _MSC_VER
	std::string path_images{ "E:/GitCode/Face_Test/testdata/detection/" };
#else
	std::string path_images{ "testdata/detection/" };
#endif

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

/* reference: dlib/examples/face_landmark_detection_ex.cpp
  This program shows how to find frontal human faces in an image and
  estimate their pose.  The pose takes the form of 68 landmarks.  These are
  points on the face such as the corners of the mouth, along the eyebrows, on
  the eyes, and so forth
*/
int test_face_landmark()
{
	// Blog: http://blog.csdn.net/fengbingchun/article/details/53646947
	// download: http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2
#ifdef _MSC_VER
	const std::string shape_predictor_68_face_landmarks = "E:/GitCode/Face_Test/src/dlib/data/shape_predictor_68_face_landmarks.dat";
#else
	const std::string shape_predictor_68_face_landmarks = "testdata/shape_predictor_68_face_landmarks.dat";
#endif

	// We need a face detector.  We will use this to get bounding boxes for
	// each face in an image.
	dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();

	// And we also need a shape_predictor.  This will predict face
	// landmark positions given an image and face bounding box
	dlib::shape_predictor sp;
	dlib::deserialize(shape_predictor_68_face_landmarks) >> sp;

	std::vector<std::string> images{ "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg",
		"11.jpg", "12.jpg", "13.jpg", "14.jpg", "15.jpg", "16.jpg", "17.jpg", "18.jpg", "19.jpg", "20.jpg" };
	std::vector<int> count_faces{ 1, 2, 6, 0, 1, 1, 1, 2, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 0, 8, 2 };

#ifdef _MSC_VER
	std::string path_images{ "E:/GitCode/Face_Test/testdata/detection/" };
#else
	std::string path_images{ "testdata/detection/" };
#endif

	if (images.size() != count_faces.size()) {
		fprintf(stderr, "their size that images and count_faces are mismatch\n");
		return -1;
	}

	for (int i = 0; i < images.size(); i++) {
		cv::Mat matSrc = cv::imread(path_images + images[i], 1);
		if (matSrc.empty()) {
			fprintf(stderr, "read image error: %s\n", images[i].c_str());
			return -1;
		}

		dlib::array2d<unsigned char> img;
		dlib::load_image(img, path_images + images[i]);
		// Make the image larger so we can detect small faces.
		pyramid_up(img);

		// Now tell the face detector to give us a list of bounding boxes
		// around all the faces it can find in the image.
		std::vector<dlib::rectangle> dets = detector(img);
		fprintf(stderr, "detect face count: %d, actual face count: %d\n", dets.size(), count_faces[i]);

		// Now we will go ask the shape_predictor to tell us the pose of
		// each face we detected.
		std::vector<dlib::full_object_detection> shapes;
		for (unsigned long j = 0; j < dets.size(); ++j) {
			dlib::full_object_detection shape = sp(img, dets[j]);
			fprintf(stderr, "landmark num: %d\n", shape.num_parts());
			dlib::rectangle rect = shape.get_rect();
			fprintf(stderr, "rect: left = %d, top = %d, width = %d, height = %d\n", rect.left() / 2, rect.top() / 2, rect.width() / 2, rect.height() / 2);
			cv::rectangle(matSrc, cv::Rect(rect.left() / 2, rect.top() / 2, rect.width() / 2, rect.height() / 2), cv::Scalar(0, 255, 0), 2);

			for (int pt = 0; pt < shape.num_parts(); pt++) {
				cv::circle(matSrc, cv::Point(shape.part(pt).x() / 2, shape.part(pt).y() / 2), 1, cv::Scalar(0, 0, 255), 2);
			}
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

	return 0;
}
