#include <sstream>
#include <fstream>

#include <opencv2/opencv.hpp>

cv::Scalar BLACK = cv::Scalar(0,0,0);
cv::Scalar BLUE = cv::Scalar(255,0,0);
cv::Scalar YELLOW = cv::Scalar(0,255, 255);
cv::Scalar RED = cv::Scalar(0,0,255);

class YOLODetector {
	public:
		/**
		 * @brief Flag to display debug information.
		 */
		bool debug = true;

		/**
		 * @brief Width of the image to be processed. The image is resized to match this size.
		 */
		float input_width = 640.0;
		
		/**
		 * @brief Height of the image to be processed. The image is resized to match this size.
		 */
		float input_height = 640.0;

		/**
		 * @brief Threshold used in non maximum suppression.
		 */
		const float NMS_THRESHOLD = 0.45;

		/**
		 * @brief Score threshold to consider that the object is of a specific object class. (e.g. confidence that is a car).
		 */
		const float SCORE_THRESHOLD = 0.45;

		/**
		 * @brief Confidence threshold to consider the object detected. Confidence that those pixels are a object.
		 */
		const float CONFIDENCE_THRESHOLD = 0.2;

		/**
		 * @brief DNN model used to detect objects.
		 */
		cv::dnn::Net net;
		
		/**
		 * @brief List of classes that the DNN can classify.
		 */
		std::vector<std::string> classes;

		/**
		 * @brief Initialize the detector with a specific DNN model.
		 * 
		 * @param model Path to the DNN model to be used.
		 */
		YOLODetector(std::string modelf, std::string classf, float width = 640.0, float height = 640.0)
		{	
			// Load classes
			this->loadClasses(classf);

			this->input_width = width;
			this->input_height = height;

			// Load model.
			this->net = cv::dnn::readNet(modelf); 
		}


		/**
		 * @brief Process a frame to detect objects using the YOLO V5 model.
		 * 
		 * @param frame Frame to be processed
		 */
		std::vector<cv::Mat> detect(cv::Mat *frame, std::string debug_window = "YOLO") {
			std::vector<cv::Mat> detections = this->classify(*frame);

			if (this->debug) {
				cv::Mat clone = frame->clone();
				cv::Mat img = this->drawPredictions(clone, detections);

				// The function getPerfProfile returns the overall time for inference(t) and the timings for each of the layers(in layersTimes)
				std::vector<double> layersTimes;
				double freq = cv::getTickFrequency() / 1000;
				double t = net.getPerfProfile(layersTimes) / freq;
				std::string label = cv::format("Time: %.2f ms", t);
				cv::putText(img, label, cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 0.7, RED);

				cv::imshow(debug_window, clone);
			}


			return detections;
		}


		/**
		 * @brief Load list of classes from file, line by line.
		 * 
		 * @param fname File to load classes from
		 */
		void loadClasses(std::string fname) {
			// Load class list.
			std::ifstream ifs(fname);
			
			// Read line by line
			std::string line;
			while (getline(ifs, line))
			{
				this->classes.push_back(line);
			}
		}

		/**
		 * @brief  Draw one predicted bounding box alongside with label.
		 * 
		 * @param frame Frame to draw the box in
		 * @param label Label of the box to draw.
		 * @param x X coordinate (left corner)
		 * @param y Y coordinate (top corner)
		 */
		void drawBox(cv::Mat& frame, std::string label, int x, int y)
		{
			// Display the label at the top of the bounding box.
			int baseLine;
			cv::Size label_size = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.7, 1, &baseLine);

			// Dont excess the image height
			y = std::max(y, label_size.height);

			// Top left corner.
			cv::Point tlc = cv::Point(x, y);

			// Bottom right corner.
			cv::Point brc = cv::Point(x + label_size.width, y + label_size.height + baseLine);

			// Draw black rectangle.
			cv::rectangle(frame, tlc, brc, BLACK, cv::FILLED);

