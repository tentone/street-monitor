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

#pragma once

class OpticalFlow {
    public:
        // Dense vars
        cv::Mat dense_flow_frame;

        // Sparse Vars
        cv::Mat sparse_old_gray;
        std::vector<cv::Point2f> sparse_p0, sparse_p1;
        cv::Mat sparse_mask;

        // Random colors for debug
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
			cv::cvtColor(*frame, dense_flow_frame, cv::COLOR_BGR2GRAY);

            // Sparse Vars
            cv::cvtColor(*frame, sparse_old_gray, cv::COLOR_BGR2GRAY);
            cv::goodFeaturesToTrack(sparse_old_gray, sparse_p0, 0, 0.1, 5);
            sparse_mask = cv::Mat::zeros(frame->size(), frame->type());

            // Generate random colors for debug
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
            cv::Mat frame_copy = frame->clone();

            cv::Mat frame_gray;
            cv::cvtColor(frame_copy, frame_gray, cv::COLOR_BGR2GRAY);

            // Calculate optical flow
            std::vector<uchar> status;
            std::vector<float> err;
            cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS), 10, 0.03);

            // Lucas-kanade optical flow
            cv::calcOpticalFlowPyrLK(sparse_old_gray, frame_gray, sparse_p0, sparse_p1, status, err, cv::Size(15, 15), 2, criteria);

            std::vector<cv::Point2f> track_points;
            for(uint i = 0; i < sparse_p0.size(); i++)
            {
                // Select good points
                if(status[i] == 1) {
                    track_points.push_back(sparse_p1[i]);

                    // Draw the tracks
                    if (debug) {
                        cv::line(sparse_mask, sparse_p1[i], sparse_p0[i], colors[i % 100], 1);
                        cv::circle(frame_copy, sparse_p1[i], 2, colors[i % 100], -1);
                    }
                }
            }

            if (debug) {
                cv::Mat img;
                cv::add(frame_copy, sparse_mask, img);
                cv::imshow("Optical Flow Sparse", img);
            }

            // Now update the previous frame and previous points
            sparse_old_gray = frame_gray.clone();
            sparse_p0 = track_points;
        }

        /**
         * @brief Calculate optical flow for the new frame combined with the previus frame stored in "dense_flow_frame"
         * 
         * @param frame New frame to calculate optical flow.
         */
        cv::Mat dense_farneback(cv::Mat *frame)
        {   
            cv::Mat next;
            cv::cvtColor(*frame, next, cv::COLOR_BGR2GRAY);
            cv::Mat flow(dense_flow_frame.size(), CV_32FC2);
            cv::calcOpticalFlowFarneback(dense_flow_frame, next, flow, 0.5, 3, 15, 3, 3, 3.0, 0);

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

            dense_flow_frame = next;

            return flow;
        }
};