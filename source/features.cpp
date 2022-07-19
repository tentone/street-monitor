#include <iostream>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>

#pragma once

class Features {
    public:
		/**
		 * @brief Flag to display debug information.
		 */
		bool debug = true;

        /**
		 * @brief Calculate and display SURF features for the entire image.
		 * 
		 * @param frame Frame to extract features for.
		 */
		std::vector<cv::KeyPoint> surf(cv::Mat *frame) {
            int minHessian = 400;

			// Detect the keypoints using SURF Detector
			cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(minHessian);
			std::vector<cv::KeyPoint> keypoints;
			detector->detect(*frame, keypoints);
            
            if (debug) {
                // Draw keypoints
                cv::Mat img;
                cv::drawKeypoints(*frame, keypoints, img);

                // Show detected (drawn) keypoints
                cv::imshow("SURF", img);
            }

            return keypoints;
		}
};