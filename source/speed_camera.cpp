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

class SpeedCamera {
	public:
		OpticalFlow opticalFlow;

		HaarDetector haarDetector;

		/**
		 * @brief Process a frame of the video feed, can be obtained from camera, video file, dataset etc.
		 * 
		 * @param fname 
		 */
		void processFrame(cv::Mat *frame) {
			cv::imshow( "Frame", *frame );
			
			// surfFeatures(frame);
			opticalFlow.dense(frame);
			haarDetector.detect(frame, "./models/haar/car.xml");
			backgroundSub(frame);

			cv::waitKey(1);
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

			// First frame
			cv::Mat frame;
			cap >> frame;
			opticalFlow.initialize(&frame);

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


		
		cv::Mat backgroundMask;
		//cv::Ptr<cv::BackgroundSubtractor> backgroundSubtrator = cv::createBackgroundSubtractorKNN(500, 400, true);
		cv::Ptr<cv::BackgroundSubtractor> backgroundSubtrator = cv::createBackgroundSubtractorMOG2(500, 16, true);
		
		/**
		 * @brief Perform background subtraction and return binary mask of moving objects. Background model is updated based on the last n frames.
		 * 
		 * Only efective when the camera is steady.
		 * 
		 * @param frame Frame to calculate moving objects.
		 */
		void backgroundSub(cv::Mat *frame)
		{
			if (frame->empty()) {
				return;
			}

			// Update the background model
			backgroundSubtrator->apply(*frame, backgroundMask);
			cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5), cv::Point(3, 3));
			cv::erode(backgroundMask, backgroundMask, element);
			cv::dilate(backgroundMask, backgroundMask, element);

			// Show the current frame and the fg masks
			cv::imshow("Background Sub", backgroundMask);
		}

		/**
		 * @brief Calculate and display SURF features for the entire image.
		 * 
		 * @param frame Frame to extract features for.
		 */
		void surfFeatures(cv::Mat *frame) {
			// Detect the keypoints using SURF Detector
			int minHessian = 400;
			cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create( minHessian );
			std::vector<cv::KeyPoint> keypoints;
			detector->detect(*frame, keypoints);

			// Draw keypoints
			cv::Mat img;
			cv::drawKeypoints(*frame, keypoints, img);

			// Show detected (drawn) keypoints
			cv::imshow("SURF", img);
		}
};
