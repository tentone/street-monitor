#include <sstream>
#include <vector>

#include <opencv2/core.hpp>

// Possible car colors detected
enum Category { unknown, car, person, truck };

// Detection of an object in an frame
class ObjectFrameInfo {
    public:
        // Frame number where the object is placed
        int frame;

        // Center of the object detected
        cv::Vec3d center;
        
        // Size of the bounding box for the object in this frame
        cv::Vec3d size;
};


// Description of the car
class Object {
    private: 
        static int _id;

    public:
        // Identifier (sequential)
        int id;

        // Category of the car
        Category category;
        
        // Vector of detection of this object
        std::vector<ObjectFrameInfo> frames;

        Object() {
            this->id = Object::_id++;
            this->category = unknown;
        }
};


