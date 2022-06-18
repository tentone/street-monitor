#include <sstream>
#include <vector>

#include <opencv2/core.hpp>

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
        cv::Point corner = cv::Point(this->center.x - this->size.x / 2, this->center.y - this->size.y / 2);

        return cv::Rect(corner, this->size.clone());
    }

};


/**
 * @brief Represents an object that is moving trough the street.
 * 
 * Can be a vehicle or a pedestrian.
 */
class StreetObject {
    private: 
        static int _id;

    public:
        // Identifier (sequential)
        int id;

        // Category of the object detected
        Category category;
        
        // Detection of this object
        std::vector<StreetObjectFrameInfo> frames;

        StreetObject() {
            this->id = StreetObject::_id++;
            this->category = unknown;
        }
        
        /**
         * @brief Number of frames available for this object.
         * 
         * @return int 
         */
        int length() {
            return this->frames.size();
        }

        /**
         * @brief Get the last bouding box for this object.
         */
        cv::Rect boudingBox() {
            int size = this->frames.size();
            if (size == 0) {
                return cv::Rect();
            }

            this->frames[size - 1];


            return cv::Rect();
        }

        /**
         * @brief Return a vector with the direction of the object.
         * 
         * The size of the vector can be used to estimate its velocity in the image.
         * 
         * @return cv::Vec3d 
         */
        cv::Vec3d directionVector(int avg_frames = 3) {
            if (this->frames.size() < 2) {
                return cv::Vec3d();
            }

            // TODO <ADD CODE HERE>

            return cv::Vec3d();
        }
};


