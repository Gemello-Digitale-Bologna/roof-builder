#pragma once

#include <iostream> 
#include <vector>
#include <algorithm>
#include <cmath>
#include <omp.h>
#include "../Geometry.h"
#include "../nanoflann/nanoflann.hpp"

class Grid {
public:
    Grid() {};
    void Init(const std::vector<Point3>& i_points, float cell_size, float tol, float radius);
    void Clear();

    void FillHoles(uint16_t tol, int numIterations = 1);
    Point3 GetGridPointCoord(int grid_x, int grid_y);
    std::vector<std::vector<float>> GetBooleanRoof();
    std::vector<Point3> GetGlobalPoints(std::vector<Point2>& points);

    float GetMinX() const { return minX; }
    float GetMaxX() const { return maxX; }
    float GetMinY() const { return minY; }
    float GetMaxY() const { return maxY; }
    float GetCellSize() const { return cellSize; }
    const std::vector<Point3>& GetPoints() const { return points; }
    const std::vector<std::vector<float>>& GetHeightMat() const { return heightMat; }
   
private:
    float minX, maxX, minY, maxY;
    float cellSize;
    std::vector<Point3> points;
    std::vector<std::vector<float>> heightMat;
};
