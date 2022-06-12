#include <iostream>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/objdetect.hpp>

#include "yolo_detector.cpp"
#include "optical_flow.cpp"
#include "haar_detector.cpp"
#include "background_subtractor.cpp"
#include "features.cpp"
#include "tracker.cpp"

class Monitor {
	public:
		OpticalFlow optical_flow;

		HaarDetector car_haar = HaarDetector("./models/haar/car.xml");

		YOLODetector yolo = YOLODetector("./models/yolo/yolov5n.onnx", "./models/yolo/yolo.names");

		BackgroundSubtractor background_detector;

		int frame_count = 0;

		/**
		 * @brief Process a frame of the video feed, can be obtained from camera, video file, dataset etc.
		 * 
		 * @param fname 
		 */
		void processFrame(cv::Mat *frame) {
			if (frame_count == 0) {
				optical_flow.initialize(frame);
				frame_count++;
				return;
			}

			// optical_flow.sparse(frame);
			// car_haar.detect(frame);

			cv::Mat mov = background_detector.update(frame);
			std::vector<cv::KeyPoint> moving = background_detector.segmentBlobs(frame, &mov);

			// for (int i = 0; i < moving.size(); i++) {
			// 	std::cout << moving[i].pt.x << ", " << moving[i].pt.y << std::endl;
			// }

			// yolo.detect(frame);

			cv::imshow("Frame", *frame);
			cv::waitKey(1);

			frame_count++;
		}

		/**
		 * @brief Start processing from computer camera.
		 */
		void startCamera() {
			cv::VideoCapture cap(0); 

			if(!cap.isOpened()) {
				std::cout << "Error opening video stream or file" << std::endl;
				return;
			}

			// Processing loop
			while(1) {
				cv::Mat frame;
				cap >> frame;

				// If the frame is empty, break immediately
				if (frame.empty()) {
					break;
				}

				this->processFrame(&frame);           
			}

			// When everything done, release the video capture object
			cap.release();

			// Closes all the frames
			cv::destroyAllWindows();
		}

		/**
		 * @brief Start the speed camera detector with a specific file
		 */
		void startVideo(const std::string fname) {
			cv::VideoCapture cap(fname); 

			if(!cap.isOpened()) {
				std::cout << "Error opening video stream or file" << std::endl;
				return;
			}

			// Processing loop
			while(1) {
				cv::Mat frame;
				cap >> frame;

				// If the frame is empty, break immediately
				if (frame.empty()) {
					break;
				}

				this->processFrame(&frame);           
			}

			// When everything done, release the video capture object
			cap.release();

			// Closes all the frames
			cv::destroyAllWindows();
		}
};
