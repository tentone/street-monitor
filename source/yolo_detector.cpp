#include <sstream>
#include <fstream>

#include <opencv2/opencv.hpp>

// Constants.
const float INPUT_WIDTH = 640.0;
const float INPUT_HEIGHT = 640.0;
const float SCORE_THRESHOLD = 0.5;
const float NMS_THRESHOLD = 0.45;
const float CONFIDENCE_THRESHOLD = 0.7;

// Text parameters.
const float FONT_SCALE = 0.7;
const int FONT_FACE = cv::FONT_HERSHEY_SIMPLEX;
const int THICKNESS = 1;

// Colors.
cv::Scalar BLACK = cv::Scalar(0,0,0);
cv::Scalar BLUE = cv::Scalar(255, 178, 50);
cv::Scalar YELLOW = cv::Scalar(0, 255, 255);
cv::Scalar RED = cv::Scalar(0,0,255);

class YOLODetector {
	public:
		/**
		 * @brief DNN model used to detect objects.
		 */
		cv::dnn::Net net;
		

		/**
		 * @brief List of classes that the DNN can classify.
		 */
		std::vector<std::string> class_list;

		/**
		 * @brief Initialize the detector with a specific DNN model.
		 * 
		 * @param model Path to the DNN model to be used.
		 */
		YOLODetector(std::string modelf, std::string classf)
		{
			this->load_classes(classf);

			// Load model.
			this->net = cv::dnn::readNet(modelf); 
		}


		/**
		 * @brief Process a frame to detect objects using the YOLO V5 model.
		 * 
		 * @param frame 
		 */
		std::vector<cv::Mat> process_frame(cv::Mat *frame) {

			std::vector<cv::Mat> detections;
			detections = this->detect(*frame, net);

			cv::Mat clone = frame->clone();

			cv::Mat img = this->draw(clone, detections, this->class_list);

			//The function getPerfProfile returns the overall time for inference(t) and the timings for each of the layers(in layersTimes)
			// std::vector<double> layersTimes;
			// double freq = cv::getTickFrequency() / 1000;
			// double t = net.getPerfProfile(layersTimes) / freq;
			// std::string label = cv::format("Inference time : %.2f ms", t);
			// cv::putText(img, label, cv::Point(20, 40), FONT_FACE, FONT_SCALE, RED);

			cv::imshow("YOLO", clone);

			return detections;
		}


		void load_classes(std::string fname) {
			// Load class list.
			std::ifstream ifs(fname);
			
			// Read line by line
			std::string line;
			while (getline(ifs, line))
			{
				this->class_list.push_back(line);
			}
		}

		// Draw the predicted bounding box.
		void draw_label(cv::Mat& frame, std::string label, int left, int top)
		{
			// Display the label at the top of the bounding box.
			int baseLine;
			cv::Size label_size = cv::getTextSize(label, FONT_FACE, FONT_SCALE, THICKNESS, &baseLine);
			top = std::max(top, label_size.height);

			// Top left corner.
			cv::Point tlc = cv::Point(left, top);

			// Bottom right corner.
			cv::Point brc = cv::Point(left + label_size.width, top + label_size.height + baseLine);

			// Draw black rectangle.
			cv::rectangle(frame, tlc, brc, BLACK, cv::FILLED);

			// Put the label on the black rectangle.
			cv::putText(frame, label, cv::Point(left, top + label_size.height), FONT_FACE, FONT_SCALE, YELLOW, THICKNESS);
		}

		std::vector<cv::Mat> detect(cv::Mat &frame, cv::dnn::Net &net)
		{
			// Convert to blob.
			cv::Mat blob;
			cv::dnn::blobFromImage(frame, blob, 1./255., cv::Size(INPUT_WIDTH, INPUT_HEIGHT), cv::Scalar(), true, false);

			net.setInput(blob);

			// Forward propagate.
			std::vector<cv::Mat> outputs;
			net.forward(outputs, net.getUnconnectedOutLayersNames());

			return outputs;
		}

		cv::Mat draw(cv::Mat &frame, std::vector<cv::Mat> &outputs, const std::vector<std::string> &class_name) 
		{
			// Initialize std::vectors to hold respective outputs while unwrapping detections.
			std::vector<int> class_ids;
			std::vector<float> confidences;
			std::vector<cv::Rect> boxes; 

			// Resizing factor.
			float x_factor = frame.cols / INPUT_WIDTH;
			float y_factor = frame.rows / INPUT_HEIGHT;

			float *data = (float *)outputs[0].data;

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
					cv::Mat scores(1, class_name.size(), CV_32FC1, classes_scores);

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
				cv::rectangle(frame, cv::Point(left, top), cv::Point(left + width, top + height), BLUE, 3*THICKNESS);

				// Get the label for the class name and its confidence.
				std::string label = cv::format("%.2f", confidences[idx]);
				label = class_name[class_ids[idx]] + ":" + label;
				
				// Draw class labels.
				draw_label(frame, label, left, top);
			}

			return frame;
		}

};