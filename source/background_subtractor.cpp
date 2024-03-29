#include <iostream>
#include <sstream>
#include <math.h>

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>

#pragma once

class BackgroundSubtractor {
	public:
		/**
		 * @brief Flag to display debug information.
		 */
		bool debug = true;

		/**
		 * @brief Reset the background subtrator if there is a sudden change in the image. (e.g. camera moved, lighting etc)
		 */
		bool reset_on_change = true;

		/**
		 * @brief Background subtractor instance that stores the last n frames and compares background for new frame.
		 */
		// cv::Ptr<cv::BackgroundSubtractor> subtractor = cv::createBackgroundSubtractorMOG2(600, 15.0, true);
		cv::Ptr<cv::BackgroundSubtractor> subtractor = cv::createBackgroundSubtractorKNN(300, 500.0, true);

		/**
		 * @brief Mask image used to store the result of background subtraction.
		 */
		cv::Mat mask;

		/**
		 * @brief Structuring element for the binary closing operation, used to remove "noise" after background subtraction.
		 */
		cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(1, 1));

		/**
		 * @brief Compare two images by getting the L2 error (square-root of sum of squared error).
		 * 
		 * @param A Image A to compare.
		 * @param B Image B to compare.
		 * @return double Level of similarity of the image.
		 */
		double getSimilarity(const cv::Mat a, const cv::Mat b) {
			if (a.rows > 0 && a.rows == b.rows && a.cols > 0 && a.cols == b.cols)
			{
				// Calculate the L2 relative error between images.
				double err = cv::norm(a, b, cv::NORM_L2);

				// Scale the output since L2 error is summed across all pixels of the image.
				return err / (double)(a.rows * a.cols);
			}

			// Images have diferente size
			return INFINITY;
		}

		/**
		 * @brief Perform background subtraction and return binary mask of moving objects. Background model is updated based on the last n frames.
		 * 
		 * Only efective when the camera is steady.
		 * 
		 * @param frame Frame to calculate moving objects.
		 */
		cv::Mat update(cv::Mat *frame, bool close_operation = false)
		{
			if (this->reset_on_change) {
				// TODO <ADD CODE HERE>
			}

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
			params.minDistBetweenBlobs = 0;
			params.filterByArea = true;
			params.minArea = 80;
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