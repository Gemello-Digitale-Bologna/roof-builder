#include "Reader.h"
#include "../Geometry.h"
#include <omp.h>
#include "../npy.hpp"

class NpyReader {
public:
    NpyReader(const std::string& filename);
    std::vector<Point2>* Read2D();
    std::vector<Point3>* Read3D();
    std::vector<std::vector<float>>* ReadMatInt();

    std::vector<Point2>* Get2D();
    std::vector<Point3>* Get3D();
    std::vector<std::vector<float>>* GetMat();

    void Flush();
    ~NpyReader();
private:
    npy::npy_data<double> data;
    std::vector<Point3> points3d;
    std::vector<Point2> points2d;
    std::vector<std::vector<float>> mat;
};
