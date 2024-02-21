#ifndef DBSCAN_H
#define DBSCAN_H

#include <vector>

struct Point{
    float x;
    float y;
};

int dbscan(const std::vector<Point> &input, std::vector<int> &labels, double eps, int min);

#endif /*DBSCAN_H*/
