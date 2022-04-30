#include <sstream>

// Possible car colors detected
enum Color { red, green, blue, black, white, grey };

// Description of the car
class Car {
    private: 
        static int _id;

    public:
        // Identifier (sequential)
        int id;

        // License plate
        std::string licensePlate;

        // Color of the car
        Color color;
        
        Car() {
            this->id = Car::_id++;
            this->color = black;
        }
};
