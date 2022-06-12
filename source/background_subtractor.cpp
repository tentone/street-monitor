#include <iostream>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>

class BackgroundSubtractor {
	public:
		/**
		 * @brief Flag to display debug information.
		 */
		bool debug = true;

		/**
		 * @brief Background subtractor instance that stores the last n frames and compares background for new frame.
		 */
		// cv::Ptr<cv::BackgroundSubtractor> subtractor = cv::createBackgroundSubtractorMOG2(600, 15.0, true);
		cv::Ptr<cv::BackgroundSubtractor> subtractor = cv::createBackgroundSubtractorKNN(300, 600.0, true);

		/**
		 * @brief Mask image used to store the result of background subtraction.
		 */
		cv::Mat mask;

		/**
		 * @brief Structuring element for the binary closing operation, used to remove "noise" after background subtraction.
		 */
		cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(1, 1));

		/**
		 * @brief Perform background subtraction and return binary mask of moving objects. Background model is updated based on the last n frames.
		 * 
		 * Only efective when the camera is steady.
		 * 
		 * @param frame Frame to calculate moving objects.
		 */
		cv::Mat update(cv::Mat *frame, bool close_operation = false)
		{
			// Update the background model
			subtractor->apply(*frame, mask);

			// Close operation
			if (close_operation) {
				cv::erode(mask, mask, element);
				cv::dilate(mask, mask, element);
			}

			// Show the current frame and the fg masks
			if(debug) {
				cv::imshow("Background Subtraction", mask);
			}

			return mask;
		}

		/**
		 * @brief Segment blobs from binary image. Useful to segment moving objects in an image after background subtraction has been performed.
		 */
		std::vector<cv::KeyPoint> segmentBlobs(cv::Mat *frame, cv::Mat *mask)
		{
			// Setup SimpleBlobDetector parameters.
			cv::SimpleBlobDetector::Params params;
			params.minThreshold = 245;
			params.maxThreshold = 255;
			params.thresholdStep = 10;
			params.minRepeatability = 1;
			params.minDistBetweenBlobs = 10;
			params.filterByArea = true;
			params.minArea = 1e2;
			params.maxArea = 1e5;
			params.filterByCircularity = false;
			params.filterByConvexity = false;
			params.maxConvexity = 1.0;
			params.minConvexity = 0.2;
			params.filterByInertia = false;
			params.minInertiaRatio = 0;
			params.maxInertiaRatio = 0.2;
			params.filterByColor = false;

			// Set up the detector with default parameters.
			cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);

			// Detect blobs.
			std::vector<cv::KeyPoint> keypoints;
			detector->detect(*mask, keypoints);

			if (debug) {
				// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
				cv::Mat img;
				cv::drawKeypoints(*frame, keypoints, img, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
				cv::imshow("Blob", img);
			}

			return keypoints;
		}

};