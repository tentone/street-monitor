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

class OpticalFlow {
    public:
        cv::Mat opticalFlowFrame;

        /**
         * @brief Flag to visualize optical flow output.
         */
        bool debug = true;
        
        /**
         * @brief Initialize the optical flow with the first frame.
         * 
         * @param frame First frame captured (used as base).
         */
        void initialize(cv::Mat *frame) {
			cv::cvtColor(*frame, opticalFlowFrame, cv::COLOR_BGR2GRAY);
        }

        /**
         * @brief Calculate optical flow using good to track features (faster than dense tracking).
         * 
         * @param frame New frame to calculate optical flow.
         */
        void lucasKanade(cv::Mat *frame)
        {   
            // TODO <ADD CODE HERE>
        }

        /**
         * @brief Calculate optical flow for the new frame combined with the previus frame stored in "opticalFlowFrame"
         * 
         * @param frame New frame to calculate optical flow.
         */
        cv::Mat dense(cv::Mat *frame)
        {   
            cv::Mat next;

            cv::cvtColor(*frame, next, cv::COLOR_BGR2GRAY);
            cv::Mat flow(opticalFlowFrame.size(), CV_32FC2);
            cv::calcOpticalFlowFarneback(opticalFlowFrame, next, flow, 0.5, 3, 15, 3, 3, 3.0, 0);

            if (debug) {
                // Visualization
                cv::Mat flow_parts[2];
                cv::split(flow, flow_parts);

                cv::Mat magnitude, angle, magn_norm;
                cv::cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
                cv::normalize(magnitude, magn_norm, 0.0f, 1.0f, cv::NORM_MINMAX);
                angle *= ((1.f / 360.f) * (180.f / 255.f));

                // Build hsv image
                cv::Mat _hsv[3], hsv, hsv8, bgr;
                _hsv[0] = angle;
                _hsv[1] = cv::Mat::ones(angle.size(), CV_32F);
                _hsv[2] = magn_norm;
                cv::merge(_hsv, 3, hsv);

                hsv.convertTo(hsv8, CV_8U, 255.0);
                cv::cvtColor(hsv8, bgr, cv::COLOR_HSV2BGR);
                cv::imshow("Optical Flow Dense", bgr);
            }

            opticalFlowFrame = next;

            return flow;
        }
};