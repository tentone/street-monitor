#include <sstream>
#include <vector>

#include <opencv2/core.hpp>

// Possible car colors detected
enum Category { unknown, car, person, bus, truck };

// Detection of an object in an frame
class StreetObjectFrameInfo {
    public:
        // Frame number where the object is placed
        int frame;

        // Center of the object detected
        cv::Vec3d center;
        
        // Size of the bounding box for the object in this frame
        cv::Vec3d size;
};


// Description of the car
class StreetObject {
    private: 
        static int _id;

    public:
        // Identifier (sequential)
        int id;

        // Category of the car
        Category category;
        
        // Vector of detection of this object
        std::vector<StreetObjectFrameInfo> frames;

        StreetObject() {
            this->id = StreetObject::_id++;
            this->category = unknown;
        }
};