			// Put the label on the black rectangle.
			cv::putText(frame, label, cv::Point(x, y + label_size.height), cv::FONT_HERSHEY_SIMPLEX, 0.7, YELLOW, 1);
		}

		/**
		 * @brief Detect object in an image using the loaded model.
		 * 
		 * @param frame Frame to detect object in.
		 * @return std::vector<cv::Mat> 
		 */
		std::vector<cv::Mat> classify(cv::Mat &frame)
		{
			// Convert to blob.
			cv::Mat blob;
			cv::dnn::blobFromImage(frame, blob, 1./255., cv::Size(this->input_width, this->input_height), cv::Scalar(), true, false);

			// Network input
			this->net.setInput(blob);

			// Forward propagate.
			std::vector<cv::Mat> predictions;
			this->net.forward(predictions, this->net.getUnconnectedOutLayersNames());

			return predictions;
		}

		/**
		 * @brief Draw the predictions obtained from the model into the image for debug.
		 * 
		 * @param frame Frame to draw the predictions on.
		 * @param outputs 
		 * @param class_name 
		 * @return cv::Mat 
		 */
		cv::Mat drawPredictions(cv::Mat &frame, std::vector<cv::Mat> &predictions) 
		{
			// Initialize std::vectors to hold respective outputs while unwrapping detections.
			std::vector<int> class_ids;
			std::vector<float> confidences;
			std::vector<cv::Rect> boxes; 

			// Resizing factor.
			float x_factor = frame.cols / this->input_width;
			float y_factor = frame.rows / this->input_height;

			float *data = (float *)predictions[0].data;

			const int dimensions = 85;
			const int rows = 25200;

			// Iterate through 25200 detections.
			for (int i = 0; i < rows; ++i) 
			{
				float confidence = data[4];

				// Discard bad detections and continue.
				if (confidence >= CONFIDENCE_THRESHOLD) 
				{
					float * classes_scores = data + 5;

					// Create a 1x85 cv::Mat and store class scores of 80 classes.
					cv::Mat scores(1, this->classes.size(), CV_32FC1, classes_scores);

					// Perform minMaxLoc and acquire index of best class score.
					cv::Point class_id;
					double max_class_score;
					minMaxLoc(scores, 0, &max_class_score, 0, &class_id);
					
					// Continue if the class score is above the threshold.
					if (max_class_score > SCORE_THRESHOLD) 
					{
						// Store class ID and confidence in the pre-defined respective std::vectors.

						confidences.push_back(confidence);
						class_ids.push_back(class_id.x);

						// Center.
						float cx = data[0];
						float cy = data[1];
						// Box dimension.
						float w = data[2];
						float h = data[3];
						// Bounding box coordinates.
						int left = int((cx - 0.5 * w) * x_factor);
						int top = int((cy - 0.5 * h) * y_factor);
						int width = int(w * x_factor);
						int height = int(h * y_factor);
						// Store good detections in the boxes std::vector.
						boxes.push_back(cv::Rect(left, top, width, height));
					}

				}
				// Jump to the next column.
				data += 85;
			}

			// Perform Non Maximum Suppression and draw predictions.
			std::vector<int> indices;

			cv::dnn::NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, indices);

			for (int i = 0; i < indices.size(); i++) 
			{
				int idx = indices[i];
				cv::Rect box = boxes[idx];

				int left = box.x;
				int top = box.y;
				int width = box.width;
				int height = box.height;

				// Draw bounding box.
				cv::rectangle(frame, cv::Point(left, top), cv::Point(left + width, top + height), BLUE, 3*1);

				// Get the label for the class name and its confidence.
				std::string label = cv::format("%.2f", confidences[idx]);
				label = this->classes[class_ids[idx]] + ":" + label;
				
				// Draw class labels.
				drawBox(frame, label, left, top);
			}

			return frame;
		}

};