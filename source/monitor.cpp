#include <iostream>
#include <sstream>
#include <future>
#include <thread>
#include <functional>
#include <exception>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/objdetect.hpp>

#include "yolo_detector.cpp"
#include "optical_flow.cpp"
#include "haar_detector.cpp"
#include "background_subtractor.cpp"
#include "features.cpp"
#include "street_object.cpp"
#include "math_utils.cpp"

#pragma once

class Monitor {
	public:
		OpticalFlow optical_flow;

		HaarDetector car_haar = HaarDetector("./models/haar/car.xml");

		YOLODetector yolo = YOLODetector("./models/yolo/yolov5x.onnx", "./models/yolo/yolo.names");

		BackgroundSubtractor background_detector;

		/**
		 * @brief Objects visible in the scene.
		 */
        std::vector<StreetObject> objects;

		int skip_frames = 500;

		int frame_count = 0;

		/**
		 * @brief Initialize the monitor detector using information from the first frame.
		 * 
		 * @param fname 
		 */
		void initialize(cv::Mat *frame) {
			optical_flow.initialize(frame);
		}
		
		/**
		 * @brief Process a frame of the video feed, can be obtained from camera, video file, dataset etc.
		 * 
		 * @param fname 
		 */
		void processFrame(cv::Mat *frame) {
			if(frame_count < skip_frames) {
				frame_count++;
				return;
			}

			if (frame_count == skip_frames) {
				this->initialize(frame);
				frame_count++;
				return;
			}

			// optical_flow.sparse(frame);

			cv::Mat mov = background_detector.update(frame);
			std::vector<cv::KeyPoint> moving = background_detector.segmentBlobs(frame, &mov);
			
			static const float tracking_speed = 40.0; 

			// Iterate list of moving objects
			for (int i = 0; i < moving.size(); i++) {
				// Check if the moving box intersects one of the objects 
				for (int j = 0; j < this->objects.size(); j++) {
					// Object still has not been updated in this frame.
					if (this->objects[j].frame < frame_count) {
						if (intersectPointCircle(moving[i].pt, tracking_speed, this->objects[j].position())) {
							cv::Point pos = cv::Point(moving[i].pt.x, moving[i].pt.y);
							this->objects[j].updatePosition(pos, frame_count);
						}
					}
				}
			}


			// If an object has not been seen for more than n frames remove it
			static const int max_age = 10;
			for (auto obj_ptr = this->objects.begin(); obj_ptr < this->objects.end(); obj_ptr++) {
				int age = frame_count - (*obj_ptr).frame;
				if (age > max_age) {
					obj_ptr = this->objects.erase(obj_ptr);
					continue;
				}
			}

			if (frame_count % 30 == 0){
				// Detect objects using the YOLO DNN
				std::vector<YOLOObject> yolo_objs = yolo.detect(frame);
				auto yolo_obj = yolo_objs.begin();

				// Check if the boxes detected match one of the objects.
				while (yolo_obj < yolo_objs.end()) {
					
					bool exists = false;

					for(auto obj_ptr = this->objects.begin(); obj_ptr < this->objects.end(); obj_ptr++) {	
						// Scale the box to prevent false detections
						float scale = 0.9;
						cv::Rect box = yolo_obj->box;
						box.width *= scale;
						box.height *= scale;

						// Center of the bouding box 
						cv::Point center = cv::Point(box.x + box.width / 2.0, box.y + box.height / 2.0);

						if (obj_ptr->insideRect(box)) {
							obj_ptr->size = cv::Size(box.width / scale, box.height / scale);
							exists = true;
							break;
						}
					}

					// Create new object in the list
					if (!exists) {
						StreetObject obj;

						// Vehicles
						if (yolo_obj->class_id >= 2 && yolo_obj->class_id <= 7) {
							obj.category = vehicle;
						// Pedestrians
						} else if (yolo_obj->class_id < 2) {
							obj.category = pedestrian;
						} else {
							obj.category = unknown;
						}

						cv::Rect box = yolo_obj->box;
						obj.size = cv::Size(box.width, box.height);
						obj.updatePosition(cv::Point(box.x + box.width / 2.0, box.y + box.height / 2.0), frame_count);
						this->objects.push_back(obj);
					}


					yolo_obj++;
				}
			}


			this->drawDebug(frame);

			frame_count++;
		}

		/**
		 * Draw debug information into screen.
		 */
		void drawDebug(cv::Mat *frame) {
			// Draw objects into the frame
			auto obj_ptr = this->objects.begin();
			while (obj_ptr < this->objects.end()) {
				if (obj_ptr->length() > 0) {
					cv::Point pos = obj_ptr->position();
					
					cv::Scalar color = obj_ptr->category == vehicle ? cv::Scalar(0,255,0) : obj_ptr->category == pedestrian ? cv::Scalar(255,0,0) : cv::Scalar(0,0,255);
					cv::circle(*frame, pos, 5, color, cv::FILLED, cv::LINE_8);
					

					cv::Point direction = obj_ptr->direction();
					cv::line(*frame, pos, pos + direction * 2, color, 1, cv::LINE_8);

					cv::putText(*frame, std::to_string(obj_ptr->id), cv::Point(pos.x + 10, pos.y), cv::FONT_HERSHEY_PLAIN, 1.0, color, 1, cv::LINE_AA);
					
					const static float factor = 3.0;
					const static float y_factor = 900;

					int speed = std::round(size(direction) * (factor + (pos.y / y_factor)));
					cv::putText(*frame, std::to_string(speed) + " kph", cv::Point(pos.x + 10, pos.y + 20), cv::FONT_HERSHEY_PLAIN, 1.0, color, 1, cv::LINE_AA);


					cv::Rect rect = obj_ptr->boudingBox();
					cv::rectangle(*frame, cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height), color, 1);
				}
	
				obj_ptr++;
			}

			cv::imshow("Frame", *frame);
			cv::waitKey(1);
		}

		/**
		 * @brief Start processing from computer camera.
		 */
		void startCamera() {
			cv::VideoCapture cap(0); 

			if(!cap.isOpened()) {
				std::cout << "Error opening video stream or file" << std::endl;
				return;
			}

			// Processing loop
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
		 * @brief Start the speed camera detector with a specific file
		 */
		void startVideo(const std::string fname) {
			cv::VideoCapture cap(fname); 

			if(!cap.isOpened()) {
				std::cout << "Error opening video stream or file" << std::endl;
				return;
			}

			// Processing loop
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
};
