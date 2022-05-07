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

class SpeedCamera {
	public:
		OpticalFlow opticalFlow;

		HaarDetector haarDetector;

		YOLODetector yoloDetector = YOLODetector("./models/yolo/yolov5n.onnx", "./models/yolo/yolo.names");

		BackgroundSubtractor backgroundSubtractor;

		int frame = 0;

		/**
		 * @brief Process a frame of the video feed, can be obtained from camera, video file, dataset etc.
		 * 
		 * @param fname 
		 */
		void processFrame(cv::Mat *frame) {
			if (frame == 0) {
				opticalFlow.initialize(frame);
				return;
			}

			cv::imshow( "Frame", *frame );
			
			// opticalFlow.sparse(frame);
			// haarDetector.detect(frame, "./models/haar/car.xml");
			backgroundSubtractor.update(frame);
			yoloDetector.processFrame(frame);

			cv::waitKey(1);

			frame++;
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
