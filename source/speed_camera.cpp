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

class SpeedCamera {
    public:
        cv::Mat opticalFlowFrame;

        /**
         * @brief Process a frame of the video feed, can be obtained from camera, video file, dataset etc.
         * 
         * @param fname 
         */
        void processFrame(cv::Mat *frame) {
            cv::imshow( "Frame", *frame );
            
            // surfFeatures(frame);
            // opticalFlow(frame);
            detectCarsHaar(frame);
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
            cvtColor(frame, opticalFlowFrame, cv::COLOR_BGR2GRAY);

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

        /**
         * @brief Detect cars using a haar cascade model. The bouding boxes of the cars found is returned.
         * 
         * @param frame Frame to extract cars.
         */
        void detectCarsHaar(cv::Mat *frame)
        {
            // Load a classifier from its XML description
            cv::CascadeClassifier classifier("./models/haar/car.xml");
            
            // Prepare a vector where the detected features will be stored
            std::vector<cv::Rect> features;

            // Grayscale version of the image
            cv::Mat grayscale;

            // Create a normalized, gray-scale version of the captured image
            cv::cvtColor(*frame, grayscale, cv::COLOR_BGR2GRAY);
            cv::equalizeHist(grayscale, grayscale);

            // Detect the features in the normalized, gray-scale version of the image.
            classifier.detectMultiScale(grayscale, features, 1.1, 3, cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

            // Draw each feature as a separate green rectangle
            for (auto&& feature : features) {
                cv::rectangle(*frame, feature, cv::Scalar(0, 255, 0), 2);
            }

            // Show the captured image and the detected features
            cv::imshow("Cars", *frame);
        }

        /**
         * @brief Calculate optical flow for the new frame combined with the previus frame stored in "opticalFlowFrame"
         * 
         * @param frame New frame to calculate optical flow.
         */
        void opticalFlowDense(cv::Mat *frame)
        {   
            cv::Mat next;

            cv::cvtColor(*frame, next, cv::COLOR_BGR2GRAY);
            cv::Mat flow(opticalFlowFrame.size(), CV_32FC2);
            cv::calcOpticalFlowFarneback(opticalFlowFrame, next, flow, 0.5, 3, 15, 3, 3, 3.0, 0);

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
            cv::imshow("Optical Flow", bgr);

            opticalFlowFrame = next;
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
