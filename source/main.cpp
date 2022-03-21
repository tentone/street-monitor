#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

class Stenographer {
    public:
        int main() {
            std::string image_path = cv::samples::findFile("derp.jpg");
            cv::Mat img = cv::imread(image_path, cv::IMREAD_COLOR);
            if(img.empty())
            {
                std::cout << "Could not read the image: " << image_path << std::endl;
                return 1;
            }



            // Detect the keypoints using SURF Detector
            int minHessian = 400;
            cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create( minHessian );
            std::vector<cv::KeyPoint> keypoints;
            detector->detect(img, keypoints);

            // Draw keypoints
            cv::Mat img_keypoints;
            cv::xfeatures2d::drawKeypoints(img, keypoints, img_keypoints);

            // Show detected (drawn) keypoints
            cv::imshow("SURF Keypoints", img_keypoints );
            cv::imshow("Image window", img);

            // Wait for a keystroke in the window
            int k = cv::waitKey(0);
            if(k == 's')
            {
                cv::imwrite("starry_night.png", img);
            }


            return 0;
        }


};

int main()
{
    return Stenographer().main();
}