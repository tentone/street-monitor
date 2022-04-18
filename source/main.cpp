#include <iostream>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>

class SpeedCamera {
    public:
        int main() {
            processVideo("highway-traffic.mp4");
            return 0;
        }

        cv::Mat opticalFlowFrame;

        void processVideo(char* fname) {
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
                
                cv::imshow( "Frame", frame );
                
                // surfFeatures(&frame);
                // opticalFlow(&frame);
                backgroundSub(&frame);

                char c=(char)cv::waitKey(25);
                if(c==27) {
                    break;
                }
            
            }

            // When everything done, release the video capture object
            cap.release();

            // Closes all the frames
            cv::destroyAllWindows();
        }

        // Calculate optical flow for new frame.
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
            cv::imshow("FG Mask", backgroundMask);
        }

        // Calculate and display SURF features
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

int main()
{
    return SpeedCamera().main();
}