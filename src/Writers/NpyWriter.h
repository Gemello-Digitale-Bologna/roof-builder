#include "../Geometry.h"
#include <omp.h>
#include "../npy.hpp"

class NpyWriter {
public:
    NpyWriter(const std::string& filename);
    void Write2D(const std::vector<Point2>& points);
    void Write3D(const std::vector<Point3>& points);
    void WriteMat(const std::vector<std::vector<float>>& points);
    ~NpyWriter();
private:
    std::string filename;
};

NpyWriter::NpyWriter(const std::string& filename) : filename(filename) {}

void NpyWriter::Write2D(const std::vector<Point2>& points) {
    unsigned long shape[2] = {points.size(), 2};
    std::vector<std::vector<double>> data;
    for (const auto& point : points) {
        data.push_back({point.x, point.y});
    }
    npy::SaveArrayAsNumpy(filename, false, 2, shape, data);
}

void NpyWriter::Write3D(const std::vector<Point3>& points) {
    unsigned long shape[2] = {points.size(), 3};
    std::vector<std::vector<double>> data;
    for (const auto& point : points) {
        data.push_back({point.x, point.y, point.z});
    }
    npy::SaveArrayAsNumpy(filename, false, 2, shape, data);
}

void NpyWriter::WriteMat(const std::vector<std::vector<float>>& points) {
    unsigned long shape[2] = {points.size(), points[0].size()};
    std::vector<float> data;

   for (const auto& point : points) {
        data.insert(data.end(), point.begin(), point.end());
    }

    npy::SaveArrayAsNumpy(filename, false, 2, shape, data);
}

NpyWriter::~NpyWriter() {}