#include <iostream>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>

class VideoStabilizer {
    public:
		/**
		 * @brief Flag to display debug information.
		 */
		bool debug = true;

        /**
		 * @brief Insert a new frame into the stabilization process.
		 * 
		 * The first frame is used as framing reference and next frames are stabilized compared to the first.
		 * 
		 * If there are too many moving objects in the frame the framing process might not be achiebable.
		 * 
		 * @param frame Frame to be stabilized.
		 */
		void stabilize(cv::Mat *frame) {
			// TODO <STABILIZATION FRAME>
		}
};