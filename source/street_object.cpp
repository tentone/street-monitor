#include <sstream>
#include <vector>

#include <opencv2/core.hpp>

#include "math_utils.cpp"

#pragma once

/**
 * @brief Possible categories detected by the system.
 */
enum Category { unknown, vehicle, pedestrian };

/**
 * @brief Detection of an object in an frame
 */
class StreetObjectFrameInfo {
    public:
        /**
         * @brief Frame number for this detection.
         */
        int frame;

        /**
         * @brief Center of the object detected
         */
        cv::Point center;
        
        /**
         * @brief Size of the bounding box for the object in this frame
         */
        cv::Size size;

    /**
     * @brief Get the bouding box for this frame.
     */
    cv::Rect boudingBox() {
        cv::Point corner = cv::Point(this->center.x - this->size.width / 2, this->center.y - this->size.height / 2);

        return cv::Rect(corner, this->size);
    }
};

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
        
        

        // Detection of this object
        std::vector<StreetObjectFrameInfo> frames;

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
         * @return StreetObjectFrameInfo 
         */
        StreetObjectFrameInfo last() {
            if (this->length() == 0) {
                throw "There are no frames for the object.";
            }

            return this->frames.back();
        }

        /**
         * @brief Update with keypoint position.
         */
        void updateKeypoint(cv::KeyPoint kp, int frame)
        {
            this->frame = frame;

            StreetObjectFrameInfo info;
            info.frame = frame;
            info.center = cv::Point(kp.pt.x, kp.pt.y);
            info.size = cv::Size(kp.size, kp.size);
            this->frames.push_back(info);
        }

        /**
         * @brief Check if object collides with keypoint from new frame.
         */
        bool collidesKeypoint(cv::KeyPoint kp)
        {
            auto frame = this->last();
            auto box = this->boudingBox();
          
            return intersectCircleRect(kp.pt, kp.size, box);
        }

        /**
         * @brief Check if object collides with rect from new frame.
         */
        bool collidesRect(cv::Rect rect)
        {
            auto frame = this->last();
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

            return this->last().boudingBox();
        }

        /**
         * @brief Return a vector with the direction of the object.
         * 
         * The size of the vector can be used to estimate its velocity in the image.
         * 
         * @return cv::Point 
         */
        cv::Point averageDirection() {
            if (this->length() < 2) {
                throw "At least two frames are required to calculate direction.";
            }
            
            cv::Point avg = cv::Point();

            for (int i = 0; i < this->frames.size() - 1; i++) {
                avg.x += this->frames[i + 1].center.x - this->frames[i].center.x;
                avg.y += this->frames[i + 1].center.y - this->frames[i].center.y;
            }

            avg.x /= this->frames.size();
            avg.y /= this->frames.size();

            return avg;
        }
};


