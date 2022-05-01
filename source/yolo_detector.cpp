#include <sstream>

#include <opencv2/opencv.hpp>

#define INPUT_WIDTH 640
#define INPUT_HEIGHT 640

class YOLODetector {
	public:
		/**
		 * @brief DNN model used to detect objects.
		 */
		cv::dnn::Net net;
		
		/**
		 * @brief Initialize the detector with a specific DNN model.
		 * 
		 * @param model Path to the DNN model to be used.
		 */
		YOLODetector(std::string model)
		{
			net = cv::dnn::readNet(model); // "../models/yolo/yolov5n.pt");
		}

		/**
		 * @brief Format image before using it to obtain predictions for the model used.
		 * 
		 * @param source 
		 * @return cv::Mat 
		 */
		cv::Mat formatImage(const cv::Mat &source) {
		
			// put the image in a square big enough
			int col = source.cols;
			int row = source.rows;
			int _max = MAX(col, row);
			cv::Mat resized = cv::Mat::zeros(_max, _max, CV_8UC3);
			source.copyTo(resized(cv::Rect(0, 0, col, row)));
			
			// resize to 640x640, normalize to [0,1[ and swap Red and Blue channels
			cv::Mat result;
			cv::dnn::blobFromImage(source, result, 1./255., cv::Size(INPUT_WIDTH, INPUT_HEIGHT), cv::Scalar(), true, false);
		
			return result;
		}

		/**
		 * @brief Obtain a preditions for an image, the image should be first preformated.
		 * 
		 */
		void predict() {
			std::vector<cv::Mat> predictions;
			net.forward(predictions, net.getUnconnectedOutLayersNames());
			const cv::Mat &output = predictions[0];
		}
};