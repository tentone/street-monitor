#include <opencv2/core.hpp>
#include <math.h>

/**
 * @brief Check the inteserction between a circle and a rect.
 * 
 * @param center Center of the circle.
 * @param radius Radius of the circle.
 * @param rect Rect object.
 * @return True if there is intesection, false otherwise.
 */
bool intersectCircleRect(cv::Point center, float radius, cv::Rect rect)
{
    float circle_distance_x = abs(center.x - rect.x);
    float circle_distance_y = abs(center.y - rect.y);

    if (circle_distance_x > (rect.width/2 + radius)) { return false; }
    if (circle_distance_y > (rect.height/2 + radius)) { return false; }

    if (circle_distance_x <= (rect.width/2)) { return true; } 
    if (circle_distance_y <= (rect.height/2)) { return true; }

    float corner_dst_sq = pow(circle_distance_x - rect.width/2, 2) + pow(circle_distance_y - rect.height/2, 2);

    return (corner_dst_sq <= pow(radius, 2));
}


/**
 * @brief Check the inteserction between two rectangles.
 * 
 * @param a Rect object.
 * @param b Rect object.
 * @return True if there is intesection, false otherwise.
 */
bool intersectRect(cv::Rect a, cv::Rect b)
{
    return (a & b).area() > 0;
}