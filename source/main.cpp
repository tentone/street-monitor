#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <iostream>

class Stenographer {
    public:
        int main() {
            processVideo("highway-traffic.mp4");
            return 0;
        }


        void processVideo(char* fname) {
            cv::VideoCapture cap(fname); 

            if(!cap.isOpened()) {
                std::cout << "Error opening video stream or file" << std::endl;
                return;
            }

            while(1) {
                cv::Mat frame;
                cap >> frame;

                // If the frame is empty, break immediately
                if (frame.empty()) {
                    break;
                }
                
                cv::imshow( "Frame", frame );
                
                processFrame(&frame);

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


        void processFrame(cv::Mat *frame) {
            // Detect the keypoints using SURF Detector
            int minHessian = 400;
            cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create( minHessian );
            std::vector<cv::KeyPoint> keypoints;
            detector->detect(*frame, keypoints);

            // Draw keypoints
            cv::Mat img_keypoints;
            cv::drawKeypoints(*frame, keypoints, img_keypoints);

            // Show detected (drawn) keypoints
            cv::imshow("SURF", img_keypoints);
        }
};

int main()
{
    return Stenographer().main();
}