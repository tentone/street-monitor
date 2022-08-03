#include <opencv2/core.hpp>
#include <math.h>

#pragma once

/**
 * @brief Calculate the size of the vector.
 */
float size(cv::Point a)
{
    return sqrt(pow(a.x, 2) + pow(a.y, 2));
}


/**
 * @brief Calculate the distance between two points.
 */
float distance(cv::Point a, cv::Point b)
{
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

/**
 * @brief Check the inteserction between a circle and a rect.
 * 
 * @param center Center of the circle.
 * @param radius Radius of the circle.
 * @param rect Rect object.
 * @return True if there is intesection, false otherwise.
 */
bool intersectPointCircle(cv::Point center, float radius, cv::Point point)
{
    float dist = distance(center, point);
    
    return dist <= radius;
}


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