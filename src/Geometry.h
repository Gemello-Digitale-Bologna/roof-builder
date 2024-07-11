#pragma once
#include <cmath>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdint>

class Point3 {
public:
    float x, y, z;

    Point3() : x(0), y(0), z(0) {}

    Point3(float x, float y, float z) : x(x), y(y), z(z) {}

    friend std::ostream& operator<<(std::ostream& os, const Point3& point) {
        os << std::fixed << std::setprecision(3);
        os << point.x << " " << point.y << " " << point.z;
        return os;
    }

    bool operator==(const Point3& other){
        return (x == other.x && y == other.y && z == other.z);
    }
};


class Point2 {
public:
    double x, y;

    Point2() : x(0), y(0) {}

    Point2(double x, double y) : x(x), y(y) {}

    float distance(const Point2& other) const {
        return std::sqrt(std::pow(other.x - x, 2) + std::pow(other.y - y, 2));
    }

    friend std::ostream& operator<<(std::ostream& os, const Point2& point) {
        os << std::fixed << std::setprecision(3);
        os << point.x << " " << point.y;
        return os;
    }
};


class Triangle {
public:
    int i1, i2, i3;

    Triangle(const int& ind1, const int& ind2, const int& ind3) : i1(ind1), i2(ind2), i3(ind3) {}
};

struct Mesh {
    std::vector<Point2> locVertices;
    std::vector<Point3> globalVertices;
    std::vector<Triangle> triangles;

    Mesh(std::vector<Point2>& vert2, std::vector<Point3>& vert3, std::vector<Triangle>& tris) : locVertices(vert2), globalVertices(vert3), triangles(tris) {}

};

struct PointCloud {
    std::vector<Point3> pts;

    inline size_t kdtree_get_point_count() const { return pts.size(); }

    inline float kdtree_distance(const float* p1, const size_t idx_p2, size_t /*size*/) const {
        const float d0 = p1[0] - pts[idx_p2].x;
        const float d1 = p1[1] - pts[idx_p2].y;
        return d0 * d0 + d1 * d1;
    }

    inline float kdtree_get_pt(const size_t idx, int dim) const {
        if (dim == 0) return pts[idx].x;
        else return pts[idx].y;
    }

    template <class BBOX>
    bool kdtree_get_bbox(BBOX& /* bb */) const { return false; }
};

