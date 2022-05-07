#include <iostream>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

class HaarDetector {
    public:
        bool debug = true;
		
        /**
		 * @brief Detect cars using a haar cascade model. The bouding boxes of the cars found is returned.
		 * 
		 * @param frame Frame to extract cars.
		 */
		std::vector<cv::Rect> detect(cv::Mat *frame, const std::string model)
		{
            // Load a classifier from its XML description
            cv::CascadeClassifier classifier(model);

			// Prepare a vector where the detected features will be stored
			std::vector<cv::Rect> features;

			// Grayscale version of the image
			cv::Mat grayscale;

			// Create a normalized, gray-scale version of the captured image
			cv::cvtColor(*frame, grayscale, cv::COLOR_BGR2GRAY);
			cv::equalizeHist(grayscale, grayscale);

			// Detect the features in the normalized, gray-scale version of the image.
			classifier.detectMultiScale(grayscale, features, 1.1, 3, cv::CASCADE_SCALE_IMAGE, cv::Size(1, 1), cv::Size(300, 300));

            if (debug) {
                // Draw each feature as a separate green rectangle
                for (auto&& feature : features) {
                    cv::rectangle(*frame, feature, cv::Scalar(0, 255, 0), 2);
                }

                // Show the captured image and the detected features
                cv::imshow("Cars", *frame);
            }
            
            return features;
		}
};