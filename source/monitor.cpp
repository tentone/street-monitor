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

#pragma once

class Monitor {
	public:
		OpticalFlow optical_flow;

		HaarDetector car_haar = HaarDetector("./models/haar/car.xml");

		// YOLODetector yolo = YOLODetector("./models/yolo/yolov5x.onnx", "./models/yolo/yolo.names");

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
			
			// Iterate list of moving objects
			for (int i = 0; i < moving.size(); i++) {
				bool exists = false;
				
				// Check if the moving box intersects one of the objects 
				for (int j = 0; j < this->objects.size(); j++) {
					// Object still has not been updated in this frame.
					if (this->objects[j].frame < frame_count) {
						if (this->objects[j].collidesKeypoint(moving[i])) {
							this->objects[j].updateKeypoint(moving[i], frame_count);
							exists = true;
						}
					}
				}

				// Create new object in the list
				if (!exists) {
					StreetObject obj;
					obj.updateKeypoint(moving[i], frame_count);
					this->objects.push_back(obj);

				}
			}


			// // If an object has not been seen for more than n frames remove it
			// static const int max_age = 100;
			// auto obj_pointer = this->objects.begin();
			// while (obj_pointer < this->objects.end()) {
			// 	int age = frame_count - (*obj_pointer).frame;
			// 	if (age > max_age) {
			// 		obj_pointer = this->objects.erase(obj_pointer);
			// 		continue;
			// 	}

			// 	obj_pointer++;
			// }

	
			// if (frame_count % 30 == 0){
			// 	// Detect objects using the YOLO DNN
			// 	std::vector<YOLOObject> yolo_objects = yolo.detect(frame);
			// 	auto yolo_object = yolo_objects.begin();

			// 	// Check if the boxes detected match one of the objects.
			// 	while (yolo_object < yolo_objects.end()) {
			// 		auto obj_pointer = this->objects.begin();
			// 		while (obj_pointer < this->objects.end()) {
			// 			if (obj_pointer->collidesRect(yolo_object->box)) {
			// 				// Vehicles
			// 				if (yolo_object->class_id >= 2 && yolo_object->class_id <= 7) {
			// 					obj_pointer->category = vehicle;
			// 				// Pedestrians
			// 				} else if (yolo_object->class_id < 2) {
			// 					obj_pointer->category = pedestrian;
			// 				}
			// 				break;
			// 			}
			// 			obj_pointer++;
			// 		}
			// 		yolo_object++;
			// 	}
			// }


			this->drawDebug(frame);

			frame_count++;
		}

		/**
		 * Draw debug information into screen.
		 */
		void drawDebug(cv::Mat *frame) {
			// Draw objects into the frame
			auto obj_pointer = this->objects.begin();
			while (obj_pointer < this->objects.end()) {
				if (obj_pointer->length() > 0) {
					StreetObjectFrameInfo info = obj_pointer->last();
					cv::Rect rect = info.boudingBox();
					cv::Scalar color = obj_pointer->category == vehicle ? cv::Scalar(0,255,0) : obj_pointer->category == pedestrian ? cv::Scalar(255,0,0) : cv::Scalar(0,0,255);
					cv::rectangle(*frame, cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height), color, 2);
				}
	
				obj_pointer++;
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
