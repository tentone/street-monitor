#include <iostream>
#include <sstream>

#include <opencv2/core.hpp>

class BackgroundSubtractor {
    public:
		bool debug = true;

        /**
         * @brief Background subtractor instance that stores the last n frames and compares background for new frame.
         */
		//cv::Ptr<cv::BackgroundSubtractor> subtractor = cv::createBackgroundSubtractorKNN(500, 400, true);
        cv::Ptr<cv::BackgroundSubtractor> subtractor = cv::createBackgroundSubtractorMOG2(500, 16, true);
		
		/**
		 * @brief Perform background subtraction and return binary mask of moving objects. Background model is updated based on the last n frames.
		 * 
		 * Only efective when the camera is steady.
		 * 
		 * @param frame Frame to calculate moving objects.
		 */
		cv::Mat update(cv::Mat *frame)
		{
            cv::Mat mask;

			// Update the background model
			subtractor->apply(*frame, mask);

            // Close operation
			cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5), cv::Point(3, 3));
			cv::erode(mask, mask, element);
			cv::dilate(mask, mask, element);

            if(debug) {
                // Show the current frame and the fg masks
                cv::imshow("Background Sub", mask);
            }

            return mask;
		}
};