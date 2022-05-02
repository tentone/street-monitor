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
        // Dense vars
        cv::Mat opticalFlowFrame;

        // Sparse Vars
        cv::Mat old_frame, old_gray;
        std::vector<cv::Point2f> p0, p1;
        cv::Mat mask;
        std::vector<cv::Scalar> colors;

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
            // Dense vars
			cv::cvtColor(*frame, opticalFlowFrame, cv::COLOR_BGR2GRAY);

            // Sparse Vars
            cv::cvtColor(*frame, old_gray, cv::COLOR_BGR2GRAY);
            cv::goodFeaturesToTrack(old_gray, p0, 100, 0.3, 7, cv::Mat(), 7, false, 0.04);
            mask = cv::Mat::zeros(old_frame.size(), old_frame.type());

            cv::RNG rng;
            for(int i = 0; i < 100; i++)
            {
                colors.push_back(cv::Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256)));
            }
        }

        /**
         * @brief Calculate optical flow using feature tracking.
         * 
         * @param frame New frame to calculate optical flow.
         */
        void sparse(cv::Mat *frame)
        {   
            cv::Mat frame_gray;

            cv::cvtColor(*frame, frame_gray, cv::COLOR_BGR2GRAY);

            // calculate optical flow
            std::vector<uchar> status;
            std::vector<float> err;

            cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS), 10, 0.03);
            cv::calcOpticalFlowPyrLK(old_gray, frame_gray, p0, p1, status, err, cv::Size(15,15), 2, criteria);

            std::vector<cv::Point2f> good_new;
            for(uint i = 0; i < p0.size(); i++)
            {
                // Select good points
                if(status[i] == 1) {
                    good_new.push_back(p1[i]);
                    // draw the tracks
                    cv::line(mask,p1[i], p0[i], colors[i], 2);
                    cv::circle(*frame, p1[i], 5, colors[i], -1);
                }
            }

            if (debug) {
                cv::Mat img;
                cv::add(*frame, mask, img);
                cv::imshow("Frame", img);
            }

            // Now update the previous frame and previous points
            old_gray = frame_gray.clone();
            p0 = good_new;
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