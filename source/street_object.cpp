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
        cv::Point corner = cv::Point(this->center.x - this->size.width / 2, this->center.y - this->size.height / 2);

        return cv::Rect(corner, this->size);
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

            return this->frames[size - 1].boudingBox();
        }

        /**
         * @brief Return a vector with the direction of the object.
         * 
         * The size of the vector can be used to estimate its velocity in the image.
         * 
         * @return cv::Point 
         */
        cv::Point averageDirection() {
            if (this->frames.size() < 2) {
                return cv::Point();
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


