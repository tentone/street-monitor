#include <sstream>
#include <vector>

#include <opencv2/core.hpp>

#include "math_utils.cpp"

#pragma once

/**
 * @brief Possible categories detected by the system.
 */
enum Category { unknown, vehicle, pedestrian };

int _id = 0;

/**
 * @brief Represents an object that is moving trough the street.
 * 
 * Can be a vehicle or a pedestrian.
 */
class StreetObject {
    public:
        /**
         * @brief Sequential identifier of the object.
         */
        int id;

        /**
         * @brief The last frame when this object was updated.
         */
        int frame;

        /**
         * @brief Category of the object detected.
         */
        Category category;
        
        /**
         * @brief Size of the bounding box for the object in this frame
         */
        cv::Size size;

        /**
         * @brief Position of this object in the last frames.
         */
        std::vector<cv::Point> frames;

        StreetObject() {
            this->id = _id++;
            this->category = unknown;
        }
        
        /**
         * @brief Number of frames available for this object.
         * 
         * @return int Number of frames for the object.
         */
        int length() {
            return this->frames.size();
        }

        /**
         * @brief Get the last frame of the object.
         * 
         * @return Last point where the object was. 
         */
        cv::Point position() {
            if (this->length() == 0) {
                throw "There are no frames for the object.";
            }

            return this->frames.back();
        }

        /**
         * @brief Update with keypoint position.
         */
        void updatePosition(cv::Point position, int frame)
        {
            this->frame = frame;
            this->frames.push_back(position);
        }

        /**
         * @brief Check if object collides with keypoint from new frame.
         */
        bool insideKeypoint(cv::KeyPoint kp)
        {
            return intersectPointCircle(kp.pt, kp.size, this->position());
        }

        /**
         * @brief Check if object collides with rect from new frame.
         */
        bool insideRect(cv::Rect rect)
        {
            auto frame = this->position();
            auto box = this->boudingBox();
          
            return intersectRect(box, rect);
        }

        /**
         * 
         * @brief Get the last bouding box for this object.
         */
        cv::Rect boudingBox() {
            if (this->length() == 0) {
                throw "There are no frames for the object.";
            }

            cv::Point last = this->position();
            cv::Point corner = cv::Point(last.x - this->size.width / 2, last.y - this->size.height / 2);
            return cv::Rect(corner, this->size);
        }

        /**
         * @brief Return a vector with the direction of the object.
         * 
         * The size of the vector can be used to estimate its velocity in the image.
         * 
         * @return cv::Point 
         */
        cv::Point direction() {
            const int points = 5;

            if (this->frames.size() < points) {
                return cv::Point(0.0, 0.0);
            }

            int length = this->frames.size() - 1;

            return cv::Point(this->frames[length].x - this->frames[length - points + 1].x, this->frames[length].y - this->frames[length - points + 1].y);
        }
};


