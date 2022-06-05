#include <sstream>

// Possible car colors detected
enum Category { unknown, car, person, truck };

// Description of the car
class Object {
    private: 
        static int _id;

    public:
        // Identifier (sequential)
        int id;

        // Category of the car
        Category category;
        
        Object() {
            this->id = Object::_id++;
            this->category = unknown;
        }
};


class ObjectFrame {
    public:
        int frame;

};
