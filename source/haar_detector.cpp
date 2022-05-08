#include <iostream>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

class HaarDetector {
    public:
		/**
		 * @brief Flag to display debug information.
		 */
		bool debug = true;
		
		/**
		 * @brief Classifier model used to detect haar features.
		 */
		cv::CascadeClassifier classifier;

		HaarDetector(std::string model) {
			classifier = cv::CascadeClassifier(model);
		}

        /**
		 * @brief Detect cars using a haar cascade model. The bouding boxes of the cars found is returned.
		 * 
		 * @param frame Frame to extract cars.
		 */
		std::vector<cv::Rect> detect(cv::Mat *frame)
		{
			// Create a normalized, gray-scale version of the captured image
			cv::Mat grayscale;
			cv::cvtColor(*frame, grayscale, cv::COLOR_BGR2GRAY);

			// Prepare a vector where the detected features will be stored
			std::vector<cv::Rect> features;

			// Detect the features in the normalized, gray-scale version of the image.
			classifier.detectMultiScale(grayscale, features, 1.1, 5, cv::CASCADE_SCALE_IMAGE, cv::Size(10, 10), cv::Size(300, 300));

			// Debug
            if (debug) {
				cv::Mat img = frame->clone();

                // Draw each feature as a separate green rectangle
                for (cv::Rect feature : features) {
                    cv::rectangle(img, feature, cv::Scalar(0, 255, 0), 2);
                }

                // Show the captured image and the detected features
                cv::imshow("Haar", img);
            }
            
            return features;
		}
};